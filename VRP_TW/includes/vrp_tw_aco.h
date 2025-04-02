#ifndef VRP_TW_ACO_H
#define VRP_TW_ACO_H

#include <stdbool.h>
#include "../includes/estructuras.h"

void recuperamos_mejor_hormiga(struct individuo *ind, struct hormiga *hormiga);
void refuerzo_feromona_mejor_ruta(struct hormiga *hormiga, double **instancia_feromona, double delta);
void actualizar_feromona(struct individuo *ind, struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_feromona, double delta);
void calcular_fitness(struct hormiga *hormiga, double **instancia_distancias);
void inicializar_hormiga(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga);
void calcular_posibles_clientes(int origen, struct vehiculo *vehiculo, struct vrp_configuracion *vrp, struct hormiga *hormiga, double **instancia_distancias);
double calcular_probabilidad(int origen, int destino, struct individuo *ind, struct vrp_configuracion *vrp, struct hormiga *hormiga, double **instancia_feromona, double **instancia_visibilidad, double **instancia_ventanas_tiempo);
void aco(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, double **instancia_visiblidad, double **instancia_feromona, double **instancia_distancias, double **instancia_ventanas_tiempo);
void liberar_memoria_hormiga(struct hormiga *hormiga, struct individuo *ind);
void reiniciar_hormiga(struct hormiga *hormiga, struct vrp_configuracion *vrp);
void vrp_tw_aco(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_visiblidad, double **instancia_distancias, double **instancia_feromona, double **instancia_ventanas_tiempo);

#endif // VRP_TW_ACO_H
