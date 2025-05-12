#ifndef CONFIGURACION_JSON_H
#define CONFIGURACION_JSON_H

// Bibliotecas necesarias
#include <cjson/cJSON.h>
// Incluimos el archivo de estructuras necesarias
#include "../include/estructuras.h"

// Función para generar un array JSON con las coordenadas de la ruta.
cJSON *generar_ruta_coordenadas(lista_ruta *ruta, cliente *clientes);

// Función para convertir un individuo (con una ruta simple) en un objeto JSON.
cJSON *individuo_a_json(individuo *ind, struct tsp_configuracion *tsp, cliente *clientes);

// Función para contar cuántos archivos JSON existen en un directorio
int contar_archivos_json(const char *directorio, const char *prefijo);

// Función que crea un directorio si no existe.
void crear_directorio_si_no_existe(const char *ruta);

// Función para guardar un individuo como un archivo JSON.
void guardar_json_en_archivo(individuo *ind, tsp_configuracion *tsp, char *archivo_instancia);

#endif // CONFIGURACION_JSON_H