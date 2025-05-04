#include <scheduling.h>
#include <fstream>
#include <iostream>
#include <list>
#include <queue>
#include <string>
#include <sstream>

using namespace std;

pqueue_arrival read_workload(string filename) {
  pqueue_arrival workload;

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
  int arrival, duration;
  
  while(iss >> arrival >> duration) {
    workload.push(Process{arrival, -1, duration, -1});
  }
  
  return workload;
}

void show_workload(pqueue_arrival workload) {
  pqueue_arrival xs = workload;
  cout << "Workload:" << endl;
  while (!xs.empty()) {
    Process p = xs.top();
    cout << '\t' << p.arrival << ' ' << p.duration << endl;
    xs.pop();
  }
}

void show_processes(list<Process> processes) {
  list<Process> xs = processes;
  cout << "Processes:" << endl;
  while (!xs.empty()) {
    Process p = xs.front();
    cout << "\tarrival=" << p.arrival << ", duration=" << p.duration
         << ", first_run=" << p.first_run << ", completion=" << p.completion
         << endl;
    xs.pop_front();
  }
}

list<Process> fifo(pqueue_arrival workload) {
  list<Process> complete;

  if(workload.empty()) return complete;

  int time = workload.top().arrival;
  
  Process cur_proc;
  while(!workload.empty()){
    cur_proc = workload.top();
    workload.pop();
    if(cur_proc.arrival > time){
      time = cur_proc.arrival;
    }
    cur_proc.first_run = time;
    time += cur_proc.duration;
    cur_proc.completion = time;
    complete.push_back(cur_proc);
  }
  return complete;
}

list<Process> sjf(pqueue_arrival workload) {
  list<Process> complete;        
  list<Process> available;       
  int time = 0;
  
  if (workload.empty()) return complete;
  
  time = workload.top().arrival;
  
  while (!workload.empty() || !available.empty()) {

    while (!workload.empty() && workload.top().arrival <= time) {
      available.push_front(workload.top());
      workload.pop();
    }
    
    if (available.empty()) {
      if (!workload.empty()) {
        time = workload.top().arrival;
        continue;
      } else {
        break; 
      }
    }
    
    available.sort(DurationComparator());
    
    Process cur_proc = available.back();
    available.pop_back();
    
    cur_proc.first_run = time;
    time += cur_proc.duration;
    cur_proc.completion = time;  
    
    complete.push_back(cur_proc);
  }
  
  return complete;
}

list<Process> stcf(pqueue_arrival workload) {
  list<Process> complete;        
  list<Process> available;       
  int time;
  
  if (workload.empty()) return complete;
  
  time = workload.top().arrival;
  
  while (!workload.empty() || !available.empty()) {

    while (!workload.empty() && workload.top().arrival <= time) {
      available.push_front(workload.top());
      workload.pop();
    }
    
    if (available.empty()) {
      if (!workload.empty()) {
        time = workload.top().arrival;
        continue;
      } else {
        break; 
      }
    }
    
    available.sort(DurationComparator());
    
    Process cur_proc = available.back();
    available.pop_back();
    
    if(cur_proc.first_run == -1){
      cur_proc.first_run = time;
    }
    time += 1;
    cur_proc.duration -= 1;
    if(cur_proc.duration == 0){
      cur_proc.completion = time;  
      complete.push_back(cur_proc);
    }else{
      available.push_front(cur_proc);
    }
  }
  
  return complete;
}

list<Process> rr(pqueue_arrival workload) {
  list<Process> complete;        
  list<Process> available;       
  int time;
  
  if (workload.empty()) return complete;
  
  time = workload.top().arrival;
  
  while (!workload.empty() || !available.empty()) {

    while (!workload.empty() && workload.top().arrival <= time) {
      available.push_front(workload.top());
      workload.pop();
    }
    
    if (available.empty()) {
      if (!workload.empty()) {
        time = workload.top().arrival;
        continue;
      } else {
        break; 
      }
    }
    
    Process cur_proc = available.back();
    available.pop_back();
    
    if(cur_proc.first_run == -1){
      cur_proc.first_run = time;
    }
    time += 1;
    cur_proc.duration -= 1;
    if(cur_proc.duration == 0){
      cur_proc.completion = time;  
      complete.push_back(cur_proc);
    }else{
      available.push_front(cur_proc);
    }
  }
  
  return complete;
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
  show_processes(processes);
  cout << '\n';
  cout << "Average Turnaround Time: " << avg_t << endl;
  cout << "Average Response Time:   " << avg_r << endl;
}
