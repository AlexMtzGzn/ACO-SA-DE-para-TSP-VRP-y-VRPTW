import json
import os
import sys
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import time
import os
from matplotlib.patches import Circle

class VRPSimulator:
    def __init__(self, archivo_json, numero_clientes):
        # Cargar los datos desde el archivo JSON
        with open(archivo_json, 'r') as file:
            self.data = json.load(file)
        
        # Guardar el nombre del archivo para usarlo en los títulos
        self.filename = os.path.basename(archivo_json).split('.')[0]
        self.directory = os.path.basename(os.path.dirname(archivo_json))  

        # Guardar el número de clientes
        self.numero_clientes = numero_clientes
        
        # Colores para cada ruta
        self.colors = plt.cm.tab10(np.linspace(0, 1, len(self.data['flota'])))
    
    def plot_static_routes(self):
        """Dibuja todas las rutas de forma estática"""
        fig, ax = plt.subplots(figsize=(12, 10))
        
        # Dibujar cada ruta
        for i, vehiculo in enumerate(self.data['flota']):
            color = self.colors[i]
            ruta = vehiculo['Ruta Coordenadas']
            
            # Extraer coordenadas
            x = [punto['X'] for punto in ruta]
            y = [punto['Y'] for punto in ruta]
            
            # Dibujar ruta
            ax.plot(x, y, '-o', color=color, markersize=5, label=f"Vehículo {vehiculo['Id_vehiculo']}")
            
            # Dibujar el depósito con un símbolo especial
            ax.plot(ruta[0]['X'], ruta[0]['Y'], 'ks', markersize=8)
        
        # Dibujar todos los clientes como puntos
        clientes_x = []
        clientes_y = []
        for vehiculo in self.data['flota']:
            for punto in vehiculo['Ruta Coordenadas'][1:-1]:  # Excluir el depósito
                clientes_x.append(punto['X'])
                clientes_y.append(punto['Y'])
        
        ax.scatter(clientes_x, clientes_y, c='black', s=20, alpha=0.5)
        
        # Añadir etiqueta al depósito
        depot_x, depot_y = self.data['flota'][0]['Ruta Coordenadas'][0]['X'], self.data['flota'][0]['Ruta Coordenadas'][0]['Y']
        ax.annotate('Depósito', (depot_x, depot_y), 
                    xytext=(5, 5), textcoords='offset points', fontsize=12)
        
        # Configurar el gráfico con el nombre del archivo en el título
        ax.set_xlabel('Coordenada X')
        ax.set_ylabel('Coordenada Y')
        ax.set_title(f'Rutas de los Vehículos - {self.filename} ({self.numero_clientes} clientes)')
        ax.legend(loc='upper right', bbox_to_anchor=(1.1, 1))
        ax.grid(True)
        
        plt.tight_layout()
        os.makedirs(f'Resultados/Resultados_{self.numero_clientes}/Imagenes/{self.directory}', exist_ok=True)
        plt.savefig(f'Resultados/Resultados_{self.numero_clientes}/Imagenes/{self.directory}/{self.filename}.png')        
    
    def animate_routes(self):
        """Anima la simulación de rutas"""
        fig, ax = plt.subplots(figsize=(12, 10))
        
        # Almacena las líneas y vehículos para la animación
        route_lines = []
        vehicles = []
        
        # Información de cada ruta
        max_steps = 0
        all_routes = []
        
        # Preparar los datos para la animación
        for i, vehiculo in enumerate(self.data['flota']):
            ruta = vehiculo['Ruta Coordenadas']
            
            # Extraer coordenadas
            x = [punto['X'] for punto in ruta]
            y = [punto['Y'] for punto in ruta]
            
            # Crear línea para la ruta
            line, = ax.plot([], [], '-', color=self.colors[i], alpha=0.6, 
                          label=f"Vehículo {vehiculo['Id_vehiculo']}")
            route_lines.append(line)
            
            # Crear marcador para el vehículo
            vehicle = Circle((x[0], y[0]), 1.5, color=self.colors[i], zorder=3)
            ax.add_patch(vehicle)
            vehicles.append(vehicle)
            
            # Guardar la ruta
            all_routes.append((x, y))
            
            # Actualizar el máximo número de pasos
            max_steps = max(max_steps, len(x))
        
        # Dibujar el depósito
        depot_x, depot_y = self.data['flota'][0]['Ruta Coordenadas'][0]['X'], self.data['flota'][0]['Ruta Coordenadas'][0]['Y']
        ax.plot(depot_x, depot_y, 'ks', markersize=8)
        ax.annotate('Depósito', (depot_x, depot_y), 
                    xytext=(5, 5), textcoords='offset points', fontsize=12)
        
        # Dibujar todos los puntos de clientes
        clientes_x = []
        clientes_y = []
        clientes_id = []
        cliente_counter = 1
        
        for vehiculo in self.data['flota']:
            for j, punto in enumerate(vehiculo['Ruta Coordenadas'][1:-1]):  # Excluir el depósito y el punto final
                clientes_x.append(punto['X'])
                clientes_y.append(punto['Y'])
                # Aquí asumimos que necesitamos asignar IDs a los clientes
                # Usamos el valor de Ruta Clientes o un contador si no está disponible
                if 'Ruta Clientes' in vehiculo and j < len(vehiculo['Ruta Clientes']) - 2:
                    clientes_id.append(vehiculo['Ruta Clientes'][j+1])  # +1 porque el depósito es el índice 0
                else:
                    clientes_id.append(cliente_counter)
                    cliente_counter += 1
        
        ax.scatter(clientes_x, clientes_y, c='black', s=20, alpha=0.5)
        
        # Añadir etiquetas a los clientes
        for i in range(len(clientes_x)):
            ax.annotate(str(clientes_id[i]), (clientes_x[i], clientes_y[i]), 
                       xytext=(3, 3), textcoords='offset points', fontsize=8)
        
        # Configuración del gráfico con el nombre del archivo en el título
        ax.set_xlabel('Coordenada X')
        ax.set_ylabel('Coordenada Y')
        ax.set_title(f'Simulación de Rutas de Vehículos TW - {self.filename} ({self.numero_clientes} clientes)')
        
        # Determinar los límites del gráfico
        x_min = min(min(clientes_x), depot_x) - 5 if clientes_x else depot_x - 5
        x_max = max(max(clientes_x), depot_x) + 5 if clientes_x else depot_x + 5
        y_min = min(min(clientes_y), depot_y) - 5 if clientes_y else depot_y - 5
        y_max = max(max(clientes_y), depot_y) + 5 if clientes_y else depot_y + 5
        
        ax.set_xlim(x_min, x_max)
        ax.set_ylim(y_min, y_max)
        ax.grid(True)
        ax.legend(loc='upper right', bbox_to_anchor=(1.1, 1))
        
        # Información de la simulación
        info_text = ax.text(0.02, 0.98, '', transform=ax.transAxes, fontsize=9,
                          verticalalignment='top', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
        
        # Función para inicializar la animación
        def init():
            for line in route_lines:
                line.set_data([], [])
            return route_lines + vehicles + [info_text]
        
        # Función para actualizar la animación en cada paso
        def update(frame):
            frame_info = f"Paso: {frame+1}/{max_steps}\n"
            
            for i, ((x, y), line, vehicle) in enumerate(zip(all_routes, route_lines, vehicles)):
                if frame < len(x):
                    # Actualizar la línea
                    line.set_data(x[:frame+1], y[:frame+1])
                    
                    # Actualizar la posición del vehículo
                    vehicle.center = (x[frame], y[frame])
                    
                    # Añadir información del vehículo
                    vehiculo = self.data['flota'][i]
                    if frame < len(x) - 1:
                        # Intentar obtener información del cliente actual y siguiente
                        if 'Ruta Clientes' in vehiculo and frame < len(vehiculo['Ruta Clientes']):
                            cliente_actual = vehiculo['Ruta Clientes'][frame]
                            cliente_siguiente = vehiculo['Ruta Clientes'][min(frame+1, len(vehiculo['Ruta Clientes'])-1)]
                        else:
                            # Si no tenemos Ruta Clientes, usamos índices
                            cliente_actual = frame
                            cliente_siguiente = min(frame+1, len(x)-1)
                        
                        frame_info += f"Vehículo {vehiculo['Id_vehiculo']}: Cliente {cliente_actual} → {cliente_siguiente}\n"
            
            info_text.set_text(frame_info)
            return route_lines + vehicles + [info_text]
        
        # Crear la animación
        ani = animation.FuncAnimation(fig, update, frames=max_steps,
                                     init_func=init, blit=True, interval=200)
        
        plt.tight_layout()
        
        os.makedirs(f'Resultados/Resultados_{self.numero_clientes}/Gifs/{self.directory}', exist_ok=True)
        # Guardar la animación como GIF con el nombre del archivo
        ani.save(f'Resultados/Resultados_{self.numero_clientes}/Gifs/{self.directory}/{self.filename}.gif', writer='pillow', fps=5)        

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python simulador_vrp_tw.py <archivo_json> [numero_clientes]")
        sys.exit(1)
    
    # Obtener el número de clientes del segundo parámetro (si existe)
    numero_clientes = sys.argv[2] if len(sys.argv) > 2 else "unknown"
    
    # Construir la ruta del archivo correctamente
    archivo_json = sys.argv[1]
    
    # Si el usuario proporciona la ruta completa, usarla directamente
    if os.path.exists(archivo_json):
        ruta_archivo = archivo_json
    else:
        # Si proporciona solo el nombre o una ruta relativa
        if '/' in archivo_json:
            # Si es algo como "C100_(25)/C100_(25)_0.json"
            ruta_archivo = f"Resultados/Resultados_{numero_clientes}/Json/{archivo_json}"
        else:
            # Si es solo un nombre de archivo
            directorio = archivo_json.split('_')[0] + "_(" + str(numero_clientes) + ")"
            ruta_archivo = f"Resultados/Resultados_{numero_clientes}/Json/{directorio}/{archivo_json}"
    
    # Uso del simulador
    simulator = VRPSimulator(ruta_archivo, numero_clientes)
    
    # Dibujar las rutas estáticas
    simulator.plot_static_routes()
    
    # Animar las rutas
    simulator.animate_routes()