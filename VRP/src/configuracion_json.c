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
        cJSON_AddNumberToObject(punto, "X", clientes[actual->cliente].coordenada_x);
        cJSON_AddNumberToObject(punto, "Y", clientes[actual->cliente].coordenada_y);
        cJSON_AddItemToArray(json_ruta, punto);
        actual = actual->siguiente;
    }
    return json_ruta;
}

// Función para convertir un vehículo en JSON
cJSON *vehiculo_a_json(vehiculo *v, cliente *clientes)
{
    cJSON *json_vehiculo = cJSON_CreateObject();
    cJSON_AddNumberToObject(json_vehiculo, "Id_vehiculo", v->id_vehiculo);
    cJSON_AddNumberToObject(json_vehiculo, "Numero Clientes", v->clientes_contados);
    cJSON_AddNumberToObject(json_vehiculo, "Fitness Vehiculo", v->fitness_vehiculo);

    cJSON *ruta_clientes = cJSON_CreateArray();
    nodo_ruta *actual = v->ruta->cabeza;
    while (actual)
    {
        cJSON_AddItemToArray(ruta_clientes, cJSON_CreateNumber(actual->cliente));
        actual = actual->siguiente;
    }
    cJSON_AddItemToObject(json_vehiculo, "Ruta Clientes", ruta_clientes);
    cJSON_AddItemToObject(json_vehiculo, "Ruta Coordenadas", generar_ruta_coordenadas(v->ruta, clientes));

    return json_vehiculo;
}

// Función para convertir un individuo en JSON
cJSON *individuo_a_json(individuo *ind, struct vrp_configuracion *vrp, cliente *clientes)
{
    cJSON *json_individuo = cJSON_CreateObject();
    cJSON_AddStringToObject(json_individuo, "Archivo", vrp->archivo_instancia);
    cJSON_AddNumberToObject(json_individuo, "Tiempo Ejecucion en Minutos", vrp->tiempo_ejecucion);
    cJSON_AddNumberToObject(json_individuo, "Alpha", ind->alpha);
    cJSON_AddNumberToObject(json_individuo, "Beta", ind->beta);
    cJSON_AddNumberToObject(json_individuo, "Rho", ind->rho);
    cJSON_AddNumberToObject(json_individuo, "Numero Hormigas", ind->numHormigas);
    cJSON_AddNumberToObject(json_individuo, "Numero Iteraciones", ind->numIteraciones);
    cJSON_AddNumberToObject(json_individuo, "Numero de Vehiculos", ind->vehiculos);
    cJSON_AddNumberToObject(json_individuo, "Fitness Global", ind->fitness);

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
    cJSON *json_individuo = individuo_a_json(ind, vrp, vrp->clientes);
    char *json_string = cJSON_Print(json_individuo);

    char ruta[300];
    snprintf(ruta, sizeof(ruta), "Resultados/Resultados_%d/Json/%s.json", (vrp->num_clientes - 1), archivo_instancia);
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

    char comando_py[300];
    snprintf(comando_py, sizeof(comando_py),
             "python3 src/Simulador_VRP/simulador_vrp.py \"%s.json\" %d",
             archivo_instancia, vrp->num_clientes - 1);

    system(comando_py);
}