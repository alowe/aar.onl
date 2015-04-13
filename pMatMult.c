/*
Parallel version of compute Dart  
Based on the parallel computation of pi

Program to compute Pi using Monte Carlo methods
http://www.dartmouth.edu/~rc/classes/soft_dev/C_simple_ex.html
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#define SEED 35791247
#include <mpi.h>   /* PROVIDES THE BASIC MPI DEFINITION AND TYPES */
#include<time.h>
int* make_matrix(int m, int n){
  //m is rows, n is columns
  int i, j;
  int* matrix = (int *) malloc(m*n*sizeof(int));
  for(i=0; i<m; i++){
    for (j = 0; j < n; j++){
      matrix[i*n+j] = (rand() % 199)-99;
    }
  }
  return matrix;
}

int* make_zero(int m, int n){
  //m is rows, n is columns
  int i, j;
  int* matrix = (int *) malloc(m*n*sizeof(int));
  for(i=0; i<m; i++){
    for (j = 0; j < n; j++){
      matrix[i*n+j] = 0;
    }
  }
  return matrix;
}

void print_matrix(int* matrix, int m, int n){
  int i, j;
  //printf("[");
  for(i=0; i<m; i++){
    printf("[");
    for (j = 0; j < n; j++){
      printf("%2d, ", matrix[i*n+j]);
    }
    printf("\b]\n");
  }
  //printf("\b]\n");
}

int* mult(int* m1, int* m2, int m, int d, int n){
  //m1 is MxD matrix m2 is a DxN matrix
  int* result = (int *) malloc(m*n*sizeof(int));
  memset(result, 0, sizeof(int)*m*n);
  int i, j, k;
  for(i=0; i<m; i++){
    for (j = 0; j < n; j++){
      for (k = 0; k < d; k++){
        result[i*n+j] += m1[i*d+k]*m2[k*n+j]; //dot product of row i m1 with column j m2
      }
    }
  }
  return result;
}


int main(int argc, char **argv)
{
	int my_rank; 
	int partner;
	int size,t;
	MPI_Status status;

	long i;
	long buffer = 0;
    int logging = 1;

	// to get node name, from http://www.cisl.ucar.edu/docs/lightning/examples/mpi.jsp
	int name_len;
	char nodename[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv); /*START MPI */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); /*DETERMINE RANK OF THIS PROCESSOR*/
	MPI_Comm_size(MPI_COMM_WORLD, &size); /*DETERMINE TOTAL NUMBER OF PROCESSORS*/
	MPI_Get_processor_name(nodename, &name_len);
	long niter = 3;
    if (argc >= 2){
      niter = atoi(argv[1]);
    }

	MPI_Barrier(MPI_COMM_WORLD); //Start all processes at the same time
	clock_t cstart = clock();


	if (my_rank == 0)	// master
	{
        int* result = make_zero(niter, niter);
        int* matrix = make_matrix(niter, niter); //we will be squaring this matrix
        //print_matrix(matrix, niter, niter);
		int i;
        for (i=1; i < size; i++){
            //pass matrix to processes
            MPI_Send(matrix, niter*niter, MPI_INT, i, 123, MPI_COMM_WORLD);
        }
		for (i = 0; i < niter*niter; i++)
		{
            int buffer[2];
			MPI_Recv(&buffer, 2, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            result[buffer[0]] = buffer[1];
		}
        //print_matrix(result, niter, niter);
        clock_t cend = clock(); 
        clock_t elapsed = cend-cstart;
        double t_elapsed = 1.0 * elapsed / CLOCKS_PER_SEC;
		printf("size of matrix= %ld x %ld on %d nodes in %f", niter, niter,size, t_elapsed);
		//printf("Total Elapsed Time: %f\n",t_elapsed);
        if(logging){
                FILE *pFile;
                pFile = fopen("results.csv", "a");
                //# trials, total score, average score, time, distribution
                //fprintf(pFile, "%d, %d, %f, %f, %d\n", ntrials, score, average, t_elapsed, distribution);
                fprintf(pFile, "%d, %d,  %f\n", niter, size, t_elapsed);
                fclose(pFile);
        }
	}
	else	// must be a worker
	{
        int* matrix = (int *) malloc(niter*niter*sizeof(int));
        MPI_Recv(matrix, niter*niter, MPI_INT, 0, 123, MPI_COMM_WORLD, &status);
		int i;
		for (i = 0; i < niter; i++){
            if ((i % (size-1)) == (my_rank-1)){
                //this is a row we want this process to multiply
                int j; //column
                for (j = 0; j < niter; j++){
                    int cell[2] = {0, 0};
                    cell[0] = i*niter+j;
                    int k;
                    for (k=0; k<niter; k++){
                        cell[1] += matrix[i*niter+k]*matrix[k*niter+j];
                    }
                    MPI_Send(&cell, 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
                }
            }
        }
	}

	printf("processor %d of %d on %s exitting ...\n", my_rank, size, nodename);
	MPI_Finalize();  /* EXIT MPI */
}

