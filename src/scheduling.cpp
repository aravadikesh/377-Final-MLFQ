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

pqueue_arrival read_workload(const string& filename) {
  pqueue_arrival workload;
  ifstream infile(filename);

  string line;

  while (getline(infile, line)) {
    istringstream iss(line);
    int arrival, duration;
    bool interactive;
    if (!(iss >> arrival >> duration >> interactive)) {
      break;
    } 

    Process p = {arrival, -1, duration, 0, duration, 0, interactive, false, 0, 0};
    workload.push(p);
  }

  infile.close();
  return workload;
}

void show_workload(pqueue_arrival workload) 
{
  pqueue_arrival xs = workload;
  cout << "Workload:" << endl;
  while (!xs.empty()) {
    Process p = xs.top();
    cout << '\t' << p.arrival << ' ' << p.duration << endl;
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

// Define four queues for each of the four priority levels
typedef queue<Process> queue0;
typedef queue<Process> queue1;
typedef queue<Process> queue2;
typedef queue<Process> queue3;

list<Process> mlfq(pqueue_arrival workload){
  // Create the queues
  queue0 q0;
  queue1 q1;
  queue2 q2;
  queue3 q3;

  // Initialize the processes list
  list<Process> processes;

  // Move the processes from the priority queue to the first queue
  while (!workload.empty()) {
    Process p = workload.top();
    p.remaining = p.duration;
    p.last_queue = 0;
    p.waitingTime = 0; // Initialize waiting time to 0
    workload.pop();
    q0.push(p);
  }

  // Start the scheduling algorithm
  int time = 0;
  int agingTimer = 0; // Timer to trigger aging mechanism
  const int agingThreshold = 100; // Define a threshold for aging (arbitrary value)

  while (!q0.empty() || !q1.empty() || !q2.empty() || !q3.empty()) {
    // Check for interactive processes in the first queue
    // Allows for quick completion of short burst interactive processes
    // Can also allow for boosted processes to receive CPU time
    while (!q0.empty() && q0.front().interactive) {
      Process p = q0.front();
      q0.pop();

      // Execute the process for 1 time slice
      p.remaining--;
      p.waitingTime++; // Increment waiting time for the process

      if (p.boosted) {
        p.boostTime++; // Track the amount of time a boosted process is boosted
      }

      if (p.remaining == 0) {
        p.completion = time + 1;
        processes.push_back(p);
      } else if (p.boosted && p.boostTime >= 10) { // If a boosted process exceeds its allotment, relegate it to a lower priority
        p.boosted = false;
        p.interactive = false;
        q1.push(p);
      } else {
        q0.push(p);
      }
    }

    // Check for CPU intensive processes in the first queue
    if (!q0.empty()) {
      Process p = q0.front();
      q0.pop();

    // Execute the process for the maximum time slice
    p.remaining -= 10;
    p.waitingTime++; // Increment waiting time for the process

    if (p.remaining <= 0) {
      p.completion = time + 10 + p.remaining;
      processes.push_back(p);
    } else {
      p.last_queue = 1;
      q1.push(p);
    }
  }

    // Check for processes in the second queue
    if (!q1.empty()) {
      Process p = q1.front();
      q1.pop();

      // Execute the process for the maximum time slice
      p.remaining -= 20;
      p.waitingTime++; // Increment waiting time for the process

      if (p.remaining <= 0) {
        p.completion = time + 20 + p.remaining;
        processes.push_back(p);
      } 
      else {
        if (p.last_queue == 1) {
          q2.push(p);
        } else {
          p.last_queue = 2;
          q2.push(p);
        }
      }
    }

    // Check for processes in the third queue
    if (!q2.empty()) {
      Process p = q2.front();
      q2.pop();

      // Execute the process for the maximum time slice
      p.remaining -= 40;
      p.waitingTime++; // Increment waiting time for the process

      if (p.remaining <= 0) {
        p.completion = time + 40 + p.remaining;
        processes.push_back(p);
      } else {
        if (p.last_queue == 2) {
          q3.push(p);
        } else {
          p.last_queue = 3;
          q3.push(p);
        }
      }
    }

    // Check for processes in the fourth queue
    if (!q3.empty()) {
      Process p = q3.front();
      q3.pop();

      // Execute the process for the maximum time slice
      p.remaining -= 80;
      p.waitingTime++; // Increment waiting time for the process

      if (p.remaining <= 0) {
        p.completion = time + 80 + p.remaining;
        processes.push_back(p);
      } else {
        // Determine boost factor based on number of processes in fourth queue
        int num_processes = q3.size();
        double boost_factor = 1.0;
        if (num_processes > 10) {
          boost_factor = 1.5;
        } else if (num_processes > 5) {
          boost_factor = 1.2;
        }

      // Check if process is CPU intensive and should be boosted
      if (!p.interactive) {
        p.last_queue = 0;
        p.interactive = true;
        p.boosted = true;
        p.boostTime = 0;

        // Adjust time slice based on boost factor
        int time_slice = static_cast<int>(10 * boost_factor);
        p.remaining += time_slice;

        q0.push(p);
      } else {
        q3.push(p);
      }
    }
    }

    // Increment the time
    time++;
    agingTimer++;

    // Check if it's time to perform aging
    if (agingTimer >= agingThreshold) {
      agingTimer = 0;

      // Aging mechanism: Promote processes in lower-priority queues if they have been waiting for a long time
      if (!q1.empty()) {
        queue1 tempQ1; // Temporary queue to hold promoted processes

        // Iterate over the processes in q1
        while (!q1.empty()) {
          Process p = q1.front();
          q1.pop();

          // Check waiting time threshold for promotion
          if (p.waitingTime >= 100) {  // Arbitrary value for promoting to a higher queue
            p.last_queue = 0;
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

      // Execute the process for the maximum time slice
      p.remaining -= 80;
      if (p.remaining <= 0) {
        p.completion = time + 80 + p.remaining;
        processes.push_back(p);
      } else {
        // Determine boost factor based on number of processes in fourth queue
        int num_processes = q3.size();
        double boost_factor = 1.0;
        if (num_processes > 10) {
          boost_factor = 1.5;
        } else if (num_processes > 5) {
          boost_factor = 1.2;
        }

        // Check if process is CPU intensive and should be boosted
        if (!p.interactive) {
          p.last_queue = 0;
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
    time++;
  }

  return processes;
}
