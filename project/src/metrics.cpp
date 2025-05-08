#include <process.h>
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

float avg_turnaround(list<Process> processes) {
  float total_turnaround = 0;
  for(Process proc : processes){
    total_turnaround += (proc.completion - proc.arrival);
  }
  return total_turnaround / processes.size();
}

float avg_response(list<Process> processes) {
  float total_response = 0;
  for(Process proc : processes){
    total_response += (proc.first_run - proc.arrival);
  }
  return total_response / processes.size();
}

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

// Calculates how fairly CPU time is used among processes. Value closer to 1 means fairer distribution.
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
    float actual_share = (float)p.duration / turnaround_time;
    
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