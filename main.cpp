#include <iostream>
using namespace  std;

#include "parser.h"
#include "tqdm.h"
#include "common.h"
#include "callbacks.h"

#define PRINT_ALL false



#define FBK_WALLET "3QzYvaRFY6bakFBW4YBRrzmwzTnfZcaA6E"


int main() {
    setBitcoinDirectory("/home/user/bitcoin/blocks");
    int files_count = filesCount();
    cout << "Files in dir: " << files_count << endl;


    ExtractOutputsCallback callback(Wallet(FBK_WALLET));
    setTransactionCallback(&callback);
    std::cerr << "Extracting outputs with wallet " << callback.comparator.trackedWallet << std::endl;

    freopen("../res_outputs.txt", "w", stdout);
    std::cout << "Transactions to wallet " << callback.comparator.trackedWallet << std::endl;
    for (int i : tq::trange(0, files_count)) {
        parseFile(i);
    }
    std::cerr << "Extracted " << callback.extractedTransactions.size() << " transactions." << std::endl;


    std::cerr << "Extracting input transactions with same outputs" << std::endl;

    freopen("../res_inputs.txt", "w", stdout);
    std::cout << "Transactions from wallet " << callback.comparator.trackedWallet << std::endl;
    ExtractInputsCallback inputsCallback(callback.extractedTransactions);
    setTransactionCallback(&inputsCallback);
    for (int i : tq::trange(0, files_count)) {
        parseFile(i);
    }
    std::cerr << "Done" << std::endl;
    return 0;
}
