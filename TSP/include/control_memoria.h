#ifndef CONTROL_MEMORIA_H
#define CONTROL_MEMORIA_H

// Bibliotecas necesarias
#include <stdbool.h>
#include "../include/estructuras.h"

// ==========================
// Asignación y liberación de memoria para estructuras básicas
// ==========================

// Asigna memoria para una instancia (matriz de distancias)
double **asignar_memoria_instancia(int tamanio_instancia);

// Libera la memoria de una instancia (matriz de distancias)
void liberar_instancia(double **instancia, int tamanio_instancia);

// Asigna memoria para un arreglo de enteros
int *asignar_memoria_arreglo_int(int tamanio_arreglo);

// Libera la memoria de un arreglo de enteros
void liberar_memoria_arreglo_int(int *arreglo);

// Asigna memoria para un arreglo de doubles
double *asignar_memoria_arreglo_double(int tamanio_arreglo);

// Libera la memoria de un arreglo de doubles
void liberar_memoria_arreglo_double(double *arreglo);

// ==========================
// Estructura de rangos
// ==========================

// Asigna memoria para la estructura de rangos
struct rangos *asignar_memoria_rangos();

// Libera la memoria de la estructura de rangos
void liberar_rangos(struct rangos *rango);

// ==========================
// Estructura de individuos
// ==========================

// Asigna memoria para una población de individuos
struct individuo *asignar_memoria_individuos(int poblacion);

// Libera la memoria de los individuos
void liberar_individuos(struct individuo *ind, int num_poblacion, bool tipo);

// ==========================
// Estructura de configuración TSP
// ==========================

// Asigna memoria para la configuración del TSP
struct tsp_configuracion *asignar_memoria_tsp_configuracion();

// Libera la memoria de la configuración del TSP
void liberar_memoria_tsp_configuracion(struct tsp_configuracion *tsp);

// ==========================
// Estructura de clientes
// ==========================

// Asigna memoria para los clientes según la configuración TSP
struct cliente *asignar_memoria_clientes(struct tsp_configuracion *tsp);

// ==========================
// Estructura de hormigas
// ==========================

// Asigna memoria para un conjunto de hormigas
struct hormiga *asignar_memoria_hormigas(int numHormigas);

// Libera la memoria de las hormigas y de los individuos relacionados
void liberar_memoria_hormiga(struct hormiga *hormiga);

// Reinicia los valores de las hormigas para una nueva iteración
void reiniciar_hormiga(struct hormiga *hormiga, struct individuo *ind, struct tsp_configuracion *tsp);

// ==========================
// Estructura metal (mejor individuo)
// ==========================

// Asigna memoria para la estructura del mejor individuo (metal)
struct metal *asignar_memoria_metal();

// Libera la memoria del metal
void liberar_memoria_metal(struct individuo *ind);

// ==========================
// Estructura de rutas
// ==========================

// Asigna memoria para una lista de rutas
struct lista_ruta *asignar_memoria_lista_ruta();

// Asigna memoria para un nodo de ruta
struct nodo_ruta *asignar_memoria_nodo_ruta();

#endif // CONTROL_MEMORIA_H
