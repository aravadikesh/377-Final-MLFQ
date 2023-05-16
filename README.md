# Multi-Level Feedback Queue (MLFQ) Scheduling Algorithm

This code implements the MLFQ scheduling algorithm, which schedules processes based on their priority level. The algorithm assigns lower priorities to CPU-bound processes and higher priorities to interactive processes. The algorithm uses four priority queues (`q0`, `q1`, `q2`, and `q3`) to store the processes, with `q0` being the highest priority queue and `q3` being the lowest priority queue.

## Process Structure

The `Process` structure represents a process and contains the following properties:

- `arrival`: The arrival time of the process (known from the text file).
- `first_run`: The time when the process is first scheduled to run (filled during the scheduling algorithm).
- `duration`: The total duration of the process (known from the text file).
- `completion`: The time when the process completes its execution (filled during the scheduling algorithm).
- `remaining`: The remaining duration of the process for the current time slice (relevant for MLFQ).
- `last_queue`: The last queue the process was in (initially 0).
- `interactive`: A boolean value indicating if the process is interactive (`true`) or CPU intensive (`false`) (known from the text file).
- `boosted`: A boolean value indicating if the process has been boosted (initially `false`).
- `boostTime`: The time duration for which the process has been boosted.
- `waitingTime`: The time spent by the process in a queue.
- `priority`: The importance of the process (known from the text file).

## Function Signatures

The MLFQ function has the following function signatures:

```cpp
list<Process> mlfq(pqueue_arrival workload);
```

The function takes a priority queue of processes (`workload`) as input, where processes are sorted based on their arrival time. It returns a list of processes representing the order in which they complete their execution.

## Queues and Priorities

The MLFQ algorithm uses four different queues to schedule processes with different priorities:

- `queue0`: First queue with the highest priority.
- `queue1`: Second queue with lower priority compared to queue0.
- `queue2`: Third queue with lower priority compared to queue1.
- `queue3`: Fourth queue with the lowest priority.

## Algorithm Description

The `mlfq` function follows the following steps to schedule processes using the MLFQ algorithm:

1. Create the four queues: `queue0`, `queue1`, `queue2`, and `queue3`.
2. Initialize an empty list of processes: `processes`.
3. Move the processes from the priority queue (`workload`) to their respective queues based on their priority.
4. Start the scheduling algorithm loop.
5. Set the time variable to 0 and initialize the aging timer to 0.
6. Check if any interactive processes are in the first queue (`queue0`):
   - While there are interactive processes in `queue0`, execute each process for 1 time slice.
   - If a process is boosted and exceeds its boost time allotment (10 time slices), relegate it to a lower priority by moving it to `queue1`.
   - If a process completes its execution, record its completion time and add it to the `processes` list.
7. Check for CPU intensive processes in `queue0`:
   - If there are CPU intensive processes in `queue0`, execute the process for the maximum time slice of 5.
   - If a process completes its execution, record its completion time and add it to the `processes` list.
   - If a process does not complete its execution, move it to `queue1` and update its `last_queue` property.
8. Check for processes in `queue1`:
   - If there are processes in `queue1`, execute the process for the maximum time slice of 10.
   - If a process completes its execution, record its completion time and add it to the `processes` list.
   - If a process does not complete its execution, move it to `queue2` and update its `last_queue` property.
9. Check for processes in `queue2`:
   - If there are processes in `queue2`, execute the process for the maximum time slice of 20.
   - If a process completes its execution, record its completion time and add it to the `processes` list.
   - If a process does not complete its execution, move it to `queue3` and update its `last_queue` property.
10. Increment the time by 1 and the aging timer by 1.
11. If the aging timer exceeds the aging threshold (10), perform aging:
    - Check for processes in `queue1` and promote processes to a higher queue (`queue0`) if they have been waiting for a long time (15 time slices in this case).
12. Check for processes in `queue3`:
    - If there are processes in `queue3`, execute the process for the maximum time slice of 30.
    - If a process completes its execution, record its completion time and add it to the `processes` list.
    - If a process does not complete its execution, check the number of processes in `queue3` and determine the boost factor based on the number of processes.
    - If the process is CPU intensive and should be boosted, promote it to `queue0` as a boosted process and adjust its time slice based on the boost factor.
13. Increment the time by 1.
14. Repeat steps 6 to 13 until all queues are empty.
15. Return the `processes` list representing the order of process completions.

## Helper Functions

The Codebase also includes the following helper functions:

- `pqueue_arrival read_workload(string filename)`: Reads the workload from a text file and returns a priority queue of processes sorted based on their arrival time.
- `void show_workload(pqueue_arrival workload)`: Displays the contents of the workload (priority queue) on the console.
- `void show_processes(list<Process> processes)`: Displays the list of completed processes on the console.
- `float avg_turnaround(list<Process> processes)`: Calculates and returns the average turnaround time for a list of completed processes.
- `float avg_response(list<Process> processes)`: Calculates and returns the average response time for a list of completed processes.
- `void show_metrics(list<Process> processes)`: Displays the average turnaround time and average response time for a list of completed processes.

