#include "utils.h"


// Verifica daca un numar este prim
int is_prime(int number) 
{
    if (number < 2) 
    { 
        return 0;
    }

    // Parcurge divizorii pana la sqrt(number) pentru optimizare
    for (int i = 2; i <= sqrt(number); i++) 
    {
        if (number % i == 0) 
        {
            return 0;
        }
    }

    return 1;
}


// Numara toate numerele prime pana la un anumit numar
int count_primes(int number) 
{
    int counter = 0;

    for (int i = 2; i <= number; i++) 
    {
        if (is_prime(i))
        {
            counter++;
        }
    }

    return counter;
}


// Numara divizorii primi ai unui numar
int count_prime_divisors(int number) 
{
    int counter = 0;

    for (int i = 2; i <= number; i++) 
    {
        if ((number % i == 0) && is_prime(i)) 
        {
            counter++;
        }
    }

    return counter;
}



static void swap(char *a, char *b) 
{
    char temp = *a;
    *a = *b;
    *b = temp;
}


// Functie recursiva pentru generarea anagramelor unui sir
static void generate_anagrams_recursive(char *str, int start, int end, FILE *output) 
{
    if (start == end) 
    {
        fprintf(output, "%s\n", str); // Scriem permutarea completa in fisier.
    } 
    else
    {
        for (int i = start; i <= end; i++) 
        {
            swap(&str[start], &str[i]);
            generate_anagrams_recursive(str, start + 1, end, output);
            swap(&str[start], &str[i]);
        }
    }
}


// Functie pentru generarea anagramelor unui cuvant si scrierea lor intr-un fisier
void generate_anagrams(const char *word, FILE *output) 
{
    char buffer[256];

    // Copiem cuvantul in buffer, fara a depasi dimensiunea maxima
    strncpy(buffer, word, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    int len = strlen(buffer);
    generate_anagrams_recursive(buffer, 0, len - 1, output);
}
