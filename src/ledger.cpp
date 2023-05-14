#include <ledger.h>
using namespace std;

// TODO: Manasa
// correctly initialize lock
pthread_mutex_t ledger_lock;

list<struct Ledger> ledger;
Bank *bank;

/**
 * @brief creates a new bank object and sets up workers
 * 
 * Requirements:
 *  - Create a new Bank object class with 10 accounts.
 *  - Load the ledger into a list: just a call to @load_ledger possibly
 * 	- Initialize lock using init()
 *  - Set up the worker threads.  
 * 	- HAVE to reap threads, destroy locks, free memory etc
 *  
 * @param num_workers 
 * @param filename 
 */
// TODO: Manasa
void InitBank(int num_workers, char *filename) {
	bank = new Bank (10);
	load_ledger(filename);
	pthread_mutex_init(&ledger_lock, NULL);
	pthread_t *thread_array = (pthread_t*) malloc(num_workers * sizeof(pthread_t));
	int arr [10] = {0,1,2,3,4,5,6,7,8,9};

	bank->print_account();
	// create threads and run them
	for(int i = 0; i < num_workers; ++i){
		pthread_create((thread_array + i), NULL, worker, (void*)(arr + i)); 
	}

	// wait for them to complete
	for(int i = 0; i < num_workers; ++i){
		pthread_join(thread_array[i], NULL);
	}
	bank->print_account();

	// free memory, destroy locks and call bank deconstructor - frees up internal memory
	pthread_mutex_destroy(&ledger_lock);
	free(thread_array);
	delete bank;	// implicitly calls the deconstructor
	thread_array = NULL;
	bank = NULL;
}

/**
 * @brief Parse a ledger file and store each line into a list
 * 
 * @param filename 
 */
void load_ledger(char *filename){
	
	ifstream infile(filename);
	int f, t, a, m, ledgerID=0;
	while (infile >> f >> t >> a >> m) {
		struct Ledger l;
		l.from = f;
		l.to = t;
		l.amount = a;
		l.mode = m;
		l.ledgerID = ledgerID++;
		ledger.push_back(l);
	}
}

/**
 * @brief Remove items from the list and execute the instruction.
 * 
 * @param workerID 
 * @return void* 
 */
// The function worker should be executed by num_workers threads given as the parameter in InitBank(). 
// Each worker thread should remove one entry from the ledger list and run the function indicated by mode 
// in struct ledger. Note: be careful how you remove an item from the ledger list by making sure there are 
// no race conditions and that the function does not become single-threaded.
// TODO: Nihar
void* worker(void *workerID){
	// use ledger lock while popping from the list - if empty termimate thread
	// determine the type of transaction - from mode attribute
	// call the corrresponding function
	int workID = *((int*) workerID);
	struct Ledger tempLedge;

	while(true)
	{
		pthread_mutex_lock(&ledger_lock);
			// ledger is empty - release lock and exit 
			if (ledger.empty()){
				pthread_mutex_unlock(&ledger_lock);
				return NULL;
			}

			// ledger has instructions - pop and run
			// release lock to allow concurrency and parallelism
			tempLedge = ledger.front();
			ledger.pop_front();
		pthread_mutex_unlock(&ledger_lock);

		// run correct transaction function
		// these functions are thread safe so no critical section here
		
		if (tempLedge.mode == D){
			bank->deposit(workID, tempLedge.ledgerID, tempLedge.from, tempLedge.amount);
		}
		else if (tempLedge.mode == W){
			bank->withdraw(workID, tempLedge.ledgerID, tempLedge.from, tempLedge.amount);
		}
		else if (tempLedge.mode == T){
			bank->transfer(workID, tempLedge.ledgerID, tempLedge.from, tempLedge.to, tempLedge.amount);
		}
	}
}
