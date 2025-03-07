#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "aed.h"
#include "configuracion_vrp_tw.h"
#include "vrp_tw_aco.h"
#include "control_memoria.h"
#include "lista_flota.h"
#include "lista_ruta.h"

#include <stdio.h>
#include "vrp_tw_aco.h"
#include "configuracion_vrp_tw.h"

// Función para imprimir la información de una ruta (lista de clientes)
void imprimir_ruta(struct lista_ruta *ruta, int vehiculo_id) {
    if (es_vacia_lista_ruta(ruta)) {
        printf("    - Ruta del vehículo %d: VACÍA\n", vehiculo_id);
        return;
    }
    
    printf("    - Ruta del vehículo %d: ", vehiculo_id);
    
    struct nodo_ruta *actual = ruta->cabeza;
    printf("Depósito -> ");
    
    while (actual != NULL) {
        printf("Cliente %d", actual->cliente);
        if (actual->siguiente != NULL) {
            printf(" -> ");
        }
        actual = actual->siguiente;
    }
    
    printf(" -> Depósito\n");
}

// Función para imprimir la información de un vehículo
void imprimir_vehiculo(struct vehiculo *vehiculo) {
    printf("  + Vehículo ID: %d\n", vehiculo->id_vehiculo);
    printf("    - Capacidad máxima: %.2f\n", vehiculo->capacidad_maxima);
    printf("    - Capacidad restante: %.2f\n", vehiculo->capacidad_restante);
    printf("    - Tiempo consumido: %.2f\n", vehiculo->tiempo_consumido);
    printf("    - Tiempo máximo: %.2f\n", vehiculo->tiempo_maximo);
    printf("    - Número de clientes: %d\n", vehiculo->clientes_contados);
    printf("    - Fitness del vehículo: %.2f\n", vehiculo->fitness_vehiculo);
    
    // Imprimir la ruta del vehículo
    imprimir_ruta(vehiculo->ruta, vehiculo->id_vehiculo);
}

// Función para imprimir la flota de vehículos de una hormiga
void imprimir_flota(struct lista_vehiculos *flota) {
    if (es_Vacia_Lista(flota)) {
        printf("  Flota: VACÍA\n");
        return;
    }
    
    printf("  Flota:\n");
    
    struct nodo_vehiculo *actual = flota->cabeza;
    int contador = 1;
    
    while (actual != NULL) {
        printf("  Vehículo #%d:\n", contador);
        imprimir_vehiculo(actual->vehiculo);
        actual = actual->siguiente;
        contador++;
    }
}

// Función para imprimir la información del array de tabu
void imprimir_tabu(int *tabu, int num_clientes) {
    printf("  Tabu: [");
    for (int i = 0; i < num_clientes; i++) {
        printf("%d", tabu[i]);
        if (i < num_clientes - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

// Función principal para imprimir toda la información de las hormigas
void imprimir_hormigas(struct hormiga *hormigas, struct vrp_configuracion *vrp, int num_hormigas) {
    printf("=================================================\n");
    printf("INFORMACIÓN DE HORMIGAS Y SUS RUTAS\n");
    printf("=================================================\n");
    
    for (int i = 0; i < num_hormigas; i++) {
        printf("\nHORMIGA #%d (ID: %d)\n", i+1, hormigas[i].id_hormiga);
        printf("  Vehículos contados: %d/%d\n", hormigas[i].vehiculos_contados, hormigas[i].vehiculos_maximos);
        printf("  Fitness global: %.2f\n", hormigas[i].fitness_global);
        
        // Imprimir tabu
        imprimir_tabu(hormigas[i].tabu, vrp->num_clientes);
        
        // Imprimir flota de vehículos
        imprimir_flota(hormigas[i].flota);
        
        printf("-------------------------------------------------\n");
    }
    
    printf("=================================================\n");
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
      hormiga[i].tabu[0]=1;

   }
}

void vrp_tw_aco(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_visiblidad, double **instancia_feromona)
{
   struct hormiga *hormiga = asignar_memoria_hormigas(ind);

   // for(int i = 0 ; i< ind->numIteraciones; i++){

   inicializar_hormiga(vrp, ind, hormiga);

   imprimir_hormigas(hormiga, vrp, ind->numHormigas);

   //}
}