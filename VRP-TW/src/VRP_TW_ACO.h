#ifndef VRP_TW_ACO_H
#define VRP_TW_ACO_H


#include "AED.h"
#include "vrp_tw_setting.h"
#include "entrada_salida_datos.h"
#include "control_memoria.h"

typedef struct vehiculo
{
    int id_vehiculo;
    double capacidad;
    double capacidad_restante;
    double timpo_comsumido;
    double timpo_maximo;
    int clientes;
    int *ruta;
};


typedef struct hormiga{
    int *tabu;
    int vehiculos;
    struct vehiculo *vehiculo;
    
};

#endif // VRP_TW_ACO_H
