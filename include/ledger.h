#ifndef _LEDGER_H
#define _LEDGER_H

#include <bank.h>

using namespace std;

// enumeration for mode checks
#define D 0 // DEPOSIT
#define W 1 // WITHDRAW
#define T 2 // TRANSFER

const int SEED_RANDOM = 377;

struct Ledger {
	int from;			// accountID being acted on
	int to;				// meaningless unless mode is 2 (i.e. transfer), represents the receiver
	int amount;		// amount involved for transactions
  int mode;			// 0 - Deposit, 1 - Withdraw, 2 - Transfer
	int ledgerID; // lineNo of the txt file
};

extern list<struct Ledger> ledger; // list of transactions - each line from .txt 

void InitBank(int num_workers, char *filename);
void load_ledger(char *filename);
void *worker(void *unused);

#endif