# CS377-Grp-Project



Part 1.1 Ideation:
Advait
The top 3 OS topics I find most interesting are CPU Scheduling Algorithms, the Dining
Philosophers Problem in Concurrency Management, and Memory Management systems like
Paging. I’d like to explore CPU scheduling algorithms, specifically the Completely Fair
Scheduler (CFS), because it balances eﬃciency and fairness using advanced data structures
and is used in real-world systems like the Linux kernel. I chose the Completely Fair Scheduler
(CFS) with the Red-Black Tree implementation, because I haven’t implemented a tree-based
scheduler before, and it’s a great way to understand how real-time OS handles fairness and
time-sharing eﬃciently.
Adhiraj
The top three OS topics I find most interesting are CPU Scheduling Algorithms, Memory
Management, and Concurrency and Synchronization. I was intrigued by how diﬀerent
scheduling approaches aﬀect system performance and fairness, the techniques for allocating
and managing memory resources eﬃciently, and how operating systems handle multiple
processes executing simultaneously. However, if I had to pick one topic to explore in depth, it
would be CPU Scheduling Algorithms, specifically the Completely Fair Scheduler (CFS). This
interests me because it directly impacts user experience and system performance, while
balancing fairness with eﬃciency. I'd like to explore CFS with Red-Black Trees, used in Linux.
I'm particularly interested in how this self-balancing data structure enables logarithmic time
complexity for task management, making the scheduler more eﬃcient as the number of
processes grows.
Luke
My top three OS related topics are concurrency control, CPU scheduling algorithms, (MLFQ,
CFS), and virtual memory implementation. I read a bunch about MLFQ in the book and was a
bit surprised, sad, and relieved we didn’t have to implement it in project 2. However,
considering how much I enjoyed project 2, I figured it might be fun to code a more
complicated scheduler. It was between MLFQ and CFS for me, but I was captivated by CFS as it
doesn’t rely on queues but rather the Red-Black trees. The overall implementation for CFS
looks really interesting, diﬀerent, and challenging. Red-Black trees are a bit confusing but I’m
determined to figure them out.
Part 1.2: Brainstorming
After discussing, we settled on CPU process scheduling algorithms. We agreed that CFS
(Completely Fair Scheduler) is an interesting topic, particularly:
1. How it maintains fairness using virtual runtime
2. The role of Red-Black Trees in maintaining scheduling eﬃciency using insertions and
deletions.
We decided to focus on the CFS implementation as it represents a modern approach to
scheduling and is widely used in production environments. We're particularly interested in
implementing the core algorithm and comparing its performance with more traditional
scheduling approaches such as SJF and RR.
Part 1.3: Proposal
What is the algorithm that your team plans to implement?
Completely Fair Scheduler (CFS)
What is the baseline algorithm that your team will compare it with?
We’ll compare it to round robin (RR), shortest-time to completion first (STCF), and potentially
multilevel feedback queue (MLFQ, might be unrealistic).
What are the metrics that your team will use to compare the algorithms?
We’ll use:
1. 2. 3. 4. 5. Turnaround Time: Average time taken to complete a process
Response Time: Time from submission to first response
Fairness Index: Measure of how equitably CPU time is distributed
Throughput: Number of processes completed per unit time
Context Switch Overhead: Time spent switching between processes
Lastly, think about 3-5 test cases your team will provide to show that the algorithm is
implemented correctly.
Test #1:
Basic Competency: A set of 10-20 CPU-bound processes with equal priority to verify basic
scheduling behavior.
Test #2:
Dynamic Workload: Processes that join and leave the ready queue frequently to test insertion
and deletion operations.
Test#3:
Priority-based Scheduling: Processes with diﬀerent nice values to test how the scheduler
adjusts time slices based on priority.
Test#4:
Mixed I/O and CPU-bound Processes: A mix of I/O and CPU bound processes. I/O bound
processes should have lower vruntime as they are given priority.
Additional CFS Implementation Notes
- CFS-
- Task struct- structure that represents all processes and threads
- Task struct contains task's name, PID, priority, etc. It also contains another structure, sched
entity.
- New task created -> added to the RBTree acc to its vruntime.
- CFS tracks how long each task has been running and updates the tree. Tasks ordered by
vruntime, so lower vruntime on the left.
- Scheduler selects task with lowest vruntime (leftmost node) to run next.
- After a task finishes, its CPU time is added to its vruntime and it is placed back into the
tree.
- As the task runs, its vruntime increases.
- If another task enters the system or becomes runnable & has a lower vruntime than the
curr task then the curr task is pre-empted & the new task is scheduled to run.
- The schedule() function interrupts a task and switches it.
- The task's vruntime is continously updated as it runs, and CFS repositions it in the RBTree
once it is finished.
- For multicore CPUs look into load balancing.
- RBTrees-
- Type of binary tree which is perfectly self-balanced.
- A node is always coloured red or black.
- Root node & All leaf nodes- Black
- Red nodes- Cannot have a red child, Can have a black child
- Every path from any node down to any of the leaf nodes has the same number of black
nodes. This is the black-height property. This balancing ensures that the longest path is never
more than twice the length of the shortest path.
- This also keeps all operations at O(log n)
- Insertion- New node is initially coloured Red.
Link -
https://www.ijraset.com/best-journal/enhancing-cfs-scheduler-performance-through-binomial
-heap-integration
