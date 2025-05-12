#ifndef CONFIGURACION_TSP_H
#define CONFIGURACION_TSP_H

// Bibliotecas necesarias
#include <cjson/cJSON.h>
// Incluimos el archivo de estructuras necesarias
#include "../include/estructuras.h"

// Declaramos prototipos de funciones que gestionan la configuración de la instancia TSP

// Función para leer una instancia de tsp desde un archivo CSV, llenando la estructura tsp_configuracion
void leemos_csv(tsp_configuracion *tsp, char *archivo_instancia,int tamanio_instancia);

// Función para crear un archivo CSV a partir de los datos de la estructura tsp_configuracion
void creamos_csv(tsp_configuracion *tsp, char *archivo_instancia,int tamanio_instancia);

// Función para leer una instancia de tsp desde un archivo de texto, llenando la estructura tsp_configuracion
void leemos_txt(tsp_configuracion *tsp, char *ruta);

// Función para leer una instancia de tsp desde un archivo de instancia dado, devolviendo un puntero a la configuración TSP
tsp_configuracion *leer_instancia(char *archivo_instancia,int tamanio_instancia);

#endif // CONFIGURACION_TSP_H