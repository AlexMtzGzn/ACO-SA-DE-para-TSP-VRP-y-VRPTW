#ifndef VRP_TW_SA_H
#define VRP_TW_SA_H

#include <stdbool.h>
#include "../include/estructuras.h"

struct hormiga *copiar_hormiga_sa(struct hormiga *hormiga_original);
void calculamosVentanasCapacidad(struct lista_vehiculos *flota, struct vrp_configuracion *vrp, double **instancia_distancias);
void evaluaFO_SA(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);
void sa(struct vrp_configuracion *vrp, struct hormiga *hormiga_solucion_vecina, struct hormiga *hormiga_solucion_actual, struct hormiga *hormiga_mejor_solucion, struct individuo *ind, double **instancia_distancias);
void vrp_tw_sa(struct vrp_configuracion *vrp, struct hormiga *hormiga_original, struct individuo *ind, double **instancia_distancias);

#endif // VRP_TW_SA_H
