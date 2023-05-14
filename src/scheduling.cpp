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

pqueue_arrival read_workload(string filename) {
  pqueue_arrival workload;
  ifstream infile(filename);

  string line;

  while (getline(infile, line)) {
    istringstream iss(line);
    int arrival, duration;
    if (!(iss >> arrival >> duration)) {
      break;
    } 

    Process p = {arrival, -1, duration, 0};
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
    workload.pop();
    q0.push(p);
  }

  // Start the scheduling algorithm
  int time = 0;
  while (!q0.empty() || !q1.empty() || !q2.empty() || !q3.empty()) {
    // Check for interactive processes in the first queue
    while (!q0.empty() && q0.front().interactive) {
      Process p = q0.front();
      q0.pop();

      // Execute the process for 1 time slice
      p.remaining--;
      if (p.remaining == 0) {
        p.completion = time + 1;
        processes.push_back(p);
      }
      else {
        q0.push(p);
      }
    }

    // Check for CPU intensive processes in the first queue
    if (!q0.empty()) {
      Process p = q0.front();
      q0.pop();

      // Execute the process for the maximum time slice
      p.remaining -= 10;
      if (p.remaining <= 0) {
        p.completion = time + 10 + p.remaining;
        processes.push_back(p);
      }
      else {
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
      if (p.remaining <= 0) {
        p.completion = time + 20 + p.remaining;
        processes.push_back(p);
      }
      else {
        if (p.last_queue == 1) {
          q2.push(p);
        }
        else {
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
      if (p.remaining <= 0) {
        p.completion = time + 40 + p.remaining;
        processes.push_back(p);
      }
      else {
        if (p.last_queue == 2) {
          q3.push(p);
        }
        else {
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
      if (p.remaining <= 0) {
        p.completion = time + 80 + p.remaining;
        processes.push_back(p);
      }
      else {
        // Boost the priority of CPU intensive processes
        if (!p.interactive) {
          p.last_queue = 0;
          p.interactive = true;
          q0.push(p);
        }
        else {
          q3.push(p);
        }
      }
    }

    // Increment the time
    time++;
  }

  return processes;
}
