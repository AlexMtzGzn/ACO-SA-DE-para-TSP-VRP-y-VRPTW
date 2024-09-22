#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "control_memoria.h"
#include "algoritmo_evolutivo_diferencial.h"
#include "tsp_ant.h"

void ruta_hormiga(hormiga * hor,individuo * ind, double ** instancia_distancia, double ** instancia_feromona, double ** instancia_visibilidad, int tamanio_instancia){

    hor->ruta[0] = rand() % (tamanio_instancia + 1);
    hor->ruta[tamanio_instancia-1];
    hor->tabu[(int)hor->ruta[0]] = 1;

    

}

void ant_system(hormiga *hor, individuo *ind, double **instancia_distancias, double **instancia_feromona, double ** instancia_visibilidad, int tamanio_instancia)
{
    for (int i = 0; i < ind->numIteraciones; i++)
    {
        for (int j = 0; j < ind->numHormigas; j++)
            ruta_hormiga(&hor[j],ind,instancia_distancias,instancia_feromona,instancia_visibilidad,tamanio_instancia);
    }
}

void inicializar_visibilidad(double **instancia_visibilidad, double **instancia_distancias, int tamanio_instancia)
{
    for (int i = 0; i < tamanio_instancia; i++)
    {
        for (int j = 0; j < tamanio_instancia; j++)
        {
            if (i == j)
                instancia_visibilidad[i][j] = 0.0;
            else
                instancia_visibilidad[i][j] = 1 / instancia_distancias[i][j];
        }
    }
}

void inializacionHormiga(hormiga *hor, int tamanio_instancia, int numHormigas)
{
    for (int i = 0; i < numHormigas; i++)
    {
        hor[i].ruta = asignacion_memoria_arreglo(tamanio_instancia+1);
        hor[i].tabu = asignacion_memoria_arreglo(tamanio_instancia);
        hor[i].fitness = 0.0;
    }
}

double aco_tsp(individuo *ind, double **instancia_feromona, double **instancia_distancias, int tamanio_instancia)
{
    hormiga *hor = asignar_memoria_arreglo_estructura_hormiga(ind->numHormigas);
    double **instancia_visibilidad = asignacion_memoria_instancia(tamanio_instancia);
    inicializar_visibilidad(instancia_visibilidad, instancia_distancias, tamanio_instancia);
    inializacionHormiga(hor, tamanio_instancia, ind->numHormigas);
    
    ant_system(hor, ind, instancia_distancias, instancia_feromona,instancia_visibilidad, tamanio_instancia);

    liberar_memoria_arreglo_estructura_hormiga(hor);
    liberar_memoria_instancia(instancia_visibilidad, tamanio_instancia);
    return mejor_fitness(hor, ind->numHormigas);
}
