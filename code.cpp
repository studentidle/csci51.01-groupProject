#include <iostream>;
using namespace std;

int main()
{
    // cout << "Hello, World!" << endl;
    int tests;
    cin >> tests;
    for (int t = 1; t <= tests; t++)
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
            cout << t;          // 3.a
            rr(q, xlines);
        }
    }
    return 0;
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
    // q is time slice
    int current_time = 0; // in ns
    for (int x = 0; x < xlines; x++)
    {
        int arrival, burst, nice;
        cin >> arrival >> burst >> nice;
    }
    return 0;
}