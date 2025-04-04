import json
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import time
from matplotlib.patches import Circle

class VRPSimulator:
    def __init__(self, json_file):
        # Cargar los datos desde el archivo JSON
        with open(json_file, 'r') as file:
            self.data = json.load(file)
        
        # Colores para cada ruta
        self.colors = plt.cm.tab10(np.linspace(0, 1, len(self.data['flota'])))
        
    def plot_static_routes(self):
        """Dibuja todas las rutas de forma estática"""
        fig, ax = plt.subplots(figsize=(12, 10))
        
        # Dibujar cada ruta
        for i, vehiculo in enumerate(self.data['flota']):
            color = self.colors[i]
            ruta = vehiculo['ruta_coordenadas']
            
            # Extraer coordenadas
            x = [punto['x'] for punto in ruta]
            y = [punto['y'] for punto in ruta]
            
            # Dibujar ruta
            ax.plot(x, y, '-o', color=color, markersize=5, label=f"Vehículo {vehiculo['id_vehiculo']}")
            
            # Dibujar el depósito con un símbolo especial
            ax.plot(ruta[0]['x'], ruta[0]['y'], 'ks', markersize=8)
        
        # Dibujar todos los clientes como puntos
        clientes_x = []
        clientes_y = []
        for vehiculo in self.data['flota']:
            for punto in vehiculo['ruta_coordenadas'][1:-1]:  # Excluir el depósito
                clientes_x.append(punto['x'])
                clientes_y.append(punto['y'])
        
        ax.scatter(clientes_x, clientes_y, c='black', s=20, alpha=0.5)
        
        # Añadir etiqueta al depósito
        ax.annotate('Depósito', (ruta[0]['x'], ruta[0]['y']), 
                    xytext=(5, 5), textcoords='offset points', fontsize=12)
        
        # Configurar el gráfico
        ax.set_xlabel('Coordenada X')
        ax.set_ylabel('Coordenada Y')
        ax.set_title('Rutas de los Vehículos')
        ax.legend(loc='upper right', bbox_to_anchor=(1.1, 1))
        ax.grid(True)
        
        plt.tight_layout()
        plt.savefig('rutas_vehiculos.png')
        plt.show()
    
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
            ruta = vehiculo['ruta_coordenadas']
            
            # Extraer coordenadas
            x = [punto['x'] for punto in ruta]
            y = [punto['y'] for punto in ruta]
            
            # Crear línea para la ruta
            line, = ax.plot([], [], '-', color=self.colors[i], alpha=0.6, 
                          label=f"Vehículo {vehiculo['id_vehiculo']}")
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
        depot_x, depot_y = self.data['flota'][0]['ruta_coordenadas'][0]['x'], self.data['flota'][0]['ruta_coordenadas'][0]['y']
        ax.plot(depot_x, depot_y, 'ks', markersize=8)
        ax.annotate('Depósito', (depot_x, depot_y), 
                    xytext=(5, 5), textcoords='offset points', fontsize=12)
        
        # Dibujar todos los puntos de clientes
        clientes_x = []
        clientes_y = []
        clientes_id = []
        for vehiculo in self.data['flota']:
            for punto in vehiculo['ruta_coordenadas'][1:-1]:
                clientes_x.append(punto['x'])
                clientes_y.append(punto['y'])
                clientes_id.append(punto['cliente'])
        
        ax.scatter(clientes_x, clientes_y, c='black', s=20, alpha=0.5)
        
        # Añadir etiquetas a los clientes
        for i in range(len(clientes_x)):
            ax.annotate(str(clientes_id[i]), (clientes_x[i], clientes_y[i]), 
                       xytext=(3, 3), textcoords='offset points', fontsize=8)
        
        # Configuración del gráfico
        ax.set_xlabel('Coordenada X')
        ax.set_ylabel('Coordenada Y')
        ax.set_title('Simulación de Rutas de Vehículos')
        
        # Determinar los límites del gráfico
        x_min = min(min(clientes_x), depot_x) - 5
        x_max = max(max(clientes_x), depot_x) + 5
        y_min = min(min(clientes_y), depot_y) - 5
        y_max = max(max(clientes_y), depot_y) + 5
        
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
                        cliente_actual = next((punto['cliente'] for punto in vehiculo['ruta_coordenadas'] if 
                                               punto['x'] == x[frame] and punto['y'] == y[frame]), 'N/A')
                        cliente_siguiente = next((punto['cliente'] for punto in vehiculo['ruta_coordenadas'] if 
                                                 punto['x'] == x[min(frame+1, len(x)-1)] and 
                                                 punto['y'] == y[min(frame+1, len(y)-1)]), 'N/A')
                        
                        frame_info += f"Vehículo {vehiculo['id_vehiculo']}: Cliente {cliente_actual} → {cliente_siguiente}\n"
            
            info_text.set_text(frame_info)
            return route_lines + vehicles + [info_text]
        
        # Crear la animación
        ani = animation.FuncAnimation(fig, update, frames=max_steps,
                                     init_func=init, blit=True, interval=200)
        
        plt.tight_layout()
        
        # Guardar la animación como GIF
        ani.save('simulacion_rutas.gif', writer='pillow', fps=5)
        
        plt.show()
        

if __name__ == "__main__":
    # Uso del simulador
    simulator = VRPSimulator('VRP_TW/Resultados/C103.json')
    
    # Dibujar las rutas estáticas
    simulator.plot_static_routes()
    
    # Animar las rutas
    simulator.animate_routes()