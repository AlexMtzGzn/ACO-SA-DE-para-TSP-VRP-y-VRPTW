#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../includes/aed.h"
#include "../includes/estructuras.h"
#include "../includes/vrp_tw_aco.h"
#include "../includes/configuracion_vrp_tw.h"
#include "../includes/control_memoria.h"

double calcular_Distancia(struct vrp_configuracion *vrp, int cliente_origen, int cliente_destino)
{
   // Retornamos la distancia de los puntos
   return sqrt(pow((vrp->clientes[cliente_destino].coordenada_x - vrp->clientes[cliente_origen].coordenada_x), 2.0) + pow((vrp->clientes[cliente_destino].coordenada_y - vrp->clientes[cliente_origen].coordenada_y), 2.0));
}

void inicializar_Visibilidad(double **instancia_visibilidad, struct vrp_configuracion *vrp)
{
   // Recorre todos los clientes y calcula la visibilidad entre ellos
   for (int i = 0; i < vrp->num_clientes; i++)
   {
      for (int j = 0; j < vrp->num_clientes; j++)
      {
         // Si i y j son diferentes, calcula la visibilidad (inversa de la distancia)
         if (i != j && calcular_Distancia(vrp, i, j) > 0)
            instancia_visibilidad[i][j] = 1.0 / calcular_Distancia(vrp, i, j);
         else
            instancia_visibilidad[i][j] = 0.0; // Para rutas a sí mismo, la visibilidad es 0.
      }
   }
}

void inicializar_Ventana_Tiempo(double **instancia_ventanas_tiempo, struct vrp_configuracion *vrp)
{
   // Recorre todos los clientes y calcula la ventana de tiempo inversa entre ellos
   for (int i = 0; i < vrp->num_clientes; i++)
   {
      for (int j = 0; j < vrp->num_clientes; j++)
      {
         // Si i y j son diferentes, asigna la ventana de tiempo como 1 / tiempo final del cliente j
         if (i != j && calcular_Distancia(vrp, i, j) > 0)
            instancia_ventanas_tiempo[i][j] = 1.0 / vrp->clientes[j].vt_final;
         else
            instancia_ventanas_tiempo[i][j] = 0.0; // Para rutas a sí mismo, la ventana de tiempo es 0.
      }
   }
}

void inicializar_Distancias(double **instancia_distancias, struct vrp_configuracion *vrp)
{
   // Recorre todos los clientes y calcula las distancias entre ellos
   for (int i = 0; i < vrp->num_clientes; i++)
   {
      for (int j = 0; j < vrp->num_clientes; j++)
      {
         // Si i y j son diferentes, asigna la distancia entre los dos clientes
         if (i != j)
            instancia_distancias[i][j] = calcular_Distancia(vrp, i, j);
         else
            instancia_distancias[i][j] = 0.0; // La distancia a sí mismo es 0.
      }
   }
}

void inicializar_Feromona(struct vrp_configuracion *vrp, double **instancia_feromona)
{
   // Recorre todos los clientes en la matriz de feromonas (fila i, columna j)
   for (int i = 0; i < vrp->num_clientes; i++)
   {
      for (int j = 0; j < vrp->num_clientes; j++)
      {
         // Si i y j son diferentes (es decir, no es la misma ciudad), se asigna un valor de feromona de 1.0
         if (i != j)
            instancia_feromona[i][j] = 1.0 / vrp->clientes[j].vt_final; // Hay que revisr si lo hacemo entre la ventana del tiempo final
         // Si i y j son iguales (es decir, es la misma ciudad), la feromona se establece en 0.0
         else
            instancia_feromona[i][j] = 0.0;
      }
   }
}

void evaluaFO_AED(struct individuo *ind, double **instancia_feromona, double **instancia_visibilidad, double **instancia_distancias, double **instancia_ventanas_tiempo, struct vrp_configuracion *vrp)
{
   // Inicializa las feromonas en la instancia
   inicializar_Feromona(vrp, instancia_feromona);
   // Ejecuta el algoritmo de optimización con ventanas de tiempo (ACO) en el individuo
   vrp_tw_aco(vrp, ind, instancia_visibilidad, instancia_distancias, instancia_feromona, instancia_ventanas_tiempo);
}

double generaAleatorio(double minimo, double maximo)
{
   // Genera un número aleatorio entre 0 y 1, luego lo escala al rango deseado
   return minimo + ((double)rand() / RAND_MAX) * (maximo - minimo);
}

void construyeRuidosos(struct individuo *objetivo, struct individuo *ruidoso, int poblacion)
{
   // Recorre cada individuo de la población ruidosa
   for (int i = 0; i < poblacion; ++i)
   {
      // Selecciona tres índices aleatorios diferentes de la población
      int aleatorio1 = rand() % poblacion;
      int aleatorio2 = rand() % poblacion;
      int aleatorio3 = rand() % poblacion;

      // Asegura que los índices sean distintos entre sí
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
      ruidoso[i].numIteraciones = objetivo[aleatorio1].numIteraciones + (int)(0.5 * (objetivo[aleatorio2].numIteraciones - objetivo[aleatorio3].numIteraciones));

      // Limita los valores de los parámetros para que estén dentro de un rango válido
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

      if (ruidoso[i].gamma < 0.0)
         ruidoso[i].gamma = 0.0;

      if (ruidoso[i].rho > 0.9)
         ruidoso[i].rho = 0.9;

      if (ruidoso[i].rho < 0.0)
         ruidoso[i].rho = 0.0;

      if (ruidoso[i].numHormigas > 30)
         ruidoso[i].numHormigas = 30;

      if (ruidoso[i].numHormigas < 10)
         ruidoso[i].numHormigas = 10;

      if (ruidoso[i].numIteraciones > 80)
         ruidoso[i].numIteraciones = 80;

      if (ruidoso[i].numIteraciones < 30)
         ruidoso[i].numIteraciones = 30;
   }
}
void construyePrueba(struct individuo *objetivo, struct individuo *ruidoso, struct individuo *prueba, int poblacion)
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

void seleccion(struct individuo *objetivo, struct individuo *prueba, int poblacion)
{
   // Recorremos la población para comparar cada individuo de ambas poblaciones.
   for (int i = 0; i < poblacion; ++i)
      // Si el fitness del individuo en la población objetivo es mayor que en la población de prueba,
      // se actualiza el individuo en la población objetivo por el de la población de prueba.
      if (objetivo[i].fitness > prueba[i].fitness)
         objetivo[i] = prueba[i];
}

void inicializaPoblacion(struct individuo *objetivo, int poblacion)
{
   for (int i = 0; i < poblacion; ++i)
   {
      // Asignamos valores aleatorios dentro de los nuevos rangos
      objetivo[i].alpha = generaAleatorio(0.1, 2.0);             // alpha: entre 0.1 y 2.0
      objetivo[i].beta = generaAleatorio(1.5, 2.5);              // beta: entre 1.5 y 2.5
      objetivo[i].gamma = generaAleatorio(0.0, 1.5);             // gamma: entre 1.5 y 2.5
      objetivo[i].rho = generaAleatorio(0.0, 0.9);               // rho: entre 0.0 y 1.0
      objetivo[i].numHormigas = (int)generaAleatorio(10, 30);    // numHormigas: entre 10 y 30
      objetivo[i].numIteraciones = (int)generaAleatorio(30, 80); // numIteraciones: entre 30 y 80
   }
}

int aed_vrp_tw(int num_poblacion, int num_generaciones, char *archivo_instancia)
{

   struct individuo *objetivo = asignar_memoria_individuos(num_poblacion); // Asignamos memoria para el arreglo objetivo
   struct individuo *ruidoso = asignar_memoria_individuos(num_poblacion);  // Asignamos memoria para el arreglo ruidoso
   struct individuo *prueba = asignar_memoria_individuos(num_poblacion);   // Asiganamos memoria para el arreglo prueba
   vrp_configuracion *vrp = leer_instancia(archivo_instancia);             // Mandamo a leer la instancia y a retormamos en un apuntador structura vrp_configuracion

   if (vrp == NULL) // Retornamos -1 si no se lleno bien el vrp
      return -1;

   double **instancia_visibilidad = asignar_memoria_instancia(vrp->num_clientes);     // Generamos memoria para la instancia de la visibilidad
   double **instancia_feromonas = asignar_memoria_instancia(vrp->num_clientes);       // Generamos memoria para la instancia de la feromona
   double **instancia_distancias = asignar_memoria_instancia(vrp->num_clientes);      // Generamos memoria para la instancia de la las distancias
   double **instancia_ventanas_tiempo = asignar_memoria_instancia(vrp->num_clientes); // Generamos memoria para la instancia de las ventanas de tiempo

   inicializar_Distancias(instancia_distancias, vrp);          // Inicializamos las distancias
   inicializar_Visibilidad(instancia_visibilidad, vrp);        // Inicializamos las visibilidad
   inicializar_Ventana_Tiempo(instancia_ventanas_tiempo, vrp); // Inicializmos las ventanas de tiempo
   inicializar_Feromona(vrp, instancia_feromonas);             // Inicializamos la feromona
   inicializaPoblacion(objetivo, num_poblacion);               // Inicializamos la poblacion

   for (int i = 0; i < num_poblacion; ++i){ // Iniciamos la funcion objetivo con el objetivo
      evaluaFO_AED(&objetivo[i], instancia_feromonas, instancia_visibilidad, instancia_distancias, instancia_ventanas_tiempo, vrp);
      printf("%lf\n",objetivo[i].fitness);
   }
   // Inicializamos ya las generaciones
   for (int i = 0; i < num_generaciones; i++)
   {
      construyeRuidosos(objetivo, ruidoso, num_poblacion);       // Contruimos Ruidosos
      construyePrueba(objetivo, ruidoso, prueba, num_poblacion); // Contruimos Prueba

      for (int j = 0; j < num_poblacion; ++j){ // Mandamos a evaluar la funcion objetivo de prueba{
         evaluaFO_AED(&prueba[j], instancia_feromonas, instancia_visibilidad, instancia_distancias, instancia_ventanas_tiempo, vrp);
         printf("%lf\n",prueba[j].fitness);
         //imprimir_hormigas(prueba[j].hormiga, vrp, 1);
      }

      seleccion(objetivo, prueba, num_poblacion); // Hacemos la seleccion
   }

   liberar_instancia(instancia_feromonas, vrp->num_clientes);       // Liberemos la memoria de la instancia feromona
   liberar_instancia(instancia_visibilidad, vrp->num_clientes);     // Liberemos la memoria de la instancia visibilidad
   liberar_instancia(instancia_distancias, vrp->num_clientes);      // Liberemos la memoria de la instancia distancias
   liberar_instancia(instancia_ventanas_tiempo, vrp->num_clientes); // Liberemos la memoria de la instancia ventanas de tiempo
   liberar_individuos(objetivo, num_poblacion, true);               // Liberemos la memoria del objetivo
   liberar_individuos(prueba, num_poblacion, true);                 // Liberemos la memoria de la prueba
   liberar_individuos(ruidoso, num_poblacion, false);               // Liberemos la memoria del ruidoso
   liberar_memoria_vrp_configuracion(vrp);                          // Liberemos la memoria del vrp

   return 0;
}