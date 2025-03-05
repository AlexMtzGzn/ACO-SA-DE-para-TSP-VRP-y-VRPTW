#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "aed.h"
#include "configuracion_vrp_tw.h"
#include "vrp_tw_aco.h"
#include "control_memoria.h"

/*void imprimir_info_hormiga(struct hormiga *hormiga, int numHormiga)
{
   printf("Hormiga %d:\n", hormiga->id_hormiga);
   printf("  Número de vehículos usados: %d\n", hormiga->vehiculos_contados);
   printf("  Número de vehículos maximos: %d\n", hormiga->vehiculos_maximos);

   for (int i = 0; i < hormiga->vehiculos_contados; i++)
   {
      printf("  Vehículo %d:\n", hormiga->flota[i].id_vehiculo);
      printf("    Capacidad: %.2lf\n", hormiga->flota[i].capacidad);
      printf("    Capacidad restante: %.2lf\n", hormiga->flota[i].capacidad_restante);
      printf("    Tiempo consumido: %.2lf\n", hormiga->flota[i].tiempo_consumido);
      printf("    Tiempo maximo: %.2lf\n", hormiga->flota[i].tiempo_maximo);
      printf("    Clientes atendidos: %d\n", hormiga->flota[i].clientes_contados);
      printf("    Ruta: ");

      for (int j = 0; j < hormiga->flota[i].clientes_contados; j++)
      {
         printf("%d ", hormiga->flota[i].ruta[j]);
      }
      printf("\n");
   }
}*/

void actualizar_feromona(struct individuo *ind, struct vrp_configuracion *vrp, struct hormiga *hormiga, double **instancia_feromona)
{
   double **delta_feromona = asignar_memoria_instancia(vrp->num_clientes);

   for (int i = 0; i < hormiga->vehiculos_contados; i++)
   {
      double delta = 1.0 / hormiga->flota[i].fitness_vehiculo;

      for (int j = 0; j < vrp->num_clientes - 1; j++)
      {
         int cliente_actual = hormiga->flota->ruta[j];
         int cliente_siguiente = hormiga->flota->ruta[j + 1];
         delta_feromona[cliente_actual][cliente_siguiente] += delta;
         delta_feromona[cliente_siguiente][cliente_actual] += delta;
      }
   }

   for (int i = 0; i < vrp->num_clientes; i++)
   {
      for (int j = 0; j < vrp->num_clientes; j++)
      {
         if (i != j)
         {
            instancia_feromona[i][j] = (1.0 - ind->rho) * instancia_feromona[i][j] +
                                       delta_feromona[i][j];
            instancia_feromona[j][i] = (1.0 - ind->rho) * instancia_feromona[j][i] +
                                       delta_feromona[j][i];
         }
      }
   }

   // Hay que liberar la isntancia delta_feromona
}

double calcular_tiempo_viaje(double distancia)
{
   int velocidad = 1; // Pendiente
   return distancia / velocidad;
}
void agregar_cliente_a_ruta(struct hormiga *hormiga, struct vehiculo *vehiculo, int nuevo_cliente)
{
   vehiculo->clientes_contados++;                                                        // Incrementamos un cliente
   vehiculo->ruta = reasignar_memoria_ruta(vehiculo->ruta, vehiculo->clientes_contados); // Reasiganamos la memoria de la ruta
   vehiculo->ruta[vehiculo->clientes_contados - 1] = nuevo_cliente;                      // Agreamos al cliente al a la ruta
   hormiga->tabu[nuevo_cliente] = 1;                                                     // Agreamos a la lista tabu el cliente
}

void inicializar_vehiculo(struct hormiga *hormiga, struct vrp_configuracion *vrp)
{
   hormiga->flota[hormiga->vehiculos_contados - 1].id_vehiculo = hormiga->vehiculos_contados;                       // Inicializmos id_vehiculo con el valor vehiculos contados de la hormiga
   hormiga->flota[hormiga->vehiculos_contados - 1].capacidad = vrp->num_capacidad;                                  // Inicializamos la capacidad
   hormiga->flota[hormiga->vehiculos_contados - 1].capacidad_restante = vrp->num_capacidad;                         // Inicializamos la capacidad restante con la capacidad
   hormiga->flota[hormiga->vehiculos_contados - 1].tiempo_consumido = 0.0;                                          // Inicializmos el tiempo consumido en 0.0
   hormiga->flota[hormiga->vehiculos_contados - 1].tiempo_maximo = vrp->clientes[0].tiempo_final;                   // Inicialiamos el tiempo final
   hormiga->flota[hormiga->vehiculos_contados - 1].clientes_contados = 0;                                           // Inicializamos los clientes en 0
   hormiga->flota[hormiga->vehiculos_contados - 1].ruta = NULL;                                                     // Inicializmos laruta en NULL
   hormiga->flota[hormiga->vehiculos_contados - 1].fitness_vehiculo = 0.0;                                          // Inicializamos  el fitness_vehiculo
   agregar_cliente_a_ruta(hormiga, &hormiga->flota[hormiga->vehiculos_contados - 1], vrp->clientes[0].id_cliente); // Enviamos a la funcion agregar_cliente_ruta, la hormiga, y el vehiculo
}
bool calcular_ruta(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, struct vehiculo *vehiculo, double **instancia_visiblidad, double **instancia_feromona)
{
   bool respuesta_agregado = false;

   int origen = vehiculo->ruta[vehiculo->clientes_contados - 1]; // Seleccionamos el último elemento de la ruta del vehiculo y lo asignamos como origen

   for (int i = 0; i < vrp->num_clientes; i++) // Usamos num_clientes en lugar de clientes
      hormiga->probabilidades[i] = 0.0;

   hormiga->suma_probabilidades = 0.0; // Inicializamos en 0.0 la suma de probabilidades

   for (int i = 0; i < vrp->num_clientes; i++) // Iteramos por todos los clientes
   {
      if (hormiga->tabu[i] == 0) // Si el cliente no está en la lista tabu
      {
         int destino = i; // El destino es el índice del cliente, no el valor en tabu
         double distancia_viaje = calcular_distancia(vrp, origen, destino);
         double tiempo_viaje = calcular_tiempo_viaje(distancia_viaje);
         double distancia_viaje_deposito = calcular_distancia(vrp, destino, 0);
         double tiempo_viaje_deposito = calcular_tiempo_viaje(distancia_viaje_deposito);

         // Verificación de ventanas de tiempo y restricciones
         if (vehiculo->tiempo_consumido + tiempo_viaje >= vrp->clientes[destino].tiempo_inicial &&
             vehiculo->tiempo_consumido + tiempo_viaje <= vrp->clientes[destino].tiempo_final)
         {
            // Verificación de capacidad
            if (vehiculo->capacidad_restante - vrp->clientes[destino].demanda >= 0) // Restamos la demanda, no la sumamos
            {
               // Verificación de tiempo máximo
               if (vehiculo->tiempo_consumido + tiempo_viaje + vrp->clientes[destino].servicio + tiempo_viaje_deposito <= vehiculo->tiempo_maximo)
               {
                  double valuacion_tiempo = (vehiculo->tiempo_maximo > 0) ? (1.0 / vehiculo->tiempo_maximo) : 0.0;
                  hormiga->probabilidades[i] = pow(instancia_feromona[origen][destino], ind->alpha) *
                                               pow(instancia_visiblidad[origen][destino], ind->beta) *
                                               pow(valuacion_tiempo, ind->gamma);
                  hormiga->suma_probabilidades += hormiga->probabilidades[i]; // Usamos i, no destino
               }
            }
         }
      }
   }

   if (hormiga->suma_probabilidades > 0.0)
   {
      double aleatorio = (double)rand() / RAND_MAX;
      double prob_acumulada = 0.0;
      for (int j = 0; j < vrp->num_clientes; j++)
      {
         if (hormiga->tabu[j] == 0 && hormiga->probabilidades[j] > 0.0) // Verificamos que sea un cliente factible
         {
            prob_acumulada += hormiga->probabilidades[j] / hormiga->suma_probabilidades;
            if (aleatorio <= prob_acumulada)
            {
               agregar_cliente_a_ruta(hormiga, vehiculo, j); // Agregamos el cliente a la ruta

               // Actualizamos la capacidad restante (restando la demanda)
               vehiculo->capacidad_restante -= vrp->clientes[j].demanda;

               // Actualizamos el tiempo consumido
               double distancia_viaje = calcular_distancia(vrp, origen, j);
               double tiempo_viaje = calcular_tiempo_viaje(distancia_viaje);
               vehiculo->tiempo_consumido += tiempo_viaje + vrp->clientes[j].servicio;

               respuesta_agregado = true;
               break; // Salimos del bucle una vez que hemos agregado un cliente
            }
         }
      }
   }

   return respuesta_agregado; // Devolvemos verdadero si se agregó un cliente, falso en caso contrario
}

int contar_tabu(struct vrp_configuracion *vrp, struct hormiga *hormiga)
{
   int contador = 0;
   for (int i = 0; i < vrp->num_clientes; i++)
   {
      if (hormiga->tabu[i] == 0)
         contador++;
   }
   return contador;
}

void aco_principal(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga, double **instancia_visiblidad, double **instancia_feromona)
{
    int max_intentos = 20; // Número máximo de intentos para agregar un cliente
    int intentos = 0;
    bool cliente_agregado;
    
    // Mientras queden clientes sin visitar
    while (contar_tabu(vrp, hormiga) < vrp->num_clientes - 1) // -1 para excluir el depósito
    {
        // Intentar agregar un cliente
        cliente_agregado = calcular_ruta(vrp, ind, hormiga, &(hormiga->flota[hormiga->vehiculos_contados - 1]), 
                                        instancia_visiblidad, instancia_feromona);
        
        if (cliente_agregado)
        {
            // Reiniciar contador de intentos si se agregó un cliente exitosamente
            intentos = 0;
        }
        else
        {
            intentos++;
            
            // Si después de varios intentos no se puede agregar un cliente,
            // verificar si necesitamos un nuevo vehículo
            if (intentos >= max_intentos || necesita_nuevo_vehiculo(vrp, hormiga))
            {
                // Verificar si hay vehículos disponibles
                if (hormiga->vehiculos_contados >= hormiga->vehiculos_maximos)
                {
                    // No hay más vehículos disponibles, no podemos completar la solución
                    // Marcar solución como infactible o penalizar en el fitness
                    hormiga->fitness_global = 999999.0; // Un valor muy alto como penalización
                    break;
                }
                
                // Agregar un nuevo vehículo
                hormiga->vehiculos_contados++;
                hormiga->flota = redimensionar_memoria_vehiculo(hormiga);
                inicializar_vehiculo(hormiga, vrp);
                intentos = 0; // Reiniciar contador de intentos
            }
        }
    }
    
    // Calcular fitness total de la hormiga
    calcular_fitness_hormiga(vrp, hormiga);
}

void inicializar_hormiga(struct vrp_configuracion *vrp, struct individuo *ind, struct hormiga *hormiga)
{
   // Bucle para iterar todas las hormigas
   for (int i = 0; i < ind->numHormigas; i++)
   {
      hormiga[i].id_hormiga = i + 1;                                                 // Asignamos el id de la hormiga con i + 1
      hormiga[i].tabu = asignar_memoria_arreglo(vrp->num_clientes);                  // Asignamos memoria para el arreglo_tabu
      hormiga[i].probabilidades = asignar_memoria_arreglo_double(vrp->num_clientes); // asiganamos memoria para el arreglo_probabilidades
      for (int j = 0; j < vrp->num_clientes; j++)
         hormiga[i].tabu[j] = 0;                         // LLenamos todas las posiciones del arreglo tabu en 0
      hormiga->suma_probabilidades = 0.0;                // Inicializamos en 0.0 la suma de probabilidades
      hormiga[i].vehiculos_maximos = vrp->num_vehiculos; // Inicializamos cuantos vehiculos disonibles tiene la hormiga para su flota
      hormiga[i].fitness_global = 0.0;                   // Inicializmos en 0.0 el fitness_global en 0.0
      hormiga[i].flota = asignar_memoria_vehiculo(vrp);  // Asignamos memoria para la foto de vehiculos de cada hormiga
      inicializar_vehiculo(&hormiga[i], vrp);            // Enviamos como parametro la hormiga y vrp
   }
}

void vrp_tw_aco(struct vrp_configuracion *vrp, struct individuo *ind, double **instancia_visiblidad, double **instancia_feromona)
{

   for (int i = 0; i < ind->numIteraciones; i++)
   {
      if (i != 0)
         i = 2;

      struct hormiga *hormiga = asignar_memoria_hormiga(ind); // retorna un aputador con la estrutura de hormiga
      inicializar_hormigas_vehiculos(vrp, ind, hormiga);      // Mandamos los parametros vrp, ind ,hormiga para inicializar los datos de la hormiga

      for (int j = 0; j < ind->numHormigas; j++)
         aco_principal(vrp, ind, &hormiga[i], instancia_visiblidad, instancia_feromona); // Se envia como argumento el vrp, ind, solo una hormiga, instancia de visibilidad y la instancia de la feromna

      for (int j = 0; j < ind->numHormigas; j++)
      {
         actualizar_feromona(ind, vrp, &hormiga[j], instancia_feromona); // Ain falta ajustarla
      }
   }
}