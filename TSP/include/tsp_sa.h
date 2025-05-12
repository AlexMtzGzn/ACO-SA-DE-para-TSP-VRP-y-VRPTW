#ifndef TSP_SA_H
#define TSP_SA_H

// Bibliotecas necesarias
#include <stdbool.h>
// Incluimos el archivo de estructuras necesarias
#include "../include/estructuras.h"

// Evalúa la función objetivo para la solución vecina
void evaluaFO_SA(struct individuo *ind, struct tsp_configuracion *tsp, double **instancia_distancias);

bool invertir_segmento_ruta(struct individuo *ind);
// Intercambia dos clientes aleatorios en la solución vecina
bool intercambiar_clientes(struct individuo *ind, struct tsp_configuracion *tsp);

bool mover_cliente_ruta(struct individuo *ind);
// Genera una solución vecina a partir de la solución actual
void generar_vecino(struct individuo *ind);

// Inicializa el metal
void inicializar_metal(struct individuo *ind);

// Ejecuta el algoritmo de Recocido Simulado
void sa(struct tsp_configuracion *tsp, struct individuo *ind, double **instancia_distancias);

// Lógica principal para aplicar SA sobre un individuo
void tsp_sa(struct tsp_configuracion *tsp, struct individuo *ind, double **instancia_distancias);

#endif /*TSP_SA_H*/
