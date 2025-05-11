#ifndef CONTROL_MEMORIA_H
#define CONTROL_MEMORIA_H

// Incluimos las bibliotecas necesarias
#include <stdbool.h>
#include "../include/estructuras.h"

// Funciones para asignar y liberar memoria para arreglos e instancias

// Asigna memoria para una instancia (matriz de distancias)
double **asignar_memoria_instancia(int tamanio_instancia);

// Asigna memoria para un arreglo de enteros de tamaño especificado
int *asignar_memoria_arreglo_int(int tamanio_arreglo);

// Asigna memoria para un arreglo de números de punto flotante (doble) de tamaño especificado
double *asignar_memoria_arreglo_double(int tamanio_arreglo);

// Libera la memoria de un arreglo de enteros
void liberar_memoria_arreglo_int(int *arreglo);

// Libera la memoria de un arreglo de números de punto flotante (doble)
void liberar_memoria_arreglo_double(double *arreglo);

// Libera la memoria de la instancia (matriz de distancias)
void liberar_instancia(double **instancia, int tamanio_instancia);

struct rangos *asignar_memoria_rangos();
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


// Funciones para asignar y liberar memoria para la estructura de hormigas

// Asigna memoria para un conjunto de hormigas
struct hormiga *asignar_memoria_hormigas(int numHormigas);

// Libera la memoria de las hormigas y de los individuos relacionados
void liberar_memoria_hormiga(struct hormiga *hormiga, struct individuo *ind);

// Reinicia los valores de las hormigas para una nueva iteración
void reiniciar_hormiga(struct hormiga *hormiga, struct vrp_configuracion *vrp);


// Funciones para asignar memoria para las estructuras de rutas

// Asigna memoria para una lista de rutas
struct lista_ruta *asignar_memoria_lista_ruta();

// Asigna memoria para un nodo de ruta
struct nodo_ruta *asignar_memoria_nodo_ruta();


// Funciones para asignar memoria para las estructuras de vehículos

// Asigna memoria para una lista de vehículos
lista_vehiculos* asignar_memoria_lista_vehiculos();

// Asigna memoria para un vehículo
struct vehiculo *asignar_memoria_vehiculo();

// Asigna memoria para un nodo de vehículo
struct nodo_vehiculo *asignar_memoria_nodo_vehiculo();

struct metal *asignar_memoria_metal();
void liberar_memoria_metal(struct individuo *ind);

#endif // CONTROL_MEMORIA_H
