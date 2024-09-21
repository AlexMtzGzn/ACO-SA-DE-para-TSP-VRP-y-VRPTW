#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "control_memoria.h"
#include "algoritmo_evolutivo_diferencial.h"
#include "tsp_ant.h"

double mejor_fitness(hormiga *hor, int numHormigas) {
    double mejor_fitness = hor[0].fitness;

    for (int i = 1; i < numHormigas; i++) {
        if (mejor_fitness > hor[i].fitness)
            mejor_fitness = hor[i].fitness;
    }

    return mejor_fitness;
}

void actualizar_feromonas(double **instacia_feromona, hormiga *hor, individuo *ind, int tamanio_instancia) {
    for (int i = 0; i < tamanio_instancia; i++) {
        for (int j = 0; j < tamanio_instancia; j++) {
            instacia_feromona[i][j] *= (1 - ind->rho);
        }
    }

    for (int k = 0; k < ind->numHormigas; k++) {
        double intensidad_feromona = 1.0 / hor[k].fitness;
        for (int i = 0; i < tamanio_instancia; i++) {
            int ciudad_actual = hor[k].ruta[i];
            int siguiente_ciudad = hor[k].ruta[(i + 1) % tamanio_instancia];
            instacia_feromona[ciudad_actual][siguiente_ciudad] += intensidad_feromona;
            instacia_feromona[siguiente_ciudad][ciudad_actual] += intensidad_feromona;
        }
    }
}

double calcular_distancia(hormiga *hor, double **instancia_distancias, int tamanio_instancia) {
    double distancia = 0.0;
    for (int i = 0; i < tamanio_instancia; i++) {
        distancia += instancia_distancias[(int)hor->ruta[i]][(int)hor->ruta[i + 1]];
    }
    distancia += instancia_distancias[(int)hor->ruta[tamanio_instancia - 1]][(int)hor->ruta[0]];
    return distancia;
}

void caminar_hormiga(hormiga *hor, double **instancia_feromona, double **probabilidad, double **visibilidad, int tamanio_instancia)
{
    for (int i = 0; i < tamanio_instancia; i++)
        hor->tabu[i] = 0;

    int ciudad_inicial = rand() % tamanio_instancia;
    hor->ruta[0] = ciudad_inicial;
    hor->tabu[ciudad_inicial] = 1;

    for (int j = 1; j < tamanio_instancia; j++)
    {
        double rand_value = (double)rand() / RAND_MAX;
        double acumulador = 0.0;
        int ciudad_seleccionada = -1;

        for (int k = 0; k < tamanio_instancia; k++)
        {
            if (hor->tabu[k] == 0) 
            {
                acumulador += probabilidad[(int)hor->ruta[j - 1]][k];
            }
        }
        double prob_acumulada = 0.0;
        for (int k = 0; k < tamanio_instancia; k++)
        {
            if (hor->tabu[k] == 0)
            {
                prob_acumulada += probabilidad[(int)hor->ruta[j - 1]][k] / acumulador;
                if (prob_acumulada >= rand_value)
                {
                    ciudad_seleccionada = k;
                    break;
                }
            }
        }

        if (ciudad_seleccionada == -1) {
            for (int k = 0; k < tamanio_instancia; k++) {
                if (hor->tabu[k] == 0) {
                    ciudad_seleccionada = k;
                    break;
                }
            }
        }

        hor->ruta[j] = ciudad_seleccionada;
        hor->tabu[ciudad_seleccionada] = 1;
    }

    hor->ruta[tamanio_instancia] = hor->ruta[0];
}



void inicializar_ruleta(double **instancia_feromona, double **instacia_distancias, double **visibilidad, double **probabilidad, hormiga *hor, individuo *ind, int tamanio_instancia) {
    for (int i = 0; i < tamanio_instancia; i++) {
        double suma_probabilidades = 0.0;
        for (int j = 0; j < tamanio_instancia; j++) {
            if (i != j) {
                visibilidad[i][j] = 1.0 / instacia_distancias[i][j];
                probabilidad[i][j] = pow(instancia_feromona[i][j], ind->alpha) * pow(visibilidad[i][j], ind->beta);
                suma_probabilidades += probabilidad[i][j];
            }
        }

        if (suma_probabilidades > 0) {
            for (int j = 0; j < tamanio_instancia; j++) {
                if (i != j) {
                    probabilidad[i][j] /= suma_probabilidades;
                }
            }
        }
    }
}

void ant_system(hormiga *hor, individuo *ind, double **instancia_distancias, double **instansia_feromona, double **probabilidad, double **visibilidad, int tamanio_instancia) {
    for (int i = 0; i < ind->numIteraciones; i++) {
        inicializar_ruleta(instansia_feromona, instancia_distancias, visibilidad, probabilidad, hor, ind, tamanio_instancia);

        for (int j = 0; j < ind->numHormigas; j++) {
            caminar_hormiga(&hor[j], instansia_feromona, probabilidad, visibilidad, tamanio_instancia);
            hor[j].fitness = calcular_distancia(&hor[j], instancia_distancias, tamanio_instancia);
        }

        actualizar_feromonas(instansia_feromona, hor, ind, tamanio_instancia);
    }
}

void inializacionHormiga(hormiga *hor, int tamanio_instancia, int numHormigas) {
    for (int i = 0; i < numHormigas; i++) {
        hor[i].ruta = (double *)malloc((tamanio_instancia + 1) * sizeof(double));
        hor[i].tabu = (double *)calloc(tamanio_instancia, sizeof(double));
        hor[i].fitness = 0.0;
    }
}

void imprimir_rutas_hormigas(hormiga *hor, int numHormigas, int tamanio_instancia) {
    for (int i = 0; i < numHormigas; i++) {
        printf("Hormiga %d: ", i + 1);
        for (int j = 0; j < tamanio_instancia + 1; j++) {
            printf("%d ->", (int)hor[i].ruta[j]);
        }
        printf("Fitness: %f\n", hor[i].fitness);
    }
}

double aco_tsp(individuo *ind, double **instancia_feromona, double **instancia_distancias, int tamanio_instancia) {
    hormiga *hor = asignar_memoria_arreglo_estructura_hormiga(ind->numHormigas);
    double **probabilidad = asignacion_memoria_instancia(tamanio_instancia);
    double **visibilidad = asignacion_memoria_instancia(tamanio_instancia);

    inializacionHormiga(hor, tamanio_instancia, ind->numHormigas);
    ant_system(hor, ind, instancia_distancias, instancia_feromona, probabilidad, visibilidad, tamanio_instancia);
    imprimir_rutas_hormigas(hor, ind->numHormigas, tamanio_instancia);

    for (int i = 0; i < ind->numHormigas; i++) {
        free(hor[i].ruta);
        free(hor[i].tabu);
    }
    free(hor);

    for (int i = 0; i < tamanio_instancia; i++) {
        free(probabilidad[i]);
        free(visibilidad[i]);
    }
    free(probabilidad);
    free(visibilidad);

    return mejor_fitness(hor, ind->numHormigas);
}
