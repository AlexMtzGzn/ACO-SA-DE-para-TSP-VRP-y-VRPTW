#include <stdio.h>
#include <stdlib.h>
#include "algoritmo_evolutivo_diferencial.h"

void inializacion_instancia_feromona(double **instancia_feromona, int tamanio_instancia, individuo *ind)
{
   for (int i = 0; i < tamanio_instancia; i++)
      for (int j = 0; j < tamanio_instancia; j++)
      {
         if (i == j)
            instancia_feromona[i][j] = 0.0;
         else
            instancia_feromona[i][j] = ind->alpha;
      }
}

void evaluaFO(individuo *ind, double **instancia_feromona, double **instancia_distancias, int tamanio_instancia)
{
   inializacion_instancia_feromona(instancia_feromona, tamanio_instancia, ind);
}

double generaAleatorio(double minimo, double maximo)
{
   double aleatorio = (double)rand() / RAND_MAX;
   return minimo + aleatorio * (maximo - minimo);
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

void algoritmo_evolutivo_diferencial(int poblacion, int generaciones, int tamanio_instancia, char *archivo_instancia)
{
   // Declaracion de estruturas de objetivo, ruidos, prueba
   individuo *objetivo, *ruidoso, *prueba;
   // Aqui vamos a asignar memoria de las estrcuturas
   objetivo = (individuo *)malloc(sizeof(individuo) * poblacion);
   ruidoso = (individuo *)malloc(sizeof(individuo) * poblacion);
   prueba = (individuo *)malloc(sizeof(individuo) * poblacion);
   // Declaramos matrices de distancias y de feromona
   double **instancia_distancias, **instancia_feromonas;
   // Aqui vamos asignar memoria a las matrices
   instancia_distancias = (double **)malloc(tamanio_instancia * sizeof(double *));
   instancia_feromonas = (double **)malloc(tamanio_instancia * sizeof(double *));
   for (int i = 0; i < tamanio_instancia; i++)
   {
      instancia_distancias[i] = (double *)malloc(tamanio_instancia * sizeof(double));
      instancia_feromonas[i] = (double *)malloc(tamanio_instancia * sizeof(double));
   }
   // Aqui vamos a leer la instacnia distancia
   leer_instancia(instancia_distancias, tamanio_instancia, archivo_instancia);
   // Aqui inicializamos los datos de objetivo de manera aleatoria
   inicializaPoblacion(objetivo, poblacion);
   // Aqui podemos imprimir los datos de la poblacion
   for (int i = 0; i < poblacion; ++i)
      printf("%i - alpha: %f, beta: %f, rho: %f, hormigas: %d, iteraciones: %d, fitness: %f\n", i + 1, objetivo[i].alpha, objetivo[i].beta, objetivo[i].rho, objetivo[i].numHormigas, objetivo[i].numIteraciones, objetivo[i].fitness);

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
   }
   //Aqui limpiamos memoria
   for (int i = 0; i < tamanio_instancia; i++)
    {
        free(instancia_distancias[i]);
        free(instancia_feromonas[i]);
    }
    free(instancia_distancias);
    free(instancia_feromonas);
    free(objetivo);
    free(ruidoso);
    free(prueba);
}
