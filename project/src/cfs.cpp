#include "rb_tree.h"
#include "process.h"
#include "schedulers.h"

// When updating vruntime for a process after it runs
void updateVRuntime(Process& process, int time_slice) {
    float effective_slice = time_slice;
    
    // If I/O-bound process => apply a scaling factor to simulate I/O benefit
    if (process.is_io_bound) {
        // Higher the io_ratio => smaller vruntime increment
        float io_bonus_factor = 0.7;  // Configurable parameter
        effective_slice = time_slice * (1.0 - (process.io_ratio * io_bonus_factor));
    }
    
    // Calculate vruntime with the effective time slice
    process.vruntime += (effective_slice * NICE_0_WEIGHT) / process.weight;
}

list<Process> cfs(pqueue_arrival workload) {
  list<Process> completed;
  RBTree rb_tree;
  int time = 0;
  int min_vruntime = 0;
  
  int num_runnable = 0;  // rb_tree size
  
  if(!workload.empty()) {
    time = workload.top().arrival;
  } else {
    return completed;  
  }
  
  while(num_runnable > 0 || !workload.empty()) {
    // Add any newly arrived processes to the tree
    while(!workload.empty() && workload.top().arrival <= time) {
      Process new_proc = workload.top();
      workload.pop();
      
      // First processes have vruntime of 0. Consequent processes have base vruntime according to the most recent minimum vruntime.
      if(num_runnable == 0) {
        new_proc.vruntime = 0;
      } else {
        new_proc.vruntime = min_vruntime;
      }
      
      rb_tree.insert(new_proc);
      num_runnable++;  
    }
    
    // If no processes in tree, jump time to next arrival
    if(num_runnable == 0 && !workload.empty()) {
      time = workload.top().arrival;
      continue;
    }
    
    // Calculate time slice based on number of runnable processes
    int time_slice = max(TARGET_LATENCY / max(1, num_runnable), MIN_GRANULARITY);
    
    // Skip if no runnable processes
    if (num_runnable == 0) {
      continue;
    }
    
    // Select process with minimum vruntime
    Process cur_proc = rb_tree.findMin();
    rb_tree.remove(cur_proc.pid);
    num_runnable--;  // Decrement counter on removal
    min_vruntime = cur_proc.vruntime;  // Update min_vruntime
    
    // Record first run time if needed
    if(cur_proc.first_run == -1) {
      cur_proc.first_run = time;
    }
    
    // Run the process for its time slice or until completion
    int actual_runtime = min(time_slice, cur_proc.duration);
    time += actual_runtime;
    cur_proc.duration -= actual_runtime;
    
    // Check if process completed
    if(cur_proc.duration == 0) {
      cur_proc.completion = time;
      completed.push_back(cur_proc);
    } else {
      // Update vruntime and reinsert into tree
      updateVRuntime(cur_proc, actual_runtime);
      rb_tree.insert(cur_proc);
      num_runnable++;  
    }
  }
  return completed;
}

