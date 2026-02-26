#include <iostream>
#include <string>
#include <algorithm>
#include <queue>
#include <iomanip>

using namespace std;

struct Process {
    int id;                 // Process number (1, 2, 3, etc.)
    int arrivalTime;        // When process arrives
    int burstTime;          // How long it needs to run
    int remainingTime;      // How much time is left
    int priority;           // Low, Medium, High Priority
    
    int waiting;
    int turnaround;
    int response;
    int completionTime;
    bool hasStarted;
};

struct GanttBlock {
    int startTime;
    int processId;
    int duration;
    bool isComplete;
};

int currentTest;

bool ArrivalSorter(const Process &a, const Process &b) 
{
    if (a.arrivalTime != b.arrivalTime)
    {
        return a.arrivalTime < b.arrivalTime;
    }
    return a.id < b.id;
}

bool OrderSorter(const Process &a, const Process &b)
{
    return a.id < b.id;
}

queue<Process> vectorToQueue(vector<Process>& v) 
{
    queue<Process> q;
    for (const Process& p : v)
    {
        q.push(p);
    }
    
    return q;
}

int fcfs(int xlines, int currentTest)
{
    // read all the processes in
    vector<Process> job_list(xlines);
    int total_cpu_burst = 0;

    for (int x = 0; x < xlines; x++)
    {
        int arrival, burst, nice;
        cin >> arrival >> burst >> nice;
        job_list[x].id = x + 1;
        job_list[x].arrivalTime = arrival;
        job_list[x].burstTime = burst;
        job_list[x].remainingTime = nice;
        job_list[x].priority = nice;

        total_cpu_burst += burst;
    }

    //Sort by arrival time so we can let them in order first come first out
    sort(job_list.begin(), job_list.end(), ArrivalSorter);

    int total_time = 0;
    vector<GanttBlock> gantt;
    vector<Process> completed = job_list;

    //Process them sequentially
    for (int i = 0; i < xlines; i++)
    {
        //If the CPU is idle, skip ahead to when the next process arrives
        if (total_time < completed[i].arrivalTime)
        {
            total_time = completed[i].arrivalTime;
        }

        //Mark the first time the process gets the CPU as its response time
        completed[i].response = total_time;

        //Open and close a gantt block for this process
        GanttBlock block;
        block.startTime = total_time;
        block.processId = completed[i].id;
        block.duration = completed[i].burstTime;
        block.isComplete = true;
        gantt.push_back(block);

        //Fast forward time by the burst amount
        total_time += completed[i].burstTime;

        //Record the completion time for stats later
        completed[i].completionTime = total_time;
        completed[i].turnaround = completed[i].completionTime - completed[i].arrivalTime;
        completed[i].waiting = completed[i].turnaround - completed[i].burstTime;
    }

    //print gantt chart
    cout << currentTest << " FCFS" << endl;
    for (int i = 0; i < gantt.size(); i++)
    {
        cout << gantt[i].startTime << " " << gantt[i].processId << " " << gantt[i].duration << "X" << endl;
    }

    //print stats
    cout << "Total time elapsed: " << total_time << "ns" << endl;
    cout << "Total CPU burst time: " << total_cpu_burst << "ns" << endl;
    cout << "CPU Utilization: " << (total_cpu_burst * 100 / total_time) << "%" << endl;
    cout << "Throughput: " << setprecision(15) << (static_cast<double>(xlines) / total_time) << " processes/ns" << endl;

    //restore original order so we can print stats in process number order
    sort(completed.begin(), completed.end(), OrderSorter);

    int wt_total = 0, tat_total = 0, rt_total = 0;

    cout << "Waiting times:" << endl;
    for (int i = 0; i < xlines; i++)
    {
        cout << " Process " << completed[i].id << ": " << completed[i].waiting << "ns" << endl;
        wt_total += completed[i].waiting;
    }
    cout << "Average waiting time: " << static_cast<double>(wt_total) / xlines << "ns" << endl;
    
    cout << "Turnaround times:" << endl;
    for (int i = 0; i < xlines; i++)
    {
        cout << " Process " << completed[i].id << ": " << completed[i].turnaround << "ns" << endl;
        tat_total += completed[i].turnaround;
    }
    cout << "Average turnaround time: " << static_cast<double>(tat_total) / xlines << "ns" << endl;

    cout << "Response times:" << endl;
    for (int i = 0; i < xlines; i++)
    {
        int rt = completed[i].response - completed[i].arrivalTime;
        cout << " Process " << completed[i].id << ": " << rt << "ns" << endl;
        rt_total += rt;
    }
    cout << "Average response time: " << static_cast<double>(rt_total) / xlines << "ns" << endl;
     
    return 0;
}

int sjf(int xlines, int currentTest)
{
    // read all the processes in
    vector<Process> job_list(xlines);
    for (int x = 0; x < xlines; x++)
    {
        int arrival, burst, nice;
        cin >> arrival >> burst >> nice;

        job_list[x].id            = x + 1;
        job_list[x].arrivalTime   = arrival;
        job_list[x].burstTime     = burst;
        job_list[x].remainingTime = burst;
        job_list[x].priority      = nice;
        job_list[x].response      = -1;   // -1 just means it hasn't run yet
        job_list[x].hasStarted    = false;
    }

    // sort by arrival time so we can let them in one by one
    sort(job_list.begin(), job_list.end(), ArrivalSorter);

    int total_time      = 0;
    int total_cpu_burst = 0;

    vector<Process> ready;      // processes that showed up and are waiting
    vector<Process> completed;  // processes that finished

    queue<Process> job_queue = vectorToQueue(job_list);

    vector<GanttBlock> gantt;

    bool working = false;  // is the cpu busy right now?
    Process current;       // whatever is currently running
    current.id = -1;

    // simulate one tick at a time
    while (!job_queue.empty() || !ready.empty() || working)
    {
        // let in any processes that have arrived by now
        while (!job_queue.empty() && job_queue.front().arrivalTime <= total_time)
        {
            ready.push_back(job_queue.front());
            job_queue.pop();
        }

        // cpu is free so pick something to run
        if (!working)
        {
            if (ready.empty())
            {
                // nothing to run yet, idle
                total_time++;
                continue;
            }

            // find whoever has the shortest burst time
            int best_index = 0;
            for (int i = 1; i < ready.size(); i++)
            {
                // shorter burst? take it
                if (ready[i].burstTime < ready[best_index].burstTime)
                {
                    best_index = i;
                }

                // same burst time? take the one that arrived earlier
                else if (ready[i].burstTime == ready[best_index].burstTime)
                {
                    if (ready[i].arrivalTime < ready[best_index].arrivalTime)
                        best_index = i;
                    
                        // still tied? just go with the lower id
                    else if (ready[i].arrivalTime == ready[best_index].arrivalTime &&
                             ready[i].id < ready[best_index].id)
                        best_index = i;
                }
            }

            // pull it out of the ready list and start running it
            current = ready[best_index];
            ready.erase(ready.begin() + best_index);

            current.response = total_time;  // first time on cpu = response time
            working = true;

            // open a new gantt block for this process
            GanttBlock block;
            block.startTime  = total_time;
            block.processId  = current.id;
            block.duration   = 0;
            block.isComplete = false;
            gantt.push_back(block);
        }

        // run for one tick
        total_time++;
        total_cpu_burst++;
        current.remainingTime--;
        gantt.back().duration++;

        // check if it just finished
        if (current.remainingTime == 0)
        {
            current.completionTime   = total_time;
            gantt.back().isComplete  = true;
            completed.push_back(current);
            working = false;
            // no preemption here, just finish naturally
        }
    }

    // print gantt chart
    cout << currentTest << " SJF" << endl;
    for (int i = 0; i < gantt.size(); i++)
    {
        cout << gantt[i].startTime << " " << gantt[i].processId << " " << gantt[i].duration;
        
        if (gantt[i].isComplete) cout << "X";
        
        cout << endl;
    }

    // print stats
    cout << "Total time elapsed: " << total_time << "ns" << endl;
    cout << "Total CPU burst time: " << total_cpu_burst << "ns" << endl;
    cout << "CPU Utilization: " << (total_cpu_burst * 100 / total_time) << "%" << endl;
    cout << "Throughput: " << setprecision(15) << (static_cast<double>(xlines) / total_time) << " processes/ns" << endl;

    sort(completed.begin(), completed.end(), OrderSorter);

    int wt_total = 0, tat_total = 0, rt_total = 0;

    cout << "Waiting times:" << endl;
        for (int i = 0; i < xlines; i++)
        {
            int tat = completed[i].completionTime - completed[i].arrivalTime;
            int wt  = tat - completed[i].burstTime;
            
            cout << " Process " << completed[i].id << ": " << wt << "ns" << endl;
            wt_total += wt;
        }
    cout << "Average waiting time: " << static_cast<double>(wt_total) / xlines << "ns" << endl;

    cout << "Turnaround times:" << endl;
        for (int i = 0; i < xlines; i++)
        {
            int tat = completed[i].completionTime - completed[i].arrivalTime;

            cout << " Process " << completed[i].id << ": " << tat << "ns" << endl;
            tat_total += tat;
        }
    cout << "Average turnaround time: " << static_cast<double>(tat_total) / xlines << "ns" << endl;

    cout << "Response times:" << endl;
        for (int i = 0; i < xlines; i++)
        {
            int rt = completed[i].response - completed[i].arrivalTime;

            cout << " Process " << completed[i].id << ": " << rt << "ns" << endl;
            rt_total += rt;
        }
    cout << "Average response time: " << static_cast<double>(rt_total) / xlines << "ns" << endl;

    return 0;
}

int srtf(int xlines, int currentTest)
{
    // read all the processes in
    vector<Process> job_list(xlines);
    for (int x = 0; x < xlines; x++)
    {
        int arrival, burst, nice;
        cin >> arrival >> burst >> nice;

        job_list[x].id            = x + 1;
        job_list[x].arrivalTime   = arrival;
        job_list[x].burstTime     = burst;
        job_list[x].remainingTime = burst;
        job_list[x].priority      = nice;
        job_list[x].response      = -1;   // -1 just means it hasn't
        job_list[x].hasStarted    = false;
    }
    
    // sort by arrival time so we can let them in one by one
    sort(job_list.begin(), job_list.end(), ArrivalSorter);

    int total_time      = 0;
    int total_cpu_burst = 0;

    vector<Process> ready;      // processes that showed up and are waiting
    vector<Process> completed;  // processes that finished
    queue<Process> job_queue = vectorToQueue(job_list);

    vector<GanttBlock> gantt;
    int gantt_start = 0;
    int current_pid = -1;
    int gantt_dur   = 0;

    Process current;
    current.id = -1;
    bool working = false;  // is the cpu busy right now?

    // simulate one tick at a time
    while (!job_queue.empty() || !ready.empty() || working)
    {
        // let in any processes that have arrived by now
        while (!job_queue.empty() && job_queue.front().arrivalTime <= total_time)
        {
            ready.push_back(job_queue.front());
            job_queue.pop();
        }

        // find whoever has the shortest remaining time
        int best_index = -1;
        for (int i = 0; i < ready.size(); i++)
        {
            if (best_index == -1)
            {
                best_index = i;
                continue;
            }

            // shorter remaining time? take it
            if (ready[i].remainingTime < ready[best_index].remainingTime)
            {
                best_index = i;
            }

            // same remaining time? take the one that arrived earlier
            else if (ready[i].remainingTime == ready[best_index].remainingTime)
            {
                if (ready[i].arrivalTime < ready[best_index].arrivalTime)
                    best_index = i;
                
                // still tied? just go with the lower id
                else if (ready[i].arrivalTime == ready[best_index].arrivalTime &&
                         ready[i].id < ready[best_index].id)
                    best_index = i;
            }
        }

        // check if we need to swap out what's running
        if (best_index != -1)
        {
            bool should_switch = false;

            if (!working)
            {
                // cpu is free, just start the best one
                should_switch = true;
            }
            else
            {
                // cpu is busy, check if the new candidate beats the current process
                if (ready[best_index].remainingTime < current.remainingTime)
                    should_switch = true;

                else if (ready[best_index].remainingTime == current.remainingTime)
                {
                    if (ready[best_index].arrivalTime < current.arrivalTime)
                        should_switch = true;

                    else if (ready[best_index].arrivalTime == current.arrivalTime &&
                             ready[best_index].id < current.id)
                        should_switch = true;
                }
            }

            if (should_switch)
            {
                // save the gantt block for whatever was running before
                if (gantt_dur > 0 && current_pid != -1)
                    gantt.push_back({gantt_start, current_pid, gantt_dur, false});

                // shove current process back into ready so it can run later
                if (working)
                    ready.push_back(current);

                // re-find best since we just added current back into ready
                best_index = -1;
                for (int i = 0; i < ready.size(); i++)
                {
                    if (best_index == -1) { best_index = i; continue; }
                    
                    if (ready[i].remainingTime < ready[best_index].remainingTime)
                        best_index = i;
                    
                    else if (ready[i].remainingTime == ready[best_index].remainingTime)
                    {
                        if (ready[i].arrivalTime < ready[best_index].arrivalTime)
                            best_index = i;

                        else if (ready[i].arrivalTime == ready[best_index].arrivalTime &&
                                 ready[i].id < ready[best_index].id)
                            best_index = i;
                    }
                }

                current = ready[best_index];
                ready.erase(ready.begin() + best_index);

                // log the first time it gets the cpu
                if (current.response == -1)
                    current.response = total_time;

                // start a new gantt block
                gantt_start = total_time;
                current_pid = current.id;
                gantt_dur   = 0;
                working     = true;
            }
        }

        // nothing to run, skip ahead
        if (!working)
        {
            total_time++;
            continue;
        }

        // run for one tick
        total_time++;
        gantt_dur++;
        total_cpu_burst++;
        current.remainingTime--;

        // check if it just finished
        if (current.remainingTime == 0)
        {
            current.completionTime = total_time;
            gantt.push_back({gantt_start, current_pid, gantt_dur, true});

            gantt_dur   = 0;
            current_pid = -1;

            completed.push_back(current);
            working = false;
        }
    }

    // print gantt chart
    cout << currentTest << " SRTF" << endl;
        for (int i = 0; i < gantt.size(); i++)
        {
            cout << gantt[i].startTime << " " << gantt[i].processId << " " << gantt[i].duration;
            if (gantt[i].isComplete) cout << "X";
            cout << endl;
        }
    
    // print stats
    cout << "Total time elapsed: " << total_time << "ns" << endl;
    cout << "Total CPU burst time: " << total_cpu_burst << "ns" << endl;
    cout << "CPU Utilization: " << (total_cpu_burst * 100 / total_time) << "%" << endl;
    cout << "Throughput: " << setprecision(15) << (static_cast<double>(xlines) / total_time) << " processes/ns" << endl;

    sort(completed.begin(), completed.end(), OrderSorter);
    int wt_total = 0, tat_total = 0, rt_total = 0;

    cout << "Waiting times:" << endl;
        for (int i = 0; i < xlines; i++)
        {
            int tat = completed[i].completionTime - completed[i].arrivalTime;
            int wt  = tat - completed[i].burstTime;

            cout << " Process " << completed[i].id << ": " << wt << "ns" << endl;
            wt_total += wt;
        }

    cout << "Average waiting time: " << static_cast<double>(wt_total) / xlines << "ns" << endl;
    cout << "Turnaround times:" << endl;
        for (int i = 0; i < xlines; i++)
        {
            int tat = completed[i].completionTime - completed[i].arrivalTime;

            cout << " Process " << completed[i].id << ": " << tat << "ns" << endl;
            tat_total += tat;
        }
    cout << "Average turnaround time: " << static_cast<double>(tat_total) / xlines << "ns" << endl;
    cout << "Response times:" << endl;
        for (int i = 0; i < xlines; i++)
        {
            int rt = completed[i].response - completed[i].arrivalTime;

            cout << " Process " << completed[i].id << ": " << rt << "ns" << endl;
            rt_total += rt;
        }
    cout << "Average response time: " << static_cast<double>(rt_total) / xlines << "ns" << endl;
    return 0;
}

int p(int xlines, int currentTest)
{
    vector<Process> job_list(xlines);
    for (int x = 0; x < xlines; x++)
    {
        int arrival, burst, nice;
        cin >> arrival >> burst >> nice;

        job_list[x].id            = x + 1;
        job_list[x].arrivalTime   = arrival;
        job_list[x].burstTime     = burst;
        job_list[x].remainingTime = burst;
        job_list[x].priority      = nice;   // lower nice = higher priority
        job_list[x].response      = -1;     // -1 means hasn't run yet
        job_list[x].hasStarted    = false;
    }

    // sort by arrival time
    sort(job_list.begin(), job_list.end(), ArrivalSorter);

    int total_time      = 0;
    int total_cpu_burst = 0;

    vector<Process> ready;
    vector<Process> completed;

    queue<Process> job_queue = vectorToQueue(job_list);

    vector<GanttBlock> gantt;
    int gantt_start = 0;
    int current_pid = -1;
    int gantt_dur   = 0;

    Process current;
    current.id = -1;
    bool working = false;

    // simulate one tick at a time
    while (!job_queue.empty() || !ready.empty() || working)
    {
        // let in any processes that have arrived by now
        while (!job_queue.empty() && job_queue.front().arrivalTime <= total_time)
        {
            ready.push_back(job_queue.front());
            job_queue.pop();
        }

        // find whoever has the highest priority (lowest nice value) in the ready list
        int best_index = -1;
        for (int i = 0; i < ready.size(); i++)
        {
            if (best_index == -1)
            {
                best_index = i;
                continue;
            }

            // lower nice = higher priority, so take it
            if (ready[i].priority < ready[best_index].priority)
            {
                best_index = i;
            }

            // same priority? take whoever arrived earlier
            else if (ready[i].priority == ready[best_index].priority)
            {
                if (ready[i].arrivalTime < ready[best_index].arrivalTime)
                    best_index = i;
                
                // still tied? go with lower id
                else if (ready[i].arrivalTime == ready[best_index].arrivalTime &&
                         ready[i].id < ready[best_index].id)
                    best_index = i;
            }
        }

        // check if we need to swap out what's running
        if (best_index != -1)
        {
            bool should_switch = false;

            if (!working)
            {
                // cpu is free, just start the best one
                should_switch = true;
            }
            else
            {
                // cpu is busy, check if the new candidate beats the current process
                if (ready[best_index].priority < current.priority)
                    should_switch = true;

                else if (ready[best_index].priority == current.priority)
                {
                    if (ready[best_index].arrivalTime < current.arrivalTime)
                        should_switch = true;

                    else if (ready[best_index].arrivalTime == current.arrivalTime &&
                             ready[best_index].id < current.id)
                        should_switch = true;
                }
            }

            if (should_switch)
            {
                // save the gantt block for whatever was running before
                if (gantt_dur > 0 && current_pid != -1)
                    gantt.push_back({gantt_start, current_pid, gantt_dur, false});

                // shove current process back into ready so it can run later
                if (working)
                    ready.push_back(current);

                // re-find best since we just added current back into ready
                best_index = -1;
                for (int i = 0; i < ready.size(); i++)
                {
                    if (best_index == -1) { best_index = i; continue; }
                    
                    if (ready[i].priority < ready[best_index].priority)
                        best_index = i;
                    
                    else if (ready[i].priority == ready[best_index].priority)
                    {
                        if (ready[i].arrivalTime < ready[best_index].arrivalTime)
                            best_index = i;

                        else if (ready[i].arrivalTime == ready[best_index].arrivalTime &&
                                 ready[i].id < ready[best_index].id)
                            best_index = i;
                    }
                }

                current = ready[best_index];
                ready.erase(ready.begin() + best_index);

                // log the first time it gets the cpu
                if (current.response == -1)
                    current.response = total_time;

                // start a new gantt block
                gantt_start = total_time;
                current_pid = current.id;
                gantt_dur   = 0;
                working     = true;
            }
        }

        // nothing to run, skip ahead
        if (!working)
        {
            total_time++;
            continue;
        }

        // run for one tick
        total_time++;
        gantt_dur++;
        total_cpu_burst++;
        current.remainingTime--;

        // check if it just finished
        if (current.remainingTime == 0)
        {
            current.completionTime = total_time;
            gantt.push_back({gantt_start, current_pid, gantt_dur, true});

            gantt_dur   = 0;
            current_pid = -1;

            completed.push_back(current);
            working = false;
        }
    }

    // print gantt chart
    cout << currentTest << " P" << endl;
        for (int i = 0; i < gantt.size(); i++)
        {
            cout << gantt[i].startTime << " " << gantt[i].processId << " " << gantt[i].duration;
            if (gantt[i].isComplete) cout << "X";
            cout << endl;
        }

    // print stats
    cout << "Total time elapsed: " << total_time << "ns" << endl;
    cout << "Total CPU burst time: " << total_cpu_burst << "ns" << endl;
    cout << "CPU Utilization: " << (total_cpu_burst * 100 / total_time) << "%" << endl;
    cout << "Throughput: " << setprecision(15) << (static_cast<double>(xlines) / total_time) << " processes/ns" << endl;

    sort(completed.begin(), completed.end(), OrderSorter);

    int wt_total = 0, tat_total = 0, rt_total = 0;

    cout << "Waiting times:" << endl;
        for (int i = 0; i < xlines; i++)
        {
            int tat = completed[i].completionTime - completed[i].arrivalTime;
            int wt  = tat - completed[i].burstTime;

            cout << " Process " << completed[i].id << ": " << wt << "ns" << endl;
            wt_total += wt;
        }
    cout << "Average waiting time: " << static_cast<double>(wt_total) / xlines << "ns" << endl;

    cout << "Turnaround times:" << endl;
        for (int i = 0; i < xlines; i++)
        {
            int tat = completed[i].completionTime - completed[i].arrivalTime;

            cout << " Process " << completed[i].id << ": " << tat << "ns" << endl;
            tat_total += tat;
        }
    cout << "Average turnaround time: " << static_cast<double>(tat_total) / xlines << "ns" << endl;

    cout << "Response times:" << endl;
        for (int i = 0; i < xlines; i++)
        {
            int rt = completed[i].response - completed[i].arrivalTime;
            
            cout << " Process " << completed[i].id << ": " << rt << "ns" << endl;
            rt_total += rt;
        }
    cout << "Average response time: " << static_cast<double>(rt_total) / xlines << "ns" << endl;

    return 0;
}


int rr(int q, size_t xlines, int t_i)
{
    cout << t_i << " RR" << endl;
    // Process job_list[xlines];
    vector<Process> job_list(xlines);
    for (int x = 0; x < xlines; x++)
    {
        int arrival, burst, nice;
        cin >> arrival >> burst >> nice;

        Process p;
        p.arrivalTime = arrival;
        p.burstTime = burst;
        p.id = x + 1;
        p.remainingTime = burst;
        job_list[x] = p;
    }

    // sort job_list by arrival timme ascending
    sort(job_list.begin(), job_list.end(), ArrivalSorter);
    
    // turn it into a queue
    queue<Process> job_queue = vectorToQueue(job_list);   

    int total_time = 0;
    int relative_time = 0;
    
    queue<Process> ready_queue;
    queue<Process> past_queue;
    
    //tracking stuff;
    int total_cpu_burst_time = 0;
    int total_processes = xlines;
    vector<Process> completed_processes;

    // Holds the current process
    Process p;
    bool working = false;
    // While we still have unfinished processes...
    while (!ready_queue.empty() || !job_queue.empty() || !past_queue.empty() || working) 
    {
        // if job arrives, add it to ready queue
        while (!job_queue.empty())
        {
            Process front = job_queue.front();
            if (job_queue.front().arrivalTime == total_time)
            {
                ready_queue.push(front);
                job_queue.pop();
            }
            else
            {
                break;
            }
        }

        // if processor is idle
        // get something to work on.
        if (!working)
        {
            // if ready queue has something
            if (!ready_queue.empty())
            {
                // work on process
                p = ready_queue.front();
                ready_queue.pop();
                // update process first cpu response
                p.response = total_time;
                working = true;
                relative_time = 0;
            }
            // if nothing in ready
            else
            {
                // work on past stuff
                if (!past_queue.empty())
                {
                    p = past_queue.front();
                    past_queue.pop();
                    working = true;
                    relative_time = 0;
                }
            }
        }

        total_time += 1;
        relative_time += 1;

        if (working)
        {
            p.remainingTime = p.remainingTime - 1;
            total_cpu_burst_time += 1;

            // if current process has ended
            if (p.remainingTime == 0)
            {
                cout << (total_time - relative_time) << " " << p.id << " " << relative_time << "X" << endl;
                relative_time = 0;

                // update completion time
                p.completionTime = total_time;
                completed_processes.push_back(p);
                working = false;
            }
            // Preemptive / time slice for process has ended
            else if (relative_time == q)
            {
                cout << total_time - relative_time << " " << p.id << " " << q << endl;
                past_queue.push(p);
                // p = NULL;
                relative_time = 0;
                working = false;
            }
        }
    }
    cout << "Total time elapsed: " << total_time << "ns" << endl;
    cout << "Total CPU burst time: " << total_cpu_burst_time << "ns" << endl;
    cout << "CPU Utilization: " << (static_cast<double>(total_cpu_burst_time) / total_time * 100) << "%" << endl;
    cout << "Throughput: " << setprecision(15) << ( static_cast<double>(total_processes) / total_time) << " processes/ns" << endl;

    sort(completed_processes.begin(), completed_processes.end(), OrderSorter);
    
    int tat_total = 0;
    int wt_total = 0;
    int rt_total = 0;

    int turnaround_times[xlines];
    int waiting_times[xlines];
    int response_times[xlines];

    for (int i = 0; i < xlines; i++) 
    {
        p = completed_processes[i];
        turnaround_times[i] = p.completionTime - p.arrivalTime;
        waiting_times[i] = turnaround_times[i] - p.burstTime;
        response_times[i] = p.response - p.arrivalTime;
    }

    cout << "Waiting times:" << endl;
    for (int i = 0; i < xlines; i++)
    {
        cout << " Process " << i + 1 << ": " << waiting_times[i] << "ns" << endl;
        wt_total += waiting_times[i];
    }
    cout << "Average waiting time: " << static_cast<double>(wt_total) / xlines << "ns" << endl;
    
    cout << "Turnaround times:" << endl;
    for (int i = 0; i < xlines; i++)
    {
        cout << " Process " << i + 1 << ": " << turnaround_times[i] << "ns" << endl;
        tat_total += turnaround_times[i];
    }
    cout << "Average turnaround time: " << static_cast<double>(tat_total) / xlines << "ns" << endl;

    cout << "Response times:" << endl;
    for (int i = 0; i < xlines; i++)
    {
        cout << " Process " << i + 1 << ": " <<response_times[i] << "ns" << endl;
        rt_total += response_times[i];
    }
    cout << "Average response time: " << static_cast<double>(rt_total) / xlines << "ns" << endl;

    return 0;
}

int main(){
    
    int tests;
    cin >> tests;
    for (int t = 0; t < tests; t++)
    {
        int xlines, q;
        string process;
        cin >> xlines >> process;
        if (process == "FCFS")
        {
            fcfs(xlines, t+1);
        }
        else if (process == "SJF")
        {
            sjf(xlines, t+1);
        }
        else if (process == "SRTF")
        {
            srtf(xlines, t+1);
        }
        else if (process == "P")
        {
            p(xlines, t+1);
        }
        else
        if (process == "RR")
        {
            cin >> q;
            rr(q, xlines, t+1);
        }
    }
    return 0;
}

