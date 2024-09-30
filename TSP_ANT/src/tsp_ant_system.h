#ifndef TSP_ANT_SYSTEM_H
#define TSP_ANT_SYSTEM_H
#include "algoritmo_evolutivo_diferencial.h"

typedef struct
{
    double fitness;
    double *probabilidades;
    int *tabu;
    int *ruta;


} hormiga;

double calcular_fitness(hormiga *hor, double **instancia_distancias, int tamanio_instancia);
void actualizar_feromona(hormiga *hor, individuo *ind, double **instancia_distancias, double **instancia_feromona, int tamanio_instancia);
void ruta_hormiga(hormiga *hor, individuo *ind, double **instancia_distancia, double **instancia_feromona, double **instancia_visibilidad, int tamanio_instancia);
void ant_system_f(hormiga *hor, individuo *ind, double **instancia_distancias, double **instancia_feromona, double **instancia_visibilidad, int tamanio_instancia);
void inializacionHormiga(hormiga *hor, individuo *ind, int tamanio_instancia);
void tsp_ant_system(individuo *ind, double **instancia_feromonas, double **instancia_distancias, int tamanio_instancia);

#endif // TSP_ANT_SYSTEM_H
