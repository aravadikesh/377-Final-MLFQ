#include <list>
#include <queue>
#include <string>

using namespace std;

typedef struct Process Process;
struct Process {
  int arrival;    // known from txt file
  int first_run;  // filled during scheduling algo
  int duration;   // known from txt file
  int completion; // filled during scheduling algo
  int remaining;    // remaining duration for current time slice
  int last_queue;   // last queue the process was in
  bool interactive; // true if process is interactive, false if CPU intensive
  bool boosted; // initially false, set to true if a process becomes boosted
  int boostTime; // tracks the time a process has been boosted for
  int waitingTime; // tracks time spent in a queue
};

class ArrivalComparator {
 public:
  bool operator()(const Process lhs, const Process rhs) const {
    if (lhs.arrival != rhs.arrival)
      return lhs.arrival > rhs.arrival;
    else
      return lhs.duration > rhs.duration;
  }
};

class DurationComparator {
 public:
  bool operator()(const Process lhs, const Process rhs) const {
    if (lhs.duration != rhs.duration)
      return lhs.duration > rhs.duration;
    else
      return lhs.arrival > rhs.arrival;
  }
};

typedef priority_queue<Process, vector<Process>, ArrivalComparator>
    pqueue_arrival;
typedef priority_queue<Process, vector<Process>, DurationComparator>
    pqueue_duration;

pqueue_arrival read_workload(string filename);
void show_workload(pqueue_arrival workload);
void show_processes(list<Process> processes);

list<Process> fifo(pqueue_arrival workload);
list<Process> sjf(pqueue_arrival workload);
list<Process> stcf(pqueue_arrival workload);
list<Process> rr(pqueue_arrival workload);
list<Process> mlfq(pqueue_arrival workload);

float avg_turnaround(list<Process> processes);
float avg_response(list<Process> processes);
void show_metrics(list<Process> processes);
