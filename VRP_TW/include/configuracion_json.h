#ifndef CONFIGURACION_JSON_H
#define CONFIGURACION_JSON_H

// Incluimos las bibliotecas necesarias para trabajar con las estructuras de datos del VRP
#include <cjson/cJSON.h>
#include "../include/estructuras.h"

// Declaramos prototipos de funciones que gestionan la configuración del archivo Json del VRP con ventanas de tiempo (VRPTW)

// Funcion para convertir un cliente en formato JSON
cJSON *detalles_clientes_json(struct datos_cliente *datos_cliente, int numClientes);

// Función para generar un objeto JSON con las coordenadas de la ruta
cJSON *generar_ruta_coordenadas(struct lista_ruta *ruta, struct cliente *clientes);

// Función para convertir un vehículo en formato JSON
cJSON *vehiculo_a_json(struct vehiculo *vehiculo, struct cliente *clientes);

// Función para convertir un individuo en formato JSON
cJSON *individuo_a_json(struct individuo *ind, struct vrp_configuracion *vrp, struct cliente *clientes);

// Función para contar el numero de archivos JSON en un directorio
int contar_archivos_json(const char *directorio, const char *prefijo);

// Función para crear un directorio si no existe
void crear_directorio_si_no_existe(const char *ruta);

// Función para guardar un archivo JSON con la representación del individuo
void guardar_json_en_archivo(struct individuo *ind, struct vrp_configuracion *vrp, char *archivo_instancia);

#endif // CONFIGURACION_JSON_H