#ifndef CONTROL_MEMORIA_H
#define CONTROL_MEMORIA_H
#include <stdbool.h>
#include "AED.h"

double **asignar_memoria_instancia(int tamanio_instancia);
individuo *asignar_memoria_individuos(int poblacion);
void liberar_instancia(double **instancia, int tamanio_instancia);
void liberar_individuos(individuo *ind, bool bandera);
int *asignar_memoria_ruta(int tamanio_instancia);


#endif // CONTROL_MEMORIA_H
