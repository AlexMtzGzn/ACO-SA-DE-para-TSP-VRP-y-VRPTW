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
    double capacidad_acumulada;
    double vt_actual; // Timepo Actual, ajustar despues
    double vt_final;    // Tiemo final de la venta
    double vt_inicial;
    double velocidad;
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
    int *posibles_clientes;
    int posibles_clientes_contador;
    double *probabilidades;
    double suma_probabilidades;
    double umbral;
    int vehiculos_necesarios;
    int vehiculos_maximos;
    double fitness_global;
    lista_vehiculos *flota;
} hormiga;

void imprimir_ruta(struct lista_ruta *ruta, int vehiculo_id);
void imprimir_vehiculo(struct vehiculo *vehiculo);
void imprimir_flota(struct lista_vehiculos *flota);
void imprimir_tabu(int *tabu, int num_clientes);
void imprimir_hormigas(struct hormiga *hormigas, struct vrp_configuracion *vrp, int num_hormigas);
void actualizar_feromona(struct individuo *ind, struct hormiga *hormiga, struct vrp_configuracion *vrp, double **instancia_feromona,double delta);
void calcular_fitness(struct hormiga *hormiga, double **instancia_distancias);
void inicializar_hormiga(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga);
void calcular_posibles_clientes(int origen, struct vehiculo *vehiculo, struct vrp_configuracion *vrp, struct hormiga *hormiga, double **instancia_distancias);
double calcular_probabilidad(int origen, int destino, struct individuo *ind, struct vrp_configuracion *vrp, struct hormiga *hormiga, double **instancia_feromona, double **instancia_visibilidad,double **instancia_ventanas_tiempo);
void aco(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, double **instancia_visiblidad, double **instancia_feromona, double **instancia_distancias,double **instancia_ventanas_tiempo);
void liberar_memoria_hormiga(struct hormiga *hormiga, struct individuo *ind);
void reiniciar_hormiga(struct hormiga *hormiga, struct vrp_configuracion *vrp);
void vrp_tw_aco(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_visiblidad, double **instancia_distancias, double **instancia_feromona,double **instancia_ventanas_tiempo);
#endif // VRP_TW_ACO_H
