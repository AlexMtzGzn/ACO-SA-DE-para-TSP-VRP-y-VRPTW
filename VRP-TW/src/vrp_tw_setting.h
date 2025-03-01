#ifndef VRP_TW_SETTING_H
#define VRP_TW_SETTING_H


struct cliente
{
    int id;
    double cordenada_x;
    double cordenada_y;
    double demanda;
    double timpo_inicial;
    double tiempo_final;
    double servicio;
};


struct vrp_configuracion
{
    int num_clientes;
    int num_vehiculos;
    int num_capacidad;
    struct cliente *clientes; 
};

void leemos_csv(struct vrp_configuracion *vrp, char *archivo_instancia);
void creamos_csv(struct vrp_configuracion *vrp, char *archivo_instancia);
void abrimostxt_creamosxcvs(struct vrp_configuracion * vrp,char * ruta);
struct vrp_configuracion * leer_instancia(char *archivo_instancia);
#endif // VRP_TW_SETTING_H
