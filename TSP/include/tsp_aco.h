#ifndef TSP_ACO_H
#define TSP_ACO_H

#include <stdbool.h>
#include "../include/estructuras.h"

// Función para recuperar la mejor hormiga y copiar sus datos a un individuo
void recuperamos_mejor_hormiga(struct individuo *ind, struct hormiga *hormiga);

// Función para reforzar la feromona en la mejor ruta de la hormiga
void refuerzo_feromona_mejor_ruta(struct hormiga *hormiga, double **instancia_feromona, double delta);

// Función para actualizar la feromona en el sistema
void actualizar_feromona(struct individuo *ind, struct hormiga *hormiga, struct tsp_configuracion *tsp, double **instancia_feromona, double delta);

// Función para calcular el fitness de una hormiga basado en las distancias
void calcular_fitness(struct hormiga *hormiga, double **instancia_distancias);

// Función para inicializar una hormiga con una configuración dada
void inicializar_hormiga(struct tsp_configuracion *tsp, struct individuo *ind, struct hormiga *hormiga);

// Función para calcular los posibles clientes que una hormiga puede visitar desde un origen en su ruta
void calcular_posibles_clientes(struct tsp_configuracion *tsp, struct hormiga *hormiga);

// Función para calcular la probabilidad de transición de una hormiga entre dos clientes
double calcular_probabilidad(int origen, int destino, struct individuo *ind, struct tsp_configuracion *tsp, struct hormiga *hormiga, double **instancia_feromona, double **instancia_visibilidad);

// Funcion para seleccionar un vehiculo aleatoriamente
struct vehiculo *seleccionamos_vehiculo_aleatoriamente(struct hormiga *hormiga);

// Función principal del algoritmo ACO (Ant Colony Optimization) para resolver el problema tsp-TW
void aco(struct tsp_configuracion *tsp, struct individuo *ind, struct hormiga *hormiga, double **instancia_visiblidad, double **instancia_feromona, double **instancia_distancias);

// Función que ejecuta el algoritmo tsp-TW con ACO (Vehicle Routing Problem with Time Windows using Ant Colony Optimization)
void tsp_aco(struct tsp_configuracion *tsp, struct individuo *ind, double **instancia_visiblidad, double **instancia_distancias, double **instancia_feromona);



#endif // TSP_ACO_H
