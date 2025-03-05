#include <stdio.h>
#include <stdlib.h>
#include "aed.h"
#include "configuracion_vrp_tw.h"
#include "vrp_tw_aco.h"
#include "control_memoria.h"

void imprimir_info_hormiga(struct hormiga *h, int numHormiga)
{
   printf("Hormiga %d:\n", numHormiga);
   printf("  Número de vehículos usados: %d\n", h->vehiculos_contados);

   for (int i = 0; i < h->vehiculos_contados; i++)
   {
      printf("  Vehículo %d:\n", h->flota[i].id_vehiculo);
      printf("    Capacidad: %.2lf\n", h->flota[i].capacidad);
      printf("    Capacidad restante: %.2lf\n", h->flota[i].capacidad_restante);
      printf("    Tiempo consumido: %.2lf\n", h->flota[i].tiempo_consumido);
      printf("    Clientes atendidos: %d\n", h->flota[i].clientes_contados);
      printf("    Ruta: ");

      for (int j = 0; j < h->flota[i].clientes_contados; j++)
      {
         printf("%d ", h->flota[i].ruta[j]);
      }
      printf("\n");
   }
}

void aco(){

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
      hormiga[i].tabu = asignar_memoria_arreglo(vrp->num_clientes);
      for(int j = 0; j < vrp->num_clientes;j++) hormiga[i].tabu[i] = 0;
      hormiga[i].tabu[0]=1;
      hormiga[i].vehiculos_contados = 1;
      hormiga[i].vehiculos_maximos = vrp->num_vehiculos;


      hormiga[i].id_hormiga = i;

      hormiga[i].flota = asignar_memoria_vehiculo(vrp);
      hormiga[i].flota[0].id_vehiculo = 0;
      hormiga[i].flota[0].capacidad = vrp->num_capacidad;
      hormiga[i].flota[0].capacidad_restante = vrp->num_capacidad;
      hormiga[i].flota[0].tiempo_consumido = 0.0;
      hormiga[i].flota[0].tiempo_maximo = 0.0;
      hormiga[i].flota[0].clientes_contados = 0;
      hormiga[i].flota[0].ruta = NULL;
      agregar_cliente_a_ruta(&hormiga[i].flota[0], vrp->clientes[0].id_cliente);
   }
}

void vrp_tw_aco(struct vrp_configuracion *vrp,struct individuo *ind, double **instancia_visiblidad, double **instancia_feromona)
{

   struct hormiga *hormiga = asignar_memoria_hormiga(ind);
   inicializar_hormigas_vehiculos(vrp, ind, hormiga);


   for (int i = 0; i < ind->numHormigas; i++)
   {
      imprimir_info_hormiga(&hormiga[i], i + 1);
     
   }
   printf("\n\n\n");
}