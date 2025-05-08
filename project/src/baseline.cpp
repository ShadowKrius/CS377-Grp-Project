#include "schedulers.h"
#include <queue>
#include <vector>
#include <iostream>

using namespace std;

// STCF (Shortest Time to Completion First) algorithm
list<Process> stcf(pqueue_arrival workload) {
  list<Process> completed;
  int current_time = 0;
  
  // Create a copy of the workload
  pqueue_arrival arrival_queue = workload;
  
  // Vector to store processes in the ready queue
  vector<pair<Process, int>> ready_queue; // (process, remaining time)
  
  while (!arrival_queue.empty() || !ready_queue.empty()) {
    // Process arrivals
    while (!arrival_queue.empty() && arrival_queue.top().arrival <= current_time) {
      Process p = arrival_queue.top();
      ready_queue.push_back(make_pair(p, p.duration));
      arrival_queue.pop();
    }
    
    // If no processes ready, advance time to next arrival
    if (ready_queue.empty() && !arrival_queue.empty()) {
      current_time = arrival_queue.top().arrival;
      continue;
    }
    
    // Find the process with the shortest remaining time
    if (!ready_queue.empty()) {
      int shortest_idx = 0;
      int shortest_remaining = ready_queue[0].second;
      
      for (int i = 1; i < ready_queue.size(); ++i) {
        if (ready_queue[i].second < shortest_remaining ||
            (ready_queue[i].second == shortest_remaining &&
             ready_queue[i].first.arrival < ready_queue[shortest_idx].first.arrival)) {
          shortest_idx = i;
          shortest_remaining = ready_queue[i].second;
        }
      }
      
      // Get the process and its remaining time
      Process& p = ready_queue[shortest_idx].first;
      int& remaining = ready_queue[shortest_idx].second;
      
      // Set first run time if this is the first time the process runs
      if (p.first_run == -1) {
        p.first_run = current_time;
      }
      
      // Calculate time to next arrival or completion
      int run_time = remaining;
      if (!arrival_queue.empty()) {
        run_time = min(run_time, arrival_queue.top().arrival - current_time);
      }
      
      // Update current time and remaining time
      current_time += run_time;
      remaining -= run_time;
      
      // If the process is complete, add it to the completed list
      if (remaining == 0) {
        p.completion = current_time;
        completed.push_back(p);
        ready_queue.erase(ready_queue.begin() + shortest_idx);
      }
    }
  }
  
  return completed;
}

// RR (Round Robin) algorithm
list<Process> rr(pqueue_arrival workload) {
  list<Process> completed;
  int current_time = 0;
  int time_slice = 1;  // As specified in the assignment
  
  // Create a copy of the workload
  pqueue_arrival arrival_queue = workload;
  
  // Queue for round robin scheduling
  queue<pair<Process, int>> ready_queue; // (process, remaining time)
  
  while (!arrival_queue.empty() || !ready_queue.empty()) {
    // Process arrivals
    while (!arrival_queue.empty() && arrival_queue.top().arrival <= current_time) {
      Process p = arrival_queue.top();
      ready_queue.push(make_pair(p, p.duration));
      arrival_queue.pop();
    }
    
    // If no processes ready, advance time to next arrival
    if (ready_queue.empty() && !arrival_queue.empty()) {
      current_time = arrival_queue.top().arrival;
      continue;
    }
    
    // Process the next ready process
    if (!ready_queue.empty()) {
      pair<Process, int> p_pair = ready_queue.front();
      Process p = p_pair.first;
      int remaining = p_pair.second;
      ready_queue.pop();
      
      // Set first run time if this is the first time the process runs
      if (p.first_run == -1) {
        p.first_run = current_time;
      }
      
      // Calculate time to run: min(time_slice, remaining duration)
      int run_time = min(time_slice, remaining);
      
      // Update current time
      current_time += run_time;
      remaining -= run_time;
      
      // Process new arrivals during this time slice
      while (!arrival_queue.empty() && arrival_queue.top().arrival <= current_time) {
        Process new_p = arrival_queue.top();
        ready_queue.push(make_pair(new_p, new_p.duration));
        arrival_queue.pop();
      }
      
      // If process completed, add to completed list
      if (remaining == 0) {
        p.completion = current_time;
        completed.push_back(p);
      } else {
        // Put the process back in the ready queue
        ready_queue.push(make_pair(p, remaining));
      }
    }
  }
  
  return completed;
}