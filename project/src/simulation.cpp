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
#include <random>

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

// Loads custom file/test case
bool Simulation::loadProcesses(string filename) {
    workload = read_workload(filename);
    return !workload.empty();
}

// Runs scheduler
list<Process> Simulation::runScheduler(string scheduler_type) {
    pqueue_arrival workload_copy = workload;
    
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



// Displays list of processes used in test case
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

// Displays the metrics of scheduler performance
void Simulation::compareSchedulers() {
    // Run each scheduler
    results["STCF"] = runScheduler("stcf");
    results["RR"] = runScheduler("rr");
    results["CFS"] = runScheduler("cfs");
    
    cout << "\n=== Scheduler Comparison ===\n";
    
    for (auto const& [name, processes] : results) {
        cout << "\n" << name << " Scheduler:\n";
        
        // Show completion order
        show_completion_order(processes);
        
        float turnaround = avg_turnaround(processes);
        float response = avg_response(processes);
        float fairness = fairness_index(processes);
        
        cout << "Average Turnaround Time: " << turnaround << endl;
        cout << "Average Response Time: " << response << endl;
        cout << "Fairness Index: " << fairness << endl;
    }
    
    cout << "\n=== End of Comparison ===\n";
}

// Displays the workload for a given test
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

// Displays the order in which processes are completed by each scheduler
void Simulation::show_completion_order(list<Process> processes) {
    vector<Process> sorted_processes(processes.begin(), processes.end());
    
    sort(sorted_processes.begin(), sorted_processes.end(), 
         [](const Process& a, const Process& b) {
             return a.completion < b.completion;
         });
    
    cout << "\nProcess Completion Order:" << endl;
    cout << "-------------------------------------------------------------------------" << endl;
    cout << "Order | PID | Completion | Nice | Priority | I/O Bound | Duration | First Run" << endl;
    cout << "-------------------------------------------------------------------------" << endl;
    
    int order = 1;
    for (const Process& p : sorted_processes) {
        string priority;
        if (p.nice_value <= -10) priority = "High+++";
        else if (p.nice_value <= -5) priority = "High+";
        else if (p.nice_value < 0) priority = "High";
        else if (p.nice_value == 0) priority = "Normal";
        else if (p.nice_value <= 5) priority = "Low";
        else if (p.nice_value <= 10) priority = "Low+";
        else priority = "Low+++";
        
        cout << setw(5) << order++ << " | " 
             << setw(3) << p.pid << " | "
             << setw(10) << p.completion << " | "
             << setw(4) << p.nice_value << " | "
             << setw(8) << priority << " | "
             << setw(8) << (p.is_io_bound ? "Yes" : "No") << " | "
             << setw(8) << p.duration << " | "
             << setw(9) << p.first_run << endl;
    }
    cout << "-------------------------------------------------------------------------" << endl;
}