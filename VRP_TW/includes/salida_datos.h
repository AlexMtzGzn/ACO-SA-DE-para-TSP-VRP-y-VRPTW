#ifndef SALIDA_DATOS_H
#define SALIDA_DATOS_H

#include "../includes/estructuras.h"

void imprimir_instancia(double **matriz_instancia, struct vrp_configuracion *vrp, char *texto_instancia);
void imprimir_ruta(struct lista_ruta *ruta, int vehiculo_id);
void imprimir_vehiculo(struct vehiculo *vehiculo);
void imprimir_flota(struct lista_vehiculos *flota);
void imprimir_tabu(int *tabu, int num_clientes);
void imprimir_hormigas(struct hormiga *hormigas, struct vrp_configuracion *vrp, int num_hormigas);


#endif // SALIDA_DATOS_H
