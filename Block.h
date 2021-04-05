#pragma once

#include "common.h"
#include "BitcoinStream.h"

Transaction staticTransaction;
struct Block {
    uint32_t blockSize;
    uint32_t blockFormatVersion;
    uint8_t* previousBlockHash;
    uint8_t* merkleRoot;
    uint32_t timeStamp;
    uint32_t bits;
    uint32_t nonce;
    uint32_t transactionCount;


    Block(BitcoinStream& stream) {
        uint32_t blockSize = stream.readU32();
        blockFormatVersion = stream.readU32();	// Read the format version
        previousBlockHash = stream.readHash();  // get the address of the hash
        merkleRoot = stream.readHash();	// Get the address of the merkle root hash
        timeStamp = stream.readU32();	// Get the timestamp
        bits = stream.readU32();	// Get the bits field
        nonce = stream.readU32();	// Get the 'nonce' random number.
        transactionCount = stream.readVarint();	// Read the number of transactions

        for (int i = 0; i < transactionCount; i++) {
            staticTransaction.read(stream);
            transactionHandler(staticTransaction);

         /*   for (int i = 0; i < staticTransaction.outputCount; i++) {
                if (staticTransaction.outputs[i].publicKey == nullptr) {
                    printReverseHash(staticTransaction.getHash());
                    std::cout << endl;
                }
            }*/
        }
    }


};