#include <scheduling.h>
#include <fstream>
#include <iostream>
#include <list>
#include <queue>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>


using namespace std;

#define slice 4 // To be defined by the user

pqueue_arrival read_workload(string filename) {
  pqueue_arrival workload;
  ifstream infile(filename);

  string line;

  while (getline(infile, line)) {
    istringstream iss(line);
    int arrival, duration, priority, interactive;
    if (!(iss >> arrival >> duration >> interactive >> priority)) { 
      break;
    } 

    Process p = {arrival, -1, duration, 0, duration, interactive, false, 0, 0, priority};
    workload.push(p);
  }

  infile.close();
  return workload;
}

void show_workload(pqueue_arrival workload) {
  pqueue_arrival xs = workload;
  cout << "Workload:" << endl;
  while (!xs.empty()) {
    Process p = xs.top();
    cout << '\t' << p.arrival << ' ' << p.duration << ' ' << p.interactive << ' ' << p.priority << endl;
    xs.pop();
  }
}

void show_processes(list<Process> processes) {
  list<Process> xs = processes;
  cout << "Processes:" << endl;
  while (!xs.empty()) {
    Process p = xs.front();
    cout << "\tarrival=" << p.arrival << ", duration=" << p.duration
         << ", first_run=" << p.first_run << ", completion=" << p.completion
         << endl;
    xs.pop_front();
  }
}

list<Process> fifo(pqueue_arrival workload) {
  list<Process> complete;
  int current_time = 0;
  while (!workload.empty()) {
    Process p = workload.top();
    workload.pop();

    p.first_run = max(p.arrival, current_time);
    p.completion = p.first_run + p.duration;

    current_time = p.completion;
    complete.push_back(p);
  }

  return complete;
}

list<Process> sjf(pqueue_arrival workload) {
  list<Process> complete;
  pqueue_duration procsArrived;   // procsArrived pqueue to order 'current arrived' proc by duration
  int current_time = 0;   // current time passed from start of scheduling

  // order proc by duration those that have arrived on or before @current time
  // because @workload won't order a later arrived proc b4 an earlier arrived despite shorter duration

  // loop to run all procs in @workload acc to sjf
  while(!workload.empty())
  {
    Process proc; // placeholder

    // get all proc that have arrived on or before @current_time
    // be mindful you don't call pop on empty @workload
    while (true)
    {
      if (workload.empty())
      {
        break;
      }

      proc = workload.top();

      if (proc.arrival <= current_time)
      {
        procsArrived.push(proc);
        workload.pop();
      }
      else
      {
        break;
      }

    }

    if (procsArrived.empty())
    {
      current_time = workload.top().arrival;
      continue;
    }

    proc = procsArrived.top();
    procsArrived.pop();

    proc.first_run = current_time;
    proc.completion = current_time + proc.duration;
    current_time = proc.completion;

    while(!procsArrived.empty())
    {
      workload.push(procsArrived.top());
      procsArrived.pop();
    }

    complete.push_back(proc);
  } 

  return complete;
}

list<Process> stcf(pqueue_arrival workload) {
  list <Process> complete; 
  pqueue_duration procsArrived; 
  int current_time = 0;

  while(!workload.empty()) {
    Process proc; 

    while(true){    
      if(workload.empty()){
        break;
      }
      proc = workload.top(); 
      if(proc.arrival <= current_time){                             
        procsArrived.push(proc);
        workload.pop(); 
      }
      else{
        break;
      }
    }

    if(procsArrived.empty()){
      current_time = workload.top().arrival;
      continue; 
    }

    proc = procsArrived.top();
    procsArrived.pop();

    if(proc.first_run == -1){
      proc.first_run = current_time;
    }

    if(!workload.empty() && workload.top().arrival < (current_time + proc.duration)){
      
      proc.duration = proc.duration - (workload.top().arrival - current_time);
      current_time = workload.top().arrival;

      if(proc.duration == 0){
        proc.completion = current_time;
        complete.push_back(proc);
      }
      else{
        procsArrived.push(proc);
      }
    }

    else{
      proc.completion = current_time + proc.duration;
      current_time = proc.completion;
      complete.push_back(proc);
    }

    while(!procsArrived.empty()){
      workload.push(procsArrived.top());
      procsArrived.pop();
    }
  }

  return complete;
}

list<Process> rr(pqueue_arrival workload) {
	list <Process> complete; 
  queue<Process> procsInExec;
	int current_time = 0;

	while(!(workload.empty() && procsInExec.empty())) 
	{
		Process proc; 

		while(true){	

			if(workload.empty()){
				break;
			}

			proc = workload.top(); 
			if(proc.arrival <= current_time){ 				
				procsInExec.push(proc);
				workload.pop();
			}
			else{
				break;
			}
		}

		if(procsInExec.empty()){
			current_time = workload.top().arrival;
			continue; 
		}

		proc = procsInExec.front();
		procsInExec.pop();

		if(proc.first_run == -1){
			proc.first_run = current_time;
		}

		proc.duration -= 1;
    current_time += 1; 

		if(proc.duration <= 0){
	    proc.completion = current_time;
	    complete.push_back(proc);
    }
    else{
      procsInExec.push(proc);
    }
  }

	return complete;
}

float avg_turnaround(list<Process> processes) {

  float turnaroundSum = 0;
  float count = 0;

  for (auto procItr = processes.begin(); procItr != processes.end(); ++procItr)
  {
    turnaroundSum += procItr->completion - procItr->arrival;
    count += 1;
  }

  return (float) (turnaroundSum/count);
}

float avg_response(list<Process> processes) {
  float responseSum = 0;
  float count = 0;
  for (auto procItr = processes.begin(); procItr != processes.end(); ++procItr)
  { 
    responseSum += procItr->first_run - procItr->arrival;
    count += 1;
  }
 return (float) (responseSum/count);
}

void show_metrics(list<Process> processes) {
  float avg_t = avg_turnaround(processes);
  float avg_r = avg_response(processes);
  show_processes(processes);
  cout << '\n';
  cout << "Average Turnaround Time: " << avg_t << endl;
  cout << "Average Response Time:   " << avg_r << endl;
}

// Define queue structure for the priority levels
typedef queue<Process> queueP;

void processHandler(int currentTime, queueP* q0, queueP* q1, queueP* q2, queueP* q3, pqueue_arrival* workload) {
  while (!workload->empty()) {
    Process p = workload->top();
    if(p.arrival > currentTime) {
        break;
    } else {
      workload->pop();
      // Determine the priority of the process and place it in the corresponding queue
      if (p.priority == 0) {
        q0->push(p);
      } else if (p.priority == 1) {
        q1->push(p);
      } else if (p.priority == 2) {
        q2->push(p);
      } else if (p.priority == 3) {
        q3->push(p);
      }
    }
  }
}

void processRunner(queueP* initialQ, queueP* finalQ, int* time, list<Process>* processes, int runSlice, int* boost) {
  Process p = initialQ->front();
  initialQ->pop();

  // Execute the process for 1 time slice
  if(p.first_run == -1){
    p.first_run = *time;
  }

  p.remaining -= runSlice;
  *time += runSlice;
  *boost += runSlice; // irrelevent for mlfq, but important for mlfqBasic
  p.waitingTime +=  runSlice; // Increment waiting time for the process

  if (p.boosted) { // irrelevent for ml
    p.boostTime += runSlice; // Track the amount of time a boosted process is boosted
  }

  if (p.remaining <= 0) {
    p.completion = *time + runSlice + p.remaining;
    processes->push_back(p);
  } else if (p.boosted && p.boostTime >= slice*2) { 
    p.boosted = false;
    p.interactive = false;
    finalQ->push(p);
  } else {
    finalQ->push(p);
  }
}

list<Process> mlfqBasic(pqueue_arrival workload){
  // Create the queues
  queueP q0;
  queueP q1;
  queueP q2;
  queueP q3;

  void processHandler(int currentTime, queueP* q0, queueP* q1, queueP* q2, queueP* q3, pqueue_arrival* workload);
  void processRunner(queueP* initialQ, queueP* finalQ, int* time, list<Process>* processes, int runSlice, int* boost);

  // Initialize the processes list
  list<Process> processes;
  // Start the scheduling algorithm
  int time = 0;
  int boostTimer = 0;
  int boostLimit = 40; // Arbitrary time limit to boost low priority processes

  processHandler(time, &q0, &q1, &q2, &q3, &workload);

  while (true) {

    if(q0.empty() && q1.empty() && q2.empty() && q3.empty() && workload.empty()) {
      break;
    }

    processHandler(time, &q0, &q1, &q2, &q3, &workload);

    while(!q0.empty()) {
      processRunner(&q0, &q1, &time, &processes, 1, &boostTimer);
    }

    if(!q1.empty()) {
      processRunner(&q1, &q2, &time, &processes, slice*2, &boostTimer);
    }

    if(!q2.empty()) {
      processRunner(&q2, &q3, &time, &processes, slice*4, &boostTimer);
    }

    if(!q3.empty()) {
      processRunner(&q3, &q3, &time, &processes, slice*5, &boostTimer);
    }

    if(boostTimer >= boostLimit) {
      if(!q1.empty()) {
        processRunner(&q1, &q0, &time, &processes, 0, &boostTimer);
      } 

      if(!q2.empty()) {
        processRunner(&q2, &q0, &time, &processes, 0, &boostTimer);
      }

      if(!q3.empty()) {
        processRunner(&q3, &q0, &time, &processes, 0, &boostTimer);
      }
      boostTimer = 0;
    }

    if(q0.empty() && q1.empty() && q2.empty() && q3.empty() && !workload.empty()){
      time = workload.top().arrival;
    }

  }
  return processes;
}

list<Process> mlfq(pqueue_arrival workload){
  // Create the queues
  queueP q0;
  queueP q1;
  queueP q2;
  queueP q3;

  void processHandler(int currentTime, queueP* q0, queueP* q1, queueP* q2, queueP* q3, pqueue_arrival* workload);
  void processRunner(queueP* initialQ, queueP* finalQ, int* time, list<Process>* processes, int runSlice, int* boost);

  // Initialize the processes list
  list<Process> processes;
  // Start the scheduling algorithm
  int time = 0;
  int agingTimer = 0; // Timer to trigger aging mechanism
  const int agingThreshold = 20; // Define a threshold for aging (arbitrary value)
  int boostTimer = 0; // irrelevent in this function, required for consistency purposes

  processHandler(time, &q0, &q1, &q2, &q3, &workload);

  while (true) {

    if(q0.empty() && q1.empty() && q2.empty() && q3.empty() && workload.empty()) {
      break;
    }

    processHandler(time, &q0, &q1, &q2, &q3, &workload);

    // Check for interactive processes in the first queue and run them in RR fashion
    // Allows for quick completion of short burst interactive processes
    // Can also allow for boosted processes to receive CPU time
    while (!q0.empty() && q0.front().interactive) {
      processRunner(&q0, &q0, &time, &processes, 1, &boostTimer);
    }

    //Check for CPU intensive processes in the first queue
    if (!q0.empty()) {
      Process p = q0.front();
      q0.pop();
      
      if(p.first_run == -1){
			  p.first_run = time;
		  }

      // Execute the process for the maximum time slice
      p.remaining -= slice;
      time += slice;
      p.waitingTime++; // Increment waiting time for the process

      if (p.remaining <= 0) {
        p.completion = time + slice + p.remaining;
        processes.push_back(p);
      } else {
        q1.push(p);
      }
    }

    if(!q1.empty()) {
      processRunner(&q1, &q2, &time, &processes, slice*2, &boostTimer);
    }

    if(!q2.empty()) {
      processRunner(&q2, &q3, &time, &processes, slice*4, &boostTimer);
    }

    // Check if it's time to perform aging
    if (agingTimer >= agingThreshold) {
      agingTimer = 0;

      // Aging mechanism: Promote processes in lower-priority queues if they have been waiting for a long time
      if (!q1.empty()) {
        queueP tempQ1; // Temporary queue to hold promoted processes

        // Iterate over the processes in q1
        while (!q1.empty()) {
          Process p = q1.front();
          q1.pop();

          // Check waiting time threshold for promotion
          if (p.waitingTime >= 15) {  // Arbitrary value for promoting to a higher queue
            p.waitingTime = 0;
            q0.push(p); // Promote the process to the first queue
          } else {
            tempQ1.push(p); // Store the process back in the temporary queue
          }
        }

        // Restore the remaining processes in tempQ1 back to q1
        q1 = tempQ1;
      }
    }

    // Check for processes in the fourth queue
    if (!q3.empty()) {
      Process p = q3.front();
      q3.pop();

      if(p.first_run == -1){
        p.first_run = time;
      }

      // Execute the process for the maximum time slice
      p.remaining -= slice*5;
      time += slice*5;
      if (p.remaining <= 0) {
        p.completion = time + slice*5 + p.remaining;
        processes.push_back(p);
      } else {
        // Determine boost factor based on number of processes in fourth queue
        int num_processes = q3.size();
        double boost_factor = 1.0;
        if (num_processes > 5) {
          boost_factor = 1.5;
        } else if (num_processes < 5) {
          boost_factor = 1.2;
        }

        // Check if process is CPU intensive and should be boosted
        if (!p.interactive) {
          p.interactive = true;
          p.boosted = true;
          p.boostTime = 0;

          // Adjust time slice based on boost factor
          // This gives a neglected/starved process direct priority to the CPU
          // This can result in inefficiencies as increasing the remaining time can give a process excess access time, over what it actually needs.
          int time_slice = (int) (10 * boost_factor);
          p.remaining += time_slice;

          q0.push(p);
        }  else {
          q3.push(p);
        }
      }
    }

    // Increment the time
    if(q0.empty() && q1.empty() && q2.empty() && q3.empty() && !workload.empty()){
      agingTimer += workload.top().arrival - time;
      time = workload.top().arrival;
    }
  }

  return processes;
}
