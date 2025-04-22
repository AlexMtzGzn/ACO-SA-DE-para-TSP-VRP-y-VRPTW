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

// Funciones para asignar y liberar memoria para la estructura de rangos
// Asigna memoria para los rangos
struct rangos *asignar_memoria_rangos();
// Libera la memoria de los rangos
void liberar_rangos(struct rangos *rango);

// Funciones para asignar y liberar memoria para la estructura de individuos
//Asiganamos la memoria a los individuos 
struct individuo *asignar_memoria_individuos(int poblacion);
// Libera la memoria de los individuos en la población
void liberar_individuos(struct individuo *ind, int num_poblacion, bool tipo);


// Funciones para asignar y liberar memoria para la estructura `tsp_configuracion`

// Asigna memoria para la configuración del tsp 
struct tsp_configuracion *asignar_memoria_tsp_configuracion();

// Libera la memoria utilizada por la configuración del tsp
void liberar_memoria_tsp_configuracion(struct tsp_configuracion *tsp);


// Funciones para asignar memoria para la estructura de clientes

// Asigna memoria para la estructura de clientes según la configuración del TSP
struct cliente *asignar_memoria_clientes(struct tsp_configuracion *tsp);


// Funciones para asignar y liberar memoria para la estructura de hormigas

// Asigna memoria para un conjunto de hormigas
struct hormiga *asignar_memoria_hormigas(int numHormigas);

// Libera la memoria de las hormigas y de los individuos relacionados
void liberar_memoria_hormiga(struct hormiga *hormiga);

// Reinicia los valores de las hormigas para una nueva iteración
void reiniciar_hormiga(struct hormiga *hormiga, struct individuo *ind, struct tsp_configuracion *tsp);

// Asigna la memoria del metal
struct metal * asignar_memoria_metal();

//Libera la memoria del metal
void liberar_memoria_metal(struct individuo * ind);

// Funciones para asignar memoria para las estructuras de rutas

// Asigna memoria para una lista de rutas
struct lista_ruta *asignar_memoria_lista_ruta();

// Asigna memoria para un nodo de ruta
struct nodo_ruta *asignar_memoria_nodo_ruta();


#endif // CONTROL_MEMORIA_H
