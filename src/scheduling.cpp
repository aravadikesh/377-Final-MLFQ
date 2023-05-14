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
  // TODO: Arav
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
  // TODO: Arav
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
  // TODO: Nihar
  // note sjf is not preemptive - so once you run one just run it to completion

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

    // either @workload is empty - all its unrun procs pushed to @procsArrived
    // or @procsArrived is empty - no unrun procs from workload have arrived at or before @current_time
    // NOTE: that proc is the last unpopped proc with @arrival > @currrent_time ==> IGNORE THIS VALUE

    // @procsArrived empty => no proc arrived yet
    // update current time to next proc arrival in workload and continue to next iter
    if (procsArrived.empty())
    {
      current_time = workload.top().arrival;
      continue;
    }

    // pop top (shortest duration) proc from @procsArrived and run it
    // update @Procedure.first_run, @Procedure.completion, @current_time
    proc = procsArrived.top();
    procsArrived.pop();

    proc.first_run = current_time;
    proc.completion = current_time + proc.duration;
    current_time = proc.completion;

    // flush @procsArrived by putting all its proc back into @workload
    // gets rid of case when workload is empty but temp still has proc to execute
    while(!procsArrived.empty())
    {
      workload.push(procsArrived.top());
      procsArrived.pop();
    }

    // add the proc just run to @complete
    complete.push_back(proc);
  } 

  return complete;
}

list<Process> stcf(pqueue_arrival workload) {
  // TODO: Manasa
  list <Process> complete; 
  pqueue_duration procsArrived; 
  int current_time = 0;

  while(!workload.empty()) {
    Process proc; 

    while(true){    
      if(workload.empty()){
        break;
      }
      // get all procs arrived on or before @current_time - duration ordered into @procsArrived
      proc = workload.top(); 
      if(proc.arrival <= current_time){                             
        procsArrived.push(proc);
        workload.pop(); 
      }
      else{
        break;
      }
    }

    // If no running procs arrived <= @current time skip to next proc arrival
    if(procsArrived.empty()){
      current_time = workload.top().arrival;
      continue; 
    }

    // run shortest duration procedure for 1 sec - shortest duration = shortest time to completion
    // proc.duration will be decremented to ensure this
    proc = procsArrived.top();
    procsArrived.pop();

    // if proc being run for first time init first_run
    if(proc.first_run == -1){
      proc.first_run = current_time;
    }

    // if during execution of current proc (@proc) another arrives
    // run @proc till @workload.top() arrives i.e. for top.arrival - cur_time
    // then update all vars - current_time, duration, completion
    // push back to procsArrived
    if(!workload.empty() && workload.top().arrival < (current_time + proc.duration)){
      
      proc.duration = proc.duration - (workload.top().arrival - current_time);
      current_time = workload.top().arrival;

      // proc finished executing can be put into @complete
      if(proc.duration == 0){
        proc.completion = current_time;
        complete.push_back(proc);
      }
      // proc not fully finished. Push back to procsArrived which will in end go back into workload
      else{
        procsArrived.push(proc);
      }
    }
    // no procs arrive during execution of @proc
    // run @proc to completion
    else{
      proc.completion = current_time + proc.duration;
      current_time = proc.completion;
      complete.push_back(proc);
    }

    // put back all procs into workload
    while(!procsArrived.empty()){
      workload.push(procsArrived.top());
      procsArrived.pop();
    }
  }

  return complete;
}

list<Process> rr(pqueue_arrival workload) {
  // TODO: Manasa
	list <Process> complete; 
	//pqueue_arrival procsArrived; // procs arrived so far
  queue<Process> procsInExec;
	int current_time = 0;

  // exit only if both @workload and @procsInExec are empty
  // i.e. keep executing if at least one is not empty
	while(!(workload.empty() && procsInExec.empty())) 
	{
		Process proc; 

		while(true){	

			if(workload.empty()){
        // exit loop if workload empty - no new procs to execute except those already in @procsInExec
				break;
			}

      // push all procs arrived in 1 sec of execution so far into @procsInExec
			proc = workload.top(); 
			if(proc.arrival <= current_time){ 				
				procsInExec.push(proc);
				workload.pop();
			}
			else{
        // exit loop if all newly arrived procs added to queue 
				break;
			}

		}

    // if no procs arrived so far then move to next proc that arrives
    // ASSUMPTION: workload is not empty: TRUE OR FALSE
		if(procsInExec.empty()){
			current_time = workload.top().arrival;
			continue; 
		}

		proc = procsInExec.front();
		procsInExec.pop();

    // if proc being run for the first time initialize first_run
		if(proc.first_run == -1){
			proc.first_run = current_time;
		}

    // run the process for 1 sec and update current_time
		proc.duration -= 1;
    current_time += 1; 
    // if proc finished executing update completion time and push it to @complete
    // else push it back to procs Arrived to be scheduled for running again
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
  // TODO: Nihar
  // turnaround time = completion - arrival

  float turnaroundSum = 0;
  float count = 0;

  // iterate through each processes
  // add its turnaround time to @turnaroundSum
  // increment @count for counting # proc
  for (auto procItr = processes.begin(); procItr != processes.end(); ++procItr)
  {
    turnaroundSum += procItr->completion - procItr->arrival;
    count += 1;
  }

  return (float) (turnaroundSum/count);
}

float avg_response(list<Process> processes) {
  // TODO: Manasa
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
