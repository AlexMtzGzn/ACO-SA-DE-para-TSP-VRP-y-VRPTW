// asignacion_memoria.c
#include <stdlib.h>
#include "asignacion_memoria.h"

individuo *asignar_memoria_arreglo_estructura_individuo(int poblacion)
{
    individuo *ind = (individuo *)malloc(sizeof(individuo) * poblacion);
    return ind;
}

hormiga * asignar_memoria_arreglo_estructura_hormiga(int numHormigas){
    hormiga *hor = (hormiga*)malloc(sizeof(hormiga)*numHormigas);
    return hor;
}

double **asignacion_memoria_instancia(int tamanio_instancia)
{
    double **matriz = (double **)malloc(tamanio_instancia * sizeof(double *));

    for (int i = 0; i < tamanio_instancia; i++)
        matriz[i] = (double *)malloc(tamanio_instancia * sizeof(double));

    return matriz;
}

double * asignacion_memoria_arreglo(int tamanio_instancia){
    double *arreglo = (double*)malloc(tamanio_instancia*sizeof(double));
    return arreglo;
}