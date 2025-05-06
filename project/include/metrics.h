#ifndef METRICS_H
#define METRICS_H

#include "process.h"
#include <list>

// Metric calculation functions
float avg_turnaround(list<Process> processes);
float avg_response(list<Process> processes);
float fairness_index(list<Process> processes);
void show_metrics(list<Process> processes);

#endif // METRICS_H