// asignacion_memoria.c
#include <stdlib.h>
#include "control_memoria.h"

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

double * asignacion_memoria_arreglo_double(int tamanio_instancia){
    double *arreglo = (double*)malloc(tamanio_instancia*sizeof(double));
    return arreglo;
}

int * asignacion_memoria_arreglo_int(int tamanio_instancia){
    int *arreglo = (int*)malloc(tamanio_instancia*sizeof(int));
    return arreglo;
}

double * asignacion_memoria_variable_double(){
    double *variable = (double *)malloc(sizeof(double));
    return variable;
}

void liberar_memoria_arreglo_estructura_individuo(individuo *ind){
    if (ind != NULL) {
        free(ind);
    }
}

void liberar_memoria_arreglo_estructura_hormiga(hormiga *hor){
    if (hor != NULL) {
        free(hor);
    }
}

void liberar_memoria_instancia(double **matriz, int tamanio_instancia){
    if (matriz != NULL) {
        for (int i = 0; i < tamanio_instancia; i++) {
            if (matriz[i] != NULL) {
                free(matriz[i]); 
            }
        }
        free(matriz);
    }
}

void liberar_memoria_arreglo_double(double *arreglo){
    if (arreglo != NULL) 
        free(arreglo);
}

