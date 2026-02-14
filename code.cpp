#include <iostream>;
#include <string>;

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
    bool hasStarted;
};

struct GanttBlock {
    int startTime;
    int processId;
    int duration;
    bool isComplete;
};

int currentTest;

void printResults(string algo, GanttBlock* gantt, int ganttSize, Process* procs, int numProcs, int totalTime){
    cout << currentTest << " " << algo << endl;

    cout << "Total time elapsed: " << totalTime << "ns" << endl;
    cout << "Total CPU burst time: " << totalBurst << "ns" << endl;
    cout << "CPU Utilization: " << cpuUtil << "%" << endl;
    cout << "Throughput: " << throughput << " processes/ns" << endl;

    cout << "Waiting times:" << endl;

    cout << "Average waiting time: " << avgWait << "ns" << endl;

    cout << "Turnaround times:" << endl;

    cout << "Response times:" << endl;

    cout << "Average response time: " << avgResp << "ns" << endl;
}

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

int rr(int q, int xlines)
{
    for (int x = 0; x < xlines; x++)
    {
        int arrival, burst, nice;
        cin >> arrival >> burst >> nice;
    }
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

