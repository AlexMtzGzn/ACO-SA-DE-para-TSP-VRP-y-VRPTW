#ifndef CONTROL_MEMORIA_H
#define CONTROL_MEMORIA_H

#include <stdbool.h>
#include "../include/estructuras.h"

// Funciones para asignar y liberar memoria para arreglos e instancias

// Asigna memoria para una instancia (matriz de distancias)
double **asignar_memoria_instancia(int tamanio_instancia);

// Libera la memoria de la instancia (matriz de distancias)
void liberar_instancia(double **instancia, int tamanio_instancia);

// Asigna memoria para un arreglo de enteros de tamaño especificado
int *asignar_memoria_arreglo_int(int tamanio_arreglo);

// Libera la memoria de un arreglo de enteros
void liberar_memoria_arreglo_int(int *arreglo);

// Asigna memoria para un arreglo de números de punto flotante (doble) de tamaño especificado
double *asignar_memoria_arreglo_double(int tamanio_arreglo);

// Libera la memoria de un arreglo de números de punto flotante (doble)
void liberar_memoria_arreglo_double(double *arreglo);

// Asigna memoria para la estructura de rangos.
struct rangos *asignar_memoria_rangos();

// Libera la memoria de la estructura de rangos.
void liberar_rangos(struct rangos *rango);

// Funciones para asignar y liberar memoria para la estructura de individuos

// Asigna memoria para una población de individuos
struct individuo *asignar_memoria_individuos(int poblacion);

// Libera la memoria de los individuos en la población
void liberar_individuos(struct individuo *ind, int num_poblacion, bool tipo);

// Funciones para asignar y liberar memoria para la estructura `vrp_configuracion`

// Asigna memoria para la configuración del VRP (Problema de Ruteo de Vehículos)
struct vrp_configuracion *asignar_memoria_vrp_configuracion();

// Libera la memoria utilizada por la configuración del VRP
void liberar_memoria_vrp_configuracion(struct vrp_configuracion *vrp);

// Funciones para asignar memoria para la estructura de clientes

// Asigna memoria para la estructura de clientes según la configuración del VRP
struct cliente *asignar_memoria_clientes(struct vrp_configuracion *vrp);

struct datos_cliente *asignar_memoria_datos_clientes(int numClientes);
void liberar_memoria_datos_cliente(struct datos_cliente *datos_cliente);

// Funciones para asignar y liberar memoria para la estructura de hormigas

// Asigna memoria para un conjunto de hormigas
struct hormiga *asignar_memoria_hormigas(int numHormigas);

// Libera la memoria de las hormigas y de los individuos relacionados
void liberar_memoria_hormiga(struct hormiga *hormiga, int numHormigas);

// Reinicia los valores de las hormigas para una nueva iteración
void reiniciar_hormiga(struct hormiga *hormiga, struct vrp_configuracion *vrp);

// Funciones para asignar memoria para las estructuras de mejores hormigas

// Asigna memoria para un arreglo de mejores hormigas
struct mejores_hormigas *asignar_memoria_mejores_hormigas(int numHormigas);

// Libera memoria para una estructura de arreglos de hormiga
void liberar_memoria_mejores_hormigas(struct mejores_hormigas *mejores_hormigas);

// Funciones para asignar memoria para las estructuras de rutas

// Asigna memoria para una lista de rutas
struct lista_ruta *asignar_memoria_lista_ruta();

// Asigna memoria para un nodo de ruta
struct nodo_ruta *asignar_memoria_nodo_ruta();

// Libera memoria del nodo de ruta
void liberar_memoria_nodo_ruta(struct nodo_ruta *nodo_ruta);

// Asigna memoria al arreglo de nodo ruta
struct nodo_ruta **asignar_memoria_arreglo_nodo_ruta(int total_clientes);

// Libera memoria de arreglo de nodo ruta
void liberar_memoria_arreglo_nodo_ruta(struct nodo_ruta **arreglo);

// Funciones para asignar memoria para las estructuras de vehículos

// Asigna memoria para una lista de vehículos
lista_vehiculos *asignar_memoria_lista_vehiculos();

// Asigna memoria para un vehículo
struct vehiculo *asignar_memoria_vehiculo();

// Asigna memoria para un nodo de vehículo
struct nodo_vehiculo *asignar_memoria_nodo_vehiculo();

// Funciones para liberar memoria de vehículos

// Libera la memoria de un nodo de vehículo.
void liberar_memoria_nodo_vehiculo(struct nodo_vehiculo *nodo_vehiculo);

// Libera la memoria de una lista de rutas.
void liberar_lista_ruta(struct lista_ruta *lista);

#endif // CONTROL_MEMORIA_H
