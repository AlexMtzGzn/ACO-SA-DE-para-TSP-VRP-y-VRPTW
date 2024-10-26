#include <stdio.h>
#include <stdlib.h>
#include "AED.h"
#include "TSP_SA.h"
#include "control_memoria.h"
#include "entrada_salida_datos.h"

void evaluaFO_AED(individuo *ind, double **instancia_distancias, int tamanio_instancia)
{
   tsp_sa(ind, instancia_distancias, tamanio_instancia);
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

      ruidoso[i].temperatura_inicial = objetivo[aleatorio1].temperatura_inicial + 0.5 * (objetivo[aleatorio2].temperatura_inicial - objetivo[aleatorio3].temperatura_inicial);
      ruidoso[i].temperatura_final = objetivo[aleatorio1].temperatura_final + 0.5 * (objetivo[aleatorio2].temperatura_final - objetivo[aleatorio3].temperatura_final);
      ruidoso[i].enfriamiento = objetivo[aleatorio1].enfriamiento + 0.5 * (objetivo[aleatorio2].enfriamiento - objetivo[aleatorio3].enfriamiento);
      ruidoso[i].numIteraciones = objetivo[aleatorio1].numIteraciones + (int)(0.5 * (objetivo[aleatorio2].numIteraciones - objetivo[aleatorio3].numIteraciones));

      if (ruidoso[i].temperatura_inicial > 1000.0)
         ruidoso[i].temperatura_inicial = 1000.0;

      if (ruidoso[i].temperatura_inicial < 900.0)
         ruidoso[i].temperatura_inicial = 900.0;

      if (ruidoso[i].temperatura_final > 0.25)
         ruidoso[i].temperatura_final = 0.25;

      if (ruidoso[i].temperatura_final < 0.1)
         ruidoso[i].temperatura_final = 0.1;

      if (ruidoso[i].enfriamiento > 0.99)
         ruidoso[i].enfriamiento = 0.99;

      if (ruidoso[i].enfriamiento < 0.9)
         ruidoso[i].enfriamiento = 0.9;

      if (ruidoso[i].numIteraciones > 20)
         ruidoso[i].numIteraciones = 20;

      if (ruidoso[i].numIteraciones < 5)
         ruidoso[i].numIteraciones = 5;
   }
}

void construyePrueba(individuo *objetivo, individuo *ruidoso, individuo *prueba, int poblacion)
{
   for (int i = 0; i < poblacion; ++i)
   {
      double aleatorio = (double)rand() / RAND_MAX;
      prueba[i] = (aleatorio <= 0.5) ? objetivo[i] : ruidoso[i];
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
      objetivo[i].temperatura_inicial = generaAleatorio(900.0, 1000.0);
      objetivo[i].temperatura_final = generaAleatorio(0.1, 0.25);
      objetivo[i].enfriamiento = generaAleatorio(0.9, 0.99);
      objetivo[i].numIteraciones = (int)generaAleatorio(5, 20);
   }
}

void aed(int poblacion, int generaciones, int tamanio_instancia, char *archivo_instancia)
{
   individuo *objetivo = asignar_memoria_individuos(poblacion);
   individuo *ruidoso = asignar_memoria_individuos(poblacion);
   individuo *prueba = asignar_memoria_individuos(poblacion);
   double **instancia_distancias = asignar_memoria_instancia(tamanio_instancia);

   leer_instancia(instancia_distancias, tamanio_instancia, archivo_instancia);
   imprimir_instancia(instancia_distancias, tamanio_instancia, "Instancia de Distancias");

   inicializaPoblacion(objetivo, poblacion);

   for (int i = 0; i < generaciones; i++)
   {
      construyeRuidosos(objetivo, ruidoso, poblacion);
      construyePrueba(objetivo, ruidoso, prueba, poblacion);

      for (int j = 0; j < poblacion; ++j)
      {
         evaluaFO_AED(&objetivo[j], instancia_distancias, tamanio_instancia);
         evaluaFO_AED(&prueba[j], instancia_distancias, tamanio_instancia);
      }

      seleccion(objetivo, prueba, poblacion);
   }

   int indice_mejor = 0;
   for (int j = 1; j < poblacion; ++j)
      if (objetivo[j].fitness < objetivo[indice_mejor].fitness)
         indice_mejor = j;

   printf("\n\nMejor Individuo de la Ultima Generacion\n");
   imprimir_ind(&objetivo[indice_mejor], tamanio_instancia, poblacion);

   individuo *individuo_prueba = asignar_memoria_individuos(1);
   *individuo_prueba = objetivo[indice_mejor];
   evaluaFO_AED(individuo_prueba, instancia_distancias, tamanio_instancia);

   printf("\n\nPrueba de Mejor Individuo: \n");
   imprimir_ind(individuo_prueba, tamanio_instancia, 1);

   /* liberar_instancia(instancia_distancias, tamanio_instancia);
    liberar_individuos(objetivo);
    liberar_individuos(prueba);
    liberar_individuos(ruidoso);
    liberar_individuos(individuo_prueba);*/
}
