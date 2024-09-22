#ifndef CONTROL_MEMORIA_H
#define CONTROL_MEMORIA_H

#include <stdlib.h>
#include "algoritmo_evolutivo_diferencial.h"
#include "tsp_ant.h"

individuo *asignar_memoria_arreglo_estructura_individuo(int poblacion);
hormiga * asignar_memoria_arreglo_estructura_hormiga(int numHormigas);
double **asignacion_memoria_instancia(int tamanio_instancia);
double * asignacion_memoria_arreglo_double(int tamanio_instancia);
int * asignacion_memoria_arreglo_int(int tamanio_instancia);
double * asignacion_memoria_variable_double();
void liberar_memoria_arreglo_estructura_individuo(individuo *ind);
void liberar_memoria_arreglo_estructura_hormiga(hormiga *hor);
void liberar_memoria_instancia(double **matriz, int tamanio_instancia);
void liberar_memoria_arreglo_double(double *arreglo);
void liberar_memoria_arreglo_int(int *arreglo);
void liberar_memoria_variable_double(double *variable);
#endif // CONTROL_MEMORIA_H
