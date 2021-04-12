#pragma once

#include <utility>

#include "hashes.h"

class Wallet {
private:
    string address;
public:
    explicit Wallet(string _address) : address(std::move(_address)) {
    }

    explicit Wallet(const Output& output) {
        if (output.publicKey == nullptr) {
            throw logic_error("Got output with null publicKey");
        }

        if (output.isRipeMD160) {
            uint8_t temp[25];
            BLOCKCHAIN_BITCOIN_ADDRESS::bitcoinRIPEMD160ToAddress(output.publicKey, output.ripeMD_version, temp);
            char scratch[256];
            BLOCKCHAIN_BITCOIN_ADDRESS::bitcoinAddressToAscii(temp, scratch, 256);
            address = scratch;
        } else if (output.isBech32) {
            address = "";
        } else {
            char scratch[256];
            BLOCKCHAIN_BITCOIN_ADDRESS::bitcoinPublicKeyToAscii(output.publicKey, scratch, 256);
            address = scratch;
        }
    }
    string asString() {
        return address;
    }

    const char* asCString() {
        return address.c_str();
    }

    void asBytesAddress(uint8_t address[25]) {
        BLOCKCHAIN_BITCOIN_ADDRESS::bitcoinAsciiToAddress(asCString(), address);
    }


};

struct WalletComparator {
    string trackedWallet;
    uint8_t trackedAddress[25];

    explicit WalletComparator(const string &wallet) {
        setTrackedWallet(wallet);
    }

    void setTrackedWallet(const string &wallet) {
        trackedWallet = wallet;
        BLOCKCHAIN_BITCOIN_ADDRESS::bitcoinAsciiToAddress(wallet.c_str(), trackedAddress);
    }

    bool isEqualToOutput(Output &output) {
        bool ok;
        char scratch[256];
        if (output.publicKey == nullptr) {
            return false;
        }

        if (output.isRipeMD160) {
            if (memcmp(output.publicKey, trackedAddress + 1, 20) == 0) {
                uint8_t temp[25];
                BLOCKCHAIN_BITCOIN_ADDRESS::bitcoinRIPEMD160ToAddress(output.publicKey, output.ripeMD_version, temp);
                if (memcmp(temp, trackedAddress, 25) == 0) {
                    ok = true;
                    BLOCKCHAIN_BITCOIN_ADDRESS::bitcoinAddressToAscii(temp, scratch, 256);
                }
            }
        } else if (output.isBech32) {
            ok = false;
        } else {
            ok = BLOCKCHAIN_BITCOIN_ADDRESS::bitcoinPublicKeyToAscii(output.publicKey, scratch, 256);
        }


        if (ok) {
            if (strcmp(scratch, trackedWallet.c_str()) == 0) {
                return true;
            }
        }
        return false;
    }
};