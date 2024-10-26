#include <stdio.h>
#include <stdlib.h>
#include "AED.h"

void evaluaFO_AED()
{
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

      ruidoso[i].temperatura_incial = objetivo[aleatorio1].temperatura_final + 0.5 * (objetivo[aleatorio2].temperatura_final - objetivo[aleatorio3].temperatura_final);
      ruidoso[i].temperatura_final = objetivo[aleatorio1].temperatura_final + 0.5 * (objetivo[aleatorio2].temperatura_final - objetivo[aleatorio3].temperatura_final);
      ruidoso[i].factor_enfriamiento = objetivo[aleatorio1].factor_enfriamiento + 0.5 * (objetivo[aleatorio2].factor_enfriamiento - objetivo[aleatorio3].factor_enfriamiento);
      ruidoso[i].numIteraciones = objetivo[aleatorio1].numIteraciones + (int)(0.5 * (objetivo[aleatorio2].numIteraciones - objetivo[aleatorio3].numIteraciones));

      if (ruidoso[i].temperatura_incial > 1000.0)
         ruidoso[i].temperatura_incial = 1000.0;

      if (ruidoso[i].temperatura_incial < 900.0)
         ruidoso[i].temperatura_incial = 900.0;

      if (ruidoso[i].temperatura_final > 0.25)
         ruidoso[i].temperatura_final = 0.25;

      if (ruidoso[i].temperatura_final < 0.1)
         ruidoso[i].temperatura_final = 0.1;

      if (ruidoso[i].factor_enfriamiento > 0.99)
         ruidoso[i].factor_enfriamiento = 0.99;

      if (ruidoso[i].factor_enfriamiento < 0.9)
         ruidoso[i].factor_enfriamiento = 0.9;

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
      objetivo[i].temperatura_incial = generaAleatorio(900.0, 1000.0);
      objetivo[i].temperatura_final = generaAleatorio(0.1, 0.25);
      objetivo[i].factor_enfriamiento = generaAleatorio(0.9, 0.99);
      objetivo[i].numIteraciones = (int)generaAleatorio(5, 20);
   }
}

void aed(int poblacion, int generaciones, int tamanio_instancia, char *archivo_instancia)
{
   individuo *objetivo, *ruidoso, *prueba, *individuo_prueba;

   objetivo = asignar_memoria_individuos(poblacion);
   ruidoso = asignar_memoria_individuos(poblacion);
   prueba = asignar_memoria_individuos(poblacion);

   double **instancia_distancias;

   instancia_distancias = (double **)malloc(tamanio_instancia * sizeof(double *));

   leer_instancia(instancia_distancias, tamanio_instancia, archivo_instancia);

   inicializaPoblacion(objetivo, poblacion, tamanio_instancia);

   for (int i = 0; i < generaciones; i++)
   {
      construyeRuidosos(objetivo, ruidoso, poblacion);
      construyePrueba(objetivo, ruidoso, prueba, poblacion);

      for (int j = 0; j < poblacion; ++j)
      {
         evaluaFO_AED();
         evaluaFO_AED();
      }

      seleccion(objetivo, prueba, poblacion);

      if (i == generaciones - 1)
      {
         for (int j = 0; j < poblacion; ++j)
            evaluaFO_AED();
      }
   }

   int indice_mejor = 0;
   for (int j = 1; j < poblacion; ++j)
      if (objetivo[j].fitness < objetivo[indice_mejor].fitness)
         indice_mejor = j;

   // Imprimir el mejor individuo de la última generación
   printf("\n\nMejor Individuo de la Ultima Generacion\n");
   imprimir_ind(&objetivo[indice_mejor], tamanio_instancia, poblacion);

   // Asignar memoria para un único individuo de prueba
   individuo_prueba = asignar_memoria_individuos(1);
   individuo_prueba->temperatura_incial = objetivo[indice_mejor].temperatura_incial;
   individuo_prueba->temperatura_final = objetivo[indice_mejor].temperatura_final;
   individuo_prueba->factor_enfriamiento = objetivo[indice_mejor].factor_enfriamiento;
   individuo_prueba->numIteraciones = objetivo[indice_mejor].numIteraciones;

   // Evaluar el mejor individuo
   evaluaFO_AED();

   // Imprimir el resultado del individuo de prueba
   printf("\n\nPrueba de Mejor Individuo: \n");
   imprimir_ind(individuo_prueba, tamanio_instancia, 1);

   // Liberar memoria
   // liberar_instancia(instancia_distancias, tamanio_instancia);
   // liberar_individuos(objetivo, true);
   // liberar_individuos(prueba, true);
   // liberar_individuos(ruidoso, false);
   // liberar_individuos(individuo_prueba, true);
}
