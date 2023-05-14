
#include <bank.h>
#include <string>

using namespace std;

/**
 * @brief prints account information
 */
void Bank::print_account() {
  for (int i = 0; i < num; i++) {
    pthread_mutex_lock(&accounts[i].lock);
    cout << "ID# " << accounts[i].accountID << " | " << accounts[i].balance
         << endl;
    pthread_mutex_unlock(&accounts[i].lock);
  }

  pthread_mutex_lock(&bank_lock);
  cout << "Success: " << num_succ << " Fails: " << num_fail << endl;
  pthread_mutex_unlock(&bank_lock);
}

/**
 * @brief helper function to increment the bank variable `num_fail` and log 
 *        message.
 * 
 * @param message
 */
void Bank::recordFail(char *message) {
  pthread_mutex_lock(&bank_lock);
  cout << message << endl;
  num_fail++;
  pthread_mutex_unlock(&bank_lock);
}

/**
 * @brief helper function to increment the bank variable `num_succ` and log 
 *        message.
 * 
 * @param message
 */
void Bank::recordSucc(char *message) {
  pthread_mutex_lock(&bank_lock);
  cout << message << endl;
  num_succ++;
  pthread_mutex_unlock(&bank_lock);
}

/**
 * @brief Construct a new Bank:: Bank object.
 * 
 * Requirements:
 *  - The function should initialize the private variables: num, num_succ, num_fail
 *  - Create a new array[N] of type Accounts. 
 *  - Initialize each account (HINT: there are three fields to initialize)
 *    - accountID, balance, lock
 * 
 * @param N 
 */
// TODO: Arav
Bank::Bank(int N) {
  num = N;
  num_succ = 0;
  num_fail = 0;

  accounts = new Account[N];

  // fill in each account and initialize locks
  for (int i = 0; i < N; i++) {
    accounts[i].accountID = i;
    accounts[i].balance = 0;
    pthread_mutex_init(&accounts[i].lock, NULL);
  }

  pthread_mutex_init(&bank_lock, NULL);
}

/**
 * @brief Destroy the Bank:: Bank object
 * 
 * Requirements:
 *  - Make sure to destroy all locks. 
 *  - Make sure to free all memory
 * 
 */
// TODO: Nihar
Bank::~Bank() {
  pthread_mutex_lock(&bank_lock);
    // loop through @accounts and destroy locks
    for (int i = 0; i < num; i++) {
      pthread_mutex_destroy(&accounts[i].lock);
    }

    // delete global allocated memory and set to NULL (dangling pointers), delete global locks
    delete[] accounts;
    accounts = NULL;
  pthread_mutex_unlock(&bank_lock);

  pthread_mutex_destroy(&bank_lock);
}

/**
 * @brief Adds money to an account 
 * 
 * Requirements:
 *  - Make sure to log in the following format
 *    `Worker [worker_id] completed ledger [ledger_id]: deposit [amount] into account [account]`
 * 
 * @param workerID the ID of the worker (thread)
 * @param ledgerID the ID of the ledger entry
 * @param accountID the account ID to deposit 
 * @param amount the amount deposited
 * @return int 
 */
// TODO: Arav
int Bank::deposit(int workerID, int ledgerID, int accountID, int amount) {
  // incremement balance: acquire lock since critical section
  pthread_mutex_lock(&accounts[accountID].lock);
    accounts[accountID].balance += amount;

    // record the success
    char message[100];
    sprintf(message, "Worker %d completed ledger %d: deposit %d into account %d", workerID, ledgerID, amount, accountID);
    recordSucc(message);
  pthread_mutex_unlock(&accounts[accountID].lock);

  return 0;
}

/**
 * @brief Withdraws money from an account
 * 
 * Requirements:
 *  - Make sure the account has a large enough balance. 
 *    - Case 1: withdraw amount <= balance, log success 
 *    - Case 2: log failure
 * 
 * @param workerID the ID of the worker (thread)
 * @param ledgerID the ID of the ledger entry
 * @param accountID the account ID to withdraw 
 * @param amount the amount withdrawn
 * @return int 0 on success -1 on failure
 */
// TODO: Manasa
int Bank::withdraw(int workerID, int ledgerID, int accountID, int amount) {
  char message[500];
 
  // incremement balance: acquire lock since critical section
  pthread_mutex_lock(&accounts[accountID].lock); 
    // amount makes balance negative: failure condition, give up lock and return -1
    if(accounts[accountID].balance < amount){
      // record the failure: not a critical section
      sprintf(message, "Worker %d failed to complete ledger %d: withdraw %d from account %d", workerID, ledgerID, amount, accountID);
      recordFail(message);
      pthread_mutex_unlock(&accounts[accountID].lock);
      return -1;
    }

    // not a failure, decrement balance and release lock
    accounts[accountID].balance -= amount;

    // record the success: not a critical section
    sprintf(message, "Worker %d completed ledger %d: withdraw %d from account %d", workerID, ledgerID, amount, accountID);
    recordSucc(message); 
  pthread_mutex_unlock(&accounts[accountID].lock);
  return 0;
}

/**
 * @brief Transfer from one account to another
 * 
 * Requirements:
 *  - Make sure there is enough money in the FROM account
 *  - Be careful with the locking order to not cause deadlocks
 *  
 * 
 * @param workerID the ID of the worker (thread)
 * @param ledgerID the ID of the ledger entry
 * @param srcID the account to transfer money out 
 * @param destID the account to receive the money
 * @param amount the amount to transfer
 * @return int 0 on success -1 on error
 */
// TODO: Nihar
int Bank::transfer(int workerID, int ledgerID, int srcID, int destID,
                   unsigned int amount) {
  // NOTE: DON'T USE withdraw or deposit FUNCTIONS!!!!!!!!!!!!!!
  // from's lock could be taken by a different thread
  // to's lock could be taken by a different thread

  // do you want to loop till both locks acquired?
  // do you want to take locks in an order but give up if second lock is unable to be acquired
  // and do so in a loop

  char message[500];

  // transfer to self: a failure condition (return -1), not a critical section
  if (srcID == destID)
  {
    sprintf(message, "Worker %d failed to complete ledger %d: transfer %d from account %d to account %d", workerID, ledgerID, amount, srcID, destID);
    recordFail(message);
    return -1;
  }
  
  // try to acquire both locks
  while(true)
  {
    pthread_mutex_lock(&accounts[srcID].lock);
    // if unable to acquire both locks give up first lock and retry
    // might want to put this thread to sleep for some time
    if (pthread_mutex_trylock(&accounts[destID].lock) != 0)
    {
      pthread_mutex_unlock(&accounts[srcID].lock);
    }
    else
    {
      // successfully acquired both locks, break out of loop
      break;
    }
  } 

  // both locks acquired suscessfully

  // 1 - WITHDRAWAL
  // failure condition, give up lock and send failure message
  if(accounts[srcID].balance < amount)
  {
    sprintf(message, "Worker %d failed to complete ledger %d: transfer %d from account %d to account %d", workerID, ledgerID, amount, srcID, destID);
    recordFail(message);
    pthread_mutex_unlock(&accounts[srcID].lock);
    pthread_mutex_unlock(&accounts[destID].lock);
    return -1;
  }

  accounts[srcID].balance -= amount;  
  // 2 - DEPOSIT 
  accounts[destID].balance += amount; 

  // send success message
  sprintf(message, "Worker %d completed ledger %d: transfer %d from account %d to account %d", workerID, ledgerID, amount, srcID, destID);
  recordSucc(message);

  // locks released - done with changing and accessing globals
  pthread_mutex_unlock(&accounts[srcID].lock);
  pthread_mutex_unlock(&accounts[destID].lock);
  return 0;
}