#ifndef VRP_TW_ACO_H
#define VRP_TW_ACO_H

#include "aed.h"
#include "configuracion_vrp_tw.h"
#include "control_memoria.h"
#include "vrp_tw_aco.h"
#include "lista_ruta.h"
#include "lista_flota.h"


struct lista_vehiculos;
struct lista_ruta;
typedef struct vehiculo {
    int id_vehiculo;
    double capacidad_maxima;
    double capacidad_restante;
    double tiempo_consumido;
    double tiempo_maximo;
    int clientes_contados;
    lista_ruta *ruta;  
    double fitness_vehiculo;
} vehiculo;

#include "lista_flota.h"
typedef struct hormiga {
    int id_hormiga;
    int *tabu;
    double suma_probabilidades;
    double *probabilidades;
    int vehiculos_contados;
    int vehiculos_maximos;
    double fitness_global;
    lista_vehiculos *flota; 
} hormiga;

void inicializar_hormiga(struct vrp_configuracion *vrp, struct individuo * ind, struct hormiga * hormiga);
void vrp_tw_aco(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_visiblidad, double **instancia_feromona);

#endif // VRP_TW_ACO_H
