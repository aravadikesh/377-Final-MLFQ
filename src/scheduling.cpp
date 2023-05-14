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
