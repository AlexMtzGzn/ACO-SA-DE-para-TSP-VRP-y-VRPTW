#include <stdio.h>
#include <stdlib.h>
#include "algoritmo_evolutivo_diferencial.h"
#include "tsp_ant_system.h"

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

void evaluaFO(individuo *ind, double **instancia_feromonas, double **instancia_distancias, int tamanio_instancia)
{
   inializacion_instancia_feromona(instancia_feromonas, tamanio_instancia, ind);

   // Podemos imprimir matriz de Feromonas
   /*printf("\n\n Instancia de Feromonas\n");
   for(int i = 0; i< tamanio_instancia; i++){
      for (int j = 0; j < tamanio_instancia; j++)
      {
         printf(" %.2lf ",instancia_feromonas[i][j]);
      }
      printf("\n");
   }*/

   tsp_ant_system(ind, instancia_feromonas, instancia_distancias, tamanio_instancia);
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

void inicializaPoblacion(individuo *objetivo, int poblacion, int tamanio_instancia)
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

   individuo *objetivo, *ruidoso, *prueba;

   objetivo = (individuo *)malloc(sizeof(individuo) * poblacion);
   ruidoso = (individuo *)malloc(sizeof(individuo) * poblacion);
   prueba = (individuo *)malloc(sizeof(individuo) * poblacion);

   double **instancia_distancias, **instancia_feromonas;

   instancia_distancias = (double **)malloc(tamanio_instancia * sizeof(double *));
   instancia_feromonas = (double **)malloc(tamanio_instancia * sizeof(double *));
   for (int i = 0; i < tamanio_instancia; i++)
   {
      instancia_distancias[i] = (double *)malloc(tamanio_instancia * sizeof(double));
      instancia_feromonas[i] = (double *)malloc(tamanio_instancia * sizeof(double));
   }

   leer_instancia(instancia_distancias, tamanio_instancia, archivo_instancia);

   // Podemos imprimir matriz de distancias
   /*printf("\n\n Instancia de Distancias\n");
   for(int i = 0; i< tamanio_instancia; i++){
      for (int j = 0; j < tamanio_instancia; j++)
      {
         printf(" %.2lf ",instancia_distancias[i][j]);
      }
      printf("\n");
   }*/

   inicializaPoblacion(objetivo, poblacion, tamanio_instancia);
   // Podemos imprimir el objetivo n
   /*printf("\nPoblacion Inicial");
   for(int i = 0; i < poblacion; i++)
      printf("\nAlpha: %.2lf, Beta: %.2lf, Rho: %.2lf, Hormigas: %d, Iteraciones: %d\n",
          objetivo[i].alpha,
          objetivo[i].beta,
          objetivo[i].rho,
          objetivo[i].numHormigas,
          objetivo[i].numIteraciones);*/

   for (int i = 0; i < generaciones; i++)
   {
      construyeRuidosos(objetivo, ruidoso, poblacion);
      construyePrueba(objetivo, ruidoso, prueba, poblacion);

      for (int j = 0; j < poblacion; ++j)
      {
         evaluaFO(&objetivo[j], instancia_feromonas, instancia_distancias, tamanio_instancia);
         // Podemos imprimir el objetivo n
         /*printf("\n\nAlpha: %.2lf, Beta: %.2lf, Rho: %.2lf, Hormigas: %d, Iteraciones: %d, Fitness: %.2lf\n",
                objetivo[j].alpha,
                objetivo[j].beta,
                objetivo[j].rho,
                objetivo[j].numHormigas,
                objetivo[j].numIteraciones,
                objetivo[j].fitness);
         printf("Ruta : ");
         for (int k = 0; k <= tamanio_instancia; k++)
         {
            if (k < tamanio_instancia)
               printf("%d -> ", objetivo[j].ruta[k]);
            else
               printf("%d\n", objetivo[j].ruta[k]);
         }*/
         evaluaFO(&prueba[j], instancia_feromonas, instancia_distancias, tamanio_instancia);

         // Podemos imprimir la prueba n
         /*printf("\n\nAlpha: %.2lf, Beta: %.2lf, Rho: %.2lf, Hormigas: %d, Iteraciones: %d, Fitness: %.2lf\n",
                prueba[j].alpha,
                prueba[j].beta,
                prueba[j].rho,
                prueba[j].numHormigas,
                prueba[j].numIteraciones,
                prueba[j].fitness);
         printf("Ruta : ");
         for (int k = 0; k <= tamanio_instancia; k++)
         {
            if (k < tamanio_instancia)
               printf("%d -> ", prueba[j].ruta[k]);
            else
               printf("%d\n", prueba[j].ruta[k]);
         }*/
      }

      seleccion(objetivo, prueba, poblacion);

      if (i == generaciones - 1)
      {
         // Imprime todos los individuos de la ultima generacion
         printf("\n\nUltima Generacion De La Poblacion:");
         for (int j = 0; j < poblacion; ++j)
         {
            evaluaFO(&objetivo[j], instancia_feromonas, instancia_distancias, tamanio_instancia);
            // Podemos imprimir el objetivo n
            printf("\n\nAlpha: %.2lf, Beta: %.2lf, Rho: %.2lf, Hormigas: %d, Iteraciones: %d, Fitness: %.2lf\n",
                   objetivo[j].alpha,
                   objetivo[j].beta,
                   objetivo[j].rho,
                   objetivo[j].numHormigas,
                   objetivo[j].numIteraciones,
                   objetivo[j].fitness);
            printf("Ruta : ");
            for (int k = 0; k <= tamanio_instancia; k++)
            {
               if (k < tamanio_instancia)
                  printf("%d -> ", objetivo[j].ruta[k]);
               else
                  printf("%d\n", objetivo[j].ruta[k]);
            }
         }
      }
   }

   int indice_mejor = 0;
   for (int j = 1; j < poblacion; ++j)
   {
      if (objetivo[j].fitness < objetivo[indice_mejor].fitness)
      {
         indice_mejor = j;
      }
   }

   // Podemos imprimir las mejores soluciones
   printf("\n\nLas Mejores Soluciones De La Ultima Generacion");
   for (int i = 0; i < poblacion; i++)
   {
      if (objetivo[indice_mejor].fitness == objetivo[i].fitness)
      {
         printf("\n\nAlpha: %.2lf, Beta: %.2lf, Rho: %.2lf, Hormigas: %d, Iteraciones: %d, Fitness: %.2lf\n",
                objetivo[i].alpha,
                objetivo[i].beta,
                objetivo[i].rho,
                objetivo[i].numHormigas,
                objetivo[i].numIteraciones,
                objetivo[i].fitness);
         printf("Ruta : ");
         for (int j = 0; j <= tamanio_instancia; j++)
         {
            if (j < tamanio_instancia)
                  printf("%d -> ", objetivo[i].ruta[j]);
               else
                  printf("%d\n", objetivo[i].ruta[j]);
         }
      }
   }

   for (int i = 0; i < tamanio_instancia; i++)
   {
      free(instancia_distancias[i]);
      free(instancia_feromonas[i]);
   }
   free(instancia_distancias);
   free(instancia_feromonas);
   for (int i = 0; i < poblacion; i++)
   {
      free(objetivo[i].ruta);
   }
   free(objetivo);
   free(ruidoso);
   free(prueba);
}
