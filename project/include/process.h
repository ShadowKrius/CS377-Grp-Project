#ifndef PROCESS_H
#define PROCESS_H

#include <list>
#include <queue>
#include <string>
#include <vector>

using namespace std;

// Process structure
struct Process {
  int pid;
  int arrival;
  int first_run;
  int duration;
  int completion;
  // CFS parameters
  int nice_value;      
  int vruntime;        
  int weight;          // Derived from nice value
  // I/O behavior parameters
  bool is_io_bound;    // Indicates if process is I/O-bound
  float io_ratio;      // Percentage of time spent on I/O (0.0-1.0)
};

// Comparator for arrival time priority queue
class ArrivalComparator {
 public:
  bool operator()(const Process lhs, const Process rhs) const {
    if (lhs.arrival != rhs.arrival)
      return lhs.arrival > rhs.arrival;
    else
      return lhs.duration > rhs.duration;
  }
};

// Comparator for duration time priority queue
class DurationComparator {
 public:
  bool operator()(const Process lhs, const Process rhs) const {
    if (lhs.duration != rhs.duration)
      return lhs.duration > rhs.duration;
    else
      return lhs.arrival > rhs.arrival;
  }
};

typedef priority_queue<Process, vector<Process>, ArrivalComparator> pqueue_arrival;
typedef priority_queue<Process, vector<Process>, DurationComparator> pqueue_duration;

// Constants
const int TARGET_LATENCY = 20;      // Needed for dynamic time slice calculation
const int MIN_GRANULARITY = 3;      // Minimum time slice
const int NICE_0_WEIGHT = 1024;     // Standard weight for nice value 0

#endif // PROCESS_H