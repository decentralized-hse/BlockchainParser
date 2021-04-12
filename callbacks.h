#pragma once

#include "common.h"
#include "wallet.h"

void printTransactionHash(Transaction& transaction) {
    std::cout << "transactionHash: ";
    printReverseHash(transaction.getHash());
    std::cout << " ";
}

void printBlockTimestamp(Block& block) {
    time_t time = block.timeStamp;
    std::cout << "time: " << std::put_time(std::localtime(&time), "%Y-%m-%dT%H:%M:%S.%z%Z");
    std::cout << " ";
}

void printBTCValue(uint64_t value) {
    std::cout << (float) value / ONE_BTC << "BTC";
}

void printOutput(Output& output) {
    if (output.publicKey == nullptr) {
        return;
    }
    std::cout << "output " << Wallet(output).asString() << " ";
    printBTCValue(output.value);
    std::cout << " ";
}

void printTransactionInfo(Transaction& transaction, Block& block) {
    printTransactionHash(transaction);
    printBlockTimestamp(block);
    std::cout << "\n";

    for (int i = 0; i < transaction.inputCount; i++) {
        std::cout << "inputHash" << i << " ";
        printReverseHash(transaction.inputs[i].transactionHash);
        std::cout << " " << transaction.inputs[i].transactionIndex << "\n";
    }
    for (int i = 0; i < transaction.outputCount; i++) {
        printOutput(transaction.outputs[i]);
        std::cout << "\n";
    }

}

struct TestCallback : TransactionCallback {
    void gotTransaction(Transaction& transaction, Block& block) override {
        printTransactionInfo(transaction, block);
    }
};

struct TransactionData {
    std::array<uint8_t, 32> hash;
    uint64_t outputId;
    uint64_t value;
};

struct ExtractInputsCallback: TransactionCallback {
    std::vector<TransactionData>& outputTransactions;
    explicit ExtractInputsCallback(std::vector<TransactionData>& _outputTransactions):
            outputTransactions(_outputTransactions) {

    }

    void gotTransaction(Transaction& transaction, Block& block) override {
        for (int i = 0; i < transaction.inputCount; i++) {
            for (int j = 0; j < outputTransactions.size(); j++) {
                if (transaction.inputs[i].transactionIndex == outputTransactions[j].outputId &&
                    memcmp(transaction.inputs[i].transactionHash, &outputTransactions[j].hash, 32) == 0) {
//                    printTransactionInfo(transaction, block);
                    printTransactionHash(transaction);
                    printBlockTimestamp(block);
                    printBTCValue(outputTransactions[j].value);
                    std::cout << "\n";

                   /* std::cout << "prev: ";
                    printReverseHash(transaction.inputs[i].transactionHash);
                    std::cout << "\n\n";*/
                }
            }
        }
    }
};

struct ExtractOutputsCallback: TransactionCallback {
    std::vector<TransactionData> extractedTransactions;
    WalletComparator comparator;
    ExtractOutputsCallback(Wallet wallet): comparator(wallet.asString()) {
    }

    void gotTransaction(Transaction& transaction, Block& block) override {
        for (int i = 0; i < transaction.outputCount; i++) {
            if (comparator.isEqualToOutput(transaction.outputs[i])) {
                uint8_t* hash = const_cast<uint8_t *>(transaction.getHash());
                std::array<uint8_t, 32> copy{};
                std::copy(hash, hash + 32, copy.begin());
                extractedTransactions.push_back({copy, static_cast<uint64_t>(i),
                                                 transaction.outputs[i].value});
            }
        }
    }
};