#include "utils.h"
#include "worker.h"

// Functia pentru procesarea unui worker
void worker_process() 
{
    char task[256];
    MPI_Status status;

    while (1) 
    {
        // Primeste comanda de la master
        MPI_Recv(task, 256, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        if (status.MPI_TAG == TERMINATE_TAG) 
            break;       // Opreste worker-ul daca se primeste tag-ul de terminare

        char result[256] = {0};
        char client_id[10], command[20], arg[20];

        // Extrage informatiile din comanda primita
        sscanf(task, "%s %s %s\n", client_id, command, arg);

        // Proceseaza comanda in functie de tip
        if (strcmp(command, "PRIMES") == 0) 
        {
            int n = atoi(arg);
            int value = count_primes(n);
            sprintf(result, "PRIMES %d -> %d", n, value);
        }
        else if (strcmp(command, "PRIMEDIVISORS") == 0) 
        {
            int n = atoi(arg);
            int value = count_prime_divisors(n);
            sprintf(result, "PRIMEDIVISORS %d -> %d", n, value);
        }
        else if (strcmp(command, "ANAGRAMS") == 0) 
        {
            char filename[256];

            // Genereaza numele fisierului de output pentru anagrame
            if (strstr(client_id, ".txt") == NULL) {
                snprintf(filename, sizeof(filename), "%s.txt", client_id);
            } else {
                snprintf(filename, sizeof(filename), "%s", client_id);
            }

            FILE *output = fopen(filename, "w");
            if (output) 
            {
                generate_anagrams(arg, output);
                fclose(output);
                sprintf(result, "%s ANAGRAMS generated", client_id);
            }
            else 
            {
                fprintf(stderr, "Failed to open the file for anagrams");
                sprintf(result, "%s ANAGRAMS error", client_id);
            }
        } 
        else 
        {
            // Gestioneaza comenzile necunoscute
            sprintf(result, "Unknown command: %s", command);
        }

        // Trimite rezultatul inapoi la master
        MPI_Send(result, strlen(result) + 1, MPI_CHAR, 0, RESULT_TAG, MPI_COMM_WORLD);
    }
}

// Implementare seriala pentru procesarea comenzilor dintr-un fisier
void serial_implementation(const char *commands_file_path) 
{
    FILE *commands_file = fopen(commands_file_path, "r");
    if (commands_file == NULL) 
    {
        fprintf(stderr, "Failed to open the file\n");
        exit(EXIT_FAILURE);
    }

    char command_buffer[256];
    while (fgets(command_buffer, sizeof(command_buffer), commands_file)) 
    {
        char client[10], command[20], arg[20];
        sscanf(command_buffer, "%s %s %s\n", client, command, arg);

        char filename[256];

        sprintf(filename, "%s_serial.txt", client);
        FILE *output = fopen(filename, "a");
        if (!output) 
        {
            fprintf(stderr, "Failed to open file %s for writing\n", filename);
            exit(EXIT_FAILURE);
        }

        // Proceseaza comanda in functie de tip
        if(strcmp(command, "WAIT") == 0)
        {
            int n = atoi(arg);
            sleep(n);
        }
        else if (strcmp(command, "PRIMES") == 0) 
        {
            int n = atoi(arg);
            int result = count_primes(n);
            fprintf(output, "PRIMES %d -> %d\n", n, result);
        }
        else if (strcmp(command, "PRIMEDIVISORS") == 0) 
        {
            int n = atoi(arg);
            int result = count_prime_divisors(n);
            fprintf(output, "PRIMEDIVISORS %d -> %d\n", n, result);
        } 
        else if (strcmp(command, "ANAGRAMS") == 0) 
        {
            generate_anagrams(arg, output);
        } else {
            printf("Unknown command: %s", command); // Comanda necunoscuta este logata pe stdout
        }

        fclose(output); // Inchide fisierul de output dupa scriere
    }

    fclose(commands_file); // Inchide fisierul de comenzi dupa procesare
}