#include "../include/simulation.h"
#include "../include/metrics.h"
#include "../include/schedulers.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
// Function to run a specific test
void runTest(int test_number) {
    Simulation sim;
    string filename;
    
    switch (test_number) {
        case 1: { // Basic Fairness Test
            filename = "test1_fairness.txt";
            ofstream outfile(filename);
            // 5 processes with equal duration but different nice values
            outfile << "0 20 -10 0 0.0\n";  // High priority
            outfile << "0 20 -5 0 0.0\n";   // Medium-high priority
            outfile << "0 20 0 0 0.0\n";    // Normal priority
            outfile << "0 20 5 0 0.0\n";    // Medium-low priority
            outfile << "0 20 10 0 0.0\n";   // Low priority
            outfile.close();
            
            cout << "\n=== Test 1: Basic Fairness Test ===\n";
            cout << "This test evaluates how schedulers handle processes with different priorities.\n";
            cout << "We expect CFS to complete high-priority processes earlier with better response time.\n\n";
            break;
        }
        case 2: { // Dynamic Workload Test
            filename = "test2_dynamic.txt";
            ofstream outfile(filename);
            // Processes with staggered arrivals and varying durations
            for (int i = 0; i < 15; i++) {
                // Arrival times spaced out every 2 time units
                // Durations alternate between short (3) and long (10)
                outfile << i*2 << " " << (i % 2 == 0 ? 3 : 10) << " 0 0 0.0\n";
            }
            outfile.close();
            
            cout << "\n=== Test 2: Dynamic Workload Test ===\n";
            cout << "This test evaluates scheduler performance with frequent process arrivals and departures.\n";
            cout << "We expect STCF to have good turnaround for short processes, while CFS maintains fairness.\n\n";
            break;
        }
        case 3: { // I/O-Bound vs. CPU-Bound Test
            filename = "test3_io_cpu.txt";
            ofstream outfile(filename);
            // 5 I/O-bound processes with high I/O ratio
            for (int i = 0; i < 5; i++) {
                outfile << "0 20 0 1 0.8\n";  // 80% I/O bound
            }
            // 5 CPU-bound processes
            for (int i = 0; i < 5; i++) {
                outfile << "0 20 0 0 0.0\n";  // 0% I/O bound (pure CPU)
            }
            outfile.close();
            
            cout << "\n=== Test 3: I/O-Bound vs. CPU-Bound Test ===\n";
            cout << "This test evaluates how schedulers handle mixed I/O and CPU-bound processes.\n";
            cout << "We expect CFS to give preference to I/O-bound processes for better interactive performance.\n\n";
            break;
        }
        case 4: { // Priority Inversion Test
            filename = "test4_priority_inversion.txt";
            ofstream outfile(filename);
            // High-priority short process arrives slightly after low-priority long process
            outfile << "0 50 10 0 0.0\n";   // Low priority, long duration
            outfile << "1 5 -10 0 0.0\n";   // High priority, short duration
            outfile << "10 20 0 0 0.0\n";   // Normal priority
            outfile << "15 10 -5 0 0.0\n";  // Medium-high priority
            outfile << "20 15 5 0 0.0\n";   // Medium-low priority
            outfile.close();
            
            cout << "\n=== Test 4: Priority Inversion Test ===\n";
            cout << "This test evaluates how schedulers handle priority inversions.\n";
            cout << "We expect CFS to quickly preempt in favor of high-priority processes.\n\n";
            break;
        }
        case 5: { // Scalability Test
            filename = "test5_scalability.txt";
            ofstream outfile(filename);
            // A bunch of processes with somewhat randomized attributes
            for (int i = 0; i < 50; i++) {
                int arrival = i / 5;  // Groups of 5 processes arrive together
                int duration = 5 + (i % 10);  // Durations from 5 to 14
                int nice = (i % 21) - 10;  // Nice values from -10 to 10
                bool io_bound = (i % 3 == 0);  // Every third process is I/O bound
                float io_ratio = io_bound ? 0.7 : 0.0;  // 70% I/O ratio for I/O-bound processes
                
                outfile << arrival << " " << duration << " " << nice << " " 
                       << (io_bound ? 1 : 0) << " " << io_ratio << "\n";
            }
            outfile.close();
            
            cout << "\n=== Test 5: Scalability Test ===\n";
            cout << "This test evaluates scheduler performance with a large number of diverse processes.\n";
            cout << "We expect to see how each scheduler scales with increasing system load.\n\n";
            break;
        }
        default:
            cout << "Invalid test number\n";
            return;
    }
    
    // Load processes and run test
    if (sim.loadProcesses(filename)) {
        sim.displayWorkload();
        sim.compareSchedulers();
    } else {
        cout << "Failed to load workload from " << filename << endl;
    }
}

int main(int argc, char* argv[]) {
    cout << "===== CPU Scheduler Test Suite =====\n";
    cout << "This program tests and compares three CPU schedulers:\n";
    cout << "  - STCF (Shortest Time to Completion First)\n";
    cout << "  - RR (Round Robin)\n";
    cout << "  - CFS (Completely Fair Scheduler)\n\n";
    
    if (argc > 1) {
        // For running a specific test
        int test_num = atoi(argv[1]);
        runTest(test_num);
    } else {
        // Run all tests
        for (int i = 1; i <= 5; i++) {
            runTest(i);
        }
    }
    
    return 0;
}