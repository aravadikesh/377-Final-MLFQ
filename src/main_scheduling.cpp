#include <scheduling.h>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {

  /* 
    Usage: Enter the following syntax into the command line.
    ./scheduling.app "required scheduling algorithm" "workloads/required workload file"
     Example : to run the basic MLFQ function on workload3 :
     ./scheduling_app mlfqBasic workloads/workload3.txt
     Example : to run the enhanced MLFQ function on workload1 :
    ./scheduling_app mlfq workloads/workload1.txt
  */

  if (argc != 3) {
    cout << "usage: [fifo|sjf|stcf|rr|mlfq|mlfqBasic] workload_file" << endl;
    exit(1);
  }

  string algorithm = argv[1];
  string workload_file = argv[2];

  pqueue_arrival workload = read_workload(workload_file);

  // To view Workload:
  // show_workload(workload);
  // return 0;
  
  if (algorithm == "fifo") {
    show_metrics(fifo(workload));
  } else if (algorithm == "sjf") {
    show_metrics(sjf(workload));
  } else if (algorithm == "stcf") {
    show_metrics(stcf(workload));
  } else if (algorithm == "rr") {
    show_metrics(rr(workload));
  } else if (algorithm == "mlfq") {
    show_metrics(mlfq(workload));
  } else if (algorithm == "mlfqBasic") {
    show_metrics(mlfqBasic(workload));
  } else {
    cout << "Error: Unknown algorithm: " << algorithm << endl;
    cout << "usage: [fifo|sjf|stcf|rr|mlfq|mlfqBasic] workload_file" << endl;
    exit(1);
  }

  return 0;
}
