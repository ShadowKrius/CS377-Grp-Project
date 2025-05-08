#include "simulation.h"
#include "metrics.h"
#include "process.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <list>
#include <queue>
#include <iomanip>
#include <vector>
#include <utility>
#include <algorithm>
#include <climits>
#include <cmath>

using namespace std;

const int nice_to_weight[40] = {
  /* -20 */ 88761, 71755, 56483, 46273, 36291,
  /* -15 */ 29154, 23254, 18705, 14949, 11916,
  /* -10 */ 9548, 7620, 6100, 4904, 3906,
  /*  -5 */ 3121, 2501, 1991, 1586, 1277,
  /*   0 */ 1024, 820, 655, 526, 423,
  /*   5 */ 335, 272, 215, 172, 137,
  /*  10 */ 110, 87, 70, 56, 45,
  /*  15 */ 36, 29, 23, 18, 15,
};

// Read workload from file
pqueue_arrival read_workload(string filename) {
  pqueue_arrival workload;
  int next_pid = 1;
  int NICE_0_WEIGHT = 1024;

  ifstream file(filename);
  if(!file.is_open()){
    cerr << "Error: Unable to open file" << filename << endl;
    return workload;
  }

  string file_contents;
  string line;
  while(getline(file, line)){
    file_contents += line + "\n";
  }
  file.close();

  istringstream iss(file_contents);
  int arrival, duration, nice_value, is_io_bound;
  float io_ratio;

  
  while(iss >> arrival >> duration >> nice_value >> is_io_bound >> io_ratio) {
    Process p;
    p.pid = next_pid++;
    p.arrival = arrival;
    p.duration = duration;
    p.first_run = -1;
    p.completion = -1;
    p.vruntime = 0;
    p.nice_value = nice_value;
    p.is_io_bound = is_io_bound;
    p.io_ratio = io_ratio;

    // int temp_nice_value = p.nice_value;
    // if(temp_nice_value < -20){
    //   temp_nice_value = -20;
    // }
    // if(temp_nice_value > 19){
    //   temp_nice_value = 19;
    // }
    // p.weight = NICE_0_WEIGHT/pow(1.25,temp_nice_value);

    // Considered the above approach, but could be a big efficiency suck as computing the power of tens of processes may slow processor. Settled on adding a matrix containing all pre-computed nice values.

    // Ensure index is within bounds
    int index = p.nice_value + 20; 
    if (index < 0) {
        index = 0;
    } else if (index >= 40) {
        index = 39;
    }
    
    p.weight = nice_to_weight[index];

    workload.push(p);
  }
  
  return workload;
}

void initializeWeight(Process& p) {
  // Convert nice value to index
  int index = p.nice_value + 20;
  
  // Ensure index is within bounds
  if (index < 0) {
      index = 0;
  } else if (index >= 40) {
      index = 39;
  }
  
  // Assign weight from lookup table
  p.weight = nice_to_weight[index];
}

void show_workload(pqueue_arrival workload) {
  pqueue_arrival xs = workload;
  cout << "Workload:" << endl;
  cout << "PID\tArrival\tDuration\tNice\tIO Bound\tIO Ratio" << endl;
  cout << "-----------------------------------------------------" << endl;
  while (!xs.empty()) {
    Process p = xs.top();
    cout << p.pid << "\t" 
         << p.arrival << "\t" 
         << p.duration << "\t\t" 
         << p.nice_value << "\t" 
         << (p.is_io_bound ? "Yes" : "No") << "\t\t" 
         << fixed << setprecision(2) << p.io_ratio << endl;
    xs.pop();
  }
}

void show_processes(list<Process> processes) {
  list<Process> xs = processes;
  cout << "Processes:" << endl;
  cout << "PID\tArr\tDur\tNice\tWeight\tVruntime\tIO\tFirst\tCompl\tTAT\tResp" << endl;
  cout << "--------------------------------------------------------------------------------" << endl;
  while (!xs.empty()) {
    Process p = xs.front();
    int turnaround = p.completion - p.arrival;
    int response = p.first_run - p.arrival;
    
    cout << p.pid << "\t" 
         << p.arrival << "\t" 
         << p.duration << "\t" 
         << p.nice_value << "\t" 
         << p.weight << "\t" 
         << p.vruntime << "\t\t" 
         << fixed << setprecision(1) << p.io_ratio << "\t" 
         << p.first_run << "\t" 
         << p.completion << "\t" 
         << turnaround << "\t" 
         << response << endl;
    xs.pop_front();
  }
}

// Simulation class implementation
bool Simulation::loadProcesses(string filename) {
    workload = read_workload(filename);
    return !workload.empty();
}

void Simulation::generateTestWorkload(int test_case) {
    // Clear existing workload
    while (!workload.empty()) {
        workload.pop();
    }
    
    // Generate processes based on test case
    switch (test_case) {
        case 1: // Basic Competency: CPU-bound processes with equal priority
            for (int i = 0; i < 10; i++) {
                Process p;
                p.pid = i;
                p.arrival = i * 5; // Staggered arrivals
                p.duration = 20;
                p.first_run = -1;
                p.completion = -1;
                p.nice_value = 0;
                p.weight = NICE_0_WEIGHT;
                p.is_io_bound = false;
                p.io_ratio = 0.0;
                workload.push(p);
            }
            break;
            
        case 2: // Dynamic Workload: Processes that join and leave frequently
            for (int i = 0; i < 20; i++) {
                Process p;
                p.pid = i;
                p.arrival = i * 2; // Frequent arrivals
                p.duration = 5 + (i % 10); // Varied durations
                p.first_run = -1;
                p.completion = -1;
                p.nice_value = 0;
                p.weight = NICE_0_WEIGHT;
                p.is_io_bound = false;
                p.io_ratio = 0.0;
                workload.push(p);
            }
            break;
            
        case 3: // Priority-based scheduling: Processes with different nice values
            for (int i = 0; i < 10; i++) {
                Process p;
                p.pid = i;
                p.arrival = i * 3;
                p.duration = 15;
                p.first_run = -1;
                p.completion = -1;
                p.nice_value = (i % 5) - 2; // Nice values from -2 to 2
                
                // Set weight based on nice value
                if (p.nice_value == 0) {
                    p.weight = NICE_0_WEIGHT;
                } else if (p.nice_value > 0) {
                    p.weight = NICE_0_WEIGHT >> (p.nice_value / 5);
                } else {
                    p.weight = NICE_0_WEIGHT << ((-p.nice_value) / 5);
                }
                
                p.is_io_bound = false;
                p.io_ratio = 0.0;
                workload.push(p);
            }
            break;
            
        case 4: // Mixed I/O and CPU-bound processes
            for (int i = 0; i < 10; i++) {
                Process p;
                p.pid = i;
                p.arrival = i * 4;
                p.duration = 20;
                p.first_run = -1;
                p.completion = -1;
                p.nice_value = 0;
                p.weight = NICE_0_WEIGHT;
                
                // Alternate between CPU-bound and I/O-bound processes
                p.is_io_bound = (i % 2 == 0);
                p.io_ratio = p.is_io_bound ? 0.7 : 0.0;
                
                workload.push(p);
            }
            break;
            
        default:
            // Default to a simple workload
            for (int i = 0; i < 5; i++) {
                Process p;
                p.pid = i;
                p.arrival = i * 10;
                p.duration = 10;
                p.first_run = -1;
                p.completion = -1;
                p.nice_value = 0;
                p.weight = NICE_0_WEIGHT;
                p.is_io_bound = false;
                p.io_ratio = 0.0;
                workload.push(p);
            }
    }
}

list<Process> Simulation::runScheduler(string scheduler_type) {
    pqueue_arrival workload_copy = workload; // Make a copy to preserve original
    
    if (scheduler_type == "stcf") {
        return stcf(workload_copy);
    } else if (scheduler_type == "rr") {
        return rr(workload_copy);
    } else if (scheduler_type == "cfs") {
        return cfs(workload_copy);
    } else {
        cout << "Invalid scheduler type: " << scheduler_type << endl;
        return list<Process>();
    }
}

void Simulation::compareSchedulers() {
    // Run each scheduler
    results["STCF"] = runScheduler("stcf");
    results["RR"] = runScheduler("rr");
    results["CFS"] = runScheduler("cfs");
    
    // Print results
    cout << "\n=== Scheduler Comparison ===\n";
    
    for (auto const& [name, processes] : results) {
        cout << "\n" << name << " Scheduler:\n";
        
        // Calculate metrics
        float turnaround = avg_turnaround(processes);
        float response = avg_response(processes);
        float fairness = fairness_index(processes);
        
        cout << "Average Turnaround Time: " << turnaround << endl;
        cout << "Average Response Time: " << response << endl;
        cout << "Fairness Index: " << fairness << endl;
    }
    
    cout << "\n=== End of Comparison ===\n";
}

void Simulation::printResults(string scheduler_type) {
    list<Process> completed = runScheduler(scheduler_type);
    
    cout << "\n=== " << scheduler_type << " Scheduler Results ===\n";
    show_processes(completed);
    
    // Calculate metrics
    float turnaround = avg_turnaround(completed);
    float response = avg_response(completed);
    float fairness = fairness_index(completed);
    
    cout << "Average Turnaround Time: " << turnaround << endl;
    cout << "Average Response Time: " << response << endl;
    cout << "Fairness Index: " << fairness << endl;
    
    cout << "=== End of Results ===\n";
}

void Simulation::displayWorkload() {
    pqueue_arrival workload_copy = workload;
    cout << "Current Workload:" << endl;
    
    while (!workload_copy.empty()) {
        Process p = workload_copy.top();
        cout << "PID: " << p.pid 
             << ", Arrival: " << p.arrival 
             << ", Duration: " << p.duration;
        
        if (p.nice_value != 0) {
            cout << ", Nice: " << p.nice_value;
        }
        
        if (p.is_io_bound) {
            cout << ", I/O-bound (" << (p.io_ratio * 100) << "%)";
        }
        
        cout << endl;
        workload_copy.pop();
    }
}