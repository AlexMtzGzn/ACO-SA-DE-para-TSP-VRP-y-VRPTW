#ifndef TSP_ACO_H
#define TSP_ACO_H
#include "AED.h"
#include "lista.h"

typedef struct
{
    double fitness;
    struct Lista *probabilidades2;
    struct Lista *tabu2;
    struct Lista *noVisitadas;
    double *probabilidades;
    int *tabu;
    int *ruta;

} hormiga;

double calcular_fitness(hormiga *hor, double **instancia_distancias, int tamanio_instancia);
void actualizar_feromona(hormiga *hor, individuo *ind, double **instancia_distancias, double **instancia_feromona, int tamanio_instancia);
void ruta_hormiga(hormiga *hor, individuo *ind, double **instancia_distancia, double **instancia_feromona, double **instancia_visibilidad, int tamanio_instancia);
void aco(hormiga *hor, individuo *ind, double **instancia_distancias, double **instancia_feromona, double **instancia_visibilidad, int tamanio_instancia);
void inicializar_visibilidad(double **instancia_visibilidad, double **instancia_distancias, int tamanio_instancia,individuo *ind);
void inializacionHormiga(hormiga *hor, individuo *ind, int tamanio_instancia);
void tsp_aco(individuo *ind, double **instancia_feromonas, double **instancia_distancias, int tamanio_instancia);

#endif // TSP_ACO_H
