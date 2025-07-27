This Arduino code implements a Home Monitoring System using an ESP32 microcontroller. Here's what it does:
Main Functionality
This is a comprehensive home automation and security system that monitors various environmental conditions and can control home appliances through relays.
Sensors and Monitoring
The system monitors:
Temperature & Humidity (DHT11 sensor)
Gas/CO levels (MQ gas sensor)
Fire detection (flame sensor)
Vibration/movement (vibration sensor)
Rain detection (rain sensor)
Soil moisture (moisture sensor)
Control Features
Relay Control (4 relays):
Can control up to 4 different appliances/devices
Controllable via web interface
Automatically activated based on sensor readings
Additional Controls:
Servo motor - Opens/closes (likely for ventilation)
Buzzer - Audio alerts for emergencies
Safety & Alert System
Automatic Responses:
Gas leak detected → Turns off appliances (relays 1-3), activates buzzer, opens servo, sends email
Fire detected → Activates buzzer, sends email alert
Vibration detected → Sends email notification
Rain detected → Sends email notification
Low soil moisture → Activates water pump (relay 4)
Communication
WiFi Connectivity:
Connects to WiFi network
Creates a web server for remote monitoring
Provides web interface to control relays
Email Notifications:
Sends Gmail alerts for critical events
Different email subjects for different alerts
High priority notifications
Web Interface
The system provides:
Real-time sensor data via JSON API
Web-based relay control
Sensor status monitoring
Key Components Used
ESP32 microcontroller
DHT11 temperature/humidity sensor
MQ gas sensor for gas detection
Flame sensor for fire detection
Vibration sensor for movement detection
Rain sensor for weather monitoring
Soil moisture sensor for plant care
4-channel relay module for appliance control
Servo motor for mechanical control
Buzzer for audio alerts
This is essentially a smart home security and automation system that can detect emergencies, control home appliances, and provide remote monitoring capabilities.
