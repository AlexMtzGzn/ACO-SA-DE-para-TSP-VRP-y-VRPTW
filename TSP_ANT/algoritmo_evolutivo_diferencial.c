// algoritmo_evolutivo_diferencial.c
#include <stdio.h>
#include <stdlib.h>
#include "algoritmo_evolutivo_diferencial.h"
#include "asignacion_memoria.h"

double evaluaFO(individuo *ind, double **instancia_feromona, double **instancia)
{
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
        {
            prueba[i] = objetivo[i];
        }
        else
        {
            prueba[i] = ruidoso[i];
        }
    }
}

void seleccion(individuo *objetivo, individuo *prueba, int poblacion)
{
    for (int i = 0; i < poblacion; ++i)
    {
        if (objetivo[i].fitness > prueba[i].fitness)
        {
            objetivo[i] = prueba[i];
        }
    }
}

void imprimePoblacion(individuo *objetivo, int poblacion)
{
    for (int i = 0; i < poblacion; ++i)
        printf("%i - alpha: %f, beta: %f, rho: %f, hormigas: %d, iteraciones: %d, fitness: %f\n", i + 1, objetivo[i].alpha, objetivo[i].beta, objetivo[i].rho, objetivo[i].numHormigas, objetivo[i].numIteraciones, objetivo[i].fitness);
}

void leer_instancia(double **instancia_matriz, int tamanio_instancia, char *archivo)
{

    FILE *instancia = fopen(archivo, "r");

    for (int i = 0; i < tamanio_instancia; i++)
    {
        for (int j = 0; j < tamanio_instancia; j++)
        {
            fscanf(instancia, "%lf", &instancia_matriz[i][j]);
        }
    }
    fclose(instancia);
}

void inializacion_instancia_feromona(double **instancia_matriz, int tamanio_instancia)
{

    for (int i = 0; i < tamanio_instancia; i++)
    {
        for (int j = 0; j < tamanio_instancia; j++)
        {
            if (i == j)
                instancia_matriz[i][j] = 0.0;
            else
                instancia_matriz[i][j] = 1.0;
        }
    }
}

void imprimir_instancias(double **instancia, int tamanio_instancia)
{
    for (int i = 0; i < tamanio_instancia; i++)
    {
        for (int j = 0; j < tamanio_instancia; j++)
        {
            printf("%0.2f ", instancia[i][j]);
        }
        printf("\n");
    }
}

void a_v_d(int tamanio_instancia, char *instancia)
{
    int generacion = 50, poblacion = 20;

    individuo *objetivo = asignar_memoria_arreglo_estructura(poblacion);
    individuo *ruidoso = asignar_memoria_arreglo_estructura(poblacion);
    individuo *prueba = asignar_memoria_arreglo_estructura(poblacion);
    double **instancia_matriz = asignacion_memoria_intancias(tamanio_instancia);
    double **instancia_feremona = asignacion_memoria_intancias(tamanio_instancia);

    leer_instancia(instancia_matriz, tamanio_instancia, instancia);
    inializacion_instancia_feromona(instancia_feremona, tamanio_instancia);
    inicializaPoblacion(objetivo, poblacion);

    for (int i = 0; i < generacion; i++)
    {
        construyeRuidosos(objetivo, ruidoso, poblacion);
        construyePrueba(objetivo, ruidoso, prueba, poblacion);
        for (int j = 0; j < poblacion; ++j)
        {
            objetivo[j].fitness = evaluaFO(&objetivo[j],instancia_feremona,instancia_matriz);
            prueba[j].fitness = evaluaFO(&prueba[j],instancia_feremona,instancia_matriz);
        }

        seleccion(objetivo, prueba, poblacion);
        imprimePoblacion(objetivo, poblacion);
    }

    free(objetivo);
    free(ruidoso);
    free(prueba);
}