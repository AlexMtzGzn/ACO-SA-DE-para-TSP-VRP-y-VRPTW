#ifndef VRP_TW_ACO_H
#define VRP_TW_ACO_H

#include "aed.h"
#include "configuracion_vrp_tw.h"
#include "control_memoria.h"

typedef struct vehiculo
{
    int id_vehiculo;
    double capacidad;
    double capacidad_restante;
    double tiempo_consumido;
    double tiempo_maximo;
    int clientes_contados;
    int *ruta;
    double fitness_vehiculo;

} vehiculo;

typedef struct hormiga
{
    int id_hormiga;
    int *tabu;
    double suma_probabilidades;
    double *probabilidades;
    int vehiculos_contados;
    int vehiculos_maximos;
    double fitness_global;
    struct vehiculo *flota;
} hormiga;

// void imprimir_info_hormiga(struct hormiga *h, int numHormiga);
double calcular_tiempo_viaje(double distancia);
void agregar_cliente_a_ruta(struct hormiga *hormiga, struct vehiculo *vehiculo, int nuevo_cliente);
void inicializar_vehiculo(struct hormiga *hormiga, struct vrp_configuracion *vrp);
bool necesita_nuevo_vehiculo(struct vrp_configuracion *vrp, struct hormiga *hormiga);
bool calcular_ruta(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga,struct vehiculo * vehiculo, double **instancia_visiblidad, double **instancia_feromona);
void aco_principal(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, double **instancia_visiblidad, double **instancia_feromona);
void inicializar_hormiga(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga);
void vrp_tw_aco(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_visiblidad, double **instancia_feromona);

#endif // VRP_TW_ACO_H
