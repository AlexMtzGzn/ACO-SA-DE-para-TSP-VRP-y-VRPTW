#ifndef CONTROL_MEMORIA_H
#define CONTROL_MEMORIA_H
#include <stdbool.h>
#include "AED.h"

 double **asignar_memoria_instancia(int tamanio_instancia);
 individuo *asignar_memoria_individuos(int poblacion);
// generacion *asignar_memoria_generaciones(int poblacion, int generaciones);
// void liberar_instancia(double **instancia, int tamanio_instancia);
// void liberar_individuos(individuo *ind, bool bandera);
// hormiga *asignar_memoria_hormiga(individuo *ind);
// int *asignar_memoria_ruta(int tamanio_instancia);
// double *asignar_memoria_posibilidades(int tamanio_instancia);
// void liberar_hormigas(hormiga *hor, individuo *ind);

#endif // CONTROL_MEMORIA_H
