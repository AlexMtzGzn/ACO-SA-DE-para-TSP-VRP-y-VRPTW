#ifndef VRP_TW_ACO_H
#define VRP_TW_ACO_H

#include <stdbool.h>
#include "aed.h"
#include "configuracion_vrp_tw.h"
#include "control_memoria.h"
#include "vrp_tw_aco.h"
#include "lista_ruta.h"

struct lista_vehiculos;
struct lista_ruta;
typedef struct vehiculo
{
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
typedef struct hormiga
{
    int id_hormiga;
    int *tabu;
    int tabu_contador;
    double suma_probabilidades;
    double *probabilidades;
    int vehiculos_contados;
    int vehiculos_maximos;
    double fitness_global;
    lista_vehiculos *flota;
} hormiga;

void imprimir_ruta(struct lista_ruta *ruta, int vehiculo_id);
void imprimir_vehiculo(struct vehiculo *vehiculo);
void imprimir_flota(struct lista_vehiculos *flota);
void imprimir_tabu(int *tabu, int num_clientes);
void imprimir_hormigas(struct hormiga *hormigas, struct vrp_configuracion *vrp, int num_hormigas);
void inicializar_hormiga(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga);
double calcular_tiempo_viaje(double distancia);
bool calcular_ruta(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, struct vehiculo *vehiculo, double **instancia_visiblidad, double **instancia_feromona);
bool necesita_nuevo_vehiculo(struct vrp_configuracion *vrp, struct hormiga *hormiga, struct vehiculo * vehiculo);
void aco(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, double **instancia_visiblidad, double **instancia_feromona);
void vrp_tw_aco(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_visiblidad, double **instancia_feromona);

#endif // VRP_TW_ACO_H
