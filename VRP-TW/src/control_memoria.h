#ifndef CONTROL_MEMORIA_H
#define CONTROL_MEMORIA_H

#include <stdbool.h>
#include "vrp_tw_aco.h"
#include "aed.h"
#include "configuracion_vrp_tw.h"

/*Para arreglos y instancias*/
double **asignar_memoria_instancia(int tamanio_instancia);
int *asignar_memoria_arreglo(int tamanio_arreglo);
int *reasignar_memoria_ruta(int *ruta, int nuevos_clientes);
void liberar_instancia(double **instancia, int tamanio_instancia);

/*Para estructura del individuo*/
struct individuo *asignar_memoria_individuos(int poblacion);
void liberar_individuos(struct individuo *ind, bool bandera);

/*Para la estructura vrp_configuracion*/
struct vrp_configuracion *asignar_memoria_vrp_configuracion();

/*Para la estructura clientes*/
struct cliente *asignar_memoria_clientes(struct vrp_configuracion *vrp);

/*Para la estructura de la hormiga*/
struct hormiga *asignar_memoria_hormiga(struct individuo *ind);

/*Para la estructura clientes*/
struct vehiculo *asignar_memoria_vehiculo(struct vrp_configuracion *vrp);

#endif // CONTROL_MEMORIA_H
