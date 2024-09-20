// asignacion_memoria.c
#include <stdlib.h>
#include "asignacion_memoria.h"

individuo *asignar_memoria_arreglo_estructura(int poblacion)
{
    individuo *ind = (individuo *)malloc(sizeof(individuo) * poblacion);
    return ind;
}

double **asignacion_memoria_intancias(int tamanio)
{
    double **matriz = (double **)malloc(tamanio * sizeof(double *));

    for (int i = 0; i < tamanio; i++)
        matriz[i] = (double *)malloc(tamanio * sizeof(double));

    return matriz;
}