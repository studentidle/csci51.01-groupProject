#include <iostream>
#include <string>
#include <algorithm>
#include <queue>

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

// void printResults(string algo, GanttBlock* gantt, int ganttSize, Process* procs, int numProcs, int totalTime)
// {
//     cout << currentTest << " " << algo << endl;

//     cout << "Total time elapsed: " << totalTime << "ns" << endl;
//     cout << "Total CPU burst time: " << totalBurst << "ns" << endl;
//     cout << "CPU Utilization: " << cpuUtil << "%" << endl;
//     cout << "Throughput: " << throughput << " processes/ns" << endl;

//     cout << "Waiting times:" << endl;

//     cout << "Average waiting time: " << avgWait << "ns" << endl;

//     cout << "Turnaround times:" << endl;

//     cout << "Response times:" << endl;

//     cout << "Average response time: " << avgResp << "ns" << endl;
// }


int fcfs(int xlines)
{
    for (int x = 0; x < xlines; x++)
    {
        int arrival, burst, nice;
        cin >> arrival >> burst >> nice;
    }
    return 0;
}

int sjf(int xlines)
{
    for (int x = 0; x < xlines; x++)
    {
        int arrival, burst, nice;
        cin >> arrival >> burst >> nice;
    }
    return 0;
}

int srtf(int xlines)
{
    for (int x = 0; x < xlines; x++)
    {
        int arrival, burst, nice;
        cin >> arrival >> burst >> nice;
    }
    return 0;
}

int p(int xlines)
{
    for (int x = 0; x < xlines; x++)
    {
        int arrival, burst, nice;
        cin >> arrival >> burst >> nice;
    }
    return 0;
}

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

int rr(int q, size_t xlines)
{
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
    cout << "CPU Utilization: " << (total_cpu_burst_time / total_time * 100) << "%" << endl;
    cout << "Throughput: " << ( static_cast<float>(total_processes) / total_time) << "processes/ns" << endl;

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
        cout << " Process " << i + 1 << " " << waiting_times[i] << "ns" << endl;
        wt_total += waiting_times[i];
    }
    cout << "Average waiting time: " << static_cast<float>(wt_total) / xlines << "ns" << endl;
    
    cout << "Turnaround times:" << endl;
    for (int i = 0; i < xlines; i++)
    {
        cout << " Process " << i + 1 << " " << turnaround_times[i] << "ns" << endl;
        tat_total += turnaround_times[i];
    }
    cout << "Average waiting time: " << static_cast<float>(tat_total) / xlines << "ns" << endl;

    cout << "Response times:" << endl;
    for (int i = 0; i < xlines; i++)
    {
        cout << " Process " << i + 1 << " " <<response_times[i] << "ns" << endl;
        rt_total += response_times[i];
    }
    cout << "Average response time: " << static_cast<float>(rt_total) / xlines << "ns" << endl;

    

    return 0;
}

int main(){
    // cout << "Hello, World!" << endl;
    int tests;
    cin >> tests;
    for (int t = 0; t < tests; t++)
    {
        int xlines, q;
        string process;
        cin >> xlines >> process;
        if (process == "FCFS")
        {
            fcfs(xlines);
        }
        else if (process == "SJF")
        {
            sjf(xlines);
        }
        else if (process == "SRTF")
        {
            srtf(xlines);
        }
        else if (process == "P")
        {
            p(xlines);
        }
        else
        if (process == "RR")
        {
            cin >> q;
            rr(q, xlines);
        }
    }
    return 0;
}

