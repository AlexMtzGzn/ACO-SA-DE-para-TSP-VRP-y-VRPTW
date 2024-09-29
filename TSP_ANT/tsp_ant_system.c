#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "tsp_ant_system.h"
#include "algoritmo_evolutivo_diferencial.h"

void calcular_fitness(hormiga *hor, double **instancia_distancias, int tamanio_instancia)
{
    hor->fitness = 0.0;
    for (int i = 0; i < tamanio_instancia - 1; i++)
        hor->fitness += instancia_distancias[hor->ruta[i]][hor->ruta[i + 1]];
    hor->fitness += instancia_distancias[hor->ruta[tamanio_instancia - 1]][hor->ruta[0]];
}

void actualizar_feromona(hormiga *hor, individuo *ind, double **instancia_distancias, double **instancia_feromona, int tamanio_instancia)
{
    calcular_fitness(hor, instancia_distancias, tamanio_instancia);
    double delta = 1.0 / hor->fitness;

    for (int i = 0; i < tamanio_instancia - 1; i++)
        instancia_feromona[hor->ruta[i]][hor->ruta[i + 1]] += delta;

    for (int i = 0; i < tamanio_instancia; i++)
    {
        for (int j = 0; j < tamanio_instancia; j++)
            instancia_feromona[i][j] *= (1.0 - ind->rho);
    }
}

void ruta_hormiga(hormiga *hor, individuo *ind, double **instancia_distancias, double **instancia_feromonas, double **instancia_visibilidad, int tamanio_instancia)
{
    for (int i = 0; i < tamanio_instancia; i++)
        hor->tabu[i] = 0; 

    hor->ruta[0] = rand() % tamanio_instancia;
    hor->tabu[hor->ruta[0]] = 1; 

    for (int i = 1; i < tamanio_instancia; i++)
    {
        double suma_probabilidades = 0.0;
        for (int j = 0; j < tamanio_instancia; j++)
        {
            if (hor->tabu[j] == 0) 
            {
                hor->probabilidades[j] = pow(instancia_feromonas[hor->ruta[i - 1]][j], ind->alpha) *
                                         pow(instancia_visibilidad[hor->ruta[i - 1]][j], ind->beta);
                suma_probabilidades += hor->probabilidades[j];
            }
            else
            {
                hor->probabilidades[j] = 0.0;
            }
        }

        
        if (suma_probabilidades > 0)
        {
            double aleatorio = (double)rand() / RAND_MAX;
            double prob_acumulada = 0.0;
            for (int j = 0; j < tamanio_instancia; j++)
            {
                if (hor->tabu[j] == 0)
                {
                    prob_acumulada += hor->probabilidades[j] / suma_probabilidades;
                    if (aleatorio <= prob_acumulada)
                    {
                        hor->ruta[i] = j;
                        hor->tabu[j] = 1; 
                        break;
                    }
                }
            }
        }
        else
        {
            int ciudades_disponibles[tamanio_instancia], num_disponibles = 0;
            for (int j = 0; j < tamanio_instancia; j++)
            {
                if (hor->tabu[j] == 0)
                {
                    ciudades_disponibles[num_disponibles++] = j;
                }
            }
            if (num_disponibles > 0)
            {
                int ciudad_elegida = ciudades_disponibles[rand() % num_disponibles];
                hor->ruta[i] = ciudad_elegida;
                hor->tabu[ciudad_elegida] = 1;
            }
            else
            {
                break;
            }
        }
    }

    hor->ruta[tamanio_instancia] = hor->ruta[0]; 
}

void ant_system(hormiga *hor, individuo *ind, double **instancia_distancias, double **instancia_feromona, double **instancia_visibilidad, int tamanio_instancia)
{
    double mejor_fitness_global = INFINITY;
    hormiga mejor_hormiga_global;
    mejor_hormiga_global.ruta = (int *)malloc(sizeof(int) * (tamanio_instancia + 1));

    for (int i = 0; i < ind->numIteraciones; i++)
    {
        double mejor_fitness_iteracion = INFINITY;
        int indice_mejor_hormiga = -1;

        for (int j = 0; j < ind->numHormigas; j++)
        {
            ruta_hormiga(&hor[j], ind, instancia_distancias, instancia_feromona, instancia_visibilidad, tamanio_instancia);
            calcular_fitness(&hor[j], instancia_distancias, tamanio_instancia);

            if (hor[j].fitness < mejor_fitness_iteracion)
            {
                mejor_fitness_iteracion = hor[j].fitness;
                indice_mejor_hormiga = j;
            }

            printf("Hormiga %d: ", j);
            for (int k = 0; k <= tamanio_instancia; k++)
                printf("%d -> ", hor[j].ruta[k]);
            printf("fitness: %.2f\n", hor[j].fitness);
        }

        if (mejor_fitness_iteracion < mejor_fitness_global)
        {
            mejor_fitness_global = mejor_fitness_iteracion;
            memcpy(mejor_hormiga_global.ruta, hor[indice_mejor_hormiga].ruta, sizeof(int) * (tamanio_instancia + 1));
            mejor_hormiga_global.fitness = mejor_fitness_iteracion;
        }

        actualizar_feromona(&hor[indice_mejor_hormiga], ind, instancia_distancias, instancia_feromona, tamanio_instancia);

        printf("Mejor fitness de la iteración %d: %.2f\n", i, mejor_fitness_iteracion);
    }

    printf("Mejor ruta global encontrada:\n");
    for (int k = 0; k <= tamanio_instancia; k++)
        printf("%d -> ", mejor_hormiga_global.ruta[k]);
    printf("fitness: %.2f\n", mejor_fitness_global);

    free(mejor_hormiga_global.ruta);
}
void inializacionHormiga(hormiga *hor, individuo *ind, int tamanio_instancia)
{
    for (int i = 0; i < ind->numHormigas; i++)
    {
        hor[i].ruta = (int *)malloc(sizeof(int) * (tamanio_instancia + 1));
        hor[i].tabu = (int *)malloc(sizeof(int) * tamanio_instancia);
        hor[i].probabilidades = (double *)malloc(sizeof(double) * tamanio_instancia);
        hor[i].fitness = 0.0;
    }
}

void tsp_ant_system(individuo *ind, double **instancia_feromonas, double **instancia_distancias, int tamanio_instancia)
{
    hormiga *hor = (hormiga *)malloc(sizeof(hormiga) * ind->numHormigas);

    double **instancia_visibilidad = (double **)malloc(tamanio_instancia * sizeof(double *));
    for (int i = 0; i < tamanio_instancia; i++)
    {
        instancia_visibilidad[i] = (double *)malloc(tamanio_instancia * sizeof(double));
    }

    for (int i = 0; i < tamanio_instancia; i++)
    {
        for (int j = 0; j < tamanio_instancia; j++)
        {
            if (i != j)
            {
                instancia_visibilidad[i][j] = 1.0 / instancia_distancias[i][j];
            }
            else
            {
                instancia_visibilidad[i][j] = 0.0; // Evitar división por 0
            }
        }
    }

    inializacionHormiga(hor, ind, tamanio_instancia);
    ant_system(hor, ind, instancia_distancias, instancia_feromonas, instancia_visibilidad, tamanio_instancia);

    for (int i = 0; i < tamanio_instancia; i++)
        free(instancia_visibilidad[i]);
    free(instancia_visibilidad);

    for (int i = 0; i < ind->numHormigas; i++)
    {
        free(hor[i].ruta);
        free(hor[i].tabu);
    }
    free(hor);
}
