#ifndef _BANK_H
#define _BANK_H

#include <stdlib.h>
#include <fstream>
#include <string>
#include <sys/wait.h>   /* for wait() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <sys/mman.h>   /* for mmap() ) */
#include <semaphore.h>  /* for sem */
#include <assert.h>		/* for assert */
#include <iostream>     /* for cout */
#include <list>
#include <array>
#include <pthread.h> 

using namespace std;

struct Account {
  unsigned int accountID;   // unique id for an account - lets you index into the bank's accounts array
  long balance;             // ensure >= 0: a failure condition
  pthread_mutex_t lock;     // lock for an account - no withdrawals/deposits at same time
  // still not too clear on why this lock exists if we have bank_lock
  // when to use which lock
};


class Bank {
  private:
    int num;       // #accounts in the Bank
    int num_succ;  // #successful transactions executed - each line in .txt is a transaction
    int num_fail;  // #failed transactions executed - withdrawing/transfer pushes balance to negative 
    
  public:
    Bank(int N);
    ~Bank(); // destructor
    
    int deposit(int workerID, int ledgerID, int accountID, int amount);
    int withdraw(int workerID, int ledgerID, int accountID, int amount);
    int transfer(int workerID, int ledgerID, int src_id, int dest_id, unsigned int amount);
    
    // helper functions for logging and print debugging
    void print_account();
    void recordSucc(char* message);
    void recordFail(char* message);

    pthread_mutex_t bank_lock;  // a global bank lock - one for each bank - but only deal with 1 bank
    // seems to be used only for private Bank attributes access and change
    struct Account *accounts;   // array of accounts in the bank
};

#endif