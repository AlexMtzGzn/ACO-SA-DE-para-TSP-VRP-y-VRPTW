#ifndef CONFIGURACION_JSON_H
#define CONFIGURACION_JSON_H

// Incluimos las bibliotecas necesarias para trabajar con las estructuras de datos del VRP
#include <cjson/cJSON.h>
#include "../include/estructuras.h"

// Prototipo de la función para generar un array JSON con las coordenadas de la ruta.
// Recibe una lista de ruta (`ruta`) y una lista de clientes (`clientes`).
// Devuelve un objeto JSON que representa las coordenadas de los clientes en la ruta.
cJSON *generar_ruta_coordenadas(lista_ruta *ruta, cliente *clientes);

// Prototipo de la función para convertir un individuo (con una ruta simple) en un objeto JSON.
// Recibe un puntero a un `individuo`, un puntero a la configuración de TSP (`tsp`),
// y un puntero a la lista de clientes (`clientes`).
// Devuelve un objeto JSON que representa al individuo, incluyendo su ruta y parámetros.
cJSON *individuo_a_json(individuo *ind, struct tsp_configuracion *tsp, cliente *clientes);

// Prototipo de la función para guardar un individuo como un archivo JSON.
// Recibe un puntero a un `individuo`, un puntero a la configuración de TSP (`tsp`),
// y el nombre del archivo de instancia (`archivo_instancia`).
// Guarda el objeto JSON correspondiente en un archivo específico.
void guardar_json_en_archivo(individuo *ind, tsp_configuracion *tsp, char *archivo_instancia);


#endif // CONFIGURACION_JSON_H