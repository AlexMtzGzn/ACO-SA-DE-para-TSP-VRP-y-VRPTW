#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/control_memoria.h"
#include "../include/estructuras.h"
#include "../include/salida_datos.h"


// Funcion que lee los datos del problema VRP-TW desde un archivo CSV.
void leemos_csv(struct vrp_configuracion *vrp, char *archivo_instancia, int tamanio_instancia)
{
    char ruta[100];
    snprintf(ruta, sizeof(ruta), "Instancias/Instancias_%d/%s.csv", tamanio_instancia, archivo_instancia);

    FILE *archivo = fopen(ruta, "r");
    if (archivo == NULL)
    {
        imprimir_mensaje("Error al abrir el archivo CSV para lectura.");
        return;
    }

    char buffer[256];

    // Leemos la primera línea (nombre de la instancia)
    if (!fgets(buffer, sizeof(buffer), archivo))
    {
        fprintf(stderr, "Error al leer salto de línea en archivo TXT.\n");
        fclose(archivo);
        return;
    }

    // Leemos parámetros de configuración: número de vehículos, capacidad y clientes
    int num_vehiculos, num_capacidad, num_clientes;
    if (fscanf(archivo, "%d, %d, %d\n", &num_vehiculos, &num_capacidad, &num_clientes) != 3)
    {
        imprimir_mensaje("Error al leer los parámetros de configuración del archivo CSV.");
        fclose(archivo);
        return;
    }

    // Asignamos los valores leídos a la estructura
    vrp->num_vehiculos = num_vehiculos;
    vrp->num_capacidad = (double)num_capacidad;
    vrp->num_clientes = num_clientes;

    // Asignamos memoria para los clientes
    vrp->clientes = asignar_memoria_clientes(vrp);
    if (vrp->clientes == NULL)
    {
        fclose(archivo);
        return;
    }

    // Leemos los datos de cada cliente
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

    fclose(archivo); // Cerramos el archivo
}

// Funcion que crea un archivo CSV a partir de los datos del VRP.
void creamos_csv(struct vrp_configuracion *vrp, char *archivo_instancia, int tamanio_instancia)
{
    char ruta[100];
    snprintf(ruta, sizeof(ruta), "Instancias/Instancias_%d/%s.csv", tamanio_instancia, archivo_instancia);

    FILE *archivo = fopen(ruta, "w");
    if (archivo == NULL)
    {
        imprimir_mensaje("Error al abrir el archivo CSV para escritura.");
        return;
    }

    // Escribimos encabezado y parámetros de configuración
    fprintf(archivo, "%s\n%d, %d, %d\n",
            archivo_instancia,
            vrp->num_vehiculos,
            (int)vrp->num_capacidad,
            vrp->num_clientes);

    // Escribimos los datos de cada cliente
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


// Funcion que lee los datos del VRP desde un archivo TXT con formato Solomon.
void leemos_txt(struct vrp_configuracion *vrp, char *ruta)
{
    FILE *archivo = fopen(ruta, "r");
    if (!archivo)
    {
        perror("Error al abrir el archivo");
        return;
    }

    char buffer[256];

    // Buscamos sección de capacidad y número de vehículos
    while (fgets(buffer, sizeof(buffer), archivo))
    {
        if (strstr(buffer, "NUMBER") && strstr(buffer, "CAPACITY"))
        {
            if (fgets(buffer, sizeof(buffer), archivo))
            {
                char *ptr = buffer;
                while (*ptr == ' ' && *ptr != '\0') ptr++;
                sscanf(ptr, "%d %lf", &vrp->num_vehiculos, &vrp->num_capacidad);
                break;
            }
            else
            {
                fprintf(stderr, "Error al leer línea de capacidad.\n");
                fclose(archivo);
                return;
            }
        }
    }

    // Buscamos la sección de datos de los clientes
    while (fgets(buffer, sizeof(buffer), archivo))
    {
        if (strstr(buffer, "CUST NO."))
        {
            if (!fgets(buffer, sizeof(buffer), archivo))  // Saltar encabezado
            {
                fprintf(stderr, "Error al leer encabezado de clientes.\n");
                fclose(archivo);
                return;
            }
            break;
        }
    }

    // Contamos el número de clientes
    int num_clientes = 0;
    long posicion_inicial = ftell(archivo);
    while (fgets(buffer, sizeof(buffer), archivo))
    {
        int id;
        if (sscanf(buffer, "%d", &id) == 1)
            num_clientes++;
    }

    vrp->num_clientes = num_clientes;

    // Asignamos memoria para los clientes
    vrp->clientes = asignar_memoria_clientes(vrp);
    if (vrp->clientes == NULL)
    {
        fclose(archivo);
        return;
    }

    // Volvemos al inicio de la sección de clientes
    fseek(archivo, posicion_inicial, SEEK_SET);

    // Leemos los datos de cada cliente
    int cliente_index = 0;
    while (fgets(buffer, sizeof(buffer), archivo) && cliente_index < vrp->num_clientes)
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

    fclose(archivo);
}


// Funcion que intenta leer una instancia desde CSV o, si no existe, desde TXT (y luego crea el CSV).
struct vrp_configuracion *leer_instancia(char *archivo_instancia, int tamanio_instancia)
{
    char ruta[100];
    struct vrp_configuracion *vrp = asignar_memoria_vrp_configuracion();

    // Inicializamos la estructura
    vrp->num_vehiculos = 0;
    vrp->num_capacidad = 0;
    vrp->num_clientes = 0;
    vrp->clientes = NULL;

    // Intentamos leer el archivo CSV
    snprintf(ruta, sizeof(ruta), "Instancias/Instancias_%d/%s.csv", tamanio_instancia, archivo_instancia);
    FILE *archivo = fopen(ruta, "r");

    if (archivo)
    {
        leemos_csv(vrp, archivo_instancia, tamanio_instancia);
        fclose(archivo);
        if (vrp == NULL || vrp->clientes == NULL)
        {
            liberar_memoria_vrp_configuracion(vrp);
            exit(EXIT_FAILURE);
        }
        return vrp;
    }

    // Intentamos leer desde el archivo TXT si el CSV no existe
    snprintf(ruta, sizeof(ruta), "VRP_Solomon/VRP_Solomon_%d/%s.txt", tamanio_instancia, archivo_instancia);
    archivo = fopen(ruta, "r");

    if (archivo)
    {
        leemos_txt(vrp, ruta);
        creamos_csv(vrp, archivo_instancia, tamanio_instancia); // Convertimos a CSV para próximas ejecuciones
        fclose(archivo);
        if (vrp == NULL || vrp->clientes == NULL)
        {
            liberar_memoria_vrp_configuracion(vrp);
            exit(EXIT_FAILURE);
        }
        return vrp;
    }

    // Si falla tanto el CSV como el TXT, finaliza el programa
    exit(EXIT_FAILURE);
}
