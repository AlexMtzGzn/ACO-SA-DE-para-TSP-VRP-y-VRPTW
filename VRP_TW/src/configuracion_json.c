#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "../include/configuracion_json.h"

// Crea un arreglo JSON con los detalles de cada cliente en la ruta
cJSON *detalles_clientes_json(struct datos_cliente *datos_cliente, int numClientes)
{
    cJSON *json_detalle = cJSON_CreateArray();

    for (int i = 0; i < numClientes; i++)
    {
        cJSON *detalle = cJSON_CreateObject();
        if (i == 0)
            cJSON_AddStringToObject(detalle, "Cliente", "Depósito");
        else
            cJSON_AddNumberToObject(detalle, "Cliente", datos_cliente[i].cliente);

        // Agregar los campos de tiempo y demanda
        cJSON_AddNumberToObject(detalle, "Demanda", datos_cliente[i].demanda_capacidad);
        cJSON_AddNumberToObject(detalle, "Ventana Inicial", datos_cliente[i].ventana_inicial); 
        cJSON_AddNumberToObject(detalle, "Ventana Final", datos_cliente[i].ventana_final);     
        cJSON_AddNumberToObject(detalle, "Llegada", datos_cliente[i].tiempo_llegada);          
        cJSON_AddNumberToObject(detalle, "Espera", datos_cliente[i].tiempo_espera);            
        cJSON_AddNumberToObject(detalle, "Inicio Servicio", datos_cliente[i].inicio_servicio); 
        cJSON_AddNumberToObject(detalle, "Duración", datos_cliente[i].duracion_servicio);      
        cJSON_AddNumberToObject(detalle, "Salida", datos_cliente[i].tiempo_salida);            

        // Añadir al array
        cJSON_AddItemToArray(json_detalle, detalle);
    }

    return json_detalle;
}


// Convierte la ruta del vehículo en una lista de coordenadas JSON
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


// Convierte un vehículo a un objeto JSON completo con su ruta, detalles y coordenadas
cJSON *vehiculo_a_json(vehiculo *vehiculo, cliente *clientes)
{
    cJSON *json_vehiculo = cJSON_CreateObject();

    // Información general del vehículo
    cJSON_AddNumberToObject(json_vehiculo, "Id_vehiculo", vehiculo->id_vehiculo);
    cJSON_AddNumberToObject(json_vehiculo, "Velocidad", vehiculo->velocidad);
    cJSON_AddNumberToObject(json_vehiculo, "Capacidad Maxima", vehiculo->capacidad_maxima);
    cJSON_AddNumberToObject(json_vehiculo, "Capacidad Acumulada", vehiculo->capacidad_acumulada);
    cJSON_AddNumberToObject(json_vehiculo, "Tiempo Salida", vehiculo->tiempo_salida_vehiculo);
    cJSON_AddNumberToObject(json_vehiculo, "Tiempo Llegada", vehiculo->tiempo_llegada_vehiculo);
    cJSON_AddNumberToObject(json_vehiculo, "Ventana Inicial", vehiculo->vt_inicial);
    cJSON_AddNumberToObject(json_vehiculo, "Ventana Final", vehiculo->vt_final);
    cJSON_AddNumberToObject(json_vehiculo, "Numero Clientes", vehiculo->clientes_contados);
    cJSON_AddNumberToObject(json_vehiculo, "Fitness Vehiculo", vehiculo->fitness_vehiculo);

    // Ruta de clientes (por índice)
    cJSON *ruta_clientes = cJSON_CreateArray();
    nodo_ruta *actual = vehiculo->ruta->cabeza;
    while (actual)
    {
        cJSON_AddItemToArray(ruta_clientes, cJSON_CreateNumber(actual->cliente));
        actual = actual->siguiente;
    }

    // Agrega la información completa al JSON
    cJSON_AddItemToObject(json_vehiculo, "Ruta Clientes", ruta_clientes);
    cJSON_AddItemToObject(json_vehiculo, "Ruta Coordenadas", generar_ruta_coordenadas(vehiculo->ruta, clientes));
    cJSON_AddItemToObject(json_vehiculo, "Detalles Cliente", detalles_clientes_json(vehiculo->datos_cliente, vehiculo->clientes_contados + 2));

    return json_vehiculo;
}


// Convierte un individuo (solución completa) a JSON
cJSON *individuo_a_json(individuo *ind, struct vrp_configuracion *vrp, cliente *clientes)
{
    cJSON *json_individuo = cJSON_CreateObject();

    // Información general del individuo y configuración
    cJSON_AddStringToObject(json_individuo, "Archivo", vrp->archivo_instancia);
    cJSON_AddNumberToObject(json_individuo, "Tiempo Ejecucion en Minutos", vrp->tiempo_ejecucion);
    cJSON_AddNumberToObject(json_individuo, "Poblacion", vrp->poblacion);
    cJSON_AddNumberToObject(json_individuo, "Generaciones", vrp->generaciones);

    // Parámetros del individuo (DE + ACO + SA)
    cJSON_AddNumberToObject(json_individuo, "Alpha", ind->alpha);
    cJSON_AddNumberToObject(json_individuo, "Beta", ind->beta);
    cJSON_AddNumberToObject(json_individuo, "Gamma", ind->gamma);
    cJSON_AddNumberToObject(json_individuo, "Rho", ind->rho);
    cJSON_AddNumberToObject(json_individuo, "Numero Hormigas", ind->numHormigas);
    cJSON_AddNumberToObject(json_individuo, "Porcentaje Hormigas", ind->porcentajeHormigas);
    cJSON_AddNumberToObject(json_individuo, "Numero Iteraciones ACO", ind->numIteracionesACO);
    cJSON_AddNumberToObject(json_individuo, "Temperatura Inicial", ind->temperatura_inicial);
    cJSON_AddNumberToObject(json_individuo, "Temperatura Final", ind->temperatura_final);
    cJSON_AddNumberToObject(json_individuo, "Factor de Enfriamiento", ind->factor_enfriamiento);
    cJSON_AddNumberToObject(json_individuo, "Numero Iteraciones SA", ind->numIteracionesSA);
    cJSON_AddNumberToObject(json_individuo, "Fitness Global", ind->fitness);

    // Serializar la flota del individuo
    cJSON *flota_json = cJSON_CreateArray();
    nodo_vehiculo *actual = ind->hormiga->flota->cabeza;
    while (actual)
    {
        cJSON_AddItemToArray(flota_json, vehiculo_a_json(actual->vehiculo, clientes));
        actual = actual->siguiente;
    }

    cJSON_AddItemToObject(json_individuo, "Flota", flota_json);

    return json_individuo;
}


// Cuenta los archivos JSON en un directorio con un cierto prefijo
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

// Crea un directorio si no existe
void crear_directorio_si_no_existe(const char *ruta)
{
    if (access(ruta, F_OK) != 0)
        if (mkdir(ruta, 0777) != 0)
            perror("Error creando directorio");
    
}

// Genera un archivo JSON con los datos del individuo y ejecuta un script de simulación
void guardar_json_en_archivo(individuo *ind, vrp_configuracion *vrp, char *archivo_instancia)
{
    // Crear JSON del individuo
    cJSON *json_individuo = individuo_a_json(ind, vrp, vrp->clientes);
    char *json_string = cJSON_Print(json_individuo);

    // Extraer nombre base de la instancia
    char nombre_instancia[512];
    sscanf(archivo_instancia, "%[^_]_", nombre_instancia);

    // Construir ruta para resultados
    char dir_base[256], dir_instancia[1024];
    snprintf(dir_base, sizeof(dir_base), "Resultados/Resultados_%d/Json", (vrp->num_clientes - 1));

    // Validar que la ruta de directorio no sea demasiado larga
    if (snprintf(dir_instancia, sizeof(dir_instancia), "%s/%s", dir_base, nombre_instancia) >= sizeof(dir_instancia))
    {
        fprintf(stderr, "Error: la ruta de la instancia es demasiado larga.\n");
        free(json_string);
        cJSON_Delete(json_individuo);
        return;
    }

    // Crear los directorios necesarios
    crear_directorio_si_no_existe("Resultados");
    char dir_res[256];
    snprintf(dir_res, sizeof(dir_res), "Resultados/Resultados_%d", (vrp->num_clientes - 1));
    crear_directorio_si_no_existe(dir_res);
    crear_directorio_si_no_existe(dir_base);
    crear_directorio_si_no_existe(dir_instancia);

    // Contar archivos existentes para enumerar el nuevo
    int numero = contar_archivos_json(dir_instancia, archivo_instancia);
    numero++;

    // Ruta final del archivo JSON
    char ruta[1024];
    if (snprintf(ruta, sizeof(ruta), "%s/%s_%d.json", dir_instancia, archivo_instancia, numero) >= sizeof(ruta))
    {
        fprintf(stderr, "Error: la ruta del archivo JSON es demasiado larga.\n");
        free(json_string);
        cJSON_Delete(json_individuo);
        return;
    }

    // Guardar JSON en archivo
    FILE *archivo = fopen(ruta, "w");
    if (archivo)
    {
        fprintf(archivo, "%s", json_string);
        fclose(archivo);
    }
    else
    {
        fprintf(stderr, "Error al abrir el archivo para escritura: %s\n", ruta);
    }

    free(json_string);
    cJSON_Delete(json_individuo);

    // Ejecutar script Python con el archivo generado
    char comando_py[1200];
    if (snprintf(comando_py, sizeof(comando_py),
                 "python3 src/Simulador_VRP_TW/simulador_vrp_tw.py \"%s\" %d",
                 ruta, vrp->num_clientes - 1) >= sizeof(comando_py))
    {
        fprintf(stderr, "Error: el comando Python es demasiado largo.\n");
        return;
    }

    int ret = system(comando_py);
    if (ret == -1)
        fprintf(stderr, "Error al ejecutar el comando Python\n");
    
}
