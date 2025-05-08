#include <list>
#include <queue>
#include <string>

using namespace std;

typedef struct Process Process;
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

class ArrivalComparator {
 public:
  bool operator()(const Process lhs, const Process rhs) const {
    if (lhs.arrival != rhs.arrival)
      return lhs.arrival > rhs.arrival;
    else
      return lhs.duration > rhs.duration;
  }
};

typedef priority_queue<Process, vector<Process>, ArrivalComparator>
    pqueue_arrival;

pqueue_arrival read_workload(string filename);
void show_workload(pqueue_arrival workload);
void show_processes(list<Process> processes);

list<Process> stcf(pqueue_arrival workload);
list<Process> rr(pqueue_arrival workload);

float avg_turnaround(list<Process> processes);
float avg_response(list<Process> processes);
void show_metrics(list<Process> processes);
float fairness_index(list<Process> processes);
float throughput(list<Process> processes, int total_time);

#ifdef DEBUGMODE
#define debug(msg) \
    std::cout <<"[" << __FILE__ << ":" << __LINE__ << "] " << msg << std::endl;
#else
#define debug(msg)
#endif
