#ifndef VRP_TW_SA_H
#define VRP_TW_SA_H

#include <stdbool.h>
#include "../include/estructuras.h"

void calculamosVentanasCapacidad(struct lista_vehiculos *flota, struct vrp_configuracion *vrp, double **instancia_distancias);
void evaluaFO_SA(struct individuo *ind, struct vrp_configuracion *vrp, double **instancia_distancias);
bool moverClienteVehiculo(struct individuo *ind, struct vrp_configuracion *vrp, double **instancia_distancias);
bool intercambiarClienteRuta(struct individuo *ind, struct vrp_configuracion *vrp, double **instancia_distancias);
void generar_vecino(struct individuo *ind, struct vrp_configuracion *vrp);
void sa(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_distancias);
void inicializar_metal(struct individuo *ind);
void vrp_tw_sa(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_distancias);

#endif // VRP_TW_SA_H


