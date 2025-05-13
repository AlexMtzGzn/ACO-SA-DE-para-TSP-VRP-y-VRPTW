#ifndef CONFIGURACION_VRP_H
#define CONFIGURACION_VRP_H

// Incluimos las bibliotecas necesarias
#include <cjson/cJSON.h>
#include "../include/estructuras.h"

// Función para leer una instancia de VRP desde un archivo CSV, llenando la estructura vrp_configuracion
void leemos_csv(vrp_configuracion *vrp, char *archivo_instancia,int tamanio_instancia);

// Función para crear un archivo CSV a partir de los datos de la estructura vrp_configuracion
void creamos_csv(vrp_configuracion *vrp, char *archivo_instancia,int tamanio_instancia);

// Función para leer una instancia de VRP desde un archivo de texto, llenando la estructura vrp_configuracion
void leemos_txt(vrp_configuracion *vrp, char *ruta);

// Función para leer una instancia de VRP desde un archivo de instancia dado, devolviendo un puntero a la configuración VRP
vrp_configuracion *leer_instancia(char *archivo_instancia,int tamanio_instancia);

#endif // CONFIGURACION_VRP_H