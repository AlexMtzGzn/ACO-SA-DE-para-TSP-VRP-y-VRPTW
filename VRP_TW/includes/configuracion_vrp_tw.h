#ifndef CONFIGURACION_VRP_TW_H
#define CONFIGURACION_VRP_TW_H

#include "../includes/estructuras.h"

// Declaramos prototipos
void leemos_csv(vrp_configuracion *vrp, char *archivo_instancia);
void creamos_csv(vrp_configuracion *vrp, char *archivo_instancia);
void leemos_txt(vrp_configuracion *vrp, char *ruta);
vrp_configuracion *leer_instancia(char *archivo_instancia);

#endif // CONFIGURACION_VRP_TW_H
