import tkinter as tk
from tkinter import ttk, messagebox
import paho.mqtt.client as mqtt
import json
import threading
import time

class BalticCreaturesDisplay:
    def __init__(self, root):
        self.root = root
        self.root.title("Baltic Creatures Display - MQTT")
        self.root.geometry("600x400")
        
        self.ROTATION_INTERVAL = 6
        
        self.mqtt_client = None
        self.devices = {}  # {device_name: connected_status}
        self.current_creature_index = 0
        self.creatures = []
        self.is_rotating = False
        self.rotation_thread = None
        
        self.load_creatures()
        self.setup_ui()
    
    def load_creatures(self):
        """Load creatures data from JSON file"""
        try:
            with open('data/baltic-creatures.json', 'r', encoding='utf-8') as f:
                data = json.load(f)
                self.creatures = list(data.values())
                if not self.creatures:
                    messagebox.showerror("Error", "No creatures found in JSON")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to load creatures: {str(e)}")
        
    def setup_ui(self):
        """Create simple UI for MQTT and creature display"""
        main_frame = ttk.Frame(self.root, padding="10")
        main_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # MQTT Configuration Frame
        mqtt_frame = ttk.LabelFrame(main_frame, text="MQTT Configuration", padding="10")
        mqtt_frame.grid(row=0, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=(0, 10))
        
        ttk.Label(mqtt_frame, text="Broker:").grid(row=0, column=0, sticky=tk.W)
        self.broker_var = tk.StringVar(value="localhost")
        ttk.Entry(mqtt_frame, textvariable=self.broker_var, width=25).grid(row=0, column=1, padx=(5, 10))
        
        ttk.Label(mqtt_frame, text="Port:").grid(row=0, column=2, sticky=tk.W)
        self.port_var = tk.StringVar(value="1883")
        ttk.Entry(mqtt_frame, textvariable=self.port_var, width=10).grid(row=0, column=3, padx=(5, 10))
        
        self.connect_btn = ttk.Button(mqtt_frame, text="Connect", command=self.toggle_mqtt_connection)
        self.connect_btn.grid(row=0, column=4, padx=(10, 0))
        
        self.status_label = ttk.Label(mqtt_frame, text="Disconnected", foreground="red")
        self.status_label.grid(row=1, column=0, columnspan=5, pady=(5, 0))
        
        # Devices Frame
        devices_frame = ttk.LabelFrame(main_frame, text="Connected Devices", padding="10")
        devices_frame.grid(row=1, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=(0, 10))
        
        ttk.Label(devices_frame, text="Device IDs (comma-separated):").pack(anchor=tk.W)
        self.devices_var = tk.StringVar(value="esp32_1,esp32_2,esp32_3")
        ttk.Entry(devices_frame, textvariable=self.devices_var, width=50).pack(fill=tk.X, pady=(5, 0))
        
        ttk.Label(devices_frame, text="(e.g., esp32_1, esp32_2, ...). Will create topics: creature/<device_id>", 
                 font=('Arial', 8), foreground='gray').pack(anchor=tk.W)
        
        # Current Creature Display
        creature_frame = ttk.LabelFrame(main_frame, text="Current Creature", padding="10")
        creature_frame.grid(row=2, column=0, columnspan=2, sticky=(tk.W, tk.E, tk.N, tk.S), pady=(0, 10))
        
        self.name_label = ttk.Label(creature_frame, text="", font=('Arial', 14, 'bold'))
        self.name_label.pack(pady=(0, 10))
        
        self.info_text = tk.Text(creature_frame, height=8, width=60, wrap=tk.WORD)
        self.info_text.pack(fill=tk.BOTH, expand=True, pady=(0, 10))
        
        self.rotation_info = ttk.Label(creature_frame, text="", font=('Arial', 9), foreground='gray')
        self.rotation_info.pack(anchor=tk.W)
        
        # Controls Frame
        controls_frame = ttk.LabelFrame(main_frame, text="Controls", padding="10")
        controls_frame.grid(row=3, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=(0, 10))
        
        ttk.Button(controls_frame, text="Start Rotation", command=self.start_rotation).pack(side=tk.LEFT, padx=(0, 5))
        ttk.Button(controls_frame, text="Stop Rotation", command=self.stop_rotation).pack(side=tk.LEFT, padx=(0, 5))
        ttk.Button(controls_frame, text="Send Now", command=self.send_now).pack(side=tk.LEFT)
        
        self.root.columnconfigure(0, weight=1)
        self.root.rowconfigure(0, weight=1)
        main_frame.columnconfigure(0, weight=1)
        main_frame.rowconfigure(2, weight=1)
        
        self.update_creature_display()
        
    def update_creature_display(self):
        """Update the UI with current creature information"""
        if not self.creatures:
            return
        
        creature = self.creatures[self.current_creature_index]
        self.name_label.config(text=creature.get('name', 'N/A'))
        
        info = f"Info: {creature.get('info', 'N/A')}\n\n"
        info += f"Dangers: {creature.get('dangers', creature.get('dangeres', 'N/A'))}"
        
        self.info_text.config(state=tk.NORMAL)
        self.info_text.delete('1.0', tk.END)
        self.info_text.insert('1.0', info)
        self.info_text.config(state=tk.DISABLED)
        
        self.rotation_info.config(
            text=f"Creature {self.current_creature_index + 1} of {len(self.creatures)}"
        )
    
    def on_mqtt_connect(self, client, userdata, flags, rc):
        """MQTT connection callback"""
        if rc == 0:
            self.status_label.config(text="Connected", foreground="green")
            self.root.after(0, lambda: messagebox.showinfo("MQTT", "Connected to broker"))
        else:
            self.status_label.config(text=f"Connection failed (code {rc})", foreground="red")
    
    def on_mqtt_disconnect(self, client, userdata, rc):
        """MQTT disconnection callback"""
        if rc != 0:
            self.status_label.config(text="Disconnected", foreground="red")
    
    def on_mqtt_message(self, client, userdata, msg):
        """MQTT message callback"""
        pass
    
    def toggle_mqtt_connection(self):
        """Connect or disconnect from MQTT broker"""
        if self.mqtt_client is None:
            try:
                broker = self.broker_var.get()
                port = int(self.port_var.get())
                
                self.mqtt_client = mqtt.Client(client_id="creature-display")
                self.mqtt_client.on_connect = self.on_mqtt_connect
                self.mqtt_client.on_disconnect = self.on_mqtt_disconnect
                self.mqtt_client.on_message = self.on_mqtt_message
                
                self.mqtt_client.connect(broker, port, keepalive=60)
                self.mqtt_client.loop_start()
                
                # Parse device IDs
                device_str = self.devices_var.get()
                self.devices = {dev.strip(): False for dev in device_str.split(',')}
                
                self.connect_btn.config(text="Disconnect")
                
            except Exception as e:
                messagebox.showerror("MQTT Error", f"Failed to connect: {str(e)}")
                self.mqtt_client = None
        else:
            try:
                self.mqtt_client.loop_stop()
                self.mqtt_client.disconnect()
                self.mqtt_client = None
                self.connect_btn.config(text="Connect")
                self.status_label.config(text="Disconnected", foreground="red")
            except Exception as e:
                messagebox.showerror("Error", f"Failed to disconnect: {str(e)}")
    
    def send_now(self):
        """Send current creature to all devices immediately"""
        if not self.mqtt_client:
            messagebox.showerror("Error", "Not connected to MQTT broker")
            return
        
        if not self.creatures:
            messagebox.showerror("Error", "No creatures loaded")
            return
        
        creature = self.creatures[self.current_creature_index]
        payload = json.dumps(creature)
        
        try:
            for device_id in self.devices.keys():
                topic = f"creature/{device_id}"
                self.mqtt_client.publish(topic, payload)
            messagebox.showinfo("Success", f"Sent to {len(self.devices)} device(s)")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to send: {str(e)}")
    
    def rotate_creatures(self):
        """Rotate to next creature every n minutes"""
        while self.is_rotating:
            time.sleep(self.ROTATION_INTERVAL)
            if self.is_rotating:
                self.current_creature_index = (self.current_creature_index + 1) % len(self.creatures)
                self.root.after(0, self.update_creature_display)
                if self.mqtt_client:
                    self.root.after(0, self.send_now)
    
    def start_rotation(self):
        """Start automatic rotation"""
        if self.is_rotating:
            messagebox.showinfo("Info", "Rotation already running")
            return
        
        if not self.creatures:
            messagebox.showerror("Error", "No creatures loaded")
            return
        
        self.is_rotating = True
        self.rotation_thread = threading.Thread(target=self.rotate_creatures, daemon=True)
        self.rotation_thread.start()
        messagebox.showinfo("Info", f"Rotation started - creatures will change every {self.ROTATION_INTERVAL} seconds")
    
    def stop_rotation(self):
        """Stop automatic rotation"""
        self.is_rotating = False
        messagebox.showinfo("Info", "Rotation stopped")

if __name__ == "__main__":
    root = tk.Tk()
    app = BalticCreaturesDisplay(root)
    root.mainloop()
