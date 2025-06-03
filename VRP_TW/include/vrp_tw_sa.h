#ifndef VRP_TW_SA_H
#define VRP_TW_SA_H

#include <stdbool.h>
#include "../include/estructuras.h"

void calculamosVentanasCapacidad(struct lista_vehiculos *flota, struct vrp_configuracion *vrp, double **instancia_distancias);
void evaluaFO_SA(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);
bool moverDosClientesVehiculos(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);
bool moverClienteEntreVehiculos(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);
bool intercambiarClienteRuta(struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_distancias);
void generar_vecino(struct hormiga *hormiga, struct vrp_configuracion *vrp);
void sa(struct vrp_configuracion *vrp, hormiga *hormiga, struct individuo *ind, double **instancia_distancias);
void inicializar_metal(struct hormiga *hormiga);
void vrp_tw_sa(struct vrp_configuracion *vrp, hormiga *hormiga, struct individuo *ind, double **instancia_distancias);

#endif // VRP_TW_SA_H
