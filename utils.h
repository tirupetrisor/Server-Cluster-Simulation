#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <mpi.h>


// Tag pentru mesajele cu task-uri trimise workerilor
#define TASK_TAG 1

// Tag pentru mesajele cu rezultate primite de la workeri
#define RESULT_TAG 2

// Tag pentru mesajele de terminare trimise workerilor
#define TERMINATE_TAG 3

// Numărul maxim de comenzi care pot fi procesate
#define MAX_COMMANDS 1024


int is_prime(int n);
int count_primes(int N);
int count_prime_divisors(int N);
void generate_anagrams(const char *word, FILE *output);

#endif