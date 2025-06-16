#ifndef VRP_TW_SA_H
#define VRP_TW_SA_H

#include <stdbool.h>
#include "../include/estructuras.h"

// Funcion que copia una hormiga para el algoritmo SA
struct hormiga *copiar_hormiga_sa(struct hormiga *hormiga_original);

// Función que calcula las ventanas de tiempo y capacidad de los vehículos en la flot
void calculamosVentanasCapacidad(struct lista_vehiculos *flota, struct vrp_configuracion *vrp, double **instancia_distancias);

// Función que evalúa la función objetivo de una hormiga en el contexto del SA
void evaluaFO_SA(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);

void sa(struct vrp_configuracion *vrp, struct hormiga *hormiga_solucion_vecina, struct hormiga *hormiga_solucion_actual, struct hormiga *hormiga_mejor_solucion, struct individuo *ind, double **instancia_distancias);
void vrp_tw_sa(struct vrp_configuracion *vrp, struct hormiga *hormiga_original, struct individuo *ind, double **instancia_distancias);

#endif // VRP_TW_SA_H
