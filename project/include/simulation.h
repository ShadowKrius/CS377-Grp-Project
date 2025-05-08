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
    
    // Print detailed results for a specific scheduler
    void printResults(string scheduler_type);
    
    // Display the current workload
    void displayWorkload();
};

#endif // SIMULATION_H