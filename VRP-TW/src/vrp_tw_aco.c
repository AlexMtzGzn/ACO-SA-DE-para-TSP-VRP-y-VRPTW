#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "aed.h"
#include "configuracion_vrp_tw.h"
#include "vrp_tw_aco.h"
#include "control_memoria.h"

/*void imprimir_info_hormiga(struct hormiga *hormiga, int numHormiga)
{
   printf("Hormiga %d:\n", hormiga->id_hormiga);
   printf("  Número de vehículos usados: %d\n", hormiga->vehiculos_contados);
   printf("  Número de vehículos maximos: %d\n", hormiga->vehiculos_maximos);

   for (int i = 0; i < hormiga->vehiculos_contados; i++)
   {
      printf("  Vehículo %d:\n", hormiga->flota[i].id_vehiculo);
      printf("    Capacidad: %.2lf\n", hormiga->flota[i].capacidad);
      printf("    Capacidad restante: %.2lf\n", hormiga->flota[i].capacidad_restante);
      printf("    Tiempo consumido: %.2lf\n", hormiga->flota[i].tiempo_consumido);
      printf("    Tiempo maximo: %.2lf\n", hormiga->flota[i].tiempo_maximo);
      printf("    Clientes atendidos: %d\n", hormiga->flota[i].clientes_contados);
      printf("    Ruta: ");

      for (int j = 0; j < hormiga->flota[i].clientes_contados; j++)
      {
         printf("%d ", hormiga->flota[i].ruta[j]);
      }
      printf("\n");
   }
}*/

void actualizar_feromona(struct individuo *ind, struct vrp_configuracion *vrp, struct hormiga *hormiga, double **instancia_feromona)
{
   double **delta_feromona = asignar_memoria_instancia(vrp->num_clientes);

   for (int i = 0; i < hormiga->vehiculos_contados; i++)
   {
      double delta = 1.0 / hormiga->flota[i].fitness_vehiculo;

      for (int j = 0; j < vrp->num_clientes - 1; j++)
      {
         int cliente_actual = hormiga->flota->ruta[j];
         int cliente_siguiente = hormiga->flota->ruta[j + 1];
         delta_feromona[cliente_actual][cliente_siguiente] += delta;
         delta_feromona[cliente_siguiente][cliente_actual] += delta;
      }
   }

   for (int i = 0; i < vrp->num_clientes; i++)
   {
      for (int j = 0; j < vrp->num_clientes; j++)
      {
         if (i != j)
         {
            instancia_feromona[i][j] = (1.0 - ind->rho) * instancia_feromona[i][j] +
                                       delta_feromona[i][j];
            instancia_feromona[j][i] = (1.0 - ind->rho) * instancia_feromona[j][i] +
                                       delta_feromona[j][i];
         }
      }
   }

   // Hay que liberar la isntancia delta_feromona
}

double calcular_tiempo_viaje(double distancia)
{
   int velocidad = 1; // Pendiente
   return distancia / velocidad;
}
void agregar_cliente_a_ruta(struct hormiga *hormiga, struct vehiculo *vehiculo, int nuevo_cliente)
{
   vehiculo->clientes_contados++;                                                        // Incrementamos un cliente
   vehiculo->ruta = reasignar_memoria_ruta(vehiculo->ruta, vehiculo->clientes_contados); // Reasiganamos la memoria de la ruta
   vehiculo->ruta[vehiculo->clientes_contados - 1] = nuevo_cliente;                      // Agreamos al cliente al a la ruta
   hormiga->tabu[nuevo_cliente] = 1;                                                     // Agreamos a la lista tabu el cliente
}

void inicializar_vehiculo(struct hormiga *hormiga, struct vrp_configuracion *vrp)
{
   hormiga->flota[hormiga->vehiculos_contados - 1].id_vehiculo = hormiga->vehiculos_contados;                       // Inicializmos id_vehiculo con el valor vehiculos contados de la hormiga
   hormiga->flota[hormiga->vehiculos_contados - 1].capacidad = vrp->num_capacidad;                                  // Inicializamos la capacidad
   hormiga->flota[hormiga->vehiculos_contados - 1].capacidad_restante = vrp->num_capacidad;                         // Inicializamos la capacidad restante con la capacidad
   hormiga->flota[hormiga->vehiculos_contados - 1].tiempo_consumido = 0.0;                                          // Inicializmos el tiempo consumido en 0.0
   hormiga->flota[hormiga->vehiculos_contados - 1].tiempo_maximo = vrp->clientes[0].tiempo_final;                   // Inicialiamos el tiempo final
   hormiga->flota[hormiga->vehiculos_contados - 1].clientes_contados = 0;                                           // Inicializamos los clientes en 0
   hormiga->flota[hormiga->vehiculos_contados - 1].ruta = NULL;                                                     // Inicializmos laruta en NULL
   hormiga->flota[hormiga->vehiculos_contados - 1].fitness_vehiculo = 0.0;                                          // Inicializamos  el fitness_vehiculo
   agregar_cliente_a_ruta(&hormiga, &hormiga->flota[hormiga->vehiculos_contados - 1], vrp->clientes[0].id_cliente); // Enviamos a la funcion agregar_cliente_ruta, la hormiga, y el vehiculo
}
bool calcular_ruta(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, double **instancia_visiblidad, double **instancia_feromona)
{
   bool respueta_agregado = false;
   int origen = hormiga->flota->ruta[hormiga->flota->clientes_contados - 1];

   for (int i = 0; i < vrp->clientes; i++)
      hormiga->probabilidades[i] = 0.0;

   hormiga->suma_probabilidades = 0.0;

   for (int i = 0; i < vrp->clientes - 1; i++)
   {
      if (hormiga->tabu[i] == 0)
      {
         double distancia_viaje = calcular_distancia(vrp, origen, hormiga->tabu[i]);
         double tiempo_viaje = calcular_tiempo_viaje(distancia_viaje);
         double distancia_viaje_deposito = calcular_distancia(vrp, hormiga->tabu[i], 0);
         double tiempo_viaje_deposito = calcular_tiempo_viaje(distancia_viaje_deposito);

         if (hormiga->flota->tiempo_consumido + tiempo_viaje >= vrp->clientes[hormiga->tabu[i]].tiempo_inicial && hormiga->flota->tiempo_consumido + tiempo_viaje <= vrp->clientes[hormiga->tabu[i]].tiempo_final)
         {
            if (hormiga->flota->capacidad_restante + vrp->clientes[hormiga->tabu[i]].demanda <= hormiga->flota->capacidad)
            {
               if (hormiga->flota->tiempo_consumido + tiempo_viaje + tiempo_viaje_deposito + vrp->clientes[hormiga->tabu[i]].servicio <= hormiga->flota->tiempo_maximo)
               {
                  double valuacion_tiempo = (hormiga->flota->tiempo_maximo > 0) ? (1.0 / hormiga->flota->tiempo_maximo) : 0.0;
                  hormiga->probabilidades[i] = pow(instancia_feromona[origen][i], ind->alpha) * pow(instancia_visiblidad[origen][i], ind->beta) * pow(valuacion_tiempo, ind->gamma);
                  hormiga->suma_probabilidades += hormiga->probabilidades[i];
               }
            }
         }
      }
      else
      {
         hormiga->probabilidades[i] = 0.0;
      }
   }

   if (hormiga->suma_probabilidades > 0.0)
   {
      double aleatorio = (double)rand() / RAND_MAX;
      double prob_acumulada = 0.0;
      for (int j = 0; j < vrp->clientes; j++)
      {
         if (hormiga->tabu[j] == 0)
         {
            prob_acumulada += hormiga->probabilidades[j] / hormiga->suma_probabilidades;
            if (aleatorio <= prob_acumulada)
            {
               // agregar_cliente_a_ruta(&hormiga->flota[hormiga->vehiculos_contados - 1], j);
               hormiga->tabu[j] = 1;
               hormiga->flota->capacidad_restante += vrp->clientes[j].demanda;
               double distancia_viaje = calcular_distancia(vrp, origen, hormiga->tabu[j]);
               double tiempo_viaje = calcular_tiempo_viaje(distancia_viaje);
               hormiga->flota->tiempo_consumido += tiempo_viaje + vrp->clientes[j].servicio;
               respueta_agregado = true;
               return respueta_agregado;
            }
         }
      }
   }
}

int contar_tabu(struct vrp_configuracion *vrp, struct hormiga *hormiga)
{
   int contador = 0;
   for (int i = 0; i < vrp->num_clientes; i++)
   {
      if (hormiga->tabu[i] == 0)
         contador++;
   }
   return contador;
}

void aco_principal(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, double **instancia_visiblidad, double **instancia_feromona)
{
   bool intento = true;
   int numero_intento = 0;
   bool resultado;

   while (true)
   {
      resultado = calcular_ruta(vrp, ind, &hormiga->flota[hormiga->vehiculos_contados - 1], instancia_visiblidad, instancia_feromona);

      if (!resultado)
      {
         numero_intento++;

         if (numero_intento == 20 && contar_tabu(vrp, hormiga) != 0)
         {
            intento = false;
            hormiga->vehiculos_contados++;
            hormiga->flota = redimensionar_memoria_vehiculo(hormiga);
            inicializar_vehiculo(&hormiga, vrp);
            break;
         }
      }
      else
      {
         numero_intento = 0;
         intento = true;
         break;
      }
   }
}

void inicializar_hormiga(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga)
{
   // Bucle para iterar todas las hormigas
   for (int i = 0; i < ind->numHormigas; i++)
   {
      hormiga[i].id_hormiga = i + 1;                                                 // Asignamos el id de la hormiga con i + 1
      hormiga[i].tabu = asignar_memoria_arreglo(vrp->num_clientes);                  // Asignamos memoria para el arreglo_tabu
      hormiga[i].probabilidades = asignar_memoria_arreglo_double(vrp->num_clientes); // asiganamos memoria para el arreglo_probabilidades
      for (int j = 0; j < vrp->num_clientes; j++)
         hormiga[i].tabu[j] = 0;                         // LLenamos todas las posiciones del arreglo tabu en 0
      hormiga->suma_probabilidades = 0.0;                // Inicializamos en 0.0 la suma de probabilidades                         // Como el primer cliente que visitamos es el deposito y esta en la posicion 0 del arreglo tabu, lo inicializamos en 1
      hormiga[i].vehiculos_contados = 1;                 // inicializamos vehiculos contados con 1
      hormiga[i].vehiculos_maximos = vrp->num_vehiculos; // Inicializamos cuantos vehiculos disonibles tiene la hormiga para su flota
      hormiga[i].fitness_global = 0.0;                   // Inicializmos en 0.0 el fitness_global en 0.0
      hormiga[i].flota = asignar_memoria_vehiculo(vrp);  // Asignamos memoria para la foto de vehiculos de cada hormiga
      inicializar_vehiculo(&hormiga[i], vrp);            // Enviamos como parametro la hormiga y vrp
   }
}

void vrp_tw_aco(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_visiblidad, double **instancia_feromona)
{

   for (int i = 0; i < ind->numIteraciones; i++)
   {
      if (i != 0)
         i = 2;

      struct hormiga *hormiga = asignar_memoria_hormiga(ind); // retorna un aputador con la estrutura de hormiga
      inicializar_hormigas_vehiculos(vrp, ind, hormiga);      // Mandamos los parametros vrp, ind ,hormiga para inicializar los datos de la hormiga

      for (int j = 0; j < ind->numHormigas; j++)
         aco_principal(vrp, ind, &hormiga[i], instancia_visiblidad, instancia_feromona); //Se envia como argumento el vrp, ind, solo una hormiga, instancia de visibilidad y la instancia de la feromna

      for (int j = 0; j < ind->numHormigas; j++)
      {
         actualizar_feromona(ind, vrp, &hormiga[j], instancia_feromona); // Ain falta ajustarla
      }
   }
}