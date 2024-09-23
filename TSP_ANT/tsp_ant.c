#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "control_memoria.h"
#include "algoritmo_evolutivo_diferencial.h"
#include "tsp_ant.h"

void calcular_costo(hormiga *hor, double **instancia_distancias, int tamanio_instancia)
{
    *(hor->fitness) = 0.0;
    for (int i = 0; i < tamanio_instancia - 1; i++)
        *(hor->fitness) += instancia_distancias[hor->ruta[i]][hor->ruta[i + 1]];
    *(hor->fitness) += instancia_distancias[hor->ruta[tamanio_instancia-1]][hor->ruta[0]];
}

void actualizar_feromona(hormiga *hor, individuo *ind, double **instancia_distancias, double **instancia_feromona, int tamanio_instancia)
{
    calcular_costo(hor, instancia_distancias, tamanio_instancia);
    double delta = 1.0 / *(hor->fitness);

    for (int i = 0; i < tamanio_instancia - 1; i++)
        instancia_feromona[hor->ruta[i]][hor->ruta[i + 1]] += delta;

    for (int i = 0; i < tamanio_instancia; i++)
    {
        for (int j = 0; j < tamanio_instancia; j++)
            instancia_feromona[i][j] *= (1.0 - ind->rho);
    }
}

void ruta_hormiga(hormiga *hor, individuo *ind, double **instancia_distancia, double **instancia_feromona, double **instancia_visibilidad, int tamanio_instancia)
{
    for (int k = 0; k < tamanio_instancia; k++)
        hor->tabu[k] = 0;

    hor->ruta[0] = rand() % tamanio_instancia;  
    hor->tabu[hor->ruta[0]] = 1;

    for (int i = 1; i < tamanio_instancia; i++)
    {
        hor->probabilidades = asignacion_memoria_arreglo_double(tamanio_instancia);
        double suma_probabilidades = 0.0;

        for (int j = 0; j < tamanio_instancia; j++)
        {
            if (hor->tabu[j] == 0)
            {
                hor->probabilidades[j] = pow(instancia_feromona[hor->ruta[i - 1]][j], ind->alpha) * pow(instancia_visibilidad[hor->ruta[i - 1]][j], ind->beta);
                suma_probabilidades += hor->probabilidades[j];
            }
            else
            {
                hor->probabilidades[j] = 0.0;
            }
        }

        if (suma_probabilidades > 0)
            for (int j = 0; j < tamanio_instancia; j++)
                hor->probabilidades[j] /= suma_probabilidades; 
        else
        {
            liberar_memoria_arreglo_double(hor->probabilidades);
            return;
        }

        double aleatorio = (double)rand() / RAND_MAX;
        double prob_acumulada = 0.0;
        for (int j = 0; j < tamanio_instancia; j++)
        {
            prob_acumulada += hor->probabilidades[j];
            if (aleatorio <= prob_acumulada)
            {
                hor->ruta[i] = j;
                hor->tabu[j] = 1;  
                break;
            }
        }

        liberar_memoria_arreglo_double(hor->probabilidades);
    }
    hor->ruta[tamanio_instancia] = hor->ruta[0];
}

void imprime_ruta_hormiga(hormiga * hor,int tamanio_instancia){
     for (int i = 0; i < tamanio_instancia+1; i++)
                printf("%d -> ",hor->ruta[i]);
            printf("\n");
            printf("%lf\n", *(hor->fitness));
}

void ant_system(hormiga *hor, individuo *ind, double **instancia_distancias, double **instancia_feromona, double **instancia_visibilidad, int tamanio_instancia)
{
    for (int i = 0; i < ind->numIteraciones; i++)
        for (int j = 0; j < ind->numHormigas; j++)
        {
            ruta_hormiga(&hor[j], ind, instancia_distancias, instancia_feromona, instancia_visibilidad, tamanio_instancia);
            actualizar_feromona(&hor[j], ind, instancia_distancias, instancia_feromona, tamanio_instancia);
            /*Podemos imprimir como va la ruta de cada hormiga
            imprime_ruta_hormiga(&hor[j],tamanio_instancia);*/
        }
    ind->fitness = *(hor[ind->numHormigas - 1].fitness);
}

void inicializar_visibilidad(double **instancia_visibilidad, double **instancia_distancias, int tamanio_instancia)
{
    for (int i = 0; i < tamanio_instancia; i++)
        for (int j = 0; j < tamanio_instancia; j++)
        {
            if (i == j)
                instancia_visibilidad[i][j] = 0.0;
            else
                instancia_visibilidad[i][j] = 1 / instancia_distancias[i][j];
        }
}

void inializacionHormiga(hormiga *hor, int tamanio_instancia, int numHormigas)
{
    for (int i = 0; i < numHormigas; i++)
    {
        hor[i].ruta = asignacion_memoria_arreglo_int(tamanio_instancia+1);
        hor[i].tabu = asignacion_memoria_arreglo_int(tamanio_instancia);
        hor[i].fitness = asignacion_memoria_variable_double();
    }    
}

void aco_tsp(individuo *ind, double **instancia_feromona, double **instancia_distancias, int tamanio_instancia)
{
    hormiga *hor = asignar_memoria_arreglo_estructura_hormiga(ind->numHormigas);
    double **instancia_visibilidad = asignacion_memoria_instancia(tamanio_instancia);
    inicializar_visibilidad(instancia_visibilidad, instancia_distancias, tamanio_instancia);
    /*Podemos imprimir la matriz de visibilidad
    printf("\n\nInstancia De Visibilidad\n");
    imprimir_instancia(instancia_visibilidad,tamanio_instancia);
    */
    inializacionHormiga(hor, tamanio_instancia, ind->numHormigas);
    ant_system(hor, ind, instancia_distancias, instancia_feromona, instancia_visibilidad, tamanio_instancia);
    liberar_memoria_arreglo_estructura_hormiga(hor);
    liberar_memoria_instancia(instancia_visibilidad, tamanio_instancia);
}
