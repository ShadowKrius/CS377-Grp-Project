#ifndef SCHEDULERS_H
#define SCHEDULERS_H

#include "process.h"
#include <list>

// Utility functions for displaying workloads and processes
pqueue_arrival read_workload(string filename);
void show_workload(pqueue_arrival workload);
void show_processes(list<Process> processes);

// Scheduler implementations
list<Process> stcf(pqueue_arrival workload);
list<Process> rr(pqueue_arrival workload);
list<Process> cfs(pqueue_arrival workload);

// Helper function for CFS
void updateVRuntime(Process& process, int time_slice);

#ifdef DEBUGMODE
#define debug(msg) \
    std::cout <<"[" << __FILE__ << ":" << __LINE__ << "] " << msg << std::endl;
#else
#define debug(msg)
#endif

#endif // SCHEDULERS_H