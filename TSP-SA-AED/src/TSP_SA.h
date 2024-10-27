#ifndef TSP_SA_H
#define TSP_SA_H
#include "AED.h"

typedef struct{
    int *solucion;
    double fitness;
} solucion;




void evaluaFO(solucion *solucion_inicial, double **instancia_distancias, int tamanio_instancia);
void copiaSolucion(solucion *solucion_original, solucion *solucion_copia, int tamanio_instancia);
void intercambiarCiudades(solucion *solucion, int tamanio_instancia);
void generaVecino(solucion *solucion_inicial, solucion *solucion_vecina, int tamanio_instancia);
void sa(individuo *ind, solucion *solucion_inicial, solucion *solucion_vecina, solucion *mejor_solucion, double **instancias_distancias, int tamanio_instancia);
void generaSolInicial(solucion *solucion_inicial, int tamanio_instancia);
void tsp_sa(individuo *ind , double ** instancias_distancias, int tamanio_instancia);

#endif // TSP_SH_H

