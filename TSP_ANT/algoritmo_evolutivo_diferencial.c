// algoritmo_evolutivo_diferencial.c
#include <stdio.h>
#include <stdlib.h>
#include "algoritmo_evolutivo_diferencial.h"
#include "control_memoria.h"
#include "tsp_ant.h"
#include "float.h"

void inializacion_instancia_feromona(double **instancia_feromona, int tamanio_instancia, double alpha)
{
    for (int i = 0; i < tamanio_instancia; i++)
        for (int j = 0; j < tamanio_instancia; j++)
        {
            if (i == j)
                instancia_feromona[i][j] = 0.0;
            else
                instancia_feromona[i][j] = alpha;
        }
}

void evaluaFO(individuo *ind, double **instancia_feromona, double **instancia_distancias, int tamanio_instancia)
{
    inializacion_instancia_feromona(instancia_feromona, tamanio_instancia, ind->alpha);
    /*Podemos imprimir la matriz de feromonas de cada individo
    printf("\n\nInstancia De Feromonas\n");
    imprimir_instancia(instancia_feromona,tamanio_instancia);
    */
    aco_tsp(ind, instancia_feromona, instancia_distancias, tamanio_instancia);
}

double generaAleatorio(double minimo, double maximo)
{
    double aleatorio = (double)rand() / RAND_MAX;
    return minimo + aleatorio * (maximo - minimo);
}

void inicializaPoblacion(individuo *objetivo, int poblacion)
{
    for (int i = 0; i < poblacion; ++i)
    {
        objetivo[i].alpha = generaAleatorio(0.1, 2.0);
        objetivo[i].beta = generaAleatorio(1.5, 2.5);
        objetivo[i].rho = generaAleatorio(0.0, 1.0);
        objetivo[i].numHormigas = (int)generaAleatorio(10, 20);
        objetivo[i].numIteraciones = (int)generaAleatorio(20, 50);
    }
}

void construyeRuidosos(individuo *objetivo, individuo *ruidoso, int poblacion)
{
    for (int i = 0; i < poblacion; ++i)
    {
        int aleatorio1 = rand() % poblacion;
        int aleatorio2 = rand() % poblacion;
        int aleatorio3 = rand() % poblacion;

        while (aleatorio1 == aleatorio2 || aleatorio2 == aleatorio3 || aleatorio1 == aleatorio3)
        {
            aleatorio1 = rand() % poblacion;
            aleatorio2 = rand() % poblacion;
            aleatorio3 = rand() % poblacion;
        }

        ruidoso[i].alpha = objetivo[aleatorio1].alpha + 0.5 * (objetivo[aleatorio2].alpha - objetivo[aleatorio3].alpha);
        ruidoso[i].beta = objetivo[aleatorio1].beta + 0.5 * (objetivo[aleatorio2].beta - objetivo[aleatorio3].beta);
        ruidoso[i].rho = objetivo[aleatorio1].rho + 0.5 * (objetivo[aleatorio2].rho - objetivo[aleatorio3].rho);
        ruidoso[i].numHormigas = objetivo[aleatorio1].numHormigas + (int)(0.5 * (objetivo[aleatorio2].numHormigas - objetivo[aleatorio3].numHormigas));
        ruidoso[i].numIteraciones = objetivo[aleatorio1].numIteraciones + (int)(0.5 * (objetivo[aleatorio2].numIteraciones - objetivo[aleatorio3].numIteraciones));

        if (ruidoso[i].alpha > 2.0)
            ruidoso[i].alpha = 2.0;

        if (ruidoso[i].alpha < 0.1)
            ruidoso[i].alpha = 0.1;

        if (ruidoso[i].beta > 2.5)
            ruidoso[i].beta = 2.5;

        if (ruidoso[i].beta < 1.5)
            ruidoso[i].beta = 1.5;

        if (ruidoso[i].rho > 1.0)
            ruidoso[i].rho = 1.0;

        if (ruidoso[i].rho < 0.0)
            ruidoso[i].rho = 0.0;

        if (ruidoso[i].numHormigas > 20)
            ruidoso[i].numHormigas = 20;

        if (ruidoso[i].numHormigas < 10)
            ruidoso[i].numHormigas = 10;

        if (ruidoso[i].numIteraciones > 50)
            ruidoso[i].numIteraciones = 50;

        if (ruidoso[i].numIteraciones < 20)
            ruidoso[i].numIteraciones = 20;
    }
}

void construyePrueba(individuo *objetivo, individuo *ruidoso, individuo *prueba, int poblacion)
{
    for (int i = 0; i < poblacion; ++i)
    {
        double aleatorio = (double)rand() / RAND_MAX;
        if (aleatorio <= 0.5)
            prueba[i] = objetivo[i];
        else
            prueba[i] = ruidoso[i];
    }
}

void seleccion(individuo *objetivo, individuo *prueba, int poblacion)
{
    for (int i = 0; i < poblacion; ++i)
        if (objetivo[i].fitness > prueba[i].fitness)
            objetivo[i] = prueba[i];
}

void imprimeIndividuo(individuo ind) {
    printf("Individuo - alpha: %f, beta: %f, rho: %f, numHormigas: %d, numIteraciones: %d, fitness: %f\n",
           ind.alpha, ind.beta, ind.rho, ind.numHormigas, ind.numIteraciones, ind.fitness);
}

void imprimePoblacion(individuo *objetivo, int poblacion)
{
    for (int i = 0; i < poblacion; ++i)
        printf("%i - alpha: %f, beta: %f, rho: %f, hormigas: %d, iteraciones: %d, fitness: %f\n", i + 1, objetivo[i].alpha, objetivo[i].beta, objetivo[i].rho, objetivo[i].numHormigas, objetivo[i].numIteraciones, objetivo[i].fitness);
}

void leer_instancia(double **instancia_distancias, int tamanio_instancia, char *archivo_instancia)
{
    FILE *instancia = fopen(archivo_instancia, "r");

    for (int i = 0; i < tamanio_instancia; i++)
    {
        for (int j = 0; j < tamanio_instancia; j++)
            fscanf(instancia, "%lf", &instancia_distancias[i][j]);
    }
    fclose(instancia);
}

void imprimir_instancia(double **instancia, int tamanio_instancia)
{
    for (int i = 0; i < tamanio_instancia; i++)
    {
        for (int j = 0; j < tamanio_instancia; j++)
            printf("%0.2f ", instancia[i][j]);
        printf("\n");
    }
}

void algoritmo_evolutivo_diferencial(int poblacion, int generaciones, int tamanio_instancia, char *archivo_instancia)
{
    // Reserva de memoria
    individuo *objetivo = asignar_memoria_arreglo_estructura_individuo(poblacion);
    individuo *ruidoso = asignar_memoria_arreglo_estructura_individuo(poblacion);
    individuo *prueba = asignar_memoria_arreglo_estructura_individuo(poblacion);
    double **instancia_distancias = asignacion_memoria_instancia(tamanio_instancia);
    double **instancia_feromonas = asignacion_memoria_instancia(tamanio_instancia);

    leer_instancia(instancia_distancias, tamanio_instancia, archivo_instancia);
    /*Podemos imprimir la matriz de distancias
    printf("\n\nInstancia De Distancias\n");
    imprimir_instancia(instancia_distancias,tamanio_instancia);
    */
    inicializaPoblacion(objetivo, poblacion);

    for (int i = 0; i < generaciones; i++)
    {
        construyeRuidosos(objetivo, ruidoso, poblacion);
        construyePrueba(objetivo, ruidoso, prueba, poblacion);

        for (int j = 0; j < poblacion; ++j)
        {
            evaluaFO(&objetivo[j], instancia_feromonas, instancia_distancias, tamanio_instancia);
            evaluaFO(&prueba[j], instancia_feromonas, instancia_distancias, tamanio_instancia);
        }

        seleccion(objetivo, prueba, poblacion);
        /*Podeemos imprimir la poblacion objetivo de cada generacion
        printf("\n\nGeneracion i %d\n")
        imprimePoblacion(objetivo, poblacion);*/
    }

    //Podemos imprimir la poblacion de la ultima generacion
    printf("\n\nUltima generacion de %d generaciones\n", generaciones);
    imprimePoblacion(objetivo, poblacion);

    liberar_memoria_arreglo_estructura_individuo(ruidoso);
    liberar_memoria_arreglo_estructura_individuo(prueba);
    mejor_individuo_t mejor;
    mejor.mejor_fitness = DBL_MAX; 

    for (int j = 0; j < poblacion; ++j)
    {
        if (objetivo[j].fitness < mejor.mejor_fitness)
        {
            mejor.mejor_fitness = objetivo[j].fitness;
            mejor.mejor_individuo = objetivo[j]; 
        }
    }

    printf("\nMejor fitness de la última generación: %f\n", mejor.mejor_fitness);
    imprimeIndividuo(mejor.mejor_individuo);

    /*Esto es al final
    liberar_memoria_arreglo_estructura_individuo(objetivo);
    liberar_memoria_instancia(instancia_distancias, tamanio_instancia);
    liberar_memoria_instancia(instancia_feromonas, tamanio_instancia);*/
}

