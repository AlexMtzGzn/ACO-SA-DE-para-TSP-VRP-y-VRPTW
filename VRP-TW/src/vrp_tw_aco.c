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

double calcular_tiempo_viaje(double distancia){
   int velocidad = 1; //Pendiente
   return distancia / velocidad;

}

void calcular_ruta(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, double **instancia_visiblidad, double **instancia_feromona)
{
   int origen = hormiga->flota->ruta[hormiga->flota->clientes_contados-1];
   double suma_propbabilidades = 0.0;
   for(int i = 0; i < vrp->clientes-1; i++){
      if (hormiga->tabu[i] == 0){
         double distancia_viaje = calcular_distancia(vrp,origen,hormiga->tabu[i]);
         double tiempo_viaje =  calcular_tiempo_viaje(distancia_viaje);
         double distancia_viaje_deposito = calcular_distancia(vrp,hormiga->tabu[i],0);
         double tiempo_viaje_deposito = calcular_tiempo_viaje(distancia_viaje_deposito);

         if(hormiga->flota->tiempo_consumido + tiempo_viaje >= vrp->clientes[hormiga->tabu[i]].tiempo_inicial && hormiga->flota->tiempo_consumido+tiempo_viaje <= vrp->clientes[hormiga->tabu[i]].tiempo_final){
            if(hormiga->flota->capacidad_restante + vrp->clientes[hormiga->tabu[i]].demanda <= hormiga->flota->capacidad){
               if(hormiga->flota->tiempo_consumido+tiempo_viaje + tiempo_viaje_deposito + vrp->clientes[hormiga->tabu[i]].servicio <=hormiga->flota->tiempo_maximo){
                  double valuacion_tiempo = (hormiga->flota->tiempo_maximo > 0) ? (1.0 / hormiga->flota->tiempo_maximo) : 0.0;
                   hormiga->probabilidades[i]= pow(instancia_feromona[origen][hormiga->tabu[1]],ind->alpha)*pow(instancia_visiblidad[origen][hormiga->tabu[i]],ind->beta)*pow(valuacion_tiempo,ind->gamma);
                   suma_propbabilidades += hormiga->probabilidades[i];
               }
            }
         }
      }
      else{
         hormiga->probabilidades[i] = 0.0;
      }
   
   }

   if(suma_propbabilidades > 0){
      double aleatorio = (double)rand() / RAND_MAX;
      double prob_acumulada = 0.0;
      for (int j = 0; j < vrp->clientes; j++)
      {
          if (hormiga->tabu[j] == 0)
          {
              prob_acumulada += hormiga->probabilidades[j] / suma_propbabilidades;
              if (aleatorio <= prob_acumulada)
              {
                  agregar_cliente_a_ruta(&hormiga->flota[hormiga->vehiculos_contados-1], j);
                  hormiga->tabu[j] = 1;
                  break;
              }
          }
      }
  }
  else
  {
      // Selección aleatoria de clientes no visitados
      int ciudades_disponibles[vrp->num_clientes], num_disponibles = 0;
      for (int j = 0; j < vrp->clientes; j++)
      {
          if (hormiga->tabu[j] == 0)
          {
              ciudades_disponibles[num_disponibles++] = j;
          }
      }
      
      if (num_disponibles > 0)
      {
          int ciudad_elegida = ciudades_disponibles[rand() % num_disponibles];
          agregar_cliente_a_ruta(&hormiga->flota[hormiga->vehiculos_contados-1], ciudad_elegida);
          hormiga->tabu[ciudad_elegida] = 1;
      }
      else
      {
          // No hay más clientes disponibles, posiblemente necesitas cambiar de vehículo
          // O terminar la construcción de rutas
          return;
      }
  }
}

void aco_principal(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, double **instancia_visiblidad, double **instancia_feromona)
{
   bool intento = true;
   int numero_intento = 0;
   bool resultado;

   while (true)
   {
      calcular_ruta(vrp,ind,&hormiga->flota[hormiga->vehiculos_contados-1],instancia_visiblidad,instancia_feromona);

      if (!resultado)
      {
         numero_intento++;

         if (numero_intento == 20)
         {
            intento = false;
            //Tenemos que creear otra instancia para vehiculo en la misma hormiga
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

void aco_individuo(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, double **instancia_visiblidad, double **instancia_feromona)
{

   for (int i = 0; i < ind->numIteraciones; i++)
   {
      for (int j = 0; j < ind->numHormigas; j++)
      {
         calcular_ruta(vrp,ind,&hormiga[i],instancia_visiblidad,instancia_feromona);
      }
      //Aqui tendria que ajustar la feromona
   }
}

void agregar_cliente_a_ruta(struct vehiculo *vehiculo, int nuevo_cliente)
{
   // Pendiente por que puedo hacer la operacion de las capacidades, etc
   vehiculo->clientes_contados++;
   vehiculo->ruta = reasignar_memoria_ruta(vehiculo->ruta, vehiculo->clientes_contados);
   vehiculo->ruta[vehiculo->clientes_contados - 1] = nuevo_cliente;
}

void inicializar_hormigas_vehiculos(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga)
{

   for (int i = 0; i < ind->numHormigas; i++)
   {
      hormiga[i].id_hormiga = i;
      hormiga[i].tabu = asignar_memoria_arreglo(vrp->num_clientes);
      hormiga[i].probabilidades = asignar_memoria_arreglo_double(vrp->num_clientes);
      for (int j = 0; j < vrp->num_clientes; j++)
      {
         hormiga[i].tabu[i] = 0;
         hormiga[i].probabilidades[i] = 0;
      }
      hormiga[i].tabu[0] = 1;
      hormiga[i].probabilidades[0] = 1;
      hormiga[i].vehiculos_contados = 1;
      hormiga[i].vehiculos_maximos = vrp->num_vehiculos;
      hormiga[i].fitness_global = 0.0;

      hormiga[i].flota = asignar_memoria_vehiculo(vrp);
      hormiga[i].flota[0].id_vehiculo = 0;
      hormiga[i].flota[0].capacidad = vrp->num_capacidad;
      hormiga[i].flota[0].capacidad_restante = vrp->num_capacidad;
      hormiga[i].flota[0].tiempo_consumido = 0.0;
      hormiga[i].flota[0].tiempo_maximo = vrp->clientes[0].tiempo_final;
      hormiga[i].flota[0].clientes_contados = 0;
      hormiga[i].flota[0].ruta = NULL;
      agregar_cliente_a_ruta(&hormiga[i].flota[0], vrp->clientes[0].id_cliente);
   }
}

void vrp_tw_aco(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_visiblidad, double **instancia_feromona)
{

   struct hormiga *hormiga = asignar_memoria_hormiga(ind);
   inicializar_hormigas_vehiculos(vrp, ind, hormiga);

   /*for (int i = 0; i < ind->numHormigas; i++)
   {
      imprimir_info_hormiga(&hormiga[i], i + 1);
   }
   printf("\n\n\n");*/
}