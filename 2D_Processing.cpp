#include <iostream>
#include <vector>
#include <math.h> 
#include "hpc_helpers.hpp"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

using namespace std;

struct thread_data {
   int * M_interno;
   int * X_interno;
   int * y_interno;
};

void *process_2D(void *threadarg) {
  thread_data * my_data = (thread_data * ) threadarg;
  int acumulador = *(my_data->M_interno) * (*(my_data->X_interno));


  *(my_data->y_interno) += acumulador;

  //cout << " Message : " << acumulador<< endl;
   pthread_exit(NULL);
}


void Gen_matrix(int ** A, int n) {
   int i, j;
   for (i = 0; i < n; i++)
      for (j = 0; j < n; j++)
         A[i][j] = rand() % 1000;
}



void Gen_vector(int *x, int n) {
   int i;
   for (i = 0; i < n; i++)
      x[i] = rand() % 1000;
}


int main( int argc, char** argv ) {
  srand (time(NULL));
  if(argc != 2) 
  {
    std::cout << "Establezca los threads" << std::endl;
    return -1;
  } 

  // Manejo de threads

  if (argv[1] == NULL) return -1;

  long thread;
  int cant_threads, rc;
  pthread_t* thread_handles;
  double start, finish;
  cant_threads = strtol(argv[1], NULL, 10);
  thread_handles = (pthread_t*)malloc(cant_threads*cant_threads*sizeof(pthread_t));
  
  struct thread_data data_evaluate[cant_threads*cant_threads];

  // Inicio de vector y matrices;

  //int M[cant_threads][cant_threads] = {{1,3,4},{5,6,7},{7,8,6}};
  int Y[cant_threads];
  int **M = new int *[cant_threads];
  for (int i = 0; i < cant_threads; ++i)
  {
    M[i] = new int[cant_threads];
    Y[i] = 0;
  }

  int *X = new int[cant_threads];
  


  Gen_matrix(M, cant_threads);
  Gen_vector(X, cant_threads);

  // Inicialización y seteo de los thread para que sean juntables(joinables)

  pthread_attr_t attr;
  void *status;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  // Inicio de los threads


  TIMERSTART(start);
  for(int i = 0; i < cant_threads; i++ ) {
      //cout <<"main() : creating thread, " << i << endl;
      
      for (int j = 0; j < cant_threads; ++j)
      {

        data_evaluate[i*cant_threads+j].M_interno = &M[j][i];
        data_evaluate[i*cant_threads+j].X_interno = &X[i];
        data_evaluate[i*cant_threads+j].y_interno = &Y[j];
        rc = pthread_create(&thread_handles[i*cant_threads+j], &attr, process_2D, (void *) (&data_evaluate[i*cant_threads+j])); //(void *)&td[i]);
        
        if (rc) {
           cout << "Error para crear thread," << rc << endl;
           exit(-1);
        }
      }
      
   }

   for(int i = 0; i < cant_threads*cant_threads; i++ ) {
      rc = pthread_join(thread_handles[i], &status);

      if (rc) {
         cout << "Error, no es posible aplicar join" << rc << endl;
         exit(-1);
      }
   }

   TIMERSTOP(start);
   for (int i = 0; i < cant_threads; ++i)
   {
      cout << Y[i] << endl;
   }
   pthread_exit(NULL);

  return 0;
}
