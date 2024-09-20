#ifndef TSP_ANT_H
#define TSP_ANT_H

typedef struct
{
    int *ruta;
    int *tabu;
    double fitness;
}hormiga;

void tsp_ant(individuo * ind, double ** instancia_feromona,double ** intancia_distancias,int tamanio_instancia);

#endif // TSP_ANT_H