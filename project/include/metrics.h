#ifndef METRICS_H
#define METRICS_H

#include <process.h>

float avg_turnaround(list<Process> processes);
float avg_response(list<Process> processes);
void show_metrics(list<Process> processes);
float fairness_index(list<Process> processes);
float throughput(list<Process> processes, int total_time);

#endif