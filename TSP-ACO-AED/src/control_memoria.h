#ifndef CONTROL_MEMORIA_H
#define CONTROL_MEMORIA_H
#include <stdbool.h>
#include "AED.h"
#include "TSP_ACO.h"

double ** asignar_memoria_instancia(int tamanio_instancia);
individuo * asignar_memoria_individuos(int poblacion);
void liberar_instancia(double ** instancia,int tamanio_instancia);
void liberar_individuos(individuo * ind, int poblacion, bool bandera);
hormiga * asignar_memoria_hormiga(individuo *ind);
int * asignar_memoria_ruta(int tamanio_instancia);
#endif // CONTROL_MEMORIA_H
