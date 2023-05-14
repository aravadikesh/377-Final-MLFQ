
# Multi-Level Feedback Queue (MLFQ) Scheduling Algorithm

This code implements the MLFQ scheduling algorithm, which schedules processes based on their priority level. The algorithm assigns lower priorities to CPU-bound processes and higher priorities to interactive processes. The algorithm uses four priority queues (`q0`, `q1`, `q2`, and `q3`) to store the processes, with `q0` being the highest priority queue and `q3` being the lowest priority queue.

## Algorithm Logic

1. Initialize the four queues and a list to store the processes' information.
2. Move all the processes from the input priority queue (workload) to `q0`, with their remaining time set to their initial duration and their `last_queue` set to 0.
3. Iterate through the four queues and execute the processes in each queue according to their priority level.
    - Check for interactive processes in `q0` and execute them for one time slice. If a process completes, it is removed from the queue, and its completion time is recorded in the processes list. If a process is not completed, it is moved to the back of `q0`.
    - Check for CPU-bound processes in `q0` and execute them for a maximum of 10 time slices. If a process is completed during this time, it is removed from the queue, and its completion time is recorded in the processes list. If a process is not completed, its `last_queue` is set to 1, and it is moved to `q1`.
    - Check `q1` for processes and execute them for a maximum of 20 time slices. If a process is completed during this time, it is removed from the queue, and its completion time is recorded in the processes list. If a process is not completed, it is moved to `q2`, and its `last_queue` is set to 2 if it was previously in `q1`.
    - Check `q2` and execute processes for a maximum of 40 time slices, following the same logic as before.
    - Check `q3` and execute processes for a maximum of 80 time slices, following the same logic as before.
    - If a process completes in `q3`, it is removed from the queue, and its completion time is recorded in the processes list. If a process is not completed, its `last_queue` is set to 0, its interactive status is set to true, and it is moved to the back of `q0`.
4. Increment the time and repeat the above process until all queues are empty.
5. Return the list of completed processes with their relevant information, including their start time, completion time, and other information stored in the Process object.