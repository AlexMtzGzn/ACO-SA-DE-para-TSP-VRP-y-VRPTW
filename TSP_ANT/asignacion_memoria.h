#ifndef ASIGNACION_MEMORIA_H
#define ASIGNACION_MEMORIA_H

#include <stdlib.h>
#include "algoritmo_evolutivo_diferencial.h"

// Prototipos de funciones
individuo *asignar_memoria_arreglo_estructura_individuo(int poblacion);
hormiga * asignar_memoria_arreglo_estructura_hormiga(int numHormigas);
double **asignacion_memoria_instancia(int tamanio_instancia);
double * asignacion_memoria_arreglo(int tamanio_instancia);
#endif // ASIGNACION_MEMORIA_H
