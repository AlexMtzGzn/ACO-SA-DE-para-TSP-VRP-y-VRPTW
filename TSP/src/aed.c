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

double calcular_Distancia(struct tsp_configuracion *tsp, int cliente_origen, int cliente_destino)
{
   // Retornamos la distancia de los puntos
   double distancia = sqrt(pow((tsp->clientes[cliente_destino].coordenada_x - tsp->clientes[cliente_origen].coordenada_x), 2.0) + pow((tsp->clientes[cliente_destino].coordenada_y - tsp->clientes[cliente_origen].coordenada_y), 2.0));
   return distancia;
}

void inicializar_Visibilidad(double **instancia_visibilidad, struct tsp_configuracion *tsp)
{
   double distancia;
   // Recorre todos los clientes y calcula la visibilidad entre ellos
   for (int i = 0; i < tsp->num_clientes; i++)
   {
      for (int j = i + 1; j < tsp->num_clientes; j++)
      {

         if (i != j)
         {
            // Calculamos la distancia entre los clientes i y j una sola vez
            distancia = calcular_Distancia(tsp, i, j);
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

void inicializar_Distancias(double **instancia_distancias, struct tsp_configuracion *tsp)
{
   double distancia;
   // Recorre todos los clientes y calcula las distancias entre ellos
   for (int i = 0; i < tsp->num_clientes; i++)
   {
      for (int j = i + 1; j < tsp->num_clientes; j++)
      {
         if (i != j)
         {
            // Calculamos la distancia entre los clientes i y j
            distancia = calcular_Distancia(tsp, i, j);
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

void inicializar_Feromona(struct tsp_configuracion *tsp, double **instancia_feromona)
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
            instancia_feromona[i][j] = 0.0;
      }
   }
}

void evaluaFO_AED(struct individuo *ind, double **instancia_feromona, double **instancia_visibilidad, double **instancia_distancias, struct tsp_configuracion *tsp)
{
   // Inicializa las feromonas en la instancia
   inicializar_Feromona(tsp, instancia_feromona);
   // Imprimimos la instancia de feromonass
   //  imprimir_instancia(instancia_feromonas,tsp,"INSTANCIA FEROMONAS");
   //  Ejecuta el algoritmo de optimización con ventanas de tiempo (ACO) en el individuo
   tsp_aco(tsp, ind, instancia_visibilidad, instancia_distancias, instancia_feromona);
}

double generaAleatorio(double minimo, double maximo)
{
   // Genera un número aleatorio entre 0 y 1, luego lo escala al rango deseado
   double aleatorio = minimo + ((double)rand() / RAND_MAX) * (maximo - minimo);
   return aleatorio;
}

void construyeRuidosos(struct individuo *objetivo, struct individuo *ruidoso, struct rangos *rango, int poblacion)
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

      // Limita 'numIteracionesSA' a estar dentro de los valores mínimos y máximos
      if (ruidoso[i].numIteracionesSA > rango->maxIteracionesSA)
         ruidoso[i].numIteracionesSA = rango->maxIteracionesSA;

      if (ruidoso[i].numIteracionesSA < rango->minIteracionesSA)
         ruidoso[i].numIteracionesSA = rango->minIteracionesSA;
   }
}


void construyePrueba(struct individuo *objetivo, struct individuo *ruidoso, struct individuo *prueba, int poblacion)
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

void seleccion(struct individuo *objetivo, struct individuo *prueba, int poblacion)
{
   // Recorremos la población para comparar cada individuo de ambas poblaciones.
   for (int i = 0; i < poblacion; ++i)
      // Si el fitness del individuo en la población objetivo es mayor que en la población de prueba,
      // se actualiza el individuo en la población objetivo por el de la población de prueba.
      if (objetivo[i].fitness > prueba[i].fitness)
         objetivo[i] = prueba[i];
}

void inicializaPoblacion(struct individuo *objetivo, struct tsp_configuracion *tsp, struct rangos *rango, int poblacion)
{
   // Itera sobre cada individuo de la población
   for (int i = 0; i < poblacion; ++i)
   {

      // Asigna rangos específicos según el número de clientes en el TSP
      if (tsp->num_clientes <= 25)
      {
         // Define los rangos para los parámetros de ACO y SA para instancias con 25 o menos clientes
         rango->maxAlpha = 2.5;
         rango->minAlpha = 0.8;
         rango->maxBeta = 6.0;
         rango->minBeta = 2.5;
         rango->maxRho = 0.5;
         rango->minRho = 0.1;
         rango->maxNumHormigas = 30;
         rango->minNumHormigas = 10;
         rango->maxNumIteracionesACO = 200;
         rango->minNumIteracionesACO = 50;
         rango->maxTemperatura_inicial = 400.0;
         rango->minTemperatura_inicial = 200.0;
         rango->maxTemperatura_final = 0.1;
         rango->minTemperatura_final = 0.01;
         rango->maxFactor_enfriamiento = 0.98;
         rango->minFactor_enfriamiento = 0.95;
         rango->maxIteracionesSA = 50;
         rango->minIteracionesSA = 30;
      }
      // Asigna rangos para instancias con más de 25 y hasta 51 clientes
      if (tsp->num_clientes > 25 && tsp->num_clientes <= 51)
      {
         rango->maxAlpha = 2.5;
         rango->minAlpha = 0.8;
         rango->maxBeta = 6.0;
         rango->minBeta = 2.5;
         rango->maxRho = 0.5;
         rango->minRho = 0.1;
         rango->maxNumHormigas = 40;
         rango->minNumHormigas = 20;
         rango->maxNumIteracionesACO = 200;
         rango->minNumIteracionesACO = 50;
         rango->maxTemperatura_inicial = 600.0;
         rango->minTemperatura_inicial = 400.0;
         rango->maxTemperatura_final = 0.1;
         rango->minTemperatura_final = 0.01;
         rango->maxFactor_enfriamiento = 0.98;
         rango->minFactor_enfriamiento = 0.95;
         rango->maxIteracionesSA = 80;
         rango->minIteracionesSA = 50;
      }

      // Asigna rangos para instancias con más de 51 y hasta 101 clientes
      if (tsp->num_clientes > 51 && tsp->num_clientes <= 101)
      {
         rango->maxAlpha = 2.0;
         rango->minAlpha = 0.8;
         rango->maxBeta = 6.0;
         rango->minBeta = 3.0;
         rango->maxRho = 0.3;
         rango->minRho = 0.1;
         rango->maxNumHormigas = 100;
         rango->minNumHormigas = 20;
         rango->maxNumIteracionesACO = 200;
         rango->minNumIteracionesACO = 50;
         rango->maxTemperatura_inicial = 1000.0;
         rango->minTemperatura_inicial = 600.0;
         rango->maxTemperatura_final = 0.1;
         rango->minTemperatura_final = 0.01;
         rango->maxFactor_enfriamiento = 0.995;
         rango->minFactor_enfriamiento = 0.98;
         rango->maxIteracionesSA = 100;
         rango->minIteracionesSA = 80;
      }

      // Genera valores aleatorios dentro de los rangos definidos para cada individuo
      objetivo[i].alpha = generaAleatorio(rango->minAlpha, rango->maxAlpha);
      objetivo[i].beta = generaAleatorio(rango->minBeta, rango->maxBeta);
      objetivo[i].rho = generaAleatorio(rango->minRho, rango->maxRho);
      objetivo[i].numHormigas = (int)generaAleatorio(rango->minNumHormigas, rango->maxNumHormigas);
      objetivo[i].numIteracionesACO = (int)generaAleatorio(rango->minNumIteracionesACO, rango->maxNumIteracionesACO);
      objetivo[i].temperatura_inicial = generaAleatorio(rango->minTemperatura_inicial, rango->maxTemperatura_inicial);
      objetivo[i].temperatura_final = generaAleatorio(rango->minTemperatura_final, rango->maxTemperatura_final);
      objetivo[i].factor_enfriamiento = generaAleatorio(rango->minFactor_enfriamiento, rango->maxFactor_enfriamiento);
      objetivo[i].numIteracionesSA = (int)generaAleatorio(rango->minIteracionesSA, rango->maxIteracionesSA);
   }
}


void aed_tsp(int num_poblacion, int num_generaciones, int tamanio_instancia, char *archivo_instancia)
{
   clock_t tiempo_inicial, tiempo_final;                                          // Decraramos las variables para el tiempo
   tiempo_inicial = clock();                                                      // Inicializamos el tiempo
   char respuesta;                                                                // Respuesta
   struct individuo *objetivo = asignar_memoria_individuos(num_poblacion);        // Asignamos memoria para el arreglo objetivo
   struct individuo *ruidoso = asignar_memoria_individuos(num_poblacion);         // Asignamos memoria para el arreglo ruidoso
   struct individuo *prueba = asignar_memoria_individuos(num_poblacion);          // Asiganamos memoria para el arreglo prueba
   struct individuo *resultado = asignar_memoria_individuos(1);                   // Asignamos memoria para el arreglo de resultados
   tsp_configuracion *tsp = leer_instancia(archivo_instancia, tamanio_instancia); // Mandamo a leer la instancia y a retormamos en un apuntador structura tsp_configuracion
   struct rangos *rango = asignar_memoria_rangos();                               // Asignamos memoria para los rangos

   tsp->generaciones = num_generaciones; // Asiganamos el numero de generaciones
   tsp->poblacion = num_poblacion;       // Asiganamos el numero de generaciones

   double **instancia_visibilidad = asignar_memoria_instancia(tsp->num_clientes); // Generamos memoria para la instancia de la visibilidad
   double **instancia_feromonas = asignar_memoria_instancia(tsp->num_clientes);   // Generamos memoria para la instancia de la feromona
   double **instancia_distancias = asignar_memoria_instancia(tsp->num_clientes);  // Generamos memoria para la instancia de la las distancias

   inicializar_Distancias(instancia_distancias, tsp);        // Inicializamos las distancias
   inicializar_Visibilidad(instancia_visibilidad, tsp);      // Inicializamos las visibilidad
   inicializar_Feromona(tsp, instancia_feromonas);           // Inicializamos la feromona
   inicializaPoblacion(objetivo, tsp, rango, num_poblacion); // Inicializamos la poblacion

   // Aqui podemos imprimir las instancias
   // imprimir_instancia(instancia_distancias,tsp,"INSTANCIA DISTANCIAS");
   // imprimir_instancia(instancia_feromonas,tsp,"INSTANCIA FEROMONAS");
   // imprimir_instancia(instancia_visibilidad,tsp,"INSTANCIA VISIBILIDAD");

   // Inicializamos la estructura de resultados
   resultado->fitness = INFINITY;                           // Inicializamos el fitness como infinito
   resultado->hormiga = asignar_memoria_hormigas(1);        // Asiganamos memoria para la mejor hormiga
   resultado->hormiga->ruta = asignar_memoria_lista_ruta(); // Asignamos memoria para la ruta de la mejor hormiga
   // resultado->metal = (struct metal *)malloc(sizeof(struct metal));
   //  Evaluamos la función objetivo para cada individuo de la población inicial

   for (int i = 0; i < num_poblacion; i++) // Iniciamos la funcion objetivo con el objetivo
      evaluaFO_AED(&objetivo[i], instancia_feromonas, instancia_visibilidad, instancia_distancias, tsp);
   // Encontramos el mejor individuo de la población inicial

   for (int i = 0; i < num_poblacion; i++)
      if (objetivo[i].fitness < resultado->fitness)
      {
         resultado->alpha = objetivo[i].alpha;                             // Copiamos alpha a la mejor hormiga
         resultado->beta = objetivo[i].beta;                               // Copiamos beta a la mejor hormiga
         resultado->rho = objetivo[i].rho;                                 // Copiamos rho a la mejor hormiga
         resultado->numHormigas = objetivo[i].numHormigas;                 // Copiamos numHormigas a la mejor hormiga
         resultado->numIteracionesACO = objetivo[i].numIteracionesACO;     // Copiamos numIteraciones a la mejor hormiga
         resultado->temperatura_inicial = objetivo[i].temperatura_inicial; // Copiamos la temperatura inicial del mejor metal
         resultado->temperatura_final = objetivo[i].temperatura_final;     // Copiamos la temperatura final  del mejor metal
         resultado->factor_enfriamiento = objetivo[i].factor_enfriamiento; // Copiamos el factor de enfriamiento del mejor metal
         resultado->numIteracionesSA = objetivo[i].numIteracionesSA;       // Copiamos el numero de iteraciones del mejor metal
         recuperamos_mejor_hormiga(resultado, objetivo[i].hormiga);        // Recuperamos la mejor hormiga
      }

   // Inicializamos ya las generaciones
   for (int i = 0; i < num_generaciones; i++)
   {
      construyeRuidosos(objetivo, ruidoso, rango, num_poblacion); // Contruimos Ruidosos
      construyePrueba(objetivo, ruidoso, prueba, num_poblacion);  // Contruimos Prueba
                                                                  // Evaluamos la función objetivo para cada individuo de prueba
      for (int j = 0; j < num_poblacion; ++j)                     // Mandamos a evaluar la funcion objetivo de prueba{
         evaluaFO_AED(&prueba[j], instancia_feromonas, instancia_visibilidad, instancia_distancias, tsp);

      for (int i = 0; i < num_poblacion; i++)
         // Actualizamos el mejor resultado si encontramos uno mejor
         if (prueba[i].fitness < resultado->fitness)
         {
            resultado->alpha = prueba[i].alpha;                             // Copiamos alpha a la mejor hormiga
            resultado->beta = prueba[i].beta;                               // Copiamos beta a la mejor hormiga
            resultado->rho = prueba[i].rho;                                 // Copiamos rho a la mejor hormiga
            resultado->numHormigas = prueba[i].numHormigas;                 // Copiamos numHormigas a la mejor hormiga
            resultado->numIteracionesACO = prueba[i].numIteracionesACO;     // Copiamos numIteraciones a la mejor hormiga
            resultado->temperatura_inicial = prueba[i].temperatura_inicial; // Copiamos la temperatura inicial del mejor metal
            resultado->temperatura_final = prueba[i].temperatura_final;     // Copiamos la temperatura final  del mejor metal
            resultado->factor_enfriamiento = prueba[i].factor_enfriamiento; // Copiamos el factor de enfriamiento del mejor metal
            resultado->numIteracionesSA = prueba[i].numIteracionesSA;       // Copiamos el numero de iteraciones del mejor metal
            recuperamos_mejor_hormiga(resultado, prueba[i].hormiga);        // Recuperamos la mejor hormiga
         }
      // Realizamos la selección de la siguiente generación
      seleccion(objetivo, prueba, num_poblacion);                                   // Hacemos la seleccion
      int barra_ancho = 50;                                                         // Ancho de la barra de progreso
      int progreso_barras = (int)((float)(i + 1) / num_generaciones * barra_ancho); // Calculamos el progreso de la barra

      // Imprimimos la barra de progreso
      printf("\r[");
      for (int j = 0; j < barra_ancho; ++j)
      {
         if (j < progreso_barras)
            printf("=");
         else
            printf(" ");
      }
      printf("] %.2f%%  Mejor Fitness: %.2lf  Tiempo: %.2lf minutos",
             ((float)(i + 1) / num_generaciones) * 100,
             resultado->fitness,
             ((double)(clock() - tiempo_inicial)) / CLOCKS_PER_SEC / 60.0);
      fflush(stdout);
   }

   tiempo_final = clock(); // Finalizamos el tiempo
   // Calculamos el tiempo de ejecución en minutos
   double minutos = (((double)(tiempo_final - tiempo_inicial)) / CLOCKS_PER_SEC) / 60.0;

   tsp->tiempo_ejecucion = ceil(minutos);      // Redondiamos minutos
   tsp->archivo_instancia = archivo_instancia; // Copiamos el archivo de instancia

   // Imprimimos la meojor hormiga
   imprimir_mejor_hormiga(resultado->hormiga, resultado);          // Imprimimos la mejor hormiga
   printf("\nEl tiempo de ejecución es: %.2f minutos\n", minutos); // Imprimimos el tiempo de ejecución

   printf("\n¿Quieres imprimir el archivo JSON (s/n)? "); // Preguntamos si quiere imprimir el archivo JSON
   scanf(" %c", &respuesta);                              // Recibimos la respuesta

   if (respuesta == 's' || respuesta == 'S')                      // Comparamos la Respuesta
      guardar_json_en_archivo(resultado, tsp, archivo_instancia); // Guradamos el archivo JSON

   liberar_instancia(instancia_feromonas, tsp->num_clientes);   // Liberemos la memoria de la instancia feromona
   liberar_instancia(instancia_visibilidad, tsp->num_clientes); // Liberemos la memoria de la instancia visibilidad
   liberar_instancia(instancia_distancias, tsp->num_clientes);  // Liberemos la memoria de la instancia distancias
   liberar_rangos(rango);                                       // Liberemos la memoria de los rangos
   liberar_individuos(objetivo, num_poblacion, true);           // Liberemos la memoria del objetivo
   liberar_individuos(prueba, num_poblacion, false);            // Liberemos la memoria de la prueba
   liberar_individuos(ruidoso, num_poblacion, false);           // Liberemos la memoria del ruidoso
   liberar_individuos(resultado, 1, true);                      // Liberemos los resultado
   liberar_memoria_tsp_configuracion(tsp);                      // Liberemos la memoria del tsp
}