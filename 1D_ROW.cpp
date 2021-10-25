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
   int cantidad;   
};

void *process_1D_row(void *threadarg) {
  thread_data * my_data = (thread_data * ) threadarg;

  int acumulador = 0;
  for (int i = 0; i < my_data->cantidad; ++i)
  {
    acumulador += *(my_data->M_interno) * (*(my_data->X_interno));
    my_data->M_interno++;
    my_data->X_interno++;
  }

  *(my_data->y_interno) = acumulador;

  //cout << " Message : " << acumulador<< endl;
   pthread_exit(NULL);
}


int main( int argc, char** argv ) {
  
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
  thread_handles = (pthread_t*)malloc(cant_threads*sizeof(pthread_t));
  
  struct thread_data data_evaluate[cant_threads];

  // Inicio de vector y matrices;

  int M[cant_threads][cant_threads] = {{1,3,4},{5,6,7},{7,8,6}};
  int X[cant_threads] = {2,5,6};
  int Y[cant_threads];


  // Inicio de los threads

  for(int i = 0; i < cant_threads; i++ ) {
      //cout <<"main() : creating thread, " << i << endl;

      data_evaluate[i].M_interno = M[i];
      data_evaluate[i].X_interno = &X[0];
      data_evaluate[i].y_interno = &Y[i];
      data_evaluate[i].cantidad = cant_threads;
      rc = pthread_create(&thread_handles[i], NULL, process_1D_row, (void *) (&data_evaluate[i])); //(void *)&td[i]);
      
      if (rc) {
         cout << "Error para crear thread," << rc << endl;
         exit(-1);
      }
   }

   for(int i = 0; i < cant_threads; i++ ) {
      rc = pthread_join(thread_handles[i], NULL);
      if (rc) {
         cout << "Error, no es posible aplicar join" << rc << endl;
         exit(-1);
      }
      cout << "Thread completado, id :" << i <<endl;
   }


   for (int i = 0; i < cant_threads; ++i)
   {
      cout << Y[i] << endl;
   }
   pthread_exit(NULL);

  return 0;
}
