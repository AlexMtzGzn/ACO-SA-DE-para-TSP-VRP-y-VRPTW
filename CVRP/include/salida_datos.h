#ifndef SALIDA_DATOS_H
#define SALIDA_DATOS_H

// Incluimos el archivo de estructuras necesarias
#include "../include/estructuras.h"

// Función para imprimir un mensaje de texto que podría contener información sobre la instancia
void imprimir_mensaje(char *texto_instancia);

// Función para imprimir la matriz de distancias de la instancia del problema junto con la configuración del VRP
void imprimir_instancia(double **matriz_instancia, struct vrp_configuracion *vrp, char *texto_instancia);

// Función para imprimir la ruta de un vehículo, identificada por su ID
void imprimir_ruta(struct lista_ruta *ruta, int vehiculo_id);

// Función para imprimir la información de un vehículo en particular
void imprimir_vehiculo(struct vehiculo *vehiculo);

// Función para imprimir la flota de vehículos (lista de vehículos)
void imprimir_flota(struct lista_vehiculos *flota);

// Función para imprimir el arreglo `tabu`, que indica los clientes que han sido visitados por la hormiga
void imprimir_tabu(int *tabu, int num_clientes);

// Función para imprimir el estado de un individuo, que representa una solución en el algoritmo
void imprimir_individuo(individuo *ind);

// Función para imprimir la información de todas las hormigas en el sistema
void imprimir_hormigas(struct hormiga *hormigas, struct vrp_configuracion *vrp, struct individuo * ind);

// Función para imprimir la mejor hormiga encontrada hasta el momento, junto con su correspondiente individuo
void imprimir_mejor_hormiga(struct hormiga *hormiga, struct individuo * ind);

#endif // SALIDA_DATOS_H
