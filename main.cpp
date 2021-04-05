#include <iostream>
using namespace  std;

#include "parser.h"
#include "tqdm.h"


#define FBK_WALLET "3QzYvaRFY6bakFBW4YBRrzmwzTnfZcaA6E"
int main() {


    setTrackedWallet(FBK_WALLET);//"1KYHb17BRZT9g7fKbawRwgboJ2cK3m1krX");

    setBitcoinDirectory("/home/user/bitcoin/blocks");
    int files_count = filesCount();
    cout << "Files in dir: " << files_count << endl;

    freopen("../res.txt", "w", stdout);
    for (int i : tq::trange(2300, files_count)) {
//    for (int i = 2300; i < files_count; i++) {
        parseFile(i);
    }
}
