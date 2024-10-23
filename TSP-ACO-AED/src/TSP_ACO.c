#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "TSP_ACO.h"
#include "AED.h"
#include "entrada_salida_datos.h"
#include "control_memoria.h"

double calcular_fitness(hormiga *hor, double **instancia_distancias, int tamanio_instancia)
{
    double fitness = 0.0;
    for (int i = 0; i < tamanio_instancia - 1; i++)
    {
        fitness += instancia_distancias[hor->ruta[i]][hor->ruta[i + 1]];
    }

    fitness += instancia_distancias[hor->ruta[tamanio_instancia - 1]][hor->ruta[0]];

    return fitness;
}

void actualizar_feromona(hormiga *hor, individuo *ind, double **instancia_distancias, double **instancia_feromona, int tamanio_instancia)
{
    double **delta_feromona = asignar_memoria_instancia(tamanio_instancia);

    for (int k = 0; k < ind->numHormigas; k++)
    {
        double delta = 1.0 / hor[k].fitness;
        for (int j = 0; j < tamanio_instancia - 1; j++)
        {
            int ciudad_actual = hor[k].ruta[j];
            int ciudad_siguiente = hor[k].ruta[j + 1];
            delta_feromona[ciudad_actual][ciudad_siguiente] += delta;
            delta_feromona[ciudad_siguiente][ciudad_actual] += delta;
        }
    }

    for (int i = 0; i < tamanio_instancia; i++)
    {
        for (int j = 0; j < tamanio_instancia; j++)
        {
            if (i != j)
            {
                instancia_feromona[i][j] = (1.0 - ind->rho) * instancia_feromona[i][j] +
                                           delta_feromona[i][j];
                instancia_feromona[j][i] = (1.0 - ind->rho) * instancia_feromona[j][i] +
                                           delta_feromona[j][i];
            }
        }
    }

    liberar_instancia(delta_feromona, tamanio_instancia);
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
                // imprimir_instancia(instancia_visibilidad,tamanio_instancia,"Instancia de Visibilidad");
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

void aco(hormiga *hor, individuo *ind, double **instancia_distancias, double **instancia_feromona, double **instancia_visibilidad, int tamanio_instancia)
{

    double mejor_fitness_iteracion = INFINITY;
    int indice_mejor_hormiga = -1;

    for (int i = 0; i < ind->numIteraciones; i++)
    {

        for (int j = 0; j < ind->numHormigas; j++)
        {
            ruta_hormiga(&hor[j], ind, instancia_distancias, instancia_feromona, instancia_visibilidad, tamanio_instancia);
            hor[j].fitness = calcular_fitness(&hor[j], instancia_distancias, tamanio_instancia);
            if (i == ind->numIteraciones - 1 && hor[j].fitness < mejor_fitness_iteracion)
            {
                mejor_fitness_iteracion = hor[j].fitness;
                indice_mejor_hormiga = j;
            }
            // Podemos imprimir la hormiga
             //imprimir_ruta_hormiga(&hor[j], tamanio_instancia, j, i);
        }

        actualizar_feromona(hor, ind, instancia_distancias, instancia_feromona, tamanio_instancia);
        // printf("\n\n");
    }

    for (int i = 0; i <= tamanio_instancia; i++)
    {
        ind->ruta[i] = hor[indice_mejor_hormiga].ruta[i];
    }
    ind->fitness = mejor_fitness_iteracion;
}

void inicializar_visibilidad(double **instancia_visibilidad, double **instancia_distancias, int tamanio_instancia, individuo *ind)
{
    for (int i = 0; i < tamanio_instancia; i++)
    {
        for (int j = 0; j < tamanio_instancia; j++)
        {
            if (i != j)
            {
                instancia_visibilidad[i][j] = 1.00 / instancia_distancias[i][j];
            }
            else
            {
                instancia_visibilidad[i][j] = 0.0;
            }
        }
    }
}

void inializacionHormiga(hormiga *hor, individuo *ind, int tamanio_instancia)
{
    for (int i = 0; i < ind->numHormigas; i++)
    {
        hor[i].ruta = asignar_memoria_ruta(tamanio_instancia + 1);
        hor[i].tabu = asignar_memoria_ruta(tamanio_instancia);
        hor[i].probabilidades = asignar_memoria_posibilidades(tamanio_instancia);
        hor[i].fitness = 0.0;
    }
}

void tsp_aco(individuo *ind, double **instancia_feromonas, double **instancia_distancias, int tamanio_instancia)
{
    hormiga *hor = asignar_memoria_hormiga(ind);
    ind->ruta = asignar_memoria_ruta(tamanio_instancia + 1);
    double **instancia_visibilidad = asignar_memoria_instancia(tamanio_instancia);
    inicializar_visibilidad(instancia_visibilidad, instancia_distancias, tamanio_instancia, ind);
    // Podemos imprimir matriz de Visibilidad
    // imprimir_instancia(instancia_visibilidad,tamanio_instancia,"Instancia de Visibilidad");
    inializacionHormiga(hor, ind, tamanio_instancia);
    aco(hor, ind, instancia_distancias, instancia_feromonas, instancia_visibilidad, tamanio_instancia);

    liberar_instancia(instancia_visibilidad, tamanio_instancia);
    liberar_hormigas(hor, ind);
}
