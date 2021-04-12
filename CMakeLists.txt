cmake_minimum_required(VERSION 3.17)
project(myBitcoinParser)

find_package(Boost)

set(CMAKE_CXX_STANDARD 17)


set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(myBitcoinParser main.cpp parser.h BitcoinStream.h common.h Block.h Transaction.h hashes.h Input.h Output.h opcodes.h tqdm.h bech32.h bech32.cpp util/vector.h callbacks.h wallet.h)