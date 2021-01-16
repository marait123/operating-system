# operating-system


# phase 2 steps
* define waiting_q , memory_linked_list
* add memory_q (free list) --> store the start and beginning of each segment and whether it is a hole or process

* at each time check if there's a space for it in the pcb 
  * if there 's space insert it in the pcb and schedule it
  * if there isn't space insert in the waiting queue of processes
* at each funeral of a process 
  * check if you can merge the freed memory with another block
  * check that if the freed space is suitable for a process in the waiting queue
