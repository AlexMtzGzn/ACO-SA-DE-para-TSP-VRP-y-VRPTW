#ifndef CONTROL_MEMORIA_H
#define CONTROL_MEMORIA_H

#include <stdbool.h>
#include "aed.h"
#include "configuracion_vrp_tw.h"

/*Para arreglos y instancias*/
double **asignar_memoria_instancia(int tamanio_instancia);
int *asignar_memoria_arreglo_int(int tamanio_arreglo);
double *asignar_memoria_arreglo_double(int tamanio_instancia);
void liberar_instancia(double **instancia, int tamanio_instancia);

/*Para estructura del individuo*/
struct individuo *asignar_memoria_individuos(int poblacion);
void liberar_individuos(struct individuo *ind, bool bandera);

/*Para la estructura vrp_configuracion*/
struct vrp_configuracion *asignar_memoria_vrp_configuracion();

/*Para la estructura clientes*/
struct cliente *asignar_memoria_clientes(struct vrp_configuracion *vrp);




#endif // CONTROL_MEMORIA_H
