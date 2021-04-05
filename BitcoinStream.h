#pragma once

#include <cstddef>
#include <cinttypes>
using namespace  std;


struct BitcoinStream {
    void* begin;
    void* pointer;
    size_t size;

    BitcoinStream(void* _pointer, size_t _size) :
    begin(_pointer),pointer(_pointer), size(_size) {

    }
    
    int64_t remaining() {
        return (int64_t)size - (int64_t)(static_cast<char*>(pointer) - static_cast<char*>(begin) + 1);
    }

    inline void advance(size_t cnt) {
        pointer = static_cast<char*>(pointer) + cnt;
    }

    inline uint8_t readU8()
    {
        uint8_t ret = *(uint8_t *)pointer;
        advance(sizeof(uint8_t));
        return ret;
    }

    // Read two bytes from the block-chain input stream.
    inline uint16_t readU16()
    {
        uint16_t ret = *(uint16_t *)pointer;
        advance(sizeof(uint16_t));
        return ret;
    }

    // Read four bytes from the block-chain input stream.
    inline uint32_t readU32()
    {
        uint32_t ret = *(uint32_t *)pointer;
        advance(sizeof(uint32_t));
        return ret;
    }

    // Read eight bytes from the block-chain input stream.
    inline uint64_t readU64()
    {
        uint64_t ret = *(uint64_t *)pointer;
        advance(sizeof(uint64_t));
        return ret;
    }

    // Return the current stream pointer representing a 32byte hash and advance the read pointer accordingly
    inline uint8_t *readHash()
    {
        uint8_t *ret = (uint8_t*)pointer;
        advance(32);
        return ret;
    }

    // reads a variable length integer.
    // See the documentation from here:  https://en.bitcoin.it/wiki/Protocol_specification#Variable_length_integer
    inline uint32_t readVarint()
    {
        uint32_t ret = 0;

        uint8_t v = readU8();
        if ( v < 0xFD ) // If it's less than 0xFD use this value as the unsigned integer
        {
            ret = (uint32_t)v;
        }
        else
        {
            uint16_t v = readU16();
            if ( v < 0xFFFF )
            {
                ret = (uint32_t)v;
            }
            else
            {
                uint32_t v = readU32();
                if ( v < 0xFFFFFFFF )
                {
                    ret = (uint32_t)v;
                }
                else
                {
                    uint64_t v = readU64();
                    ret = (uint32_t)v;
                }
            }
        }
        return ret;
    }
};
