#ifndef CONTROL_MEMORIA_H
#define CONTROL_MEMORIA_H

#include <stdbool.h>
#include "../include/estructuras.h"

double **asignar_memoria_instancia(int tamanio_instancia);
void liberar_instancia(double **instancia, int tamanio_instancia);

int *asignar_memoria_arreglo_int(int tamanio_arreglo);
void liberar_memoria_arreglo_int(int *arreglo);

double *asignar_memoria_arreglo_double(int tamanio_arreglo);
void liberar_memoria_arreglo_double(double *arreglo);

struct rangos *asignar_memoria_rangos();
void liberar_rangos(struct rangos *rango);

struct vrp_configuracion *asignar_memoria_vrp_configuracion();
void liberar_memoria_vrp_configuracion(struct vrp_configuracion *vrp);

struct cliente *asignar_memoria_clientes(struct vrp_configuracion *vrp);
struct datos_cliente *asignar_memoria_datos_clientes(int numClientes);
void liberar_memoria_datos_cliente(struct datos_cliente *datos_cliente);

struct lista_ruta *asignar_memoria_lista_ruta();
struct nodo_ruta *asignar_memoria_nodo_ruta();
struct nodo_ruta **asignar_memoria_arreglo_nodo_ruta(int total_clientes);
void liberar_memoria_arreglo_nodo_ruta(struct nodo_ruta **arreglo);
void liberar_memoria_nodo_ruta(struct nodo_ruta *nodo_ruta);
void liberar_lista_ruta(struct lista_ruta *ruta);

struct lista_vehiculos *asignar_memoria_lista_vehiculos();
struct vehiculo *asignar_memoria_vehiculo();
struct nodo_vehiculo *asignar_memoria_nodo_vehiculo();
void liberar_memoria_nodo_vehiculo(struct nodo_vehiculo *nodo_vehiculo);
void liberar_vehiculo(struct vehiculo *vehiculo);
void vaciar_lista_vehiculos(struct lista_vehiculos *flota);
void liberar_lista_vehiculos(struct lista_vehiculos *flota);

struct hormiga *asignar_memoria_hormigas(int numHormigas);
void liberar_memoria_hormiga(struct hormiga *hormiga, int numHormigas);
void reiniciar_hormiga(struct hormiga *hormiga, struct vrp_configuracion *vrp);

struct individuo *asignar_memoria_individuos(int poblacion);
void liberar_individuos(struct individuo *ind, int num_poblacion, bool tipo);

struct mejores_hormigas *asignar_memoria_mejores_hormigas(int numHormigas);
void liberar_memoria_mejores_hormigas(struct mejores_hormigas *mejores_hormigas);


#endif // CONTROL_MEMORIA_H
