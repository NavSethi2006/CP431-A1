
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
        mpz_t prime;
        mpz_init(prime);
        mpz_set_d(prime, 101);

        char prime_str[100];
        mpz_get_str(prime_str, 10, prime);
        
        sprintf(message, "Greeting from process %d, the prime number is %s", rank, prime_str);
        
        dest = 0;
        // uses strlen+1 so that '/0' gets transmitted
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