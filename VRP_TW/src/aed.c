#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../include/aed.h"
#include "../include/estructuras.h"
#include "../include/vrp_tw_aco.h"
#include "../include/configuracion_vrp_tw.h"
#include "../include/control_memoria.h"
#include "../include/salida_datos.h"
#include "../include/configuracion_json.h"

double calcular_Distancia(struct vrp_configuracion *vrp, int cliente_origen, int cliente_destino)
{
   // Retornamos la distancia de los puntos
   double distancia = sqrt(pow((vrp->clientes[cliente_destino].coordenada_x - vrp->clientes[cliente_origen].coordenada_x), 2.0) + pow((vrp->clientes[cliente_destino].coordenada_y - vrp->clientes[cliente_origen].coordenada_y), 2.0));
   return distancia;
}

void inicializar_Visibilidad(double **instancia_visibilidad, struct vrp_configuracion *vrp)
{
   double distancia; // Declaramos la variable distancia
   // Recorre todos los clientes y calcula la visibilidad entre ellos
   for (int i = 0; i < vrp->num_clientes; i++)
   {
      for (int j = i + 1; j < vrp->num_clientes; j++)
      {
         if (i != j)
         {
            distancia = calcular_Distancia(vrp, i, j);
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

void inicializar_Ventana_Tiempo(double **instancia_ventanas_tiempo, struct vrp_configuracion *vrp)
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

void inicializar_Distancias(double **instancia_distancias, struct vrp_configuracion *vrp)
{
   double distancia;
   // Recorre todos los clientes y calcula las distancias entre ellos
   for (int i = 0; i < vrp->num_clientes; i++)
   {
      for (int j = i + 1; j < vrp->num_clientes; j++)
      {
         if (i != j)
         {
            distancia = calcular_Distancia(vrp, i, j);
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

void inicializar_Feromona(struct vrp_configuracion *vrp, double **instancia_feromona)
{
   // Recorre todos los clientes en la matriz de feromonas (fila i, columna j)
   for (int i = 0; i < vrp->num_clientes; i++)
   {
      for (int j = i + 1; j < vrp->num_clientes; j++)
      {
         // Si i y j son diferentes (es decir, no es la misma ciudad), se asigna un valor de feromona de 1.0
         if (i != j)
         {
            instancia_feromona[i][j] = 1.0;
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

void evaluaFO_AED(struct individuo *ind, double **instancia_feromona, double **instancia_visibilidad, double **instancia_distancias, double **instancia_ventanas_tiempo, struct vrp_configuracion *vrp)
{
   // Inicializa las feromonas en la instancia
   inicializar_Feromona(vrp, instancia_feromona);
   // imprimir_instancia(instancia_feromonas,vrp,"INSTANCIA FEROMONAS");
   // Ejecuta el algoritmo de optimización con ventanas de tiempo (ACO) en el individuo
   vrp_tw_aco(vrp, ind, instancia_visibilidad, instancia_distancias, instancia_feromona, instancia_ventanas_tiempo);
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
      ruidoso[i].gamma = objetivo[aleatorio1].gamma + 0.5 * (objetivo[aleatorio2].gamma - objetivo[aleatorio3].gamma);
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

void construyePrueba(struct individuo *objetivo, struct individuo *ruidoso, struct individuo *prueba, int poblacion)
{
   // Itera sobre todos los individuos en la población.
   for (int i = 0; i < poblacion; ++i)
   {
      // Genera un número aleatorio en el rango [0,1].
      double aleatorio = (double)rand() / RAND_MAX;

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

void inicializaPoblacion(struct individuo *objetivo, struct vrp_configuracion *vrp, struct rangos *rango, int poblacion)
{
   // Itera sobre cada individuo de la población
   for (int i = 0; i < poblacion; ++i)
   {

      rango->maxAlpha = 6.0;
      rango->minAlpha = 1.0;

      rango->maxBeta = 8.0;
      rango->minBeta = 1.0;

      rango->minGamma = 0.1;
      rango->maxGamma = 5.0;

      rango->maxRho = 0.9;
      rango->minRho = 0.1;

      rango->maxNumHormigas = 50;
      rango->minNumHormigas = 5;

      rango->maxNumIteracionesACO = 200;
      rango->minNumIteracionesACO = 30;

      rango->maxTemperatura_inicial = 2000.0;
      rango->minTemperatura_inicial = 100.0;

      rango->maxTemperatura_final = 0.5;
      rango->minTemperatura_final = 0.001;

      rango->maxFactor_enfriamiento = 0.999;
      rango->minFactor_enfriamiento = 0.90;

      rango->maxFactor_control = 0.9;
      rango->minFactor_control = 0.5;

      rango->maxIteracionesSA = 300;
      rango->minIteracionesSA = 50;

      // Genera valores aleatorios dentro de los rangos definidos para cada individuo
      objetivo[i].alpha = generaAleatorio(rango->minAlpha, rango->maxAlpha);
      objetivo[i].beta = generaAleatorio(rango->minBeta, rango->maxBeta);
      objetivo[i].gamma = generaAleatorio(rango->minGamma, rango->maxGamma);
      objetivo[i].rho = generaAleatorio(rango->minRho, rango->maxRho);
      objetivo[i].numHormigas = (int)generaAleatorio(rango->minNumHormigas, rango->maxNumHormigas);
      objetivo[i].numIteracionesACO = (int)generaAleatorio(rango->minNumIteracionesACO, rango->maxNumIteracionesACO);
      objetivo[i].temperatura_inicial = generaAleatorio(rango->minTemperatura_inicial, rango->maxTemperatura_inicial);
      objetivo[i].temperatura_final = generaAleatorio(rango->minTemperatura_final, rango->maxTemperatura_final);
      objetivo[i].factor_enfriamiento = generaAleatorio(rango->minFactor_enfriamiento, rango->maxFactor_enfriamiento);
      objetivo[i].factor_control = generaAleatorio(rango->minFactor_control, rango->maxFactor_control);
      objetivo[i].numIteracionesSA = (int)generaAleatorio(rango->minIteracionesSA, rango->maxIteracionesSA);
   }
}

void aed_vrp_tw(int num_poblacion, int num_generaciones, int tamanio_instancia, char *archivo_instancia)
{
   clock_t timepo_inicial, timepo_final;
   timepo_inicial = clock();
   char respuesta;                                                                // Respuesta
   struct individuo *objetivo = asignar_memoria_individuos(num_poblacion);        // Asignamos memoria para el arreglo objetivo
   struct individuo *ruidoso = asignar_memoria_individuos(num_poblacion);         // Asignamos memoria para el arreglo ruidoso
   struct individuo *prueba = asignar_memoria_individuos(num_poblacion);          // Asiganamos memoria para el arreglo prueba
   struct individuo *resultado = asignar_memoria_individuos(1);                   // Asignamos memoria para el arreglo de resultados
   vrp_configuracion *vrp = leer_instancia(archivo_instancia, tamanio_instancia); // Mandamo a leer la instancia y a retormamos en un apuntador structura vrp_configuracion
   struct rangos *rango = asignar_memoria_rangos();

   vrp->generaciones = num_generaciones; // Asignamos el numero de generaciones
   vrp->poblacion = num_poblacion;       // Asiganamos el numero de poblacion

   double **instancia_visibilidad = asignar_memoria_instancia(vrp->num_clientes);     // Generamos memoria para la instancia de la visibilidad
   double **instancia_feromonas = asignar_memoria_instancia(vrp->num_clientes);       // Generamos memoria para la instancia de la feromona
   double **instancia_distancias = asignar_memoria_instancia(vrp->num_clientes);      // Generamos memoria para la instancia de la las distancias
   double **instancia_ventanas_tiempo = asignar_memoria_instancia(vrp->num_clientes); // Generamos memoria para la instancia de las ventanas de tiempo

   inicializar_Distancias(instancia_distancias, vrp);          // Inicializamos las distancias
   inicializar_Visibilidad(instancia_visibilidad, vrp);        // Inicializamos las visibilidad
   inicializar_Ventana_Tiempo(instancia_ventanas_tiempo, vrp); // Inicializmos las ventanas de tiempo
   inicializar_Feromona(vrp, instancia_feromonas);             // Inicializamos la feromona
   inicializaPoblacion(objetivo, vrp, rango, num_poblacion);   // Inicializamos la poblacion

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
         resultado->numIteracionesACO = objetivo[i].numIteracionesACO;
         resultado->temperatura_inicial = objetivo[i].temperatura_inicial; // Copiamos la temperatura inicial del mejor metal
         resultado->temperatura_final = objetivo[i].temperatura_final;     // Copiamos la temperatura final  del mejor metal
         resultado->factor_enfriamiento = objetivo[i].factor_enfriamiento; // Copiamos el factor de enfriamiento del mejor metal
         resultado->factor_control = objetivo[i].factor_control;           // Copiamos el factor de control del mejor metal
         resultado->numIteracionesSA = objetivo[i].numIteracionesSA;       // Copiamos el numero de iteraciones del mejor metal
         recuperamos_mejor_hormiga(resultado, objetivo[i].hormiga);
      }

   // Inicializamos ya las generaciones
   for (int i = 0; i < num_generaciones; i++)
   {
      construyeRuidosos(objetivo, ruidoso, rango, num_poblacion); // Contruimos Ruidosos
      construyePrueba(objetivo, ruidoso, prueba, num_poblacion);  // Contruimos Prueba
                                                                  // Evaluamos la función objetivo para cada individuo de prueba
      for (int j = 0; j < num_poblacion; ++j)                     // Mandamos a evaluar la funcion objetivo de prueba{
         evaluaFO_AED(&prueba[j], instancia_feromonas, instancia_visibilidad, instancia_distancias, instancia_ventanas_tiempo, vrp);

      for (int i = 0; i < num_poblacion; i++)
         // Actualizamos el mejor resultado si encontramos uno mejor
         if (prueba[i].fitness < resultado->fitness)
         {
            resultado->alpha = prueba[i].alpha;
            resultado->beta = prueba[i].beta;
            resultado->gamma = prueba[i].gamma;
            resultado->rho = prueba[i].rho;
            resultado->numHormigas = prueba[i].numHormigas;
            resultado->numIteracionesACO = prueba[i].numIteracionesACO;
            resultado->temperatura_inicial = prueba[i].temperatura_inicial; // Copiamos la temperatura inicial del mejor metal
            resultado->temperatura_final = prueba[i].temperatura_final;     // Copiamos la temperatura final  del mejor metal
            resultado->factor_enfriamiento = prueba[i].factor_enfriamiento; // Copiamos el factor de enfriamiento del mejor metal
            resultado->factor_control = prueba[i].factor_control;           // Copiamos el factor de control del mejor metal
            resultado->numIteracionesSA = prueba[i].numIteracionesSA;       // Copiamos el numero de iteraciones del mejor metal
            recuperamos_mejor_hormiga(resultado, prueba[i].hormiga);
         }
      // Realizamos la selección de la siguiente generación
      seleccion(objetivo, prueba, num_poblacion); // Hacemos la seleccion
      int barra_ancho = 50;                       // Ancho de la barra de progreso
      int progreso_barras = (int)((float)(i + 1) / num_generaciones * barra_ancho);

      printf("\r[");
      for (int j = 0; j < barra_ancho; ++j)
      {
         if (j < progreso_barras)
            printf("#");
         else
            printf(" ");
      }
      printf("] %.2f%%  Mejor Fitness: %.2lf  Tiempo: %.2lf minutos",
             ((float)(i + 1) / num_generaciones) * 100,
             resultado->fitness,
             ((double)(clock() - timepo_inicial)) / CLOCKS_PER_SEC / 60.0);
      fflush(stdout);
   }

   timepo_final = clock();
   double minutos = (((double)(timepo_final - timepo_inicial)) / CLOCKS_PER_SEC) / 60.0;

   vrp->tiempo_ejecucion = ceil(minutos);
   vrp->archivo_instancia = archivo_instancia;

   // Imprimimos la meojor homriga
   imprimir_mejor_hormiga(resultado->hormiga, resultado);
   printf("\nEl tiempo de ejecución es: %.2f minutos\n", minutos);

   printf("\n¿Quieres imprimir el archivo JSON (s/n)? ");
   scanf(" %c", &respuesta);

   if (respuesta == 's' || respuesta == 'S')
      guardar_json_en_archivo(resultado, vrp, archivo_instancia);

   liberar_instancia(instancia_feromonas, vrp->num_clientes);       // Liberemos la memoria de la instancia feromona
   liberar_instancia(instancia_visibilidad, vrp->num_clientes);     // Liberemos la memoria de la instancia visibilidad
   liberar_instancia(instancia_distancias, vrp->num_clientes);      // Liberemos la memoria de la instancia distancias
   liberar_instancia(instancia_ventanas_tiempo, vrp->num_clientes); // Liberemos la memoria de la instancia ventanas de tiempo
   liberar_rangos(rango);                                           // Liberemos la memoria de los rangos
   liberar_individuos(objetivo, num_poblacion, true);               // Liberemos la memoria del objetivo
   liberar_individuos(prueba, num_poblacion, true);                 // Liberemos la memoria de la prueba
   liberar_individuos(ruidoso, num_poblacion, false);               // Liberemos la memoria del ruidoso
   liberar_individuos(resultado, 1, true);                          // Liberemos los resultado
   liberar_memoria_vrp_configuracion(vrp);                          // Liberemos la memoria del vrp
}