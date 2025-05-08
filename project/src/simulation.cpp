#include "simulation.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>

using namespace std;

// Read workload from file
pqueue_arrival read_workload(string filename) {
  pqueue_arrival workload;
  static int pid_counter = 0; // For assigning unique PIDs
  
  try {
    ifstream file(filename);
    if (file.is_open()) {
      int arrival, duration;
      int nice_value = 0; // Default nice value
      
      // Check if file is empty
      if (file.peek() == ifstream::traits_type::eof()) {
        file.close();
        return workload;  // Return empty workload for empty file
      }
      
      string line;
      while (getline(file, line)) {
        // Skip empty lines or comment lines
        if (line.empty() || line[0] == '#') continue;
        
        istringstream iss(line);
        if (iss >> arrival >> duration) {
          // Optional nice value (if present in file)
          if (!(iss >> nice_value)) {
            nice_value = 0; // Default if not specified
          }
          
          // Create a new process
          Process p;
          p.pid = pid_counter++;
          p.arrival = arrival;
          p.duration = duration;
          p.first_run = -1;  // Not yet run
          p.completion = -1; // Not yet completed
          
          // CFS parameters
          p.nice_value = nice_value;
          p.vruntime = 0;  // Will be properly initialized in CFS scheduler
          
          // Set weight based on nice value
          // Formula: weight = 1024 / (1.25^nice_value)
          if (nice_value == 0) {
            p.weight = NICE_0_WEIGHT;
          } else if (nice_value > 0) {
            // Lower weight for lower priority
            p.weight = NICE_0_WEIGHT >> (nice_value / 5);
          } else {
            // Higher weight for higher priority
            p.weight = NICE_0_WEIGHT << ((-nice_value) / 5);
          }
          
          // Default I/O behavior - can be modified for Test #4
          p.is_io_bound = false;
          p.io_ratio = 0.0;
          
          workload.push(p);
        }
      }
      file.close();
    } else {
      cerr << "Warning: Could not open file " << filename << endl;
      // Return empty workload
    }
  } catch (const exception& e) {
    cerr << "Error reading workload: " << e.what() << endl;
    // Return empty workload on exception
  }
  
  return workload;
}

// Show workload contents
void show_workload(pqueue_arrival workload) {
  pqueue_arrival xs = workload;
  cout << "Workload:" << endl;
  while (!xs.empty()) {
    Process p = xs.top();
    cout << '\t' << "PID: " << p.pid 
         << ", Arrival: " << p.arrival 
         << ", Duration: " << p.duration;
    
    if (p.nice_value != 0) {
      cout << ", Nice: " << p.nice_value;
    }
    
    cout << endl;
    xs.pop();
  }
}

// Show completed processes
void show_processes(list<Process> processes) {
  list<Process> xs = processes;
  cout << "Processes:" << endl;
  while (!xs.empty()) {
    Process p = xs.front();
    cout << "\tPID: " << p.pid
         << ", Arrival: " << p.arrival 
         << ", Duration: " << p.duration
         << ", First Run: " << p.first_run 
         << ", Completion: " << p.completion;
    
    if (p.nice_value != 0) {
      cout << ", Nice: " << p.nice_value;
    }
    
    cout << endl;
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