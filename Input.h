#pragma once


#include <cstdint>

#include "BitcoinStream.h"

struct Input {
    uint8_t* transactionHash;
    uint32_t transactionIndex;
    uint32_t sequenceNumber;
    void read(BitcoinStream& stream) {
        transactionHash = stream.readHash();
        transactionIndex = stream.readU32();
        uint32_t responseScriptLength = stream.readVarint();
        uint8_t* responseScript = static_cast<uint8_t *>(stream.pointer);
        stream.advance(responseScriptLength);

        sequenceNumber = stream.readU32();
    }
};