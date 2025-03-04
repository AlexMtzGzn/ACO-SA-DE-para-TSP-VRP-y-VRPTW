#include <stdio.h>
#include <stdlib.h>
#include "aed.h"
#include "configuracion_vrp_tw.h"
#include "vrp_tw_aco.h"
#include "entrada_salida_datos.h"
#include "control_memoria.h"

void imprimir_info_hormiga(hormiga *h, int numHormiga) {
   printf("Hormiga %d:\n", numHormiga);
   printf("  Número de vehículos usados: %d\n", h->vehiculos_contados);

   for (int i = 0; i < h->vehiculos_contados; i++) {
       printf("  Vehículo %d:\n", h->flota[i].id_vehiculo);
       printf("    Capacidad: %d\n", h->flota[i].capacidad);
       printf("    Capacidad restante: %d\n", h->flota[i].capacidad_restante);
       printf("    Tiempo consumido: %.2f\n", h->flota[i].tiempo_consumido);
       printf("    Clientes atendidos: %d\n", h->flota[i].clientes_contados);
       printf("    Ruta: ");

       for (int j = 0; j < h->flota[i].clientes_contados; j++) {
           printf("%d ", h->flota[i].ruta[j]);
       }
       printf("\n");
   }
   printf("\n");
}


void agregar_cliente_a_ruta(vehiculo *vehiculo, int nuevo_cliente) {
   //Pendiente por que puedo hacer la operacion de las capacidades, etc 
   vehiculo->clientes_contados++;
   vehiculo->ruta = reasignar_memoria_arreglo(vehiculo);
   vehiculo->ruta[vehiculo->clientes_contados - 1] = nuevo_cliente;
}

void inicializar_hormigas_vehiculos(vrp_configuracion *vrp, individuo *ind, hormiga *hormiga)
{

   for (int i = 0; i < ind->numHormigas; i++)
   {
      hormiga[i].tabu = asignar_memoria_arreglo(vrp->num_clientes);
      hormiga[i].vehiculos_contados = 1;
      hormiga[i].vehiculos_maximos = vrp->num_vehiculos;
      hormiga[i].id_hormiga = i + 1;

      hormiga[i].flota = asignar_memoria_vehiculo();
      hormiga[i].flota[0].id_vehiculo = i + 1;
      hormiga[i].flota[0].capacidad = vrp->num_capacidad;
      hormiga[i].flota[0].capacidad_restante = vrp->num_capacidad;
      hormiga[i].flota[0].tiempo_consumido = 0.0;
      hormiga[i].flota[0].tiempo_maximo = 0.0;
      hormiga[i].flota[0].clientes_contados = 1;
      hormiga[i].flota[0].ruta = asignar_memoria_arreglo(1);
      agregar_cliente_a_ruta(&hormiga[i].flota[0], vrp->clientes[0].id_cliente); 

   }
}

void vrp_tw_aco(vrp_configuracion *vrp, individuo *ind, double **instancia_visiblidad, double **instancia_feromona)
{

   struct hormiga *hormiga = asignar_memoria_hormiga(ind);
   inicializar_hormigas_vehiculos(vrp, ind, hormiga);
   
   for (int i = 0; i < ind->numHormigas; i++) {
      imprimir_info_hormiga(&hormiga[i], i + 1);
  }

}