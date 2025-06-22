#ifndef VRP_TW_SA_H
#define VRP_TW_SA_H

#include <stdbool.h>
#include "../include/estructuras.h"

// Copia una hormiga para usar en recocido simulado
struct hormiga *copiar_hormiga_sa(struct hormiga *hormiga_original);

// Calcula ventanas de tiempo y capacidad para cada vehículo en la flota
void calculamosVentanasCapacidad(struct lista_vehiculos *flota, struct vrp_configuracion *vrp, double **instancia_distancias);

// Evalúa la función objetivo (fitness) de una hormiga
void evaluaFO_SA(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);

// Ejecuta el algoritmo de recocido simulado (SA)
void sa(struct vrp_configuracion *vrp, struct hormiga *hormiga_vecina, struct hormiga *hormiga_actual, struct hormiga *hormiga_mejor, struct individuo *ind, double **instancia_distancias);

// Función principal para ejecutar SA sobre un individuo VRP con ventanas de tiempo
void vrp_tw_sa(struct vrp_configuracion *vrp, struct hormiga *hormiga_original, struct individuo *ind, double **instancia_distancias);

#endif // VRP_TW_SA_H
