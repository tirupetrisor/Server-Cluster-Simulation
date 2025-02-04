This project simulates a server cluster to process client commands using both serial and parallel approaches. The application demonstrates the functionality of a main server (job dispatcher) and multiple worker servers, comparing the performance of serial execution with parallel execution using MPI (Message Passing Interface).

Features
-Job Dispatching: Main server coordinates task assignment to worker servers.
-Task Management: Worker servers process commands such as finding prime numbers, calculating prime divisors, and generating anagrams.
-Performance Evaluation: Compares execution time between serial and parallel implementations and calculates speedup.

Project Structure
-utils.h and utils.c: Provide utility functions for command processing: prime number calculation, prime divisors computation, anagram generation.
-server.h and server.c: Implement main server functionality: job dispatching and task management, result aggregation and logging.
-worker.h and worker.c: Implement worker server functionality: task reception, processing, and result return, includes a serial implementation of a job dispatcher.
main.c: manages the initialization of MPI, runs serial and parallel implementations, measures performance and calculates speedup.

To compile the project, use the following command: gcc -o main main.c utils.c server.c worker.c -I"<path_to_include_directory>" -L"<path_to_library_directory>" -lmsmpi  

To run the simulation, use the following command: mpiexec -n <number_of_processes> main <commands_file_name>  
<number_of_processes>: The total number of processes, including the main server and worker servers.
<commands_file_name>: Path to the file containing commands to process.

This project demonstrates a scalable approach to server cluster simulation, leveraging MPI for efficient parallelization and performance gains.
