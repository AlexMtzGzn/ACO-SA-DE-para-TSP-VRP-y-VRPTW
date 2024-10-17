#include <stdlib.h>
#include <stdbool.h>
#include "AED.h"
#include "TSP_ACO.h"

double **asignar_memoria_instancia(int tamanio_instancia)
{
    double **instancia = (double **)malloc(tamanio_instancia * sizeof(double *));
    for (int i = 0; i < tamanio_instancia; i++)
        instancia[i] = (double *)malloc(tamanio_instancia * sizeof(double));
    return instancia;
}
individuo *asignar_memoria_individuos(int poblacion)
{
    return (individuo *)malloc(sizeof(individuo) * poblacion);
}

void liberar_instancia(double **instancia, int tamanio_instancia)
{
    for (int i = 0; i < tamanio_instancia; i++)
        free(instancia[i]);
    free(instancia);
}

void liberar_individuos(individuo *ind,bool bandera)
{
    free(ind);
    if (bandera == true)
        free(ind->ruta);
}

hormiga *asignar_memoria_hormiga(individuo *ind)
{
    return (hormiga *)malloc(sizeof(hormiga) * ind->numHormigas);
}

int *asignar_memoria_ruta(int tamanio_instancia)
{
    return (int *)malloc(sizeof(int) * (tamanio_instancia));
}


double *asignar_memoria_posibilidades(int tamanio_instancia)
{
    return (double *)malloc(sizeof(double) * (tamanio_instancia));
}

void liberar_hormigas(hormiga *hor, individuo *ind)
{
    for (int i = 0; i < ind->numHormigas; i++)
    {
        free(hor[i].ruta);
        free(hor[i].tabu);
       
    }
    free(hor->probabilidades);
    free(hor);
}