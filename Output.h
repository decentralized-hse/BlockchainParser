#pragma once


#include <cstdint>
#include "BitcoinStream.h"
#include "opcodes.h"
#include "common.h"

struct Output {
    uint64_t value;
    bool isRipeMD160;
    bool isBech32;
    int bech32_len;
    uint8_t ripeMD_version;
    const uint8_t *publicKey;

    void read(BitcoinStream& stream) {
        value = stream.readU64();
        uint32_t challengeScriptLength = stream.readVarint();
        uint8_t* challengeScript = static_cast<uint8_t *>(stream.pointer);
        stream.advance(challengeScriptLength);

        decodeScript(challengeScriptLength, challengeScript);
    }


    void decodeScript(uint32_t challengeScriptLength, uint8_t* challengeScript) {
        publicKey = nullptr;
        ripeMD_version = 0;
        isBech32 = false;
        isRipeMD160 = false;
        if (challengeScriptLength == 67 && challengeScript[0] == 65 &&
            challengeScript[66] == OP_CHECKSIG) { // is_pubkey
            publicKey = challengeScript + 1;
        } else if (challengeScriptLength == 66 && challengeScript[65] == OP_CHECKSIG) {
            publicKey = challengeScript;
        } else if (challengeScriptLength >= 25 && // is_pubkeyhash
                   challengeScript[0] == OP_DUP &&
                   challengeScript[1] == OP_HASH160 &&
                   challengeScript[2] == 20) {
            publicKey = challengeScript + 3;
            isRipeMD160 = true;
        } else if (challengeScriptLength == 5 &&
                   challengeScript[0] == OP_DUP &&
                   challengeScript[1] == OP_HASH160 &&
                   challengeScript[2] == OP_0 &&
                   challengeScript[3] == OP_EQUALVERIFY &&
                   challengeScript[4] == OP_CHECKSIG) {
            WARN("Unexpected output script");
        } else if (challengeScriptLength == 23 &&
                challengeScript[0] == OP_HASH160 &&
                challengeScript[1] == 0x14 &&
                challengeScript[22] == OP_EQUAL) {  // is_p2sh
            publicKey = challengeScript + 2;
            isRipeMD160 = true;
            ripeMD_version = 5;
        } else if (challengeScriptLength == 22 &&
            challengeScript[0] == 0x00 &&
            challengeScript[1] == 0x14) { // is_p2wpkh
            publicKey = challengeScript + 2;
            isBech32 = true;
            bech32_len = 20;
        } else if (challengeScriptLength == 34 &&
        challengeScript[0] == 0x00 &&
        challengeScript[1] == 0x20) { // is_p2wsh
            publicKey = challengeScript + 2;
            isBech32 = true;
            bech32_len = 32;
        } else if (challengeScript[0] == OP_RETURN) { // is_return
            return;
        } else {
            // Ok..we are going to scan for this pattern.. OP_DUP, OP_HASH160, 0x14 then exactly 20 bytes after 0x88,0xAC
            // 25...
            if (challengeScriptLength > 25) {
                uint32_t endIndex = challengeScriptLength - 25;

                for (uint32_t i = 0; i < endIndex; i++) {
                    const uint8_t *scan = &challengeScript[i];
                    if (scan[0] == OP_DUP &&
                        scan[1] == OP_HASH160 &&
                        scan[2] == 20 &&
                        scan[23] == OP_EQUALVERIFY &&
                        scan[24] == OP_CHECKSIG) {
                        publicKey = &(scan[3]);
                        isRipeMD160 = true;
                        WARN("Unexpected output script(2)")
                        break;
                    }
                }
            }
            if (!publicKey) {
                if (challengeScriptLength >= 66 &&
                    challengeScript[challengeScriptLength - 1] == OP_CHECKSIG) {
                    WARN("Unexpected output script(3)")
                }
      //          std::cout << "undecoded output script" << endl;
            }
        }
    }

};