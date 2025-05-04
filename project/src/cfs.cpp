#include <rb_tree.h>
#include <process.h>
#include <metrics.h>
#include <schedulers.h>

list<Process> cfs(pqueue_arrival workload) {
  list<Process> complete;
  RBTree rb_tree;
  int time;
  // CFS implementation using the RB-Tree
  // Similar structure to your existing schedulers
  // but using the RB-Tree for process selection
  if(workload.empty()) return complete;

  int time = workload.top().arrival;

  while(!workload.empty()){
    rb_tree.insert(workload.top());
    workload.pop();
  }
  while(!rb_tree.empty()){
    
  }


  return complete;
}
