#pragma once

#include "iostream"

#define NEED_TRANSACTION_HASH
#define WARN(x) std::cerr << x;
//#define WARN(x)
#define ONE_BTC 100000000.0

#include "Transaction.h"

struct Block;
struct TransactionCallback {
    virtual void gotTransaction(Transaction& transaction, Block& block) = 0;
    virtual ~TransactionCallback() = default;
};

TransactionCallback* transactionCallback = nullptr;
void setTransactionCallback(TransactionCallback* callback) {
    transactionCallback = callback;
}

void transactionHandler(Transaction& transaction, Block& block) {
    if (transactionCallback != nullptr) {
        transactionCallback->gotTransaction(transaction, block);
    }
}

#include "Block.h"
