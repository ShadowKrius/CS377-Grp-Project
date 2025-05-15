#include "gtest/gtest.h"
#include <scheduling.h>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

TEST(SchedulingTest, ReadWorkload1) {
  pqueue_arrival pq = read_workload("workloads/workload_01.txt");
  EXPECT_EQ(pq.size(), 3);
}

TEST(SchedulingTest, FIFO1) {
  pqueue_arrival pq = read_workload("workloads/workload_01.txt");
  list<Process> xs = fifo(pq);
  float t = avg_turnaround(xs);
  float r = avg_response(xs);
  EXPECT_FLOAT_EQ(t, 20);
  EXPECT_FLOAT_EQ(r, 10);
}

TEST(SchedulingTest, SFJ01) {
  pqueue_arrival pq = read_workload("workloads/workload_02.txt");
  list<Process> xs = sjf(pq);
  float t = avg_turnaround(xs);
  float r = avg_response(xs);
  EXPECT_FLOAT_EQ(t, 50);
  EXPECT_FLOAT_EQ(r, 10);
}

TEST(Scheduling_Test, STCF01) {
  pqueue_arrival pq = read_workload("workloads/workload_06.txt");
  list<Process> xs = stcf(pq);
  float t = avg_turnaround(xs);
  float r = avg_response(xs);
  EXPECT_FLOAT_EQ(t, 8);
  EXPECT_FLOAT_EQ(r, 0);
}

TEST(Scheduling_Test, RR01) {
  pqueue_arrival pq = read_workload("workloads/workload_05.txt");
  list<Process> xs = rr(pq);
  float t = avg_turnaround(xs);
  float r = avg_response(xs);
  EXPECT_FLOAT_EQ(t, 19);
  EXPECT_FLOAT_EQ(r, 1);
}



int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
