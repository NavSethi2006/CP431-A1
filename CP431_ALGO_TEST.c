
/**
 * Author : Navin Sethi
 * ID : 169086962
 * Email : seth6962@mylaurier.ca
 * Desc : Identify the largest gap between prime numbers
 */

#include <stdio.h>
#include <gmp.h>
#include <mpi/mpi.h>

#define LIMIT 10000000

int main (char *argc, int *argv[]) {

    int process;
    int rank;
    int dest;
    int size;
    char message[100];
    MPI_Status s;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_rank(MPI_COMM_WORLD, &size);
    
    MPI_Init(argc, &argv);

    int limit = 0;
    mpz_t max_gap;

    // inital prime number
    mpz_t prime;
    // the prime number that comes before
    mpz_t num_b;

    // initilzed using the GMP library
    mpz_init(prime);
    mpz_init(num_b);
    // set variables to zero

    if(limit != LIMIT) {
        mpz_set_d(prime, 0);
        mpz_set(num_b, prime);

        mpz_nextprime(prime, prime);

        mpz_t gap;
        mpz_sub(gap, prime, num_b);
        if(mpz_cmp(max_gap, gap) > 0) {
            mpz_set(max_gap, gap);
        }
        
        sprintf(message, " Calculating from process %d", rank);

        MPI_Send(message, strlen(message)+1, 
                MPI_CHAR, dest, 0, MPI_COMM_WORLD);

        

        limit++;

        MPI_Finalize();
    }
}