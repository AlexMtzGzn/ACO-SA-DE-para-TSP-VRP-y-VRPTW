#ifndef CONTROL_MEMORIA_H
#define CONTROL_MEMORIA_H

#include <stdbool.h>
#include "../include/estructuras.h"

// Función que asigna memoria a una instancia (matriz de distancias o feromonas)
double **asignar_memoria_instancia(int tamanio_instancia);

// Función que libera la memoria de una instancia (matriz de distancias o feromonas)
void liberar_instancia(double **instancia, int tamanio_instancia);

// Función que asigna memoria a un arreglo de enteros
int *asignar_memoria_arreglo_int(int tamanio_arreglo);

// Función que libera la memoria de un arreglo de enteros
void liberar_memoria_arreglo_int(int *arreglo);

// Función que asigna memoria a un arreglo de dobles
double *asignar_memoria_arreglo_double(int tamanio_arreglo);

// Función que libera la memoria de un arreglo de dobles
void liberar_memoria_arreglo_double(double *arreglo);

// Función que asigna memoria a una estructura de rangos
struct rangos *asignar_memoria_rangos();

// Función que libera la memoria de una estructura de rangos
void liberar_rangos(struct rangos *rango);

// Función que asigna memoria al arreglo de clientes
struct cliente *asignar_memoria_clientes(struct vrp_configuracion *vrp);

// Función que liberar memoria al arreglo de clientes
void liberar_memoria_clientes(struct cliente *clientes);

// Función que asigna memoria a una configuración de VRP_TW
struct vrp_configuracion *asignar_memoria_vrp_configuracion();

// Función que libera la memoria de una configuración de VRP_TW
void liberar_memoria_vrp_configuracion(struct vrp_configuracion *vrp);

// Función que asigna memoria a los datos de clientes
struct datos_cliente *asignar_memoria_datos_clientes(int numClientes);

// Función que libera la memoria de los datos de clientes
void liberar_memoria_datos_cliente(struct datos_cliente *datos_cliente);

// Función que asigna memoria a una lista de ruta
struct lista_ruta *asignar_memoria_lista_ruta();

// Función que asigna memoria a un nodo de ruta
struct nodo_ruta *asignar_memoria_nodo_ruta();

// Función que asigna memoria a un arreglo de punteros a nodos de ruta
struct nodo_ruta **asignar_memoria_arreglo_nodo_ruta(int total_clientes);

// Función que libera la memoria de un arreglo de punteros a nodos de ruta
void liberar_memoria_arreglo_nodo_ruta(struct nodo_ruta **arreglo);

// Función que libera la memoria de un nodo de ruta
void liberar_memoria_nodo_ruta(struct nodo_ruta *nodo_ruta);

// Función que libera la memoria de una lista de ruta completa
void liberar_lista_ruta(struct lista_ruta *ruta);

// Función que asigna memoria a una lista de vehículos
struct lista_vehiculos *asignar_memoria_lista_vehiculos();

// Función que asigna memoria a un vehículo
struct vehiculo *asignar_memoria_vehiculo();

// Función que asigna memoria a un nodo de vehículo
struct nodo_vehiculo *asignar_memoria_nodo_vehiculo();

// Función que libera la memoria de un nodo de vehículo
void liberar_memoria_nodo_vehiculo(struct nodo_vehiculo *nodo_vehiculo);

// Función que libera la memoria de un vehículo
void liberar_vehiculo(struct vehiculo *vehiculo);

// Función que vacía la lista de vehículos (sin liberar la lista)
void vaciar_lista_vehiculos(struct lista_vehiculos *flota);

// Función que libera la memoria de la lista completa de vehículos
void liberar_lista_vehiculos(struct lista_vehiculos *flota);

// Función que asigna memoria para un conjunto de hormigas
struct hormiga *asignar_memoria_hormigas(int numHormigas);

// Función que libera la memoria de un conjunto de hormigas
void liberar_memoria_hormiga(struct hormiga *hormiga, int numHormigas);

// Función que reinicia el estado de una hormiga
void reiniciar_hormiga(struct hormiga *hormiga, struct vrp_configuracion *vrp);

// Función que asigna memoria para una población de individuos
struct individuo *asignar_memoria_individuos(int poblacion);

// Función que libera la memoria de los individuos (según tipo de datos)
void liberar_individuos(struct individuo *ind, int num_poblacion, bool tipo);

// Función que asigna memoria para almacenar las mejores hormigas
struct mejores_hormigas *asignar_memoria_mejores_hormigas(int numHormigas);

// Función que libera la memoria de las mejores hormigas
void liberar_memoria_mejores_hormigas(struct mejores_hormigas *mejores_hormigas);

#endif // CONTROL_MEMORIA_H
