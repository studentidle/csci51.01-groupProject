# simple python implementation of rr to make sure my
# algorithm works properly.

from collections import deque;

def rr(q, xlines):
    job_queue = []
    for x in range(xlines):
        arrival, burst, nice = map(int, input().split())
        # drop nice since we don't need it, I think,
        # replace with process order instead [1,x]
        job_queue.append([arrival, burst, x + 1])
    # sort into ascending by process order first
    job_queue.sort(key = lambda x: x[2])
    # and then by arrivals
    # thankfully, python sorts are safe
    job_queue.sort(key = lambda x: x[0])
    print(job_queue)

    job_queue = deque(job_queue)

    total_time = 0
    relative_time = 0 

    ready_queue = deque()
    past_queue = deque()

    # tracking stuff
    total_cpu_burst_time = 0
    total_processes = len(job_queue);
    #

    p = None
    while len(ready_queue) > 0 or len(job_queue) > 0 or len(past_queue) > 0 or p != None:
        # if job arrives, add it to ready queue.
        while len(job_queue) > 0:
            if job_queue[0][0] == total_time:
                ready_queue.append(job_queue.popleft())
            else:
                break
        
        # if processor is idle

        if p == None:
            # if p in ready
            if len(ready_queue) > 0:
                # work on p
                p = ready_queue.popleft()
            # if ready empty
            else:
                # past stuff queue not empty
                if len(past_queue) > 0:
                    # work on preemptively terminated processes
                    p = past_queue.popleft()

        total_time += 1
        relative_time += 1

        if p != None:
            p[1] = p[1] - 1
            total_cpu_burst_time += 1
            # if current process has ended
            if p[1] == 0:
                print(total_time - relative_time, p[2], str(relative_time) + "X")
                relative_time = 0
                p = None
            # if time slice for the process has ended
            elif relative_time == q:
                print(total_time - relative_time, p[2], q)
                
                past_queue.append(p)
                p = None
                relative_time = 0
        # print(total_time, p, ready_queue, past_queue, job_queue)

    print("Total time elapsed:", str(total_time) + "ns")
    print("Total CPU burst time:", str(total_cpu_burst_time) + "ns")
    print("CPU Utilization:", str(total_cpu_burst_time / total_time * 100) + "%")
    print("Throughput:", (total_processes / total_time), "processes/ns")

rr(25, 4)
        

    