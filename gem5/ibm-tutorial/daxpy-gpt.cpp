#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N 10 // Tamaño de los vectores
#define NUM_THREADS 2 // Número de hilos

double A[N], X[N], Y[N];

void* daxpy(void* arg) {
    int id = *((int*) arg);
    int i, start, end;

    start = id * (N / NUM_THREADS);
    end = (id + 1) * (N / NUM_THREADS);

    for (i = start; i < end; i++) {
        Y[i] =  A[i] + X[i];
    }

    pthread_exit(NULL);
}

int main() {
    int i, rc;
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // Inicializar los vectores A y X
    for (i = 0; i < N; i++) {
        A[i] = i;
        X[i] = i * 2;
    }

    // Crear los hilos
    for (i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        rc = pthread_create(&threads[i], NULL, daxpy, (void*) &thread_ids[i]);
        if (rc) {
            printf("Error: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // Esperar a que los hilos terminen
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
     // Imprimir el vector Y
    for (i = 0; i < N; i++) {
        printf("%f ", A[i]);
    }
    printf("+\n");   // Imprimir el vector Y
    for (i = 0; i < N; i++) {
        printf("%f ", X[i]);
    }
    printf("=====================\n");
    // Imprimir el vector Y
    for (i = 0; i < N; i++) {
        printf("%f ", Y[i]);
    }
    printf("\n");

    pthread_exit(NULL);
}
