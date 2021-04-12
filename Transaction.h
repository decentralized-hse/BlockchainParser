#pragma once

#include <vector>
#include "BitcoinStream.h"
#include "common.h"
#include "stdexcept"
#include "hashes.h"
#include "Input.h"
#include "Output.h"

using namespace  std;

#define MAX_INPUT_OUTPUT_COUNT 16000

struct Transaction {
    bool isSegwit;
    uint32_t inputCount;
    Input inputs[MAX_INPUT_OUTPUT_COUNT];

    uint32_t outputCount;
    Output outputs[MAX_INPUT_OUTPUT_COUNT];

    uint32_t lockTime;

#ifdef NEED_TRANSACTION_HASH
private:
    void* begin;
    void* _offset_before_tx_witnesses = nullptr;
    void* end;
    uint8_t			transactionHash[32];
    bool hashCalculated = false;
public:
#endif

    inline const unsigned char * getHash() {
#ifdef NEED_TRANSACTION_HASH

        if (!hashCalculated) {
            hashCalculated = true;

            std::string hashData;
            if (isSegwit) {
                hashData.append((char*)begin, (char *) begin + 4);
                hashData.append((char*)begin + 6, (char*)_offset_before_tx_witnesses);
                hashData.append((char*)end-4, (char*)end);
            } else {
                hashData.append((char*)begin, (char*)end);
            }
            BLOCKCHAIN_SHA256::doubleSHA256((void *) hashData.c_str(), hashData.size(), transactionHash);
        }
        return transactionHash;
#else
        throw logic_error("flag NEED_TRANSACTION_HASH not enabled");
#endif
    }


    void read(BitcoinStream& stream) {
#ifdef NEED_TRANSACTION_HASH
        hashCalculated = false;
        begin  = stream.pointer;
#endif
        uint16_t transactionVersionNumber = stream.readU32();

        int16_t firstBytes = *static_cast<uint16_t*>(stream.pointer);
        if (firstBytes == 256) {
            isSegwit = true;
            stream.readU16();
        } else {
            isSegwit = false;
        }

        inputCount = stream.readVarint();
        if (inputCount > MAX_INPUT_OUTPUT_COUNT) {
            WARN("too many inputs: " + to_string(inputCount));
            while (inputCount > MAX_INPUT_OUTPUT_COUNT) {
                inputs[0].read(stream); // just skip them
                inputCount--;
            }
        }
        for (int i = 0; i < inputCount; i++) {
            inputs[i].read(stream);
        }

        outputCount = stream.readVarint();
        if (outputCount > MAX_INPUT_OUTPUT_COUNT) {
            WARN("too many outputs: " + to_string(outputCount));
            while (outputCount > MAX_INPUT_OUTPUT_COUNT) {
                outputs[0].read(stream); // just skip them
                outputCount--;
            }
        }
        for (int i = 0; i < outputCount; i++) {
            outputs[i].read(stream);
        }

        if (isSegwit) {
#ifdef NEED_TRANSACTION_HASH
            _offset_before_tx_witnesses = stream.pointer;
#endif
            for (int i = 0; i < inputCount; i++) {
                int tx_witnesses_n = stream.readVarint();
                for (int j = 0; j < tx_witnesses_n; j++) {
                    int component_size = stream.readVarint();
                    stream.advance(component_size);
                }
            }
        }

        lockTime = stream.readU32();

#ifdef NEED_TRANSACTION_HASH
        end = stream.pointer;
#endif
    }



};