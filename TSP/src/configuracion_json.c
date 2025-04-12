#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <cjson/cJSON.h>
#include "../include/configuracion_json.h"

// Función para generar un array JSON con coordenadas de la ruta
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

// Función para convertir un individuo (ruta simple) en JSON
cJSON *individuo_a_json(individuo *ind, struct tsp_configuracion *tsp, cliente *clientes)
{
    cJSON *json_individuo = cJSON_CreateObject();
    cJSON_AddStringToObject(json_individuo, "Archivo", tsp->archivo_instancia);
    cJSON_AddNumberToObject(json_individuo, "Tiempo Ejecucion en Minutos", tsp->tiempo_ejecucion);
    cJSON_AddNumberToObject(json_individuo, "Alpha", ind->alpha);
    cJSON_AddNumberToObject(json_individuo, "Beta", ind->beta);
    cJSON_AddNumberToObject(json_individuo, "Rho", ind->rho);
    cJSON_AddNumberToObject(json_individuo, "Numero Hormigas", ind->numHormigas);
    cJSON_AddNumberToObject(json_individuo, "Numero Iteraciones", ind->numIteraciones);
    cJSON_AddNumberToObject(json_individuo, "Fitness Global", ind->fitness);

    // Ruta como lista de índices de clientes
    cJSON *ruta_clientes = cJSON_CreateArray();
    nodo_ruta *actual = ind->hormiga->ruta->cabeza;
    while (actual)
    {
        cJSON_AddItemToArray(ruta_clientes, cJSON_CreateNumber(actual->cliente));
        actual = actual->siguiente;
    }
    cJSON_AddItemToObject(json_individuo, "Ruta Clientes", ruta_clientes);

    // Ruta como coordenadas
    cJSON_AddItemToObject(json_individuo, "Ruta Coordenadas", generar_ruta_coordenadas(ind->hormiga->ruta, clientes));

    return json_individuo;
}

// Función para guardar el JSON en un archivo
void guardar_json_en_archivo(individuo *ind, tsp_configuracion *tsp, char *archivo_instancia)
{
    cJSON *json_individuo = individuo_a_json(ind, tsp, tsp->clientes);
    char *json_string = cJSON_Print(json_individuo);

    char ruta[300];
    snprintf(ruta, sizeof(ruta), "Resultados/Resultados_%d/Json/%s.json", (tsp->num_clientes - 1), archivo_instancia);
    
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

    // Ejecutar el simulador
    char comando_py[300];
    snprintf(comando_py, sizeof(comando_py),
             "python3 src/Simulador_TSP/simulador_tsp.py \"%s.json\" %d",
             archivo_instancia, tsp->num_clientes - 1);

    system(comando_py);
}
