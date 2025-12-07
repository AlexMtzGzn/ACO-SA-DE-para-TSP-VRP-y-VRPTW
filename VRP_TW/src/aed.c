#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../include/estructuras.h"
#include "../include/vrp_tw_aco.h"
#include "../include/configuracion_vrp_tw.h"
#include "../include/control_memoria.h"
#include "../include/salida_datos.h"
#include "../include/configuracion_json.h"
#include "../include/lista_ruta.h"

// Función para calcular la distancia entre dos clientes dados sus coordenadas
double calcular_distancia(struct vrp_configuracion *vrp, int cliente_origen, int cliente_destino)
{
   // Retornamos la distancia de los puntos
   double distancia;
   distancia = sqrt(pow((vrp->clientes[cliente_destino].coordenada_x - vrp->clientes[cliente_origen].coordenada_x), 2.0) + pow((vrp->clientes[cliente_destino].coordenada_y - vrp->clientes[cliente_origen].coordenada_y), 2.0));
   return distancia;
}

// Inicializa la visibilidad entre los clientes en la instancia de VRP
void inicializar_visibilidad(double **instancia_visibilidad, struct vrp_configuracion *vrp)
{
   double distancia; // Declaramos la variable distancia
   // Recorre todos los clientes y calcula la visibilidad entre ellos
   for (int i = 0; i < vrp->num_clientes; i++)
   {
      for (int j = i + 1; j < vrp->num_clientes; j++)
      {
         if (i != j)
         {
            distancia = calcular_distancia(vrp, i, j);
            // La visibilidad es inversamente proporcional a la distancia
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

// Inicializa las ventanas de tiempo entre los clientes en la instancia de VRP
void inicializar_ventana_tiempo(double **instancia_ventanas_tiempo, struct vrp_configuracion *vrp)
{
   // Recorre todos los clientes y calcula la ventana de tiempo entre ellos
   for (int i = 0; i < vrp->num_clientes; i++)
   {
      for (int j = i + 1; j < vrp->num_clientes; j++)
      {
         if (i != j)
         {
            instancia_ventanas_tiempo[i][j] = 1.0 / vrp->clientes[j].vt_final;
            instancia_ventanas_tiempo[j][i] = instancia_ventanas_tiempo[i][j]; // Aprovechamos la simetría
         }
         else
         {
            instancia_ventanas_tiempo[i][j] = 0.0; // Es 0.0
         }
      }
   }
}

// Inicializa las distancias entre los clientes en la instancia de VRP
void inicializar_distancias(double **instancia_distancias, struct vrp_configuracion *vrp)
{
   double distancia;
   // Recorre todos los clientes y calcula las distancias entre ellos
   for (int i = 0; i < vrp->num_clientes; i++)
   {
      for (int j = i + 1; j < vrp->num_clientes; j++)
      {
         if (i != j)
         {
            distancia = calcular_distancia(vrp, i, j);
            distancia = floor(distancia * pow(10.0, 1.0)) / pow(10.0, 1.0);
            instancia_distancias[i][j] = distancia;
            instancia_distancias[j][i] = distancia; // Aprovechamos la simetría
         }
         else
         {
            instancia_distancias[i][j] = 0.0; // Es 0.0
         }
      }
   }
}

// Inicializa la matriz de feromonas para el algoritmo ACO
void inicializar_feromona(struct vrp_configuracion *vrp, double **instancia_feromona)
{
   // Recorre todos los clientes en la matriz de feromonas (fila i, columna j)
   for (int i = 0; i < vrp->num_clientes; i++)
   {
      for (int j = i + 1; j < vrp->num_clientes; j++)
      {
         // Si i y j son diferentes (es decir, no es la misma ciudad), se asigna un valor de feromona de 1.0
         if (i != j)
         {
            instancia_feromona[i][j] = 1.0;                      // Se asigna un valor de feromona
            instancia_feromona[j][i] = instancia_feromona[i][j]; // Aprovechamos la simetría
         }

         else
         {
            // Si i y j son iguales, se asigna 0.0
            instancia_feromona[i][j] = 0.0;
         }
      }
   }
}

// Evalúa la función objetivo (FO) para un individuo usando el algoritmo ACO con ventanas de tiempo
void evaluaFO_AED(struct individuo *ind, double **instancia_feromona, double **instancia_visibilidad, double **instancia_distancias, double **instancia_ventanas_tiempo, struct vrp_configuracion *vrp)
{
   // Inicializa las feromonas en la instancia
   inicializar_feromona(vrp, instancia_feromona);
   // imprimir_instancia(instancia_feromonas,vrp,"INSTANCIA FEROMONAS");
   // Ejecuta el algoritmo de optimización con ventanas de tiempo (ACO) en el individuo
   vrp_tw_aco(vrp, ind, instancia_visibilidad, instancia_distancias, instancia_feromona, instancia_ventanas_tiempo);
}

// Genera un número aleatorio entre un rango dado (mínimo y máximo)
double genera_aleatorio(double minimo, double maximo)
{
   // Genera un número aleatorio entre 0 y 1, luego lo escala al rango deseado
   double aleatorio;
   aleatorio = minimo + ((double)rand() / RAND_MAX) * (maximo - minimo);
   return aleatorio;
}

// Construye un nuevo individuo ruidoso a partir de un individuo objetivo y un rango dado
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
      ruidoso[i].gamma = objetivo[aleatorio1].gamma + 0.5 * (objetivo[aleatorio2].gamma - objetivo[aleatorio3].gamma);
      ruidoso[i].rho = objetivo[aleatorio1].rho + 0.5 * (objetivo[aleatorio2].rho - objetivo[aleatorio3].rho);
      ruidoso[i].numHormigas = objetivo[aleatorio1].numHormigas + (int)(0.5 * (objetivo[aleatorio2].numHormigas - objetivo[aleatorio3].numHormigas));
      ruidoso[i].porcentajeHormigas = objetivo[aleatorio1].porcentajeHormigas + 0.5 * (objetivo[aleatorio2].porcentajeHormigas - objetivo[aleatorio3].porcentajeHormigas);
      ruidoso[i].numIteracionesACO = objetivo[aleatorio1].numIteracionesACO + (int)(0.5 * (objetivo[aleatorio2].numIteracionesACO - objetivo[aleatorio3].numIteracionesACO));
      ruidoso[i].temperatura_inicial = objetivo[aleatorio1].temperatura_inicial + 0.5 * (objetivo[aleatorio2].temperatura_inicial - objetivo[aleatorio3].temperatura_inicial);
      ruidoso[i].temperatura_final = objetivo[aleatorio1].temperatura_final + 0.5 * (objetivo[aleatorio2].temperatura_final - objetivo[aleatorio3].temperatura_final);
      ruidoso[i].factor_enfriamiento = objetivo[aleatorio1].factor_enfriamiento + 0.5 * (objetivo[aleatorio2].factor_enfriamiento - objetivo[aleatorio3].factor_enfriamiento);
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

      // Limita 'gamma' a estar dentro de los valores mínimos y máximos
      if (ruidoso[i].gamma > rango->maxGamma)
         ruidoso[i].gamma = rango->maxGamma;

      if (ruidoso[i].gamma < rango->minGamma)
         ruidoso[i].gamma = rango->minGamma;

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

      // Limita 'procentajeHormigas' a estar dentro de los valores mínimos y máximos
      if (ruidoso[i].porcentajeHormigas > rango->maxPorcentajeHormigas)
         ruidoso[i].porcentajeHormigas = rango->maxPorcentajeHormigas;

      if (ruidoso[i].porcentajeHormigas < rango->minPorcentajeHormigas)
         ruidoso[i].porcentajeHormigas = rango->minPorcentajeHormigas;

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

      // Limita 'numIteracionesSA' a estar dentro de los valores mínimos y máximos
      if (ruidoso[i].numIteracionesSA > rango->maxIteracionesSA)
         ruidoso[i].numIteracionesSA = rango->maxIteracionesSA;

      if (ruidoso[i].numIteracionesSA < rango->minIteracionesSA)
         ruidoso[i].numIteracionesSA = rango->minIteracionesSA;
   }
}

// Construye una población de prueba a partir de un individuo objetivo y un individuo ruidoso
void construye_prueba(struct individuo *objetivo, struct individuo *ruidoso, struct individuo *prueba, int poblacion)
{
   double aleatorio;
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

// Selecciona el mejor individuo entre dos poblaciones: objetivo y prueba.
void seleccion(struct individuo *objetivo, struct individuo *prueba, int poblacion)
{
   // Recorremos la población para comparar cada individuo de ambas poblaciones.
   for (int i = 0; i < poblacion; ++i)
      // Si el fitness del individuo en la población objetivo es mayor que en la población de prueba,
      // se actualiza el individuo en la población objetivo por el de la población de prueba.
      if (objetivo[i].fitness > prueba[i].fitness)
         objetivo[i] = prueba[i];
}

// Inicializa la población de individuos con valores aleatorios dentro de los rangos especificados.
void inicializa_poblacion(struct individuo *objetivo, struct vrp_configuracion *vrp, struct rangos *rango, int poblacion)
{

   if (vrp->num_clientes <= 26)
   {
      rango->maxAlpha = 3.5; // Asignamos 3.5 a maxAlpha
      rango->minAlpha = 2.0; // Asignamos 2.0 a minAlpha

      rango->maxBeta = 4.0; // Asignamos 4.0 a maxBeta
      rango->minBeta = 2.5; // Asignamos 2.5 a minBeta

      rango->maxGamma = 2.5; // Asignamos 2.5 a maxGamma
      rango->minGamma = 1.0; // Asignamos 1.0 a minGamma

      rango->maxRho = 0.5; // Asignamos 0.5 a maxRho
      rango->minRho = 0.3; // Asignamos 0.3 a minRho

      rango->maxNumHormigas = 6; // Asignamos 6 a maxNumHormigas
      rango->minNumHormigas = 3; // Asignamos 3 a minNumHormigas

      rango->maxPorcentajeHormigas = 0.60; // Asignamos 0.60 a maxPorcentajeHormigas
      rango->minPorcentajeHormigas = 0.30; // Asignamos 0.30 a minPorcentateHormigas

      rango->maxNumIteracionesACO = 80; // Asignamos 80 a maxNumIteracionesACO
      rango->minNumIteracionesACO = 60; // Asignamos 60 a minNumIteracionesACO

      rango->maxTemperatura_inicial = 1800.0; // Asignamos 1800.0 a maxTemperatura_inicial
      rango->minTemperatura_inicial = 1200.0; // Asignamos 1200.0 a minTemperatura_inicial

      rango->maxTemperatura_final = 0.1;  // Asignamos 0.1 a maxTemperatura_final
      rango->minTemperatura_final = 0.01; // Asignamos 0.01 a minTemperatura_final

      rango->maxFactor_enfriamiento = 0.98; // Asignamos 0.98 a maxFactor_enfriamiento
      rango->minFactor_enfriamiento = 0.95; // Asignamos 0.95 a minFactor_enfriamiento

      rango->maxIteracionesSA = 120; // Asignamos 120 a maxIteracionesSA
      rango->minIteracionesSA = 80;  // Asignamos 80 a minIteracionesSA
   }

   if (vrp->num_clientes > 26 && vrp->num_clientes <= 51)
   {
      rango->maxAlpha = 4.0; // Asignamos 4.0 a maxAlpha
      rango->minAlpha = 2.5; // Asignamos 2.5 a minAlpha

      rango->maxBeta = 5.0; // Asignamos 5.0 a maxBeta
      rango->minBeta = 3.5; // Asignamos 3.5 a minBeta

      rango->maxGamma = 3.0; // Asignamos 3.0 a maxGamma
      rango->minGamma = 1.5; // Asignamos 1.5 a minGamma

      rango->maxRho = 0.45; // Asignamos 0.45 a maxRho
      rango->minRho = 0.25; // Asignamos 0.25 a minRho

      rango->maxNumHormigas = 12; // Asignamos 12 a maxNumHormigas
      rango->minNumHormigas = 6;  // Asignamos 6 a minNumHormigas

      rango->maxPorcentajeHormigas = 0.50; // Asignamos 0.50 a maxPorcentajeHormigas
      rango->minPorcentajeHormigas = 0.20; // Asignamos 0.20 a minPorcentajeHormigas

      rango->maxNumIteracionesACO = 150; // Asignamos 150 a maxNumIteracionesACO
      rango->minNumIteracionesACO = 100; // Asignamos 100 a minNumIteracionesACO

      rango->maxTemperatura_inicial = 2200.0; // Asignamos 2200.0 a maxTemperatura_inicial
      rango->minTemperatura_inicial = 1600.0; // Asignamos 1600.0 a minTemperatura_inicial

      rango->maxTemperatura_final = 0.05;  // Asignamos 0.05 a maxTemperatura_final
      rango->minTemperatura_final = 0.005; // Asignamos 0.005 a minTemperatura_final

      rango->maxFactor_enfriamiento = 0.99; // Asignamos 0.99 a maxFactor_enfriamiento
      rango->minFactor_enfriamiento = 0.97; // Asignamos 0.97 a minFactor_enfriamiento

      rango->maxIteracionesSA = 200; // Asignamos 200 a maxIteracionesSA
      rango->minIteracionesSA = 120; // Asignamos 120 a minIteracionesSA
   }

   if (vrp->num_clientes > 51 && vrp->num_clientes <= 101)
   {
      rango->maxAlpha = 5.0; // Asignamos 5.0 a maxAlpha
      rango->minAlpha = 3.0; // Asignamos 3.0 a minAlpha

      rango->maxBeta = 6.0; // Asignamos 6.0 a maxBeta
      rango->minBeta = 4.0; // Asignamos 4.0 a minBeta

      rango->maxGamma = 4.0; // Asignamos 4.0 a maxGamma
      rango->minGamma = 2.0; // Asignamos 2.0 a minGamma

      rango->maxRho = 0.3; // Asignamos 0.3 a maxRho
      rango->minRho = 0.1; // Asignamos 0.1 a minRho

      rango->maxNumHormigas = 20; // Asignamos 20 a maxNumHormigas
      rango->minNumHormigas = 10; // Asignamos 10 a minNumHormigas

      rango->maxPorcentajeHormigas = 0.35; // Asignamos 0.35 a maxPorcentajeHormigas
      rango->minPorcentajeHormigas = 0.15; // Asignamos 0.15 a minPorcentajeHormigas

      rango->maxNumIteracionesACO = 180; // Asignamos 180 a maxNumIteracionesACO
      rango->minNumIteracionesACO = 120; // Asignamos 120 a minNumIteracionesACO

      rango->maxTemperatura_inicial = 4000.0; // Asignamos 4000.0 a maxTemperatura_inicial
      rango->minTemperatura_inicial = 2500.0; // Asignamos 2500.0 a minTemperatura_inicial

      rango->maxTemperatura_final = 0.01;  // Asignamos 0.01 a maxTemperatura_final
      rango->minTemperatura_final = 0.001; // Asignamos 0.001 a minTemperatura_final

      rango->maxFactor_enfriamiento = 0.995; // Asignamos 0.995 a maxFactor_enfriamiento
      rango->minFactor_enfriamiento = 0.97;  // Asignamos 0.98 a minFactor_enfriamiento

      rango->maxIteracionesSA = 300; // Asignamos 300 a maxIteracionesSA
      rango->minIteracionesSA = 200; // Asignamos 200 a minIteracionesSA
   }

   // Itera sobre cada individuo de la población
   for (int i = 0; i < poblacion; ++i)
   {

      // Genera valores aleatorios dentro de los rangos definidos para cada individuo
      objetivo[i].alpha = genera_aleatorio(rango->minAlpha, rango->maxAlpha);
      objetivo[i].beta = genera_aleatorio(rango->minBeta, rango->maxBeta);
      objetivo[i].gamma = genera_aleatorio(rango->minGamma, rango->maxGamma);
      objetivo[i].rho = genera_aleatorio(rango->minRho, rango->maxRho);
      objetivo[i].numHormigas = (int)genera_aleatorio(rango->minNumHormigas, rango->maxNumHormigas);
      objetivo[i].porcentajeHormigas = genera_aleatorio(rango->minPorcentajeHormigas, rango->maxPorcentajeHormigas);
      objetivo[i].numIteracionesACO = (int)genera_aleatorio(rango->minNumIteracionesACO, rango->maxNumIteracionesACO);
      objetivo[i].temperatura_inicial = genera_aleatorio(rango->minTemperatura_inicial, rango->maxTemperatura_inicial);
      objetivo[i].temperatura_final = genera_aleatorio(rango->minTemperatura_final, rango->maxTemperatura_final);
      objetivo[i].factor_enfriamiento = genera_aleatorio(rango->minFactor_enfriamiento, rango->maxFactor_enfriamiento);
      objetivo[i].numIteracionesSA = (int)genera_aleatorio(rango->minIteracionesSA, rango->maxIteracionesSA);
   }
}

// Función principal que ejecuta el algoritmo de optimización basado en ACO para el problema VRP con ventanas de tiempo
void aed_vrp_tw(int num_poblacion, int num_generaciones, int tamanio_instancia, char *archivo_instancia)
{
   clock_t tiempo_inicial, tiempo_final; // Variables para medir el tiempo de ejecución
   int barra_ancho, progreso_barras;
   double minutos;                                                                       // Variables para la barra de progreso y el tiempo en minutos
   tiempo_inicial = clock();                                                             // Iniciar Tiempo                                                               // Respuesta
   struct individuo *objetivo = asignar_memoria_individuos(num_poblacion);               // Asignamos memoria para el arreglo objetivo
   struct individuo *ruidoso = asignar_memoria_individuos(num_poblacion);                // Asignamos memoria para el arreglo ruidoso
   struct individuo *prueba = asignar_memoria_individuos(num_poblacion);                 // Asiganamos memoria para el arreglo prueba
   struct individuo *resultado = asignar_memoria_individuos(1);                          // Asignamos memoria para el arreglo de resultados
   struct vrp_configuracion *vrp = leer_instancia(archivo_instancia, tamanio_instancia); // Mandamo a leer la instancia y a retormamos en un apuntador structura vrp_configuracion
   struct rangos *rango = asignar_memoria_rangos();

   vrp->generaciones = num_generaciones; // Asignamos el numero de generaciones
   vrp->poblacion = num_poblacion;       // Asiganamos el numero de poblacion

   double **instancia_visibilidad = asignar_memoria_instancia(vrp->num_clientes);     // Generamos memoria para la instancia de la visibilidad
   double **instancia_feromonas = asignar_memoria_instancia(vrp->num_clientes);       // Generamos memoria para la instancia de la feromona
   double **instancia_distancias = asignar_memoria_instancia(vrp->num_clientes);      // Generamos memoria para la instancia de la las distancias
   double **instancia_ventanas_tiempo = asignar_memoria_instancia(vrp->num_clientes); // Generamos memoria para la instancia de las ventanas de tiempo

   inicializar_distancias(instancia_distancias, vrp);          // Inicializamos las distancias
   inicializar_visibilidad(instancia_visibilidad, vrp);        // Inicializamos las visibilidad
   inicializar_ventana_tiempo(instancia_ventanas_tiempo, vrp); // Inicializmos las ventanas de tiempo
   inicializar_feromona(vrp, instancia_feromonas);             // Inicializamos la feromona
   inicializa_poblacion(objetivo, vrp, rango, num_poblacion);  // Inicializamos la poblacion

   // Aqui podemos imprimir las instancias
   // imprimir_instancia(instancia_distancias,vrp,"INSTANCIA DISTANCIAS");
   // imprimir_instancia(instancia_feromonas,vrp,"INSTANCIA FEROMONAS");
   // imprimir_instancia(instancia_ventanas_tiempo,vrp,"INSTANCIA VENTANAS DE TIEMPO");
   // imprimir_instancia(instancia_visibilidad,vrp,"INSTANCIA VISIBILIDAD");

   // Inicializamos la estructura de resultados
   resultado->fitness = INFINITY;
   // resultado->hormiga = asignar_memoria_hormigas(1);
   //  Evaluamos la función objetivo para cada individuo de la población inicial
   for (int i = 0; i < num_poblacion; ++i) // Iniciamos la funcion objetivo con el objetivo
      evaluaFO_AED(&objetivo[i], instancia_feromonas, instancia_visibilidad, instancia_distancias, instancia_ventanas_tiempo, vrp);
   // Encontramos el mejor individuo de la población inicial
   for (int i = 0; i < num_poblacion; i++)
      if (objetivo[i].fitness < resultado->fitness)
      {
         resultado->alpha = objetivo[i].alpha;
         resultado->beta = objetivo[i].beta;
         resultado->gamma = objetivo[i].gamma;
         resultado->rho = objetivo[i].rho;
         resultado->numHormigas = objetivo[i].numHormigas;
         resultado->porcentajeHormigas = objetivo[i].porcentajeHormigas;
         resultado->numIteracionesACO = objetivo[i].numIteracionesACO;
         resultado->temperatura_inicial = objetivo[i].temperatura_inicial; // Copiamos la temperatura inicial del mejor metal
         resultado->temperatura_final = objetivo[i].temperatura_final;     // Copiamos la temperatura final  del mejor metal
         resultado->factor_enfriamiento = objetivo[i].factor_enfriamiento; // Copiamos el factor de enfriamiento del mejor metal
         resultado->numIteracionesSA = objetivo[i].numIteracionesSA;       // Copiamos el numero de iteraciones del mejor metal
         recuperamos_mejor_hormiga(resultado, objetivo[i].hormiga);
      }

   // Inicializamos ya las generaciones
   for (int i = 0; i < num_generaciones; i++)
   {
      construye_ruidosos(objetivo, ruidoso, rango, num_poblacion); // Contruimos Ruidosos
      construye_prueba(objetivo, ruidoso, prueba, num_poblacion);  // Contruimos Prueba
                                                                   // Evaluamos la función objetivo para cada individuo de prueba
      for (int j = 0; j < num_poblacion; ++j)                      // Mandamos a evaluar la funcion objetivo de prueba{
         evaluaFO_AED(&prueba[j], instancia_feromonas, instancia_visibilidad, instancia_distancias, instancia_ventanas_tiempo, vrp);

      for (int i = 0; i < num_poblacion; i++)
         // Actualizamos el mejor resultado si encontramos uno mejor
         if (prueba[i].fitness < resultado->fitness)
         {
            resultado->alpha = prueba[i].alpha;                             // Copiamos el alpha
            resultado->beta = prueba[i].beta;                               // Copiamos el beta
            resultado->gamma = prueba[i].gamma;                             // Copiamos el gamma
            resultado->rho = prueba[i].rho;                                 // Copiamos el rho
            resultado->numHormigas = prueba[i].numHormigas;                 // Copiamos el numero de hormigas
            resultado->porcentajeHormigas = prueba[i].porcentajeHormigas;   // Copiamos el porcentaje de las hormigas
            resultado->numIteracionesACO = prueba[i].numIteracionesACO;     // Copiamos la iteraciones de las hormigas
            resultado->temperatura_inicial = prueba[i].temperatura_inicial; // Copiamos la temperatura inicial
            resultado->temperatura_final = prueba[i].temperatura_final;     // Copiamos la temperatura final
            resultado->factor_enfriamiento = prueba[i].factor_enfriamiento; // Copiamos el factor de enfriamiento
            resultado->numIteracionesSA = prueba[i].numIteracionesSA;       // Copiamos el numero de iteraciones
            recuperamos_mejor_hormiga(resultado, prueba[i].hormiga);
         }
      // Realizamos la selección de la siguiente generación
      seleccion(objetivo, prueba, num_poblacion);                               // Hacemos la seleccion
      barra_ancho = 50;                                                         // Ancho de la barra de progreso
      progreso_barras = (int)((float)(i + 1) / num_generaciones * barra_ancho); // Cuántos bloques pintar

      printf("\r["); // Regresa al inicio de la línea
      for (int j = 0; j < barra_ancho; ++j)
      {
         if (j < progreso_barras)
            printf("#"); // Imprime bloque completado
         else
            printf(" "); // Espacio restante
      }

      // Muestra porcentaje, fitness y tiempo transcurrido
      printf("] %.2f%%  Mejor Fitness: %.2lf  Tiempo: %.2lf minutos",
             ((float)(i + 1) / num_generaciones) * 100,
             resultado->fitness,
             ((double)(clock() - tiempo_inicial)) / CLOCKS_PER_SEC / 60.0);
      fflush(stdout); // Limpia buffer para mostrar en consola al instante
   }

   // Finaliza cronómetro
   tiempo_final = clock();
   minutos = (((double)(tiempo_final - tiempo_inicial)) / CLOCKS_PER_SEC) / 60.0;

   // Guardamos el tiempo y datos finales
   vrp->tiempo_ejecucion = ceil(minutos);
   vrp->archivo_instancia = archivo_instancia;

   llenar_datos_clientes(resultado->hormiga->flota, vrp, instancia_distancias);
   imprimir_mejor_hormiga(resultado->hormiga, resultado, vrp);
   printf("\nEl tiempo de ejecución es: %.2f minutos\n\n\n", minutos);
   guardar_json_en_archivo(resultado, vrp, archivo_instancia);

   liberar_instancia(instancia_feromonas, vrp->num_clientes);       // Liberemos la memoria de la instancia feromona
   liberar_instancia(instancia_visibilidad, vrp->num_clientes);     // Liberemos la memoria de la instancia visibilidad
   liberar_instancia(instancia_distancias, vrp->num_clientes);      // Liberemos la memoria de la instancia distancias
   liberar_instancia(instancia_ventanas_tiempo, vrp->num_clientes); // Liberemos la memoria de la instancia ventanas de tiempo
   liberar_rangos(rango);                                           // Liberemos la memoria de los rangos
   liberar_individuos(objetivo, num_poblacion, false);              // Liberemos la memoria del objetivo
   liberar_individuos(prueba, num_poblacion, false);                // Liberemos la memoria de la prueba
   liberar_individuos(ruidoso, num_poblacion, false);               // Liberemos la memoria del ruidoso
   liberar_individuos(resultado, 1, true);                          // Liberemos los resultado
   liberar_memoria_vrp_configuracion(vrp);                          // Liberemos la memoria del vrp
}