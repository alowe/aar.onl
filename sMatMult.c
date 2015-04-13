#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
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

int main(int argc, char *argv[])
{
  int seed = 5;
  int m, d, n; 
  m = 2; d = 2; n = 2; //defaults to 2x2 times 2x2
  int print = 0;
  if (argc == 2){
    //multiplying a n*n by n*n
    m = atoi(argv[1]);
    n = atoi(argv[1]);
    d = atoi(argv[1]);
  }
  if (argc == 3){
    //multiplying a n*d by d*n
    m = atoi(argv[1]);
    n = atoi(argv[1]);
    d = atoi(argv[2]);
    if (d == 0){
      print = 0;
      d = m;
    }
  }
  if (argc > 3){
    //multiplying a n*d by d*m
    m = atoi(argv[1]);
    n = atoi(argv[2]);
    d = atoi(argv[3]);
    if (argc > 4){
      print = atoi(argv[4]);
    }
  }
  //}
  srand(seed);
  clock_t cstart = clock();
  int* matrix = make_matrix(m,d);
  int* matrix2 = make_matrix(d,n);
  int* result = mult(matrix, matrix2, m, d, n);
  clock_t cend = clock();

  clock_t elapsed = cend-cstart;
  double t_elapsed = 1.0 * elapsed / CLOCKS_PER_SEC;
  //cend = time(NULL);
  if(print){
    //don't include printing in time of multiplying
    print_matrix(matrix,  m, d);
    printf("\n");
    print_matrix(matrix2, d, n);
    printf("\n");
    print_matrix(result, m, n);
  }
  printf("Process took %f seconds\n", t_elapsed);
  //FILE *pFile;
  //pFile = fopen("results.csv", "a");
  //# trials, total score, average score, time, distribution
  //fprintf(pFile, "%d, %d, %f, %f, %d\n", ntrials, score, average, t_elapsed, distribution);
  //fprintf(pFile, "%d, %f, %f, %d, %f, %d, %d\n", ntrials, x, y, (int) score, average, precision, distribution);
  //fclose(pFile);
}
