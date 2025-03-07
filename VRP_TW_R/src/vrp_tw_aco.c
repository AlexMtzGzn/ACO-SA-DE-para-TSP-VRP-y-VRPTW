#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "aed.h"
#include "configuracion_vrp_tw.h"
#include "vrp_tw_aco.h"
#include "control_memoria.h"
#include "lista_flota.h"
#include "lista_ruta.h"

void inicializar_vehiculo(struct hormiga *hormiga, struct vrp_configuracion *vrp)
{
}

void inicializar_hormiga(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga)
{
   for (int i = 0; i < ind->numHormigas; i++)
   {
      hormiga[i].id_hormiga = i;                                                     // Le asiganamos el id de la hormiga que es i
      hormiga[i].tabu = asignar_memoria_arreglo_int(vrp->num_clientes);              // Generamos memoria para el tabu que es el numero de clientes que tenemos
      hormiga[i].probabilidades = asignar_memoria_arreglo_double(vrp->num_clientes); // Asignamos memoria para las probablidadades el cual es el numero de clientes que hay
      hormiga[i].vehiculos_contados = 1;                                             // Inicializamos el numero de vehiculos contados en 0
      hormiga[i].vehiculos_maximos = vrp->num_vehiculos;                             // Inicializamos el nuemro de vehiculos maximos con vrp->num_vehiculos
      do
         hormiga[i].flota = asignar_memoria_lista_vehiculos();
      while (hormiga[i].flota == NULL); // Asignamos memoria para la flota de la hormiga
      inserta_vehiculo_flota(&hormiga[i],vrp); //Insertamos el vehiculo y sus datos

   }
}

void vrp_tw_aco(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_visiblidad, double **instancia_feromona)
{
   struct hormiga *hormiga = asignar_memoria_hormigas(ind);

   // for(int i = 0 ; i< ind->numIteraciones; i++){

   inicializar_hormiga(vrp, ind, hormiga);

   //}
}