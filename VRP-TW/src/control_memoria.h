#ifndef CONTROL_MEMORIA_H
#define CONTROL_MEMORIA_H
#include <stdbool.h>
#include "aed.h"
#include "configuracion_vrp_tw.h"

/*Para arreglos y instancias*/
double **asignar_memoria_instancia(int tamanio_instancia);
int *asignar_memoria_arreglo(int tamanio_arreglo);
int *reasignar_memoria_arreglo(vehiculo *vehiculo);
void liberar_instancia(double **instancia, int tamanio_instancia);

/*Para estructura del individuo*/
individuo *asignar_memoria_individuos(int poblacion);
void liberar_individuos(individuo *ind, bool bandera);

/*Para la estructura vrp_configuracion*/
vrp_configuracion *asignar_memoria_vrp_configuracion();

/*Para la estructura clientes*/
cliente *asignar_memoria_clientes(struct vrp_configuracion *vrp);

/*Para la estructura de la hormiga*/
hormiga *asignar_memoria_hormiga(individuo *ind);


/*Para la estructura clientes*/
vehiculo *asignar_memoria_vehiculo();



// generacion *asignar_memoria_generaciones(int poblacion, int generaciones);
// void liberar_instancia(double **instancia, int tamanio_instancia);
// hormiga *asignar_memoria_hormiga(individuo *ind);
// int *asignar_memoria_ruta(int tamanio_instancia);
// double *asignar_memoria_posibilidades(int tamanio_instancia);
// void liberar_hormigas(hormiga *hor, individuo *ind);

#endif // CONTROL_MEMORIA_H
