#include <stdlib.h>
#include <stdbool.h>
#include "AED.h"
#include "TSP_SA.h"

double **asignar_memoria_instancia(int tamanio_instancia)
{
    double **instancia = (double **)malloc(tamanio_instancia * sizeof(double *));
    for (int i = 0; i < tamanio_instancia; i++)
        instancia[i] = (double *)malloc(tamanio_instancia * sizeof(double));
    return instancia;
}
individuo *asignar_memoria_individuos(int poblacion) { return (individuo *)malloc(sizeof(individuo) * poblacion); }

solucion *asignar_memoria_soluciones() { return (solucion *)malloc(sizeof(solucion)); }

int *asignar_memoria_ruta(int tamanio_instancia) { return (int *)malloc(sizeof(int) * (tamanio_instancia)); }

void liberar_instancia(double **instancia, int tamanio_instancia)
{
    for (int i = 0; i < tamanio_instancia; i++)
        free(instancia[i]);
    free(instancia);
}

void liberar_individuos(individuo *ind, bool bandera)
{

    if (bandera == true)
        free(ind->ruta);
    free(ind);
}

void liberar_soluciones(solucion *solucion, bool bandera)
{

    if (bandera == true)
        free(solucion->solucion);
    free(solucion);
}
