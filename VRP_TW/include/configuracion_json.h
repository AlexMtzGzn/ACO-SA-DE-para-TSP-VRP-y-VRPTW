#ifndef CONFIGURACION_JSON_H
#define CONFIGURACION_JSON_H

// Incluimos las bibliotecas necesarias para trabajar con las estructuras de datos del VRP
#include <cjson/cJSON.h>
#include "../include/estructuras.h"

// Declaramos prototipos de funciones que gestionan la configuración del archivo Json del VRP con ventanas de tiempo (VRPTW)

// Función para generar un objeto JSON con las coordenadas de la ruta
cJSON *generar_ruta_coordenadas(lista_ruta *ruta, cliente *clientes);

// Función para convertir un vehículo en formato JSON
cJSON *vehiculo_a_json(vehiculo *v, cliente *clientes);

// Función para convertir un individuo en formato JSON
cJSON *individuo_a_json(individuo *ind, cliente *clientes);

// Función para guardar un archivo JSON con la representación del individuo
void guardar_json_en_archivo(individuo *ind, vrp_configuracion *vrp, char *archivo_instancia);

#endif // CONFIGURACION_JSON_H