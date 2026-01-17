
/**
 * Author : Manveer Sohal
 * ID : 169033553
 * Email : soha3356@mylaurier.ca
 * Desc : MPI Test
 */


#include <stdio.h>
#include <gmp.h>
#include "mpi.h"
#include <string.h>



// /**
//  * Get the prime number
//  * @param mpz_t limit - the limit number
//  * @return the prime number
//  */
// mpz_t get_prime(mpz_t limit) {
//     mpz_t prime;
//     mpz_init(prime);
//     mpz_set_ui(prime, 1);
//     while( mpz_cmp(prime, limit) < 0) {
//         mpz_nextprime(prime, prime);
//     }
//     return prime;
// }

/**
 * Create the limit number
 * @param int limit - the limit number
 * @return void
 */
void create_limit(mpz_t limit, int limit_num) {
    mpz_init(limit);
    mpz_set_ui(limit, limit_num);
}



/**
 * Find the largest prime number less than the limit
 * @param mpz_t prime - the prime number
 * @param mpz_t limit - the limit number
 * @return void
 */
void find_largest_prime_in_interval(mpz_t prime, mpz_t start, mpz_t end) {
    mpz_init(prime); // initialize the prime number
    mpz_set(prime, start); // set the prime number to the start intercal

    // find the largest prime number less than the end interval
    while( mpz_cmp(prime, end) < 0) {
        mpz_nextprime(prime, prime);
    }
}

/**
 * Find the largest prime number less than the limit
 * @param mpz_t gap - the gap between the prime numbers
 * @param mpz_t limit - the limit number
 * @return void
 */
void find_largest_prime_gap_in_interval(int *ret_gap, mpz_t start, mpz_t end) {

    mpz_t gap; // gap between the prime numbers
    mpz_init(gap); // initialize the gap between the prime numbers
    mpz_set_ui(gap, 0); // set the gap between the prime numbers to 0

    mpz_t max_gap; // maximum gap between the prime numbers
    mpz_init(max_gap); // initialize the maximum gap between the prime numbers
    mpz_set_ui(max_gap, 0); // set the maximum gap between the prime numbers to 0


    mpz_t prime_prev; // previous prime number
    mpz_init(prime_prev); // initialize the prime number that comes before

    mpz_t prime_cur; // current prime number
    mpz_init(prime_cur); // initialize the current prime number



   

    // find the largest prime number less than the end interval
    while( mpz_cmp(prime_cur, end) < 0) {
        mpz_set(prime_prev, prime_cur); // set the prime_prev to prime_cur
        mpz_nextprime(prime_cur, prime_cur); // find the next prime number
        mpz_sub(gap, prime_cur, prime_prev); // find the gap between the prime numbers
        if(mpz_cmp(gap, max_gap) > 0) {
            mpz_set(max_gap, gap); // set the maximum gap between the prime numbers to the gap
        }
    }
    *ret_gap = mpz_get_ui(max_gap); // get the maximum gap between the prime numbers

}

int main (int argc, char *argv[]) {

    int process; // number of processes
    int rank; // rank of the process
    int dest; // rank of receiver
    int source; // rank of sender
    int tag = 0; // tag of the message

    
    char message[100]; // message to be sent

    MPI_Status status; // status of the message
   

    // start up MPI
    MPI_Init(&argc, &argv);

    // get the rank and size of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &process);
    // printf("Hello from rank %d out of %d\n", rank, size);


    if(rank != 0) {
        int start_num = 10; // start of the interval
        int end_num = 40; // end of the interval
        int gap = 0; // gap between the prime numbers

        mpz_t start; // start of the interval
        mpz_t end; // end of the interval

        create_limit(start,start_num); // create the start of the interval
        create_limit(end,end_num); // create the end of the interval


        // mpz_t prime; // prime number

        // find_largest_prime_in_interval(prime, start, end);
        find_largest_prime_gap_in_interval(&gap, start, end);
       
        
        sprintf(message, "Greeting from process %d, the largest gap is %d", rank, gap);

        dest = 0;
        MPI_Send(message, strlen(message)+1, MPI_CHAR, dest, 0, MPI_COMM_WORLD);
    }
    
    else {
        
        for(source = 1; source < process; source++) {
            MPI_Recv(message, 100, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
            printf("%s\n", message);
        }
    }
    MPI_Finalize();
}