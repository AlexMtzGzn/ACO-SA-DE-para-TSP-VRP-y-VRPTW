#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
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
        cJSON_AddNumberToObject(punto, "Capacidad", clientes[actual->cliente].demanda_capacidad);
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
    cJSON_AddNumberToObject(json_vehiculo, "Capacidad Maxima", v->capacidad_maxima);
    cJSON_AddNumberToObject(json_vehiculo, "Capacidad Acumulada", v->capacidad_acumulada);
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
    cJSON_AddNumberToObject(json_individuo, "Poblacion: ", vrp->poblacion);
    cJSON_AddNumberToObject(json_individuo, "Generaciones: ", vrp->generaciones);
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

void crear_directorio_si_no_existe(const char *ruta)
{
    if (access(ruta, F_OK) != 0)
        mkdir(ruta, 0777);
}

// Función para leer una instancia desde archivo CSV o TXT
void guardar_json_en_archivo(individuo *ind, vrp_configuracion *vrp, char *archivo_instancia) {
    cJSON *json_individuo = individuo_a_json(ind, vrp, vrp->clientes);
    char *json_string = cJSON_Print(json_individuo);

    char nombre_instancia[512];
    sscanf(archivo_instancia, "%[^_]_", nombre_instancia);

    // Crear los directorios necesarios
    char dir_base[256], dir_instancia[1024];
    snprintf(dir_base, sizeof(dir_base), "Resultados/Resultados_%d/Json", (vrp->num_clientes - 1));
    
    // Verificamos que no se exceda el tamaño del buffer
    if (snprintf(dir_instancia, sizeof(dir_instancia), "%s/%s", dir_base, nombre_instancia) >= sizeof(dir_instancia)) {
        fprintf(stderr, "Error: la ruta de la instancia es demasiado larga.\n");
        free(json_string);
        cJSON_Delete(json_individuo);
        return;
    }

    crear_directorio_si_no_existe("Resultados");

    char dir_res[256];
    snprintf(dir_res, sizeof(dir_res), "Resultados/Resultados_%d", (vrp->num_clientes - 1));
    crear_directorio_si_no_existe(dir_res);
    crear_directorio_si_no_existe(dir_base);
    crear_directorio_si_no_existe(dir_instancia);

    // Contar cuántos archivos existen ya con ese prefijo
    int numero = contar_archivos_json(dir_instancia, archivo_instancia);
    numero++;

    // Ruta final del archivo JSON
    char ruta[1024];
    if (snprintf(ruta, sizeof(ruta), "%s/%s_%d.json", dir_instancia, archivo_instancia, numero) >= sizeof(ruta)) {
        fprintf(stderr, "Error: la ruta del archivo JSON es demasiado larga.\n");
        free(json_string);
        cJSON_Delete(json_individuo);
        return;
    }

    // Abrir el archivo y escribir el JSON
    FILE *archivo = fopen(ruta, "w");
    if (archivo) {
        fprintf(archivo, "%s", json_string);
        fclose(archivo);
    } else {
        printf("Error al abrir el archivo para escritura: %s\n", ruta);
    }

    free(json_string);
    cJSON_Delete(json_individuo);

    // Ejecutar el script de simulación en Python
    char comando_py[1200]; 
    if (snprintf(comando_py, sizeof(comando_py),
                 "python3 src/Simulador_VRP/simulador_vrp.py \"%s\" %d",
                 ruta, vrp->num_clientes - 1) >= sizeof(comando_py)) {
        fprintf(stderr, "Error: el comando Python es demasiado largo.\n");
        return;
    }

    system(comando_py);
}
