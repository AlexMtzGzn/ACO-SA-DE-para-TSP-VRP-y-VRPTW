#ifndef MOVIMIENTOS_SA_H
#define MOVIMIENTOS_SA_H

#include <stdbool.h>

// Incluimos las bibliotecas necesarias para trabajar con las estructuras de datos del VRPTW
#include "../include/estructuras.h"

// Funcion que intercambia dos clientes dentro de la misma ruta
bool swap_intra(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);

// Funcion que intercambia clientes entre rutas distintas
bool swap_inter(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);

// Funcion que realiza una reinserción de cliente dentro de la misma ruta o entre rutas
bool reinsercion_intra_inter(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);

// Funcion que aplica la mejora 2-opt dentro de una ruta (intra-ruta)
bool opt_2(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);

// Funcion que es variante de 2-opt para tramos más largos
bool opt_2_5(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);

// Funcion que mueve uno o más clientes contiguos dentro de una ruta
bool or_opt(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);

// Funcion que intercambia bloques entre rutas (cross exchange)
bool cross_exchange(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);

// Funcion que reubica una cadena de clientes en otra parte de la solución
bool relocate_chain(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);


#endif // MOVIMIENTOS_SA_H
