#include "simulation.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
  Simulation sim;
  
  if (argc < 2) {
    cout << "Usage: " << argv[0] << " <command> [options]" << endl;
    cout << "Commands:" << endl;
    cout << "  run <scheduler> <workload_file>   - Run a specific scheduler on a workload file" << endl;
    cout << "  test <test_number>               - Run a predefined test case" << endl;
    cout << "  compare <workload_file>          - Compare all schedulers on a workload file" << endl;
    cout << "  compare-test <test_number>       - Compare all schedulers on a test case" << endl;
    return 1;
  }
  
  string command = argv[1];
  
  if (command == "run" && argc == 4) {
    string scheduler = argv[2];
    string workload_file = argv[3];
    
    if (sim.loadProcesses(workload_file)) {
      sim.printResults(scheduler);
    } else {
      cout << "Failed to load workload from " << workload_file << endl;
    }
  }
  else if (command == "test" && argc == 3) {
    int test_number = atoi(argv[2]);
    sim.generateTestWorkload(test_number);
    
    cout << "Running test case " << test_number << ":" << endl;
    sim.displayWorkload();
    sim.compareSchedulers();
  }
  else if (command == "compare" && argc == 3) {
    string workload_file = argv[2];
    
    if (sim.loadProcesses(workload_file)) {
      sim.compareSchedulers();
    } else {
      cout << "Failed to load workload from " << workload_file << endl;
    }
  }
  else if (command == "compare-test" && argc == 3) {
    int test_number = atoi(argv[2]);
    sim.generateTestWorkload(test_number);
    
    cout << "Comparing schedulers on test case " << test_number << ":" << endl;
    sim.displayWorkload();
    sim.compareSchedulers();
  }
  else {
    cout << "Invalid command or missing arguments." << endl;
    return 1;
  }
  
  return 0;
}