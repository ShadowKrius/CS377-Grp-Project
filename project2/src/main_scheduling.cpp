#include <scheduling.h>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
  if (argc != 3) {
    cout << "usage: [fifo|sjf|stcf|rr] workload_file" << endl;
    exit(1);
  }

  string algorithm = argv[1];
  string workload_file = argv[2];

  pqueue_arrival workload = read_workload(workload_file);

  if (algorithm == "fifo") {
    show_metrics(fifo(workload));
  } else if (algorithm == "sjf") {
    show_metrics(sjf(workload));
  } else if (algorithm == "stcf") {
    show_metrics(stcf(workload));
  } else if (algorithm == "rr") {
    show_metrics(rr(workload));
  } else {
    cout << "Error: Unknown algorithm: " << algorithm << endl;
    cout << "usage: [fifo|sjf|stcf|rr] workload_file" << endl;
    exit(1);
  }

  return 0;
}
