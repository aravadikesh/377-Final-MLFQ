## Multi-Level Feedback Queue (MLFQ) Scheduling Algorithm

The Multi-Level Feedback Queue (MLFQ) is a CPU scheduling algorithm that manages processes by assigning them to different priority queues based on their characteristics. Each queue has a different priority level, and processes can move between queues based on predefined criteria. MLFQ provides a balance between responsiveness and fairness by allowing short jobs to be executed quickly while still giving long jobs a chance to run.

The MLFQ algorithm can be implemented using multiple queues, where each queue has a different priority level and a different time quantum (time slice). The basic idea is to give higher priority queues smaller time slices to provide responsiveness, while lower priority queues have larger time slices to allow long-running jobs to progress.

There are variations of the MLFQ algorithm with different features such as priority boosting and aging. Here, I have implemented two functions: `mlfqBasic` and `mlfq`, which implement the basic MLFQ algorithm and an enhanced version with priority boosting and aging, respectively. Furthermore, implementations for additional simpler scheduling algorithms have been included for comparison purposes. Respectively, these are : `fifo`, `sjf`, `stcf`, and `rr`.

## Code Structure

This project contains the following important files/folders:

- `include`: This is where the C/C++ header files are kept. 
- `obj`: This folder is used for object files that are generated from the C compilation process.
- `src`: This is the source folder where all the source code lies. 
- `Makefile`: This is a "build" file. This file is used to compile the code.
- `workloads`: This folder contains several text files that serve as the base upon which the scheduling algorithms are run.

## Compiling and Running the code

To compile the code (if necessary), the following command must be run:

```cpp
$ make
```

This project was built and compiled in the edLabs environment and will run ideally in the same environment.

To run the code, you must run the `scheduling_app` executable from the command line.

The syntax to do so is specified in the `main_scheduling` file.

### Running the Code 
   -Enter the following syntax into the command line.
    - `./scheduling.app "required scheduling algorithm" "workloads/required workload file"`
   -Example : to run the basic MLFQ function on workload3 :
   -`./scheduling_app mlfqBasic workloads/workload3.txt`
   -Example : to run the enhanced MLFQ function on workload1 :
   -`./scheduling_app mlfq workloads/workload1.txt`

## Process Structure

The `Process` structure represents a process and contains the following properties:

- `arrival`: The arrival time of the process (known from the text file).
- `first_run`: The time when the process is first scheduled to run (filled during the scheduling algorithm).
- `duration`: The total duration of the process (known from the text file).
- `completion`: The time when the process completes its execution (filled during the scheduling algorithm).
- `remaining`: The remaining duration of the process for the current time slice (relevant for MLFQ).
- `interactive`: A boolean value indicating if the process is interactive (`true`) or CPU intensive (`false`) (known from the text file).
- `boosted`: A boolean value indicating if the process has been boosted (initially `false`).
- `boostTime`: The time duration for which the process has been boosted.
- `waitingTime`: The time spent by the process in a queue.
- `priority`: The importance of the process (known from the text file).


### The `mlfqBasic` Function

The `mlfqBasic` function implements the basic MLFQ algorithm without priority boosting or aging. It takes a workload of processes as input and returns a list of processes after scheduling them.

#### Signature

```cpp
list<Process> mlfqBasic(queue_arrival workload);
```

#### Algorithm

The `mlfqBasic` function follows the following algorithm:

1. Create multiple queues, each representing a priority level, for example, `q0`, `q1`, `q2`, `q3`.
2. Initialize an empty list to store the processes after scheduling.
3. Initialize the current time to 0.
4. While there are still processes in the workload or any of the queues:
   - Move all processes from the workload that have arrived by the current time to their respective queues.
   - Iterate over the queues starting from the highest priority to the lowest:
     - If the current queue is not empty:
       - Extract the first process from the queue.
       - Execute the process for a fixed time slice.
       - Update the process state and remaining time.
       - If the process is completed, mark it as finished and set its completion time.
       - If the process is not completed, move it to the next lower priority queue.
   - Increment the current time.
5. If all queues and the workload are empty, and there are still processes in the `processes` list, sort the list based on the completion time and return it as the final scheduling result.
6. Exit the loop.
7. Return the sorted `processes` list as the final scheduling result.

### The `mlfq` Function

The mlfq function is an enhanced version of the MLFQ algorithm. It takes a priority queue of processes as input and returns a list of processes after scheduling them. The function includes additional features such as priority boosting and aging.

#### Signature

```cpp
list<Process> mlfq(queue_arrival workload);
```

#### Algorithm

The `mlfq` function extends the basic MLFQ algorithm with additional features, including aging and boosting. It follows the following algorithm:

1. Create multiple queues, each representing a priority level, for example, `q0`, `q1`, `q2`, `q3`.
2. Initialize an empty list to store the processes after scheduling.
3. Initialize the current time to 0.
4. While there are still processes in the workload or any of the queues:
   - Move all processes from the workload that have arrived by the current time to their respective queues.
   - Iterate over the queues starting from the highest priority to the lowest:
     - If the current queue is not empty:
       - Extract the first process from the queue.
       - Execute the process for a fixed time slice.
       - Update the process state and remaining time.
       - If the process is completed, mark it as finished and set its completion time.
       - If the process is not completed:
         - Check if the process has reached its time quantum in the current queue.
         - If it has reached the time quantum:
           - Move the process to the next lower priority queue.
           - Reset its remaining time to the time quantum of the new queue.
         - If it has not reached the time quantum:
           - Check if aging needs to be applied:
             - If the process has been waiting in the current queue for a certain threshold time:
               - Increase its priority by moving it to a higher priority queue.
           - Check if boosting needs to be applied:
             - If there are processes waiting in the lower priority queues and no processes in the current queue:
               - Boost the priority of all the processes in the lower priority queues to prevent starvation.
     - Increment the current time.
5. If all queues and the workload are empty, and there are still processes in the `processes` list, sort the list based on the completion time and return it as the final scheduling result.
6. Exit the loop.
7. Return the sorted `processes` list as the final scheduling result.

Let's dive deeper into the steps involved in the algorithm, specifically the aging and boosting processes:

- Aging: The aging process involves increasing the priority of a process that has been waiting in a queue for a certain threshold time. This prevents long-waiting processes from being starved at lower priority levels. In the algorithm, after executing a process, if it is not completed, we check if it has reached the aging threshold in its current queue. If it has, we increase its priority by moving it to a higher priority queue.

- Boosting: The boosting process helps prevent starvation of processes in lower priority queues. If there are processes waiting in lower priority queues and no processes in the current queue, boosting is applied. Boosting involves increasing the priority of all the processes in the lower priority queues, allowing them to have a chance to execute even if they have been waiting for a long time.

These additional features of aging and boosting enhance the fairness and responsiveness of the MLFQ algorithm by dynamically adjusting the priorities of processes based on their waiting times.

By combining the basic MLFQ algorithm with aging and boosting, this extended algorithm provides an effective approach to process scheduling that balances fairness, responsiveness, and prevention of starvation.


#### Example

```cpp
list<Process> processes = mlfqBasic(workload);
```

## Helper Functions

The Codebase also includes the following helper functions:

- `pqueue_arrival read_workload(string filename)`: Reads the workload from a text file and returns a priority queue of processes sorted based on their arrival time.
- `void show_workload(pqueue_arrival workload)`: Displays the contents of the workload (priority queue) on the console.
- `void show_processes(list<Process> processes)`: Displays the list of completed processes on the console.
- `float avg_turnaround(list<Process> processes)`: Calculates and returns the average turnaround time for a list of completed processes.
- `float avg_response(list<Process> processes)`: Calculates and returns the average response time for a list of completed processes.
- `void show_metrics(list<Process> processes)`: Displays the average turnaround time and average response time for a list of completed processes.


