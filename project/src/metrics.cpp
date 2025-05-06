#include "metrics.h"
#include <cmath>
#include <iostream>

using namespace std;

// Calculate average turnaround time (completion - arrival)
float avg_turnaround(list<Process> processes) {
  float total = 0;
  int count = 0;
  
  for (const Process& p : processes) {
    if (p.completion > 0 && p.arrival >= 0) {
      total += (p.completion - p.arrival);
      count++;
    }
  }
  
  return count > 0 ? total / count : 0;
}

// Calculate average response time (first_run - arrival)
float avg_response(list<Process> processes) {
  float total = 0;
  int count = 0;
  
  for (const Process& p : processes) {
    if (p.first_run >= 0 && p.arrival >= 0) {
      total += (p.first_run - p.arrival);
      count++;
    }
  }
  
  return count > 0 ? total / count : 0;
}

// Calculate fairness index based on Jain's fairness index
float fairness_index(list<Process> processes) {
  if (processes.empty()) {
    return 1.0; // Perfect fairness for no processes
  }
  
  // Calculate throughput for each process (actual duration / expected duration based on weight)
  vector<float> throughputs;
  float sum = 0;
  float sum_squared = 0;
  
  for (const Process& p : processes) {
    if (p.completion > 0 && p.first_run >= 0) {
      float actual_runtime = p.completion - p.first_run;
      float expected_runtime = p.duration;
      
      // If process has non-default weight, adjust expected runtime
      if (p.weight != NICE_0_WEIGHT) {
        expected_runtime = (float)p.duration * NICE_0_WEIGHT / p.weight;
      }
      
      float throughput = expected_runtime / actual_runtime;
      
      // Add to calculations
      sum += throughput;
      sum_squared += throughput * throughput;
      throughputs.push_back(throughput);
    }
  }
  
  if (throughputs.empty()) {
    return 1.0;
  }
  
  // Jain's fairness index: (sum(x_i))^2 / (n * sum(x_i^2))
  int n = throughputs.size();
  float fairness = (sum * sum) / (n * sum_squared);
  
  return fairness; // Returns a value between 0 and 1, where 1 is perfect fairness
}

// Display all metrics
void show_metrics(list<Process> processes) {
  float avg_t = avg_turnaround(processes);
  float avg_r = avg_response(processes);
  float fairness = fairness_index(processes);
  
  cout << "Average Turnaround Time: " << avg_t << endl;
  cout << "Average Response Time:   " << avg_r << endl;
  cout << "Fairness Index:          " << fairness << endl;
}