#pragma once

#include "iostream"

#define NEED_TRANSACTION_HASH
#define WARN(x) std::cout << x;
#define ONE_BTC 100000000.0

#include "Transaction.h"

string trackedWallet;
uint8_t trackedAddress[25];
void setTrackedWallet(const string& wallet) {
    trackedWallet = wallet;
    BLOCKCHAIN_BITCOIN_ADDRESS::bitcoinAsciiToAddress(wallet.c_str(), trackedAddress);

}

#define PRINT_ALL false
void transactionHandler(Transaction& transaction) {
    for (int i = 0; i < transaction.outputCount; i++) {

        Output& output = transaction.outputs[i];
        if (output.publicKey) {
            char scratch[256];
            bool ok = false;

            if (output.isRipeMD160) {
                if (PRINT_ALL || memcmp(output.publicKey, trackedAddress + 1, 20) == 0) {
                    uint8_t temp[25];
                    BLOCKCHAIN_BITCOIN_ADDRESS::bitcoinRIPEMD160ToAddress(output.publicKey, output.ripeMD_version, temp);
                    if (PRINT_ALL || memcmp(temp, trackedAddress, 25) == 0) {
                        ok = true;
                        BLOCKCHAIN_BITCOIN_ADDRESS::bitcoinAddressToAscii(temp, scratch, 256);
                    }
                }
            } else if (output.isBech32) {
                ok = false;
            } else {
                    ok = BLOCKCHAIN_BITCOIN_ADDRESS::bitcoinPublicKeyToAscii(output.publicKey, scratch, 256);
            }


            if (ok || PRINT_ALL) {
                if (PRINT_ALL || strcmp(scratch, trackedWallet.c_str()) == 0) {
                    printf("Transaction: %d inputs %d outputs\r\n",
                           transaction.inputCount,
                           transaction.outputCount);
                    printf("TransactionHash: ");
                    printReverseHash(transaction.getHash());
                    printf("\r\n");
                    printf("    Output: %d : %f BTC \r\n", i,
                           (float) output.value / ONE_BTC);
                    printf("PublicKey: %s : %s\r\n", scratch, output.isRipeMD160 ? "RIPEMD160" : "ECDSA");
                    std::cout << flush;
                }
            } else {
                //          printf("Failed to encode the public key.\r\n");
            }
        }
    }

   /* if (rand() % 100000 == 0) {
        printReverseHash(transaction.getHash());
        std::cout << "\n";
    }*/


   /* std::cout << endl;

    for (int i = 0; i < transaction.outputCount; i++) {
        if (transaction.outputs[i].publicKey) {
            printOutputAddress(transaction.outputs[i]);
        }
    }*/
}