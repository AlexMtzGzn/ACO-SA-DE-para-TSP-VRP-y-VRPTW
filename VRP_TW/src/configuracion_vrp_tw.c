#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/cJSON_Utils.h"
#include "../includes/control_memoria.h"
#include "../includes/configuracion_vrp_tw.h"
#include "../includes/estructuras.h"




void leemos_csv(struct vrp_configuracion *vrp, char *archivo_instancia)
{
    char ruta[100];
    snprintf(ruta, sizeof(ruta), "Instancias/%s.csv", archivo_instancia);

    FILE *archivo = fopen(ruta, "r");
    if (archivo == NULL)
    {
        printf("Error al abrir el archivo CSV para lectura.\n");
        return;
    }

    char buffer[256];
    fgets(buffer, sizeof(buffer), archivo);

    int num_vehiculos, num_capacidad, num_clientes;
    if (fscanf(archivo, "%d, %d, %d\n", &num_vehiculos, &num_capacidad, &num_clientes) != 3)
    {
        printf("Error al leer los parámetros de configuración del archivo CSV.\n");
        fclose(archivo);
        return;
    }

    vrp->num_vehiculos = num_vehiculos;
    vrp->num_capacidad = num_capacidad;
    vrp->num_clientes = num_clientes;

    vrp->clientes = asignar_memoria_clientes(vrp);
    if (vrp->clientes == NULL)
    {
        printf("Error al asignar memoria para los clientes.\n");
        fclose(archivo);
        return;
    }

    int cliente_index = 0;
    while (fgets(buffer, sizeof(buffer), archivo) && cliente_index < vrp->num_clientes)
    {
        int id;
        double x, y, demanda, inicio, fin, servicio;

        if (sscanf(buffer, "%d, %lf, %lf, %lf, %lf, %lf, %lf",
                   &id, &x, &y, &demanda, &inicio, &fin, &servicio) == 7)
        {
            vrp->clientes[cliente_index].id_cliente = id;
            vrp->clientes[cliente_index].coordenada_x = x;
            vrp->clientes[cliente_index].coordenada_y = y;
            vrp->clientes[cliente_index].demanda_capacidad = demanda;
            vrp->clientes[cliente_index].vt_inicial = inicio;
            vrp->clientes[cliente_index].vt_final = fin;
            vrp->clientes[cliente_index].tiempo_servicio = servicio;

            cliente_index++;
        }
    }

    fclose(archivo);
}

void creamos_csv(struct vrp_configuracion *vrp, char *archivo_instancia)
{
    char ruta[100];
    snprintf(ruta, sizeof(ruta), "Instancias/%s.csv", archivo_instancia);

    FILE *archivo = fopen(ruta, "w");

    if (archivo == NULL)
    {
        printf("Error al abrir el archivo CSV para escritura.\n");
        return;
    }

    fprintf(archivo, "%s\n%d, %d, %d\n",
            archivo_instancia,
            vrp->num_vehiculos,
            vrp->num_capacidad,
            vrp->num_clientes);

    for (int i = 0; i < vrp->num_clientes; i++)
    {
        fprintf(archivo, "%d, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf\n",
                vrp->clientes[i].id_cliente,
                vrp->clientes[i].coordenada_x,
                vrp->clientes[i].coordenada_y,
                vrp->clientes[i].demanda_capacidad,
                vrp->clientes[i].vt_inicial,
                vrp->clientes[i].vt_final,
                vrp->clientes[i].tiempo_servicio);
    }

    fclose(archivo);
}

void leemos_txt(struct vrp_configuracion *vrp, char *ruta)
{
    FILE *file = fopen(ruta, "r");
    if (!file)
    {
        perror("Error al abrir el archivo");
        return;
    }

    char buffer[256];

    while (fgets(buffer, sizeof(buffer), file))
    {
        if (strstr(buffer, "NUMBER") && strstr(buffer, "CAPACITY"))
        {
            if (fgets(buffer, sizeof(buffer), file))
            {
                char *ptr = buffer;
                while (*ptr == ' ' && *ptr != '\0')
                    ptr++;
                sscanf(ptr, "%d %d", &vrp->num_vehiculos, &vrp->num_capacidad);
                break;
            }
        }
    }

    while (fgets(buffer, sizeof(buffer), file))
    {
        if (strstr(buffer, "CUST NO."))
        {
            fgets(buffer, sizeof(buffer), file);
            break;
        }
    }

    int num_clientes = 0;
    long posicion_inicial = ftell(file);
    while (fgets(buffer, sizeof(buffer), file))
    {
        int id;
        if (sscanf(buffer, "%d", &id) == 1)
        {
            num_clientes++;
        }
    }

    vrp->num_clientes = num_clientes;

    vrp->clientes = asignar_memoria_clientes(vrp);
    if (vrp->clientes == NULL)
    {
        printf("Error al asignar memoria para los clientes.\n");
        fclose(file);
        return;
    }

    fseek(file, posicion_inicial, SEEK_SET);

    int cliente_index = 0;
    while (fgets(buffer, sizeof(buffer), file) && cliente_index < vrp->num_clientes)
    {
        int id;
        double x, y, demanda, inicio, fin, servicio;

        if (sscanf(buffer, "%d %lf %lf %lf %lf %lf %lf",
                   &id, &x, &y, &demanda, &inicio, &fin, &servicio) == 7)
        {
            vrp->clientes[cliente_index].id_cliente = id;
            vrp->clientes[cliente_index].coordenada_x = x;
            vrp->clientes[cliente_index].coordenada_y = y;
            vrp->clientes[cliente_index].demanda_capacidad = demanda;
            vrp->clientes[cliente_index].vt_inicial = inicio;
            vrp->clientes[cliente_index].vt_final = fin;
            vrp->clientes[cliente_index].tiempo_servicio = servicio;

            cliente_index++;
        }
    }

    fclose(file);
}

struct vrp_configuracion *leer_instancia(char *archivo_instancia)
{
    char ruta[100];
    struct vrp_configuracion *vrp = asignar_memoria_vrp_configuracion(); // Asiganamos memoria para la estructura vrp_configuracion

    if (vrp == NULL)
    {
        printf("Error al asignar memoria para la configuración VRP.\n");
        return NULL;
    }

    vrp->num_vehiculos = 0; // Inicializamos numero de vehiculos en 0
    vrp->num_capacidad = 0; // Inicializamos la capacidad del vehiculo en 0
    vrp->num_clientes = 0;  // Inicializamos numero de clientes en 0
    vrp->clientes = NULL;   // Inicializamos la estructura vrp_clientes en NULL

    snprintf(ruta, sizeof(ruta), "Instancias/%s.csv", archivo_instancia);
    FILE *archivo = fopen(ruta, "r");

    if (archivo)
    {
        leemos_csv(vrp, archivo_instancia);
        fclose(archivo); 
        return vrp;
    }

    // Si no existe el CSV, intentamos con el TXT
    snprintf(ruta, sizeof(ruta), "VRP_Solomon/%s.txt", archivo_instancia);
    archivo = fopen(ruta, "r");

    if (archivo)
    {
        leemos_txt(vrp, ruta);
        creamos_csv(vrp, archivo_instancia);
        fclose(archivo); 
    }


    return vrp;

}




// Función para generar un objeto JSON con las coordenadas de la ruta
cJSON *generar_ruta_coordenadas(lista_ruta *ruta, cliente *clientes) {
    cJSON *json_ruta = cJSON_CreateArray();
    nodo_ruta *actual = ruta->cabeza;
    while (actual) {
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
cJSON *vehiculo_a_json(vehiculo *v, cliente *clientes) {
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
    while (actual) {
        cJSON_AddItemToArray(ruta_clientes, cJSON_CreateNumber(actual->cliente));
        actual = actual->siguiente;
    }
    cJSON_AddItemToObject(json_vehiculo, "ruta_clientes", ruta_clientes);
    
    cJSON_AddItemToObject(json_vehiculo, "ruta_coordenadas", generar_ruta_coordenadas(v->ruta, clientes));
    
    return json_vehiculo;
}

// Función para convertir un individuo en JSON
cJSON *individuo_a_json(individuo *ind, cliente *clientes) {
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
    while (actual) {
        cJSON_AddItemToArray(flota_json, vehiculo_a_json(actual->vehiculo, clientes));
        actual = actual->siguiente;
    }
    cJSON_AddItemToObject(json_individuo, "flota", flota_json);
    
    return json_individuo;
}

// Función para guardar el JSON en un archivo
void guardar_json_en_archivo(individuo *ind, cliente *clientes, const char *nombre_archivo) {
    cJSON *json_individuo = individuo_a_json(ind, clientes);
    char *json_string = cJSON_Print(json_individuo);
    
    FILE *archivo = fopen(nombre_archivo, "w");
    if (archivo) {
        fprintf(archivo, "%s", json_string);
        fclose(archivo);
    }
    
    free(json_string);
    cJSON_Delete(json_individuo);
}
