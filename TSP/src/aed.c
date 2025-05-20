#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../include/aed.h"
#include "../include/estructuras.h"
#include "../include/tsp_aco.h"
#include "../include/configuracion_tsp.h"
#include "../include/control_memoria.h"
#include "../include/salida_datos.h"
#include "../include/configuracion_json.h"

// Calcula la distancia euclidiana entre dos clientes dados sus índices
double calcular_distancia(struct tsp_configuracion *tsp, int cliente_origen, int cliente_destino)
{
   // Declaramos distancia
   double distancia;
   // Calculamos la distancia entre el cliente origen y el cliente destino
   distancia = sqrt(pow((tsp->clientes[cliente_destino].coordenada_x - tsp->clientes[cliente_origen].coordenada_x), 2.0) + pow((tsp->clientes[cliente_destino].coordenada_y - tsp->clientes[cliente_origen].coordenada_y), 2.0));
   // Retornamos la distancia
   return distancia;
}

// Inicializa la visibilidad entre todos los pares de clientes (1 / distancia)
void inicializar_visibilidad(double **instancia_visibilidad, struct tsp_configuracion *tsp)
{
   // Declaramos distancia
   double distancia;
   // Recorre todos los clientes y calcula la visibilidad entre ellos
   for (int i = 0; i < tsp->num_clientes; i++)
   {
      for (int j = i + 1; j < tsp->num_clientes; j++)
      {
         if (i != j)
         {
            // Calculamos la distancia entre los clientes i y j una sola vez
            distancia = calcular_distancia(tsp, i, j);
            instancia_visibilidad[i][j] = 1.0 / distancia;
            instancia_visibilidad[j][i] = instancia_visibilidad[i][j]; // Aprovechamos la simetría
         }
         else
         {
            instancia_visibilidad[i][j] = 0.0; // Es 0.0
         }
      }
   }
}

// Inicializa la matriz de distancias entre clientes
void inicializar_distancias(double **instancia_distancias, struct tsp_configuracion *tsp)
{
   // Declaramos distancia
   double distancia;
   // Recorre todos los clientes y calcula las distancias entre ellos
   for (int i = 0; i < tsp->num_clientes; i++)
   {
      for (int j = i + 1; j < tsp->num_clientes; j++)
      {
         if (i != j)
         {
            // Calculamos la distancia entre los clientes i y j
            distancia = calcular_distancia(tsp, i, j);
            instancia_distancias[i][j] = distancia;
            instancia_distancias[j][i] = instancia_distancias[i][j]; // Aprovechamos la simetría
         }
         else
         {
            instancia_distancias[i][j] = 0.0; // Es 0.0
         }
      }
   }
}

// Inicializa la matriz de feromonas con valores predeterminados
void inicializar_feromona(struct tsp_configuracion *tsp, double **instancia_feromona)
{
   // Recorre todos los clientes en la matriz de feromonas (fila i, columna j)
   for (int i = 0; i < tsp->num_clientes; i++)
   {
      for (int j = i + 1; j < tsp->num_clientes; j++)
      {
         // Si i y j son diferentes (es decir, no es la misma ciudad), se asigna un valor de feromona de 1.0
         if (i != j)
            instancia_feromona[i][j] = 1.0;
         else
            instancia_feromona[i][j] = 0.0; // Si son iguales, se asigana 0.0
      }
   }
}

// Evalúa la función objetivo del individuo usando el algoritmo ACO
void evaluaFO_AED(struct individuo *ind, double **instancia_feromona, double **instancia_visibilidad, double **instancia_distancias, struct tsp_configuracion *tsp)
{
   // Inicializa las feromonas en la instancia
   inicializar_feromona(tsp, instancia_feromona);
   // Imprimimos la instancia de feromonass
   // imprimir_instancia(instancia_feromonas,tsp,"INSTANCIA FEROMONAS");
   //  Ejecuta el algoritmo de optimización con ventanas de tiempo (ACO) en el individuo
   tsp_aco(tsp, ind, instancia_visibilidad, instancia_distancias, instancia_feromona);
}

// Genera un número aleatorio en el rango [minimo, maximo]
double genera_aleatorio(double minimo, double maximo)
{
   // Genera un número aleatorio entre 0 y 1, luego lo escala al rango deseado
   double aleatorio;
   aleatorio = minimo + ((double)rand() / RAND_MAX) * (maximo - minimo);
   return aleatorio;
}

// Construye una población ruidosa a partir de la población objetivo aplicando perturbaciones
void construye_ruidosos(struct individuo *objetivo, struct individuo *ruidoso, struct rangos *rango, int poblacion)
{
   // Recorre cada individuo de la población ruidosa
   for (int i = 0; i < poblacion; ++i)
   {
      // Selecciona tres índices aleatorios diferentes de la población objetivo
      int aleatorio1 = rand() % poblacion;
      int aleatorio2 = rand() % poblacion;
      int aleatorio3 = rand() % poblacion;

      // Asegura que los índices seleccionados sean distintos entre sí
      while (aleatorio1 == aleatorio2 || aleatorio2 == aleatorio3 || aleatorio1 == aleatorio3)
      {
         aleatorio1 = rand() % poblacion;
         aleatorio2 = rand() % poblacion;
         aleatorio3 = rand() % poblacion;
      }

      // Genera un individuo ruidoso como una combinación de tres individuos aleatorios
      ruidoso[i].alpha = objetivo[aleatorio1].alpha + 0.5 * (objetivo[aleatorio2].alpha - objetivo[aleatorio3].alpha);
      ruidoso[i].beta = objetivo[aleatorio1].beta + 0.5 * (objetivo[aleatorio2].beta - objetivo[aleatorio3].beta);
      ruidoso[i].rho = objetivo[aleatorio1].rho + 0.5 * (objetivo[aleatorio2].rho - objetivo[aleatorio3].rho);
      ruidoso[i].numHormigas = objetivo[aleatorio1].numHormigas + (int)(0.5 * (objetivo[aleatorio2].numHormigas - objetivo[aleatorio3].numHormigas));
      ruidoso[i].numIteracionesACO = objetivo[aleatorio1].numIteracionesACO + (int)(0.5 * (objetivo[aleatorio2].numIteracionesACO - objetivo[aleatorio3].numIteracionesACO));
      ruidoso[i].temperatura_inicial = objetivo[aleatorio1].temperatura_inicial + 0.5 * (objetivo[aleatorio2].temperatura_inicial - objetivo[aleatorio3].temperatura_inicial);
      ruidoso[i].temperatura_final = objetivo[aleatorio1].temperatura_final + 0.5 * (objetivo[aleatorio2].temperatura_final - objetivo[aleatorio3].temperatura_final);
      ruidoso[i].factor_enfriamiento = objetivo[aleatorio1].factor_enfriamiento + 0.5 * (objetivo[aleatorio2].factor_enfriamiento - objetivo[aleatorio3].factor_enfriamiento);
      ruidoso[i].factor_control = objetivo[aleatorio1].factor_control + 0.5 * (objetivo[aleatorio2].factor_control - objetivo[aleatorio3].factor_control);
      ruidoso[i].numIteracionesSA = objetivo[aleatorio1].numIteracionesSA + (int)(0.5 * (objetivo[aleatorio2].numIteracionesSA - objetivo[aleatorio3].numIteracionesSA));

      // Limita los valores de los parámetros para asegurarse de que estén dentro de un rango válido

      // Limita 'alpha' a estar dentro de los valores mínimos y máximos
      if (ruidoso[i].alpha > rango->maxAlpha)
         ruidoso[i].alpha = rango->maxAlpha;

      if (ruidoso[i].alpha < rango->minAlpha)
         ruidoso[i].alpha = rango->minAlpha;

      // Limita 'beta' a estar dentro de los valores mínimos y máximos
      if (ruidoso[i].beta > rango->maxBeta)
         ruidoso[i].beta = rango->maxBeta;

      if (ruidoso[i].beta < rango->minBeta)
         ruidoso[i].beta = rango->minBeta;

      // Limita 'rho' a estar dentro de los valores mínimos y máximos
      if (ruidoso[i].rho > rango->maxRho)
         ruidoso[i].rho = rango->maxRho;

      if (ruidoso[i].rho < rango->minRho)
         ruidoso[i].rho = rango->minRho;

      // Limita 'numHormigas' a estar dentro de los valores mínimos y máximos
      if (ruidoso[i].numHormigas > rango->maxNumHormigas)
         ruidoso[i].numHormigas = rango->maxNumHormigas;

      if (ruidoso[i].numHormigas < rango->minNumHormigas)
         ruidoso[i].numHormigas = rango->minNumHormigas;

      // Limita 'numIteracionesACO' a estar dentro de los valores mínimos y máximos
      if (ruidoso[i].numIteracionesACO > rango->maxNumIteracionesACO)
         ruidoso[i].numIteracionesACO = rango->maxNumIteracionesACO;

      if (ruidoso[i].numIteracionesACO < rango->minNumIteracionesACO)
         ruidoso[i].numIteracionesACO = rango->minNumIteracionesACO;

      // Limita 'temperatura_inicial' a estar dentro de los valores mínimos y máximos
      if (ruidoso[i].temperatura_inicial > rango->maxTemperatura_inicial)
         ruidoso[i].temperatura_inicial = rango->maxTemperatura_inicial;

      if (ruidoso[i].temperatura_inicial < rango->minTemperatura_inicial)
         ruidoso[i].temperatura_inicial = rango->minTemperatura_inicial;

      // Limita 'temperatura_final' a estar dentro de los valores mínimos y máximos
      if (ruidoso[i].temperatura_final > rango->maxTemperatura_final)
         ruidoso[i].temperatura_final = rango->maxTemperatura_final;

      if (ruidoso[i].temperatura_final < rango->minTemperatura_final)
         ruidoso[i].temperatura_final = rango->minTemperatura_final;

      // Limita 'factor_enfriamiento' a estar dentro de los valores mínimos y máximos
      if (ruidoso[i].factor_enfriamiento > rango->maxFactor_enfriamiento)
         ruidoso[i].factor_enfriamiento = rango->maxFactor_enfriamiento;

      if (ruidoso[i].factor_enfriamiento < rango->minFactor_enfriamiento)
         ruidoso[i].factor_enfriamiento = rango->minFactor_enfriamiento;

      // Limita 'factor_control' a estar dentro de los valores mínimos y máximos
      if (ruidoso[i].factor_control > rango->maxFactor_control)
         ruidoso[i].factor_control = rango->maxFactor_control;

      if (ruidoso[i].factor_control < rango->minFactor_control)
         ruidoso[i].factor_control = rango->minFactor_control;

      // Limita 'numIteracionesSA' a estar dentro de los valores mínimos y máximos
      if (ruidoso[i].numIteracionesSA > rango->maxIteracionesSA)
         ruidoso[i].numIteracionesSA = rango->maxIteracionesSA;

      if (ruidoso[i].numIteracionesSA < rango->minIteracionesSA)
         ruidoso[i].numIteracionesSA = rango->minIteracionesSA;
   }
}

// Esta función construye una población de prueba combinando aleatoriamente individuos de dos poblaciones distintas: objetivo y ruidosa.
void construye_prueba(struct individuo *objetivo, struct individuo *ruidoso, struct individuo *prueba, int poblacion)
{
   double aleatorio; // Variable para almacenar un número aleatorio

   // Itera sobre todos los individuos en la población.
   for (int i = 0; i < poblacion; ++i)
   {
      // Genera un número aleatorio en el rango [0,1].
      aleatorio = (double)rand() / RAND_MAX;

      // Con una probabilidad del 50%, selecciona el individuo de la población objetivo.
      if (aleatorio <= 0.5)
         prueba[i] = objetivo[i];
      // En caso contrario, selecciona el individuo de la población ruidosa.
      else
         prueba[i] = ruidoso[i];
   }
}

// Esta función compara los individuos de dos poblaciones y reemplaza al individuo en 'objetivo' si el de 'prueba' es mejor.
// En este contexto, se asume que un menor valor de fitness representa una mejor solución.
void seleccion(struct individuo *objetivo, struct individuo *prueba, int poblacion)
{
   // Recorremos la población para comparar cada individuo de ambas poblaciones.
   for (int i = 0; i < poblacion; ++i)
      // Si el fitness del individuo en la población objetivo es mayor que en la población de prueba,
      // se actualiza el individuo en la población objetivo por el de la población de prueba.
      if (objetivo[i].fitness > prueba[i].fitness)
         objetivo[i] = prueba[i];
}

// Esta función inicializa los individuos de la población con valores aleatorios dentro de ciertos rangos
// definidos específicamente para diferentes tamaños de problema del TSP (26, 51 y 101 clientes).
void inicializa_poblacion(struct individuo *objetivo, struct tsp_configuracion *tsp, struct rangos *rango, int poblacion)
{
   // Asigna rangos específicos según el número de clientes en el TSP.
   // Estos rangos determinan los valores posibles para los parámetros de ACO y SA.

   if (tsp->num_clientes == 26)
   {
      // Parámetros adecuados para instancias pequeñas.
      rango->maxAlpha = 3.0;
      rango->minAlpha = 1.0;

      rango->maxBeta = 3.0;
      rango->minBeta = 1.0;

      rango->maxRho = 0.7;
      rango->minRho = 0.5;

      rango->maxNumHormigas = 25;
      rango->minNumHormigas = 10;

      rango->maxNumIteracionesACO = 100;
      rango->minNumIteracionesACO = 50;

      rango->maxTemperatura_inicial = 2000.0;
      rango->minTemperatura_inicial = 1000.0;

      rango->maxTemperatura_final = 0.5;
      rango->minTemperatura_final = 0.1;

      rango->maxFactor_enfriamiento = 0.999;
      rango->minFactor_enfriamiento = 0.99;

      rango->maxFactor_control = 0.7;
      rango->minFactor_control = 0.5;

      rango->maxIteracionesSA = 150;
      rango->minIteracionesSA = 100;
   }

   if (tsp->num_clientes == 51)
   {
      // Rango medio: requiere más recursos computacionales que el caso de 26 clientes.
      rango->maxAlpha = 4.0;
      rango->minAlpha = 2.0;

      rango->maxBeta = 4.0;
      rango->minBeta = 2.0;

      rango->maxRho = 0.6;
      rango->minRho = 0.4;

      rango->maxNumHormigas = 40;
      rango->minNumHormigas = 25;

      rango->maxNumIteracionesACO = 150;
      rango->minNumIteracionesACO = 100;

      rango->maxTemperatura_inicial = 2500.0;
      rango->minTemperatura_inicial = 1500.0;

      rango->maxTemperatura_final = 0.3;
      rango->minTemperatura_final = 0.1;

      rango->maxFactor_enfriamiento = 0.999;
      rango->minFactor_enfriamiento = 0.95;

      rango->maxFactor_control = 0.8;
      rango->minFactor_control = 0.6;

      rango->maxIteracionesSA = 200;
      rango->minIteracionesSA = 150;
   }

   if (tsp->num_clientes == 101)
   {
      // Rango grande: parámetros más exigentes para tratar con más combinaciones posibles.
      rango->maxAlpha = 5.0;
      rango->minAlpha = 3.0;

      rango->maxBeta = 5.0;
      rango->minBeta = 3.0;

      rango->maxRho = 0.5;
      rango->minRho = 0.3;

      rango->maxNumHormigas = 50; 
      rango->minNumHormigas = 40;

      rango->maxNumIteracionesACO = 200;
      rango->minNumIteracionesACO = 150;

      rango->maxTemperatura_inicial = 3000.0;
      rango->minTemperatura_inicial = 2000.0;

      rango->maxTemperatura_final = 0.2;
      rango->minTemperatura_final = 0.1;

      rango->maxFactor_enfriamiento = 0.999;
      rango->minFactor_enfriamiento = 0.95;

      rango->maxFactor_control = 0.9;
      rango->minFactor_control = 0.6;

      rango->maxIteracionesSA = 300;
      rango->minIteracionesSA = 200;
   }

   // Itera sobre cada individuo de la población
   for (int i = 0; i < poblacion; ++i)
   {
      // Genera valores aleatorios dentro de los rangos definidos para cada parámetro.
      objetivo[i].alpha = genera_aleatorio(rango->minAlpha, rango->maxAlpha);
      objetivo[i].beta = genera_aleatorio(rango->minBeta, rango->maxBeta);
      objetivo[i].rho = genera_aleatorio(rango->minRho, rango->maxRho);
      objetivo[i].numHormigas = (int)genera_aleatorio(rango->minNumHormigas, rango->maxNumHormigas);
      objetivo[i].numIteracionesACO = (int)genera_aleatorio(rango->minNumIteracionesACO, rango->maxNumIteracionesACO);
      objetivo[i].temperatura_inicial = genera_aleatorio(rango->minTemperatura_inicial, rango->maxTemperatura_inicial);
      objetivo[i].temperatura_final = genera_aleatorio(rango->minTemperatura_final, rango->maxTemperatura_final);
      objetivo[i].factor_enfriamiento = genera_aleatorio(rango->minFactor_enfriamiento, rango->maxFactor_enfriamiento);
      objetivo[i].factor_control = genera_aleatorio(rango->minFactor_control, rango->maxFactor_control);
      objetivo[i].numIteracionesSA = (int)genera_aleatorio(rango->minIteracionesSA, rango->maxIteracionesSA);
   }
}

void aed_tsp(int num_poblacion, int num_generaciones, int tamanio_instancia, char *archivo_instancia)
{
   clock_t tiempo_inicial, tiempo_final; // Declaramos las variables para medir el tiempo de ejecución
   tiempo_inicial = clock();             // Guardamos el tiempo inicial

   tsp_configuracion *tsp = leer_instancia(archivo_instancia, tamanio_instancia); // Leemos el archivo de instancia TSP y guardamos la configuración
   // Asignamos memoria para los arreglos de individuos de cada etapa del algoritmo
   struct individuo *objetivo = asignar_memoria_individuos(num_poblacion);        // Individuos actuales (soluciones base)
   struct individuo *ruidoso = asignar_memoria_individuos(num_poblacion);         // Individuos perturbados aleatoriamente
   struct individuo *prueba = asignar_memoria_individuos(num_poblacion);          // Individuos resultantes de la mezcla entre objetivo y ruidoso
   struct individuo *resultado = asignar_memoria_individuos(1);                   // Individuo que almacena el mejor resultado obtenido

   struct rangos *rango = asignar_memoria_rangos();                               // Reservamos memoria para los rangos de parámetros

   // Asignamos parámetros básicos del algoritmo evolutivo
   tsp->generaciones = num_generaciones; // Asignamos número total de generaciones
   tsp->poblacion = num_poblacion;       // Asignamos tamaño de la población

   // Asignamos memoria para las matrices relacionadas al TSP
   double **instancia_visibilidad = asignar_memoria_instancia(tsp->num_clientes); // Visibilidad entre nodos (1/distancia)
   double **instancia_feromonas = asignar_memoria_instancia(tsp->num_clientes);   // Niveles de feromonas entre nodos
   double **instancia_distancias = asignar_memoria_instancia(tsp->num_clientes);  // Matriz de distancias entre nodos

   // Inicializamos las matrices con sus respectivos valores
   inicializar_distancias(instancia_distancias, tsp);        // Calcula las distancias euclidianas
   inicializar_visibilidad(instancia_visibilidad, tsp);      // Calcula visibilidad = 1/distancia
   inicializa_poblacion(objetivo, tsp, rango, num_poblacion); // Inicializa aleatoriamente la población objetivo

   // Inicializamos el individuo resultado con valores por defecto
   resultado->fitness = INFINITY;                    // El mejor fitness comienza como infinito
   resultado->hormiga = asignar_memoria_hormigas(1); // Asignamos memoria para la mejor hormiga (camino)

   // Evaluamos la función objetivo (FO) para cada individuo inicial
   for (int i = 0; i < num_poblacion; i++)
      evaluaFO_AED(&objetivo[i], instancia_feromonas, instancia_visibilidad, instancia_distancias, tsp);

   // Buscamos el mejor individuo dentro de la población inicial
   for (int i = 0; i < num_poblacion; i++)
      if (objetivo[i].fitness < resultado->fitness)
      {
         // Copiamos los parámetros del mejor individuo
         resultado->alpha = objetivo[i].alpha;
         resultado->beta = objetivo[i].beta;
         resultado->rho = objetivo[i].rho;
         resultado->numHormigas = objetivo[i].numHormigas;
         resultado->numIteracionesACO = objetivo[i].numIteracionesACO;
         resultado->temperatura_inicial = objetivo[i].temperatura_inicial;
         resultado->temperatura_final = objetivo[i].temperatura_final;
         resultado->factor_enfriamiento = objetivo[i].factor_enfriamiento;
         resultado->factor_control = objetivo[i].factor_control;
         resultado->numIteracionesSA = objetivo[i].numIteracionesSA;
         recuperamos_mejor_hormiga(resultado, objetivo[i].hormiga); // Copiamos la mejor hormiga (camino)
      }

   // Ciclo principal de generaciones (optimización)
   for (int i = 0; i < num_generaciones; i++)
   {
      construye_ruidosos(objetivo, ruidoso, rango, num_poblacion); // Generamos individuos ruidosos
      construye_prueba(objetivo, ruidoso, prueba, num_poblacion);  // Combinamos individuos objetivo y ruidoso para formar la población de prueba

      // Evaluamos la FO para cada individuo de prueba
      for (int j = 0; j < num_poblacion; ++j)
         evaluaFO_AED(&prueba[j], instancia_feromonas, instancia_visibilidad, instancia_distancias, tsp);

      // Buscamos si algún individuo de prueba es mejor que el actual resultado
      for (int i = 0; i < num_poblacion; i++)
         if (prueba[i].fitness < resultado->fitness)
         {
            // Si es mejor, actualizamos el individuo resultado
            resultado->alpha = prueba[i].alpha;
            resultado->beta = prueba[i].beta;
            resultado->rho = prueba[i].rho;
            resultado->numHormigas = prueba[i].numHormigas;
            resultado->numIteracionesACO = prueba[i].numIteracionesACO;
            resultado->temperatura_inicial = prueba[i].temperatura_inicial;
            resultado->temperatura_final = prueba[i].temperatura_final;
            resultado->factor_enfriamiento = prueba[i].factor_enfriamiento;
            resultado->factor_control = prueba[i].factor_control;
            resultado->numIteracionesSA = prueba[i].numIteracionesSA;
            recuperamos_mejor_hormiga(resultado, prueba[i].hormiga); // Copiamos la mejor hormiga
         }

      seleccion(objetivo, prueba, num_poblacion); // Seleccionamos la siguiente generación de individuos

      // Cálculo e impresión de barra de progreso
      int barra_ancho = 50; // Ancho total de la barra
      int progreso_barras = (int)((float)(i + 1) / num_generaciones * barra_ancho); // Progreso actual

      printf("\r[");
      for (int j = 0; j < barra_ancho; ++j)
      {
         if (j < progreso_barras)
            printf("#");
         else
            printf(" ");
      }

      // Mostramos porcentaje, fitness y tiempo transcurrido
      printf("] %.2f%%  Mejor Fitness: %.2lf  Tiempo: %.2lf minutos",
             ((float)(i + 1) / num_generaciones) * 100,
             resultado->fitness,
             ((double)(clock() - tiempo_inicial)) / CLOCKS_PER_SEC / 60.0);
      fflush(stdout);
   }

   // Finalizamos la medición de tiempo
   tiempo_final = clock();

   // Calculamos el tiempo total de ejecución en minutos
   double minutos = (((double)(tiempo_final - tiempo_inicial)) / CLOCKS_PER_SEC) / 60.0;

   // Guardamos tiempo y nombre de la instancia
   tsp->tiempo_ejecucion = ceil(minutos);      // Redondeamos el tiempo
   tsp->archivo_instancia = archivo_instancia; // Guardamos nombre del archivo de instancia

   imprimir_mejor_hormiga(resultado->hormiga, resultado);          // Mostramos el mejor resultado obtenido
   printf("\nEl tiempo de ejecución es: %.2f minutos\n", minutos); // Mostramos tiempo total

   // Guardamos los resultados en un archivo JSON
   guardar_json_en_archivo(resultado, tsp, archivo_instancia);

   // Liberamos toda la memoria dinámica usada
   liberar_instancia(instancia_feromonas, tsp->num_clientes);
   liberar_instancia(instancia_visibilidad, tsp->num_clientes);
   liberar_instancia(instancia_distancias, tsp->num_clientes);
   liberar_rangos(rango);
   liberar_individuos(objetivo, num_poblacion, false);
   liberar_individuos(prueba, num_poblacion, false);
   liberar_individuos(ruidoso, num_poblacion, false);
   liberar_individuos(resultado, 1, true);
   liberar_memoria_tsp_configuracion(tsp); 
}
