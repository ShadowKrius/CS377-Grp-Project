#include "process.h"
#include "metrics.h"
#include <fstream>
#include <iostream>
#include <list>
#include <queue>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <utility>
#include <algorithm>
#include <climits>
#include <cmath>

using namespace std;

// Calculates average turnaround time for list of processes
float avg_turnaround(list<Process> processes) {
  float total_turnaround = 0;
  for(Process proc : processes){
    total_turnaround += (proc.completion - proc.arrival);
  }
  return total_turnaround / processes.size();
}

// Calculates average response time for list of processes
float avg_response(list<Process> processes) {
  float total_response = 0;
  for(Process proc : processes){
    total_response += (proc.first_run - proc.arrival);
  }
  return total_response / processes.size();
}

// Calculates how fairly CPU time is used for list of processes. Value closer to 1 means fairer distribution.
float fairness_index(list<Process> processes){
  if(processes.empty()) return 1.0;

  // Formula: (sum(allocation_ratio))² / (n * sum(allocation_ratio²))
  // Where allocation_ratio = actual_time / (expected_time_based_on_weight)
  int total_weight = 0;
  for(const Process& p: processes){
    total_weight += p.weight;
  }

  vector<float> allocation_ratios;
  for (const Process& p : processes) {
    // Expect share of CPU based on weight
    float expected_share = (float)p.weight / total_weight;
    
    // Actual CPU share
    float turnaround_time = p.completion - p.arrival;
    float original_duration = turnaround_time - (p.first_run - p.arrival);
    float actual_share = original_duration / turnaround_time;
    
    // Normalize as max service rate / CPU share is 1
    float normalized_rate = actual_share / expected_share;
    allocation_ratios.push_back(normalized_rate);
  }
  
  // Jain's fairness index
  float sum_ratios = 0.0;
  float sum_squared_ratios = 0.0;
  
  for (float ratio : allocation_ratios) {
      sum_ratios += ratio;
      sum_squared_ratios += (ratio * ratio);
  }
  
  float n = allocation_ratios.size();
  return (sum_ratios * sum_ratios) / (n * sum_squared_ratios);
}

// Measures how many processes the scheduler completes per unit of time
float throughput(list<Process> processes, int total_time) {
  if(processes.size() == 0 || total_time <= 0){
    return 0.0f;
  }

  return processes.size() / (float)total_time;
}

// Displays metrics of tests
void show_metrics(list<Process> processes) {
  float avg_t = avg_turnaround(processes);
  float avg_r = avg_response(processes);
  float fairness = fairness_index(processes);
  
  // Calculate total time (max completion time)
  int total_time = 0;
  for (const Process& p : processes) {
    total_time = max(total_time, p.completion);
  }
  float throughput_val = throughput(processes, total_time);
  
  show_processes(processes);
  cout << '\n';
  cout << "Average Turnaround Time: " << fixed << setprecision(2) << avg_t << endl;
  cout << "Average Response Time:   " << fixed << setprecision(2) << avg_r << endl;
  cout << "Fairness Index:          " << fixed << setprecision(4) << fairness << endl;
  cout << "Throughput:              " << fixed << setprecision(4) << throughput_val 
       << " processes/time unit" << endl;
}