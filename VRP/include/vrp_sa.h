#ifndef VRP_SA_H
#define VRP_SA_H

#include <stdbool.h>
#include "../include/estructuras.h"

// Evalúa la función objetivo del individuo para el problema VRP,usando las distancias proporcionadas.
void evaluaFO_SA(struct individuo *ind, struct vrp_configuracion *vrp, double **instancia_distancias);

// Intenta mover un cliente de un vehículo a otro para generar un vecino.
bool moverClienteVehiculo(struct individuo *ind, struct vrp_configuracion *vrp);

// Intenta invertir un segmento de ruta en un vehículo para generar un vecino.
bool invertirSegmentoRuta(struct individuo *ind);

// Intenta intercambiar dos clientes entre rutas para generar un vecino.
bool intercambiarClientes(struct individuo *ind, struct vrp_configuracion *vrp);

// Genera un vecino del individuo actual usando técnicas de movimiento.
void generar_vecino(struct individuo *ind, struct vrp_configuracion *vrp);

// Ejecuta el algoritmo de Recocido Simulado (SA) para optimizar la solución VRP.
void sa(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_distancias);

// Inicializa los parámetros del algoritmo de recocido simulado.
void inicializar_metal(struct individuo *ind);

// Función principal que ejecuta el algoritmo de Recocido Simulado sobre una instancia VRP.
void vrp_sa(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_distancias);

#endif // VRP_SA_H
