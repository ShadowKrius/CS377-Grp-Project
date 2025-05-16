#ifndef SIMULATION_H
#define SIMULATION_H

#include "process.h"
#include "schedulers.h"
#include "metrics.h"
#include <map>
#include <string>

class Simulation {
private:
    pqueue_arrival workload;
    map<string, list<Process>> results;

public:
    // Load processes from a file
    bool loadProcesses(string filename);
    
    // Generate a test workload programmatically
    void generateTestWorkload(int test_case);
    
    // Run a specific scheduler
    list<Process> runScheduler(string scheduler_type);
    
    // Run all schedulers for comparison
    void compareSchedulers();
    
    // Displays the current workload
    void displayWorkload();

    // Displays process completion order of scheduler
    void show_completion_order(list<Process> processes);
};

#endif // SIMULATION_H