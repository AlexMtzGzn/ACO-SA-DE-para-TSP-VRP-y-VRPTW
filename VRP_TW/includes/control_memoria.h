#ifndef CONTROL_MEMORIA_H
#define CONTROL_MEMORIA_H

#include <stdbool.h>
#include "../includes/estructuras.h"


/*Para arreglos y instancias*/
double **asignar_memoria_instancia(int tamanio_instancia);
int *asignar_memoria_arreglo_int(int tamanio_arreglo);
double *asignar_memoria_arreglo_double(int tamanio_arreglo);
void liberar_memoria_arreglo_int(int *arreglo);
void liberar_memoria_arreglo_double(double *arreglo);
void liberar_instancia(double **instancia, int tamanio_instancia);

/*Para estructura del individuo*/
struct individuo *asignar_memoria_individuos(int poblacion);
void liberar_individuos(struct individuo *ind, int num_poblacion, bool tipo);
/*Para la estructura vrp_configuracion*/
struct vrp_configuracion *asignar_memoria_vrp_configuracion();
void liberar_memoria_vrp_configuracion(struct vrp_configuracion *vrp);

/*Para la estructura clientes*/
struct cliente *asignar_memoria_clientes(struct vrp_configuracion *vrp);

/*Para estructura de hormiga*/
struct hormiga *asignar_memoria_hormigas(int numHormigas);
void liberar_memoria_hormiga(struct hormiga * hormiga, struct individuo * ind);
void reiniciar_hormiga(struct hormiga *hormiga, struct vrp_configuracion *vrp);
/*Para estructura de lista_ruta*/
struct lista_ruta * asignar_memoria_lista_ruta();

/*Para estructura de lista_vehiculos*/
lista_vehiculos* asignar_memoria_lista_vehiculos();

struct vehiculo * asignar_memoria_vehiculo();
struct nodo_vehiculo *asignar_memoria_nodo_vehiculo();

struct nodo_ruta *asignar_memoria_nodo_ruta();
#endif // CONTROL_MEMORIA_H
