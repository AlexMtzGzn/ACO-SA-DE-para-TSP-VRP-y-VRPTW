#ifndef VRP_TW_SETTING_H
#define VRP_TW_SETTING_H

typedef struct cliente{
    int id;
    double cordenada_x;
    double cordenada_y;
    double demanda;
    double timpo_inicial;  
    double tiempo_final;
    double servicio;
} cliente;

typedef struct vrp_configuracion {
    int num_clientes;
    int num_vehiculos;
    int num_capacidad;
    cliente *clientes; 
} vrp_configuracion;

void leemos_csv(vrp_configuracion *vrp, char *archivo_instancia);
void creamos_csv(vrp_configuracion *vrp, char *archivo_instancia);
void abrimostxt_creamosxcvs(vrp_configuracion *vrp, char *ruta);
vrp_configuracion *leer_instancia(char *archivo_instancia);

#endif // VRP_TW_SETTING_H
