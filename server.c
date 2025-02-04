#include "utils.h"
#include "server.h"

// Functie pentru a scrie mesaje in log-ul serverului master
void log_message(const char *message) 
{
    FILE *log_file = fopen("log_server.txt", "a");
    if (!log_file) 
    {
        fprintf(stderr, "Failed to open the log file\n");
        exit(EXIT_FAILURE);
    }

    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);

    // Scrie timul si mesajul in log
    fprintf(log_file, "[%02d-%02d-%04d %02d:%02d:%02d] %s\n",
            local_time->tm_mday,
            local_time->tm_mon + 1,      // Luna este indexata de la 0
            local_time->tm_year + 1900, // Anul este indexat de la 1900
            local_time->tm_hour,
            local_time->tm_min,
            local_time->tm_sec,
            message);

    fclose(log_file);
}

// Functie pentru a procesa comenzile
void main_server(int num_workers, const char *filename) 
{
    // Deschide fisierul cu comenzile clientilor
    FILE *command_file = fopen(filename, "r");
    if (!command_file) 
    {
        fprintf(stderr, "Failed to open the command file\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Citeste toate comenzile din fisier si le stocheaza intr-un buffer
    char commands_buffer[MAX_COMMANDS][256];
    int number_commands = 0;
    char line[256];

    while (fgets(line, sizeof(line), command_file)) 
    {
        // Elimina newline-ul si ignora liniile goale
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) > 0) {
            strcpy(commands_buffer[number_commands], line);
            number_commands++;
        }
    }
    fclose(command_file);

    // Trimite catre log inceputul procesarii comenzilor
    char start_log[256];
    sprintf(start_log, "Started processing %d commands", number_commands);
    log_message(start_log);

    // Variabile de stari ale worker-ilor si buffer-elor pentru mesaje
    int worker_available[num_workers];

    char client_names[num_workers][16];
    char response_requests[num_workers][256];
    char dispatch_requests[num_workers][256];

    MPI_Request send_requests[num_workers];
    MPI_Request recv_requests[num_workers];

    // Initializeaza workerii ca fiind liberi
    for (int i = 0; i < num_workers; i++) 
    {
        worker_available[i] = 0;
        send_requests[i] = MPI_REQUEST_NULL;
        recv_requests[i] = MPI_REQUEST_NULL;
    }

    int commands_sent = 0;
    int commands_completed = 0;

    // Bucla principala de procesare
    while (commands_completed < number_commands) 
    {
        // Verifica daca workerii au terminat comemzile de procesat
        for (int worker = 0; worker < num_workers; worker++) 
        {
            if (recv_requests[worker] != MPI_REQUEST_NULL) 
            {
                int flag = 0;
                MPI_Status status;
                MPI_Test(&recv_requests[worker], &flag, &status);

                if (flag) 
                {
                    // Trimite catre log rezultatul procesarii comenzii
                    char command_log[512];
                    sprintf(command_log, "Command from %s processed. Worker %d.", client_names[worker], worker + 1);
                    log_message(command_log);

                    // Scrie rezultatul in fisierul clientului
                    char client_filename[32];
                    sprintf(client_filename, "%s.txt", client_names[worker]);
                    FILE *client_file = fopen(client_filename, "a");
                    if (client_file) 
                    {
                        fprintf(client_file, "%s\n", response_requests[worker]);
                        fclose(client_file);
                    } 
                    else 
                    {
                        fprintf(stderr, "Failed to open the client file\n");
                        exit(EXIT_FAILURE);
                    }

                    // Marcheaza workerul ca fiind liber
                    worker_available[worker] = 0;
                    commands_completed++;
                    recv_requests[worker] = MPI_REQUEST_NULL;
                }
            }

            // Daca worker-ul este liber si mai sunt comenzi de trimis
            if (worker_available[worker] == 0 && commands_sent < number_commands) 
            {
                // Trimite catre log comanda trimisa catre worker
                char dispatch_log[512];
                sprintf(dispatch_log, "Dispatching command to worker %d: %s", worker + 1, commands_buffer[commands_sent]);
                log_message(dispatch_log);

                // Buffer pentru trimiterea comenzii
                strcpy(dispatch_requests[worker], commands_buffer[commands_sent]);

                // Trimite comanda catre worker
                MPI_Isend(dispatch_requests[worker], 256, MPI_CHAR, worker + 1, TASK_TAG, MPI_COMM_WORLD, &send_requests[worker]);

                // Primeste rezultatul de la worker
                MPI_Irecv(response_requests[worker], 256, MPI_CHAR, worker + 1, RESULT_TAG, MPI_COMM_WORLD, &recv_requests[worker]);

                // Extrage ID-ul clientului pentru log
                sscanf(commands_buffer[commands_sent], "%s", client_names[worker]);

                // Marcheaza workerul ca fiind ocupat
                worker_available[worker] = 1;
                commands_sent++;
            }
        }
    }

    // Trimite catre log finalizarea comenzilor
    char end_log[256];
    sprintf(end_log, "All %d commands processed successfully", number_commands);
    log_message(end_log);

    // Trimite semnale de terminare tuturor workerilor
    for (int i = 1; i <= num_workers; i++) 
    {
        MPI_Send(NULL, 0, MPI_CHAR, i, TERMINATE_TAG, MPI_COMM_WORLD);
    }
}
