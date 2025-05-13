#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>         
#include <sys/stat.h>       
#include <unistd.h>         
#include <cjson/cJSON.h>    
#include "../include/configuracion_json.h"

// Función para generar un arreglo JSON con las coordenadas de una ruta
cJSON *generar_ruta_coordenadas(lista_ruta *ruta, cliente *clientes)
{
    cJSON *json_ruta = cJSON_CreateArray(); // Creamos un arreglo JSON
    nodo_ruta *actual = ruta->cabeza;

    // Recorremos la ruta y convertimos cada cliente en un objeto JSON con coordenadas
    while (actual)
    {
        cJSON *punto = cJSON_CreateObject(); // Creamos un objeto JSON para cada punto
        cJSON_AddNumberToObject(punto, "X", clientes[actual->cliente].coordenada_x);
        cJSON_AddNumberToObject(punto, "Y", clientes[actual->cliente].coordenada_y);
        cJSON_AddItemToArray(json_ruta, punto); // Lo añadimos al arreglo
        actual = actual->siguiente;
    }

    return json_ruta; // Devolvemos el arreglo
}

// Función para convertir un individuo en un objeto JSON
cJSON *individuo_a_json(individuo *ind, struct tsp_configuracion *tsp, cliente *clientes)
{
    cJSON *json_individuo = cJSON_CreateObject(); // Creamos el objeto JSON del individuo

    // Añadimos los parámetros generales del experimento
    cJSON_AddStringToObject(json_individuo, "Archivo", tsp->archivo_instancia);
    cJSON_AddNumberToObject(json_individuo, "Tiempo Ejecucion en Minutos", tsp->tiempo_ejecucion);
    cJSON_AddNumberToObject(json_individuo, "Poblacion: ", tsp->poblacion);
    cJSON_AddNumberToObject(json_individuo, "Generaciones: ", tsp->generaciones);

    // Añadimos los parámetros del individuo
    cJSON_AddNumberToObject(json_individuo, "Alpha", ind->alpha);
    cJSON_AddNumberToObject(json_individuo, "Beta", ind->beta);
    cJSON_AddNumberToObject(json_individuo, "Rho", ind->rho);
    cJSON_AddNumberToObject(json_individuo, "Numero Hormigas", ind->numHormigas);
    cJSON_AddNumberToObject(json_individuo, "Numero Iteraciones ACO", ind->numIteracionesACO);
    cJSON_AddNumberToObject(json_individuo, "Temperatura Inicial: ", ind->temperatura_inicial);
    cJSON_AddNumberToObject(json_individuo, "Temperatura Final: ", ind->temperatura_final);
    cJSON_AddNumberToObject(json_individuo, "Factor de Enfriamiento: ", ind->factor_enfriamiento);
    cJSON_AddNumberToObject(json_individuo, "Factor de Control: ", ind->factor_control);
    cJSON_AddNumberToObject(json_individuo, "Numero Iteraciones SA: ", ind->numIteracionesSA);
    cJSON_AddNumberToObject(json_individuo, "Fitness Global", ind->fitness);

    // Añadimos la ruta como lista de índices de clientes
    cJSON *ruta_clientes = cJSON_CreateArray();
    nodo_ruta *actual = ind->hormiga->ruta->cabeza;
    while (actual)
    {
        cJSON_AddItemToArray(ruta_clientes, cJSON_CreateNumber(actual->cliente));
        actual = actual->siguiente;
    }
    cJSON_AddItemToObject(json_individuo, "Ruta Clientes", ruta_clientes);

    // Añadimos la ruta como coordenadas reales
    cJSON_AddItemToObject(json_individuo, "Ruta Coordenadas", generar_ruta_coordenadas(ind->hormiga->ruta, clientes));

    return json_individuo; // Retornamos el objeto JSON
}

// Función para contar cuántos archivos JSON hay en un directorio con cierto prefijo
int contar_archivos_json(const char *directorio, const char *prefijo)
{
    int contador = 0;
    DIR *dir = opendir(directorio);
    if (dir == NULL)
        return 0;

    struct dirent *entrada;
    while ((entrada = readdir(dir)) != NULL)
        if (strstr(entrada->d_name, prefijo) && strstr(entrada->d_name, ".json"))
            contador++;

    closedir(dir);
    return contador;
}

// Función que crea un directorio si no existe
void crear_directorio_si_no_existe(const char *ruta)
{
    if (access(ruta, F_OK) != 0)
        mkdir(ruta, 0777);
}

// Función principal que guarda un individuo en un archivo JSON
void guardar_json_en_archivo(individuo *ind, tsp_configuracion *tsp, char *archivo_instancia)
{
    // Convertimos el individuo en JSON
    cJSON *json_individuo = individuo_a_json(ind, tsp, tsp->clientes);
    char *json_string = cJSON_Print(json_individuo); // Obtenemos el string JSON

    // Obtenemos el nombre base de la instancia (antes del primer '_')
    char nombre_instancia[512];
    sscanf(archivo_instancia, "%[^_]_", nombre_instancia);

    // Creamos los directorios necesarios
    char dir_base[256], dir_instancia[1024];
    snprintf(dir_base, sizeof(dir_base), "Resultados/Resultados_%d/Json", (tsp->num_clientes - 1));

    // Comprobamos que la ruta no exceda el tamaño del buffer
    if (snprintf(dir_instancia, sizeof(dir_instancia), "%s/%s", dir_base, nombre_instancia) >= sizeof(dir_instancia))
    {
        fprintf(stderr, "Error: la ruta de la instancia es demasiado larga.\n");
        free(json_string);
        cJSON_Delete(json_individuo);
        return;
    }

    // Creamos los directorios base y específicos si no existen
    crear_directorio_si_no_existe("Resultados");

    char dir_res[256];
    snprintf(dir_res, sizeof(dir_res), "Resultados/Resultados_%d", (tsp->num_clientes - 1));
    crear_directorio_si_no_existe(dir_res);
    crear_directorio_si_no_existe(dir_base);
    crear_directorio_si_no_existe(dir_instancia);

    // Contamos cuántos JSON ya existen para evitar sobreescribir
    int numero = contar_archivos_json(dir_instancia, archivo_instancia);
    numero++; // Incrementamos para el nuevo archivo

    // Construimos la ruta del archivo JSON a guardar
    char ruta[1024];
    if (snprintf(ruta, sizeof(ruta), "%s/%s_%d.json", dir_instancia, archivo_instancia, numero) >= sizeof(ruta))
    {
        fprintf(stderr, "Error: la ruta del archivo JSON es demasiado larga.\n");
        free(json_string);
        cJSON_Delete(json_individuo);
        return;
    }

    // Escribimos el JSON en el archivo
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

    // Liberamos recursos
    free(json_string);
    cJSON_Delete(json_individuo);

    // Ejecutamos el simulador de TSP con el archivo JSON generado
    char comando_py[1200];
    if (snprintf(comando_py, sizeof(comando_py),
                 "python3 src/Simulador_TSP/simulador_tsp.py \"%s\" %d",
                 ruta, tsp->num_clientes - 1) >= sizeof(comando_py))
    {
        fprintf(stderr, "Error: el comando Python es demasiado largo.\n");
        return;
    }

    // Ejecutamos el comando del simulador
    system(comando_py);
}
