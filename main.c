#include "utils.h"
#include "server.h"
#include "worker.h"


int main(int argc, char *argv[]) 
{
    MPI_Init(&argc, &argv); // Initializeaza mediul MPI

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);       // Obtine rangul procesului curent
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs); // Obtine numărul total de procese

    if (num_procs < 2) 
    {
        // Sistemul are nevoie de cel putin un main server si un worker
        fprintf(stderr, "At least two processes are required\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    double serial_time = 0.0, parallel_time = 0.0;

    if (rank == 0) 
    {
        // Procesul cu rank-ul 0 este serverul principal
        if (argc != 2) 
        {
            // Verifica daca a fost furnizat fisierul cu comenzi
            fprintf(stderr, "Usage: %s <command_file>\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        const char *commands_file = argv[1];

        // Implementarea seriala
        printf("Starting serial implementation...\n");
        double start_time_serial = MPI_Wtime();
        serial_implementation(commands_file);
        double end_time_serial = MPI_Wtime();

        serial_time = end_time_serial - start_time_serial;
        printf("Serial execution time: %.6f seconds\n", serial_time);

        // Implementarea paralela
        printf("\nStarting parallel implementation...\n");
        double start_time_parallel = MPI_Wtime();
        main_server(num_procs - 1, commands_file);
        double end_time_parallel = MPI_Wtime();

        parallel_time = end_time_parallel - start_time_parallel;
        printf("Parallel execution time: %.6f seconds\n", parallel_time);

        double speedup = serial_time / parallel_time;
        printf("\nSpeedup: %f\n", speedup);
    } 
    else 
    {
        worker_process();    // Toate celelalte procese sunt workeri
    }

    MPI_Finalize();   // Inchide mediul MPI
    return 0;
}
