#pragma once
#include <string>
#include <filesystem>
#include <sstream>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include "BitcoinStream.h"
#include "common.h"
#include "Block.h"

namespace bip = boost::interprocess;

using namespace std;

string bitcoinDirectory;
void setBitcoinDirectory(string dir) {
    bitcoinDirectory = dir;
}

int filesCount() {
    int cnt = 0;
    for (const auto & entry : filesystem::directory_iterator(bitcoinDirectory)) {
        string path = entry.path();
        if (path.find("blk") != std::string::npos) {
            cnt++;
        }
    }
    return cnt;
}



#define MAGIC_ID 0xD9B4BEF9
Block parseBlock(BitcoinStream& stream) {
    uint32_t magicId = stream.readU32();
    if (magicId != MAGIC_ID) {
        WARN("invalid magicid");
    }
    Block block(stream);
    return block;
}

void parseFile(int fileId) {
    ostringstream filePathStream;
    filePathStream << bitcoinDirectory << "/blk" << setw(5) << setfill('0') << fileId << ".dat";

    string filePath = filePathStream.str();
    bip::file_mapping mapping(filePath.c_str(), bip::read_only);
    bip::mapped_region mapped_rgn(mapping, bip::read_only);

    BitcoinStream stream(mapped_rgn.get_address(), mapped_rgn.get_size());
    while (stream.remaining() > 0) {
        parseBlock(stream);
    }
}