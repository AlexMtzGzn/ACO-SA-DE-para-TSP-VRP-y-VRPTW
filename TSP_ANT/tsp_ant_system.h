#ifndef TSP_ANT_SYSTEM_H
#define TSP_ANT_SYSTEM_H
#include "algoritmo_evolutivo_diferencial.h"

typedef struct
{
    double fitness;
    double *posibilidades;
    int *tabu;
    int *ruta;

}hormiga;

void tsp_ant_system(individuo * ind,double ** instancia_feromonas, double ** instancia_distancias, int tamanio_instancia);

#endif // TSP_ANT_SYSTEM_H
