// algoritmo_evolutivo_diferencial.c
#include <stdio.h>
#include <stdlib.h>
#include "algoritmo_evolutivo_diferencial.h"
#include "asignacion_memoria.h"

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
        objetivo[i].beta = generaAleatorio(0.1, 5.0);
        objetivo[i].rho = generaAleatorio(0.1, 0.9);
        objetivo[i].numHormigas = (int)generaAleatorio(10, 100);
        objetivo[i].numIteraciones = (int)generaAleatorio(100, 1000);
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

        if (ruidoso[i].beta > 5.0)
            ruidoso[i].beta = 5.0;

        if (ruidoso[i].beta < 0.1)
            ruidoso[i].beta = 0.1;

        if (ruidoso[i].rho > 0.9)
            ruidoso[i].rho = 0.9;
      
        if (ruidoso[i].rho < 0.1)
            ruidoso[i].rho = 0.1;
      
        if (ruidoso[i].numHormigas > 100)
            ruidoso[i].numHormigas = 100;
      
        if (ruidoso[i].numHormigas < 10)
            ruidoso[i].numHormigas = 10;
      
        if (ruidoso[i].numIteraciones > 1000)
            ruidoso[i].numIteraciones = 1000;
      
        if (ruidoso[i].numIteraciones < 100)
            ruidoso[i].numIteraciones = 100;
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

void imprimePoblacion(individuo *objetivo, int poblacion) {
    for (int i = 0; i < poblacion; ++i) {
        printf("\nSoy el individuo %d de la población: ", i + 1);
        printf("alpha: %f ", objetivo[i].alpha);
        printf("beta: %f ", objetivo[i].beta);
        printf("rho: %f ", objetivo[i].rho);
        printf("Número de hormigas: %d ", objetivo[i].numHormigas);
        printf("Número de iteraciones: %d ", objetivo[i].numIteraciones);
        printf("\nCon fitness: %f", objetivo[i].fitness);
    }
}

void a_v_d(int poblacion)
{
    int generacion = 0;

    individuo *objetivo = asignar_memoria_arreglo(poblacion);
    individuo *ruidoso = asignar_memoria_arreglo(poblacion);
    individuo *prueba = asignar_memoria_arreglo(poblacion);

    inicializaPoblacion(objetivo, poblacion);

    while (generacion < 1000)
    {
        construyeRuidosos(objetivo, ruidoso, poblacion);
        construyePrueba(objetivo, ruidoso, prueba, poblacion);
        for (int i = 0; i < poblacion; ++i)
        {
            objetivo[i].fitness = evaluaFO(&objetivo[i]);
            prueba[i].fitness = evaluaFO(&prueba[i]);
        }

        seleccion(objetivo, prueba, poblacion);
        imprimePoblacion(objetivo, poblacion);
        generacion++;
    }

    free(objetivo);
    free(ruidoso);
    free(prueba);
}