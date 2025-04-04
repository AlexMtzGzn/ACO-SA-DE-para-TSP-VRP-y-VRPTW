#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <cjson/cJSON.h>
#include "../include/configuracion_json.h"

// Función para generar un objeto JSON con las coordenadas de la ruta
cJSON *generar_ruta_coordenadas(lista_ruta *ruta, cliente *clientes)
{
    cJSON *json_ruta = cJSON_CreateArray();
    nodo_ruta *actual = ruta->cabeza;
    while (actual)
    {
        cJSON *punto = cJSON_CreateObject();
        cJSON_AddNumberToObject(punto, "cliente", actual->cliente);
        cJSON_AddNumberToObject(punto, "x", clientes[actual->cliente].coordenada_x);
        cJSON_AddNumberToObject(punto, "y", clientes[actual->cliente].coordenada_y);
        cJSON_AddItemToArray(json_ruta, punto);
        actual = actual->siguiente;
    }
    return json_ruta;
}

// Función para convertir un vehículo en JSON
cJSON *vehiculo_a_json(vehiculo *v, cliente *clientes)
{
    cJSON *json_vehiculo = cJSON_CreateObject();
    cJSON_AddNumberToObject(json_vehiculo, "id_vehiculo", v->id_vehiculo);
    cJSON_AddNumberToObject(json_vehiculo, "capacidad_maxima", v->capacidad_maxima);
    cJSON_AddNumberToObject(json_vehiculo, "capacidad_acumulada", v->capacidad_acumulada);
    cJSON_AddNumberToObject(json_vehiculo, "tiempo_consumido", v->vt_actual);
    cJSON_AddNumberToObject(json_vehiculo, "tiempo_maximo", v->vt_final);
    cJSON_AddNumberToObject(json_vehiculo, "numero_clientes", v->clientes_contados);
    cJSON_AddNumberToObject(json_vehiculo, "fitness_vehiculo", v->fitness_vehiculo);

    cJSON *ruta_clientes = cJSON_CreateArray();
    nodo_ruta *actual = v->ruta->cabeza;
    while (actual)
    {
        cJSON_AddItemToArray(ruta_clientes, cJSON_CreateNumber(actual->cliente));
        actual = actual->siguiente;
    }
    cJSON_AddItemToObject(json_vehiculo, "ruta_clientes", ruta_clientes);

    cJSON_AddItemToObject(json_vehiculo, "ruta_coordenadas", generar_ruta_coordenadas(v->ruta, clientes));

    return json_vehiculo;
}

// Función para convertir un individuo en JSON
cJSON *individuo_a_json(individuo *ind, cliente *clientes)
{
    cJSON *json_individuo = cJSON_CreateObject();
    cJSON_AddNumberToObject(json_individuo, "alpha", ind->alpha);
    cJSON_AddNumberToObject(json_individuo, "beta", ind->beta);
    cJSON_AddNumberToObject(json_individuo, "gamma", ind->gamma);
    cJSON_AddNumberToObject(json_individuo, "rho", ind->rho);
    cJSON_AddNumberToObject(json_individuo, "numHormigas", ind->numHormigas);
    cJSON_AddNumberToObject(json_individuo, "numIteraciones", ind->numIteraciones);
    cJSON_AddNumberToObject(json_individuo, "fitness_global", ind->fitness);

    cJSON *flota_json = cJSON_CreateArray();
    nodo_vehiculo *actual = ind->hormiga->flota->cabeza;
    while (actual)
    {
        cJSON_AddItemToArray(flota_json, vehiculo_a_json(actual->vehiculo, clientes));
        actual = actual->siguiente;
    }
    cJSON_AddItemToObject(json_individuo, "flota", flota_json);

    return json_individuo;
}

// Función para guardar el JSON en un archivo
void guardar_json_en_archivo(individuo *ind, vrp_configuracion *vrp, char *archivo_instancia)
{
    cJSON *json_individuo = individuo_a_json(ind, vrp->clientes);
    char *json_string = cJSON_Print(json_individuo);

    char ruta[150];
    snprintf(ruta, sizeof(ruta), "Resultados/%s.json", archivo_instancia);

    int contador = 1;
    struct stat buffer;
    while (stat(ruta, &buffer) == 0)
    {
        snprintf(ruta, sizeof(ruta), "Resultados/%s_%d.json", archivo_instancia, contador);
        contador++;
    }

    // Crear el archivo con el nombre final
    FILE *archivo = fopen(ruta, "w");
    if (archivo)
    {
        fprintf(archivo, "%s", json_string);
        fclose(archivo);
    }
    else
    {
        printf("Error al abrir el archivo para escritura: %s\n", ruta);
    }

    free(json_string);
    cJSON_Delete(json_individuo);
}