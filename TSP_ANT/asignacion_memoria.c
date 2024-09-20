// asignacion_memoria.c
#include <stdlib.h>
#include "asignacion_memoria.h"

individuo *asignar_memoria_arreglo_estructura(int poblacion)
{
    individuo *ind = (individuo *)malloc(sizeof(individuo) * poblacion);
    return ind;
}

double **asignacion_memoria_instancia(int tamanio_instancia)
{
    double **matriz = (double **)malloc(tamanio_instancia * sizeof(double *));

    for (int i = 0; i < tamanio_instancia; i++)
        matriz[i] = (double *)malloc(tamanio_instancia * sizeof(double));

    return matriz;
}