#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "AED.h"
// #include "TSP_ACO.h"
#include "entrada_salida_datos.h"
#include "control_memoria.h"

void inializacion_instancia_feromona(double **instancia_feromonas, int tamanio_instancia, individuo *ind)
{
   for (int i = 0; i < tamanio_instancia; i++)
      for (int j = 0; j < tamanio_instancia; j++)
      {
         if (i == j)
            instancia_feromonas[i][j] = 0.0;
         else
            instancia_feromonas[i][j] = ind->alpha;
      }
}

void evaluaFO_AED(individuo *ind, double **instancia_feromonas, double **instancia_distancias, int tamanio_instancia)
{
   inializacion_instancia_feromona(instancia_feromonas, tamanio_instancia, ind);

   // Podemos imprimir matriz de Feromonas
   // imprimir_instancia(instancia_feromonas,tamanio_instancia,"Instancia de Feromonas");
   //tsp_aco(ind, instancia_feromonas, instancia_distancias, tamanio_instancia);
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
      ruidoso[i].gamma = objetivo[aleatorio1].gamma + 0.5 * (objetivo[aleatorio2].gamma - objetivo[aleatorio3].gamma);
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

      if (ruidoso[i].gamma > 1.5)
         ruidoso[i].gamma = 1.5;

      if (ruidoso[i].gamma < 0)
         ruidoso[i].gamma = 0;

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

void inicializaPoblacion(individuo *objetivo, int poblacion, int tamanio_instancia)
{
   for (int i = 0; i < poblacion; ++i)
   {
      objetivo[i].alpha = generaAleatorio(0.1, 2.0);
      objetivo[i].beta = generaAleatorio(1.5, 2.5);
      objetivo[i].gamma = generaAleatorio(0.0,1.5);
      objetivo[i].rho = generaAleatorio(0.0, 1.0);
      objetivo[i].numHormigas = (int)generaAleatorio(10, 20);
      objetivo[i].numIteraciones = (int)generaAleatorio(20, 50);
   }
}

int aed_vrp_tw(int num_poblacion, int num_generaciones, int num_clientes /*, faltan algunos valores*/)
{

   individuo *objetivo = asignar_memoria_individuos(num_poblacion);                        /*Arreglo para objetivos*/
   individuo *ruidoso = asignar_memoria_individuos(num_poblacion);                         /*Arreglo para ruidosos*/
   individuo *prueba = asignar_memoria_individuos(num_poblacion);                          /*Arreglo para prueba*/
   generacion *generacion = asignar_memoria_generaciones(num_poblacion, num_generaciones); /*Arreglo para prueba*/

   for (int i = 0; i < num_generaciones; i++)
   {
      construyeRuidosos(objetivo, ruidoso, num_poblacion);
      construyePrueba(objetivo, ruidoso, prueba, num_poblacion);

      for (int j = 0; j < num_poblacion; ++j)
      {
         /*evaluaFO_AED(&objetivo[j], instancia_feromonas, instancia_distancias, tamanio_instancia);
         evaluaFO_AED(&prueba[j], instancia_feromonas, instancia_distancias, tamanio_instancia);*/

         /*if (objetivo[j].fitness < individuo_mejor_global->fitness)
         {
            individuo_mejor_global->alpha = objetivo[j].alpha;
            individuo_mejor_global->beta = objetivo[j].beta;
            individuo_mejor_global->rho = objetivo[j].rho;
            individuo_mejor_global->numHormigas = objetivo[j].numHormigas;
            individuo_mejor_global->numIteraciones = objetivo[j].numIteraciones;
            individuo_mejor_global->fitness = objetivo[j].fitness;

            for (int k = 0; k <= tamanio_instancia; k++)
               individuo_mejor_global->ruta[k] = objetivo[j].ruta[k];
         }

         if (prueba[j].fitness < individuo_mejor_global->fitness)
         {
            individuo_mejor_global->alpha = objetivo[j].alpha;
            individuo_mejor_global->beta = objetivo[j].beta;
            individuo_mejor_global->rho = objetivo[j].rho;
            individuo_mejor_global->numHormigas = objetivo[j].numHormigas;
            individuo_mejor_global->numIteraciones = objetivo[j].numIteraciones;
            individuo_mejor_global->fitness = objetivo[j].fitness;

            for (int k = 0; k <= tamanio_instancia; k++)
               individuo_mejor_global->ruta[k] = prueba[j].ruta[k];
         }
      }*/

      for (int j = 0; j < poblacion; j++)
      {
         indice_generacion = i * poblacion + j;
         generacion[indice_generacion].fitness = objetivo[j].fitness;
         generacion[indice_generacion].generacion = i + 1;
         generacion[indice_generacion].poblacion = j + 1;
      }

      seleccion(objetivo, prueba, num_poblacion);

      if (i == num_generaciones - 1)
      {
         for (int j = 0; j < num_poblacion; j++)
         {
            //evaluaFO_AED(&objetivo[j], instancia_feromonas, instancia_distancias, tamanio_instancia);
            indice_generacion = i * poblacion + j;
            generacion[indice_generacion].fitness = objetivo[j].fitness;
            generacion[indice_generacion].generacion = i + 1;
            generacion[indice_generacion].poblacion = j + 1;

            if (objetivo[j].fitness < individuo_prueba->fitness)
            {
               indice_mejor = j;
               individuo_prueba->alpha = objetivo[j].alpha;
               individuo_prueba->beta = objetivo[j].beta;
               individuo_prueba->rho = objetivo[j].rho;
               individuo_prueba->numHormigas = objetivo[j].numHormigas;
               individuo_prueba->numIteraciones = objetivo[j].numIteraciones;
            }
         }
      }
   }

}