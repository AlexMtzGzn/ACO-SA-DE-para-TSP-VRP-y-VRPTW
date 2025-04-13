#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
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
int contar_archivos_json(const char *directorio, const char *prefijo) {
    int contador = 0;
    DIR *dir = opendir(directorio);
    if (dir == NULL) return 0;

    struct dirent *entrada;
    while ((entrada = readdir(dir)) != NULL) {
        if (strstr(entrada->d_name, prefijo) && strstr(entrada->d_name, ".json")) {
            contador++;
        }
    }
    closedir(dir);
    return contador;
}

void crear_directorio_si_no_existe(const char *ruta) {
    if (access(ruta, F_OK) != 0) {
        mkdir(ruta, 0777);
    }
}

void guardar_json_en_archivo(individuo *ind, tsp_configuracion *tsp, char *archivo_instancia) {
    cJSON *json_individuo = individuo_a_json(ind, tsp, tsp->clientes);
    char *json_string = cJSON_Print(json_individuo);

    char directorio[512];
    snprintf(directorio, sizeof(directorio), "Resultados/Resultados_%d/Json/%s", 
             tsp->num_clientes - 1, archivo_instancia);

    // Crear directorios si no existen
    char subdir1[256], subdir2[256];
    snprintf(subdir1, sizeof(subdir1), "Resultados/Resultados_%d/Json", tsp->num_clientes - 1);
    crear_directorio_si_no_existe("Resultados");
    snprintf(subdir2, sizeof(subdir2), "Resultados/Resultados_%d", tsp->num_clientes - 1);
    crear_directorio_si_no_existe(subdir2);
    crear_directorio_si_no_existe(subdir1);
    crear_directorio_si_no_existe(directorio);

    // Contar archivos previos
    int numero = contar_archivos_json(directorio, archivo_instancia);

    // Generar nombre del archivo JSON
    char ruta[1024];
    snprintf(ruta, sizeof(ruta), "%s/%s_%d.json", directorio, archivo_instancia, numero);

    FILE *archivo = fopen(ruta, "w");
    if (archivo) {
        fprintf(archivo, "%s", json_string);
        fclose(archivo);
    } else {
        printf("Error al abrir el archivo para escritura: %s\n", ruta);
    }

    free(json_string);
    cJSON_Delete(json_individuo);

    
    char comando_py[1200]; 
    snprintf(comando_py, sizeof(comando_py),
             "python3 src/Simulador_TSP/simulador_tsp.py \"%s\" %d",
             ruta, tsp->num_clientes - 1);

    system(comando_py);
}