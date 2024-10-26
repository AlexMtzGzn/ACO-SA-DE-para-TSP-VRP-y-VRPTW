#ifndef TSP_SA_H
#define TSP_SA_H
#include "AED.h"

typedef struct{
    int *solucion;
    double fitness;
} solucion;

void evaluaFO(solucion *solucion_inicial, double **instancia_distancias, int tamanio_instancia);
void sa(individuo * ind,solucion * solucion_incial,solucion * solucion_vecina,solucion * mejor_solucion,double ** instancias_distancias,int tamanio_instancia);
void generaSolucion(solucion *solucion, int tamanio_instacia);
void tsp_sa(individuo *ind , double ** instancias_distancias, int tamanio_instancia);

#endif // TSP_SH_H

