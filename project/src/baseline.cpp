#include "schedulers.h"
#include <queue>
#include <vector>
#include <iostream>

using namespace std;

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