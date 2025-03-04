#ifndef VRP_TW_ACO_H
#define VRP_TW_ACO_H

#include "aed.h"
#include "configuracion_vrp_tw.h"
#include "entrada_salida_datos.h"
#include "control_memoria.h"

typedef struct vehiculo {
    int id_vehiculo;
    double capacidad;
    double capacidad_restante;
    double tiempo_consumido;
    double tiempo_maximo;
    int clientes_contados;
    int *ruta;
} vehiculo;

typedef struct hormiga {
    int id_hormiga;
    int *tabu;
    int vehiculos_contados;
    int vehiculos_maximos;
    vehiculo *flota;  
} hormiga;

void inicializar_hormigas_vehiculos(vrp_configuracion * vrp, individuo *ind, hormiga * hormiga);
void vrp_tw_aco(vrp_configuracion * vrp,individuo *ind, double ** instancia_visiblidad, double ** instancia_feromona);

#endif // VRP_TW_ACO_H
