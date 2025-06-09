#ifndef MOVIMIENTOS_SA_H
#define MOVIMIENTOS_SA_H

#include <stdbool.h>

// Incluimos las bibliotecas necesarias para trabajar con las estructuras de datos del VRPTW
#include "../include/estructuras.h"

bool opt_2(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);
bool swap_intra(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);
bool swap_inter(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);
bool reinsercion_intra_inter(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);
bool opt_2_5(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);
bool or_opt(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);
bool cross_exchange(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);
bool relocate_chain(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);

#endif // MOVIMIENTOS_SA_H
