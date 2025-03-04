#include <stdio.h>
#include <stdlib.h>
#include "aed.h"
#include "configuracion_vrp_tw.h"
#include "vrp_tw_aco.h"
#include "entrada_salida_datos.h"
#include "control_memoria.h"


void agregar_cliente_a_ruta(vehiculo *vehiculo, int nuevo_cliente) {
   //Pendiente por que puedo hacer la operacion de las capacidades, etc 
   vehiculo->clientes_contados++;
   vehiculo->ruta = (int *)realloc(vehiculo->ruta, vehiculo->clientes_contados * sizeof(int));
   
   if (vehiculo->ruta == NULL) {
       printf("Error al asignar memoria para la ruta\n");
       exit(EXIT_FAILURE);
   }

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
      agregar_cliente_a_ruta(&hormiga[i].flota[0], 0); 



   }
}

void vrp_tw_aco(vrp_configuracion *vrp, individuo *ind, double **instancia_visiblidad, double **instancia_feromona)
{

   struct hormiga *hormiga = asignar_memoria_hormiga(ind);
   

}