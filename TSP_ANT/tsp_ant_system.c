#include <stdio.h>
#include <stdlib.h>
#include "tsp_ant_system.h"
#include "algoritmo_evolutivo_diferencial.h"

void inializacionHormiga(hormiga *hor,individuo *ind, int tamanio_instancia)
{
    for (int i = 0; i < ind->numHormigas; i++)
    {
        hor[i].ruta = (int *)malloc(sizeof(int)*tamanio_instancia+1);
        hor[i].tabu = (int *) malloc(sizeof(int)*tamanio_instancia);
    }
}

void tsp_ant_system(individuo *ind, double **instancia_feromonas, double **instancia_distancias, int tamanio_instancia)
{
    hormiga *hor = (hormiga *)malloc(sizeof(hormiga) * ind->numHormigas);
    double ** instancia_visibilidad =(double **)malloc(tamanio_instancia * sizeof(double *));
    for (int i = 0; i < tamanio_instancia; i++)
        instancia_visibilidad[i] = (double *)malloc(tamanio_instancia * sizeof(double));
    inializacionHormiga(hor,ind,tamanio_instancia);
}
