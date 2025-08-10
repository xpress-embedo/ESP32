import cv2
import time
import imutils
from imutils.video import VideoStream
import serial
import serial.tools.list_ports
import paho.mqtt.client as mqtt

# global variables of the project
GREEN_TIME = 10
YELLOW_TIME = 3
# extra time added per car detected
_EXTRA_PER_CAR = 1

# this variable holds the value of the number of cars detected on the side-1
num_of_cars_detected = 0

LINE_POSITION_Y = 75
LINE_IN_X_1 = 300
LINE_IN_X_2 = 600

LINE_OUT_X_1 = 50
LINE_OUT_X_2 = 350
OFFSET = 10

def get_green_time(side_index):
  if side_index == 0: # NORTH side
    return GREEN_TIME + num_of_cars_detected
  else:
    return GREEN_TIME

def generate_message():
  message = "<"
  for i in range(4):
    message += f"{i}:"
    if green[i] :
      message += f"G{green[i]:02}"
    elif yellow[i] :
      message += f"Y{yellow[i]:02}"
    elif red[i]:
      message += f"R{red[i]:02}"
      message += ""
    message += ">"
  print (message)

# Callback Function on Connection with MQTT Server
def on_connect( client, userdata, flags, rc, properties):
    print ("Connected with Code :" +str(rc))
    # Subscribe Topic from here
    # client.subscribe("home/#")

# Callback Function on Receiving the Subscribed Topic/Message
def on_message( client, userdata, msg):
    # print the message received from the subscribed topic
    print ( str(msg.payload) )

# main program starts from here
# if __name__ == "__main__":  
# start point of the project
green = [ GREEN_TIME, 0, 0, 0]
yellow = [0, 0, 0, 0]
red = [ 0, (GREEN_TIME + YELLOW_TIME), (GREEN_TIME + YELLOW_TIME)*2, (GREEN_TIME + YELLOW_TIME)*3 ]

print ("Traffic Light Controller Started")
print ("Open CV Cersion: ", cv2.__version__)

# Get the list of serial ports
ports = serial.tools.list_ports.comports()
for port, desc, hwid in sorted(ports):
    print(f"{port}: {desc} [{hwid}]")

# Select the first available serial port
port = 'COM1'  # Change this to your actual port
baud = 115200
# Configure the serial port
serial_port = serial.Serial(port, baud, timeout=1)
# Open the serial port
if not serial_port.is_open:
    serial_port.open()
    print(f"Serial port {port} opened successfully.")


# Configure MQTT Client (Starts)
client = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message

mqtt_server = "test.mosquitto.org"
mqtt_port = 1883

# in case of username and password, update these fields and un-comment the code
# user_name = "pyptiouq";
# mqtt_pswd = "aQp113ENJeO9";
# client.username_pw_set( user_name, mqtt_pswd );

client.connect( mqtt_server, mqtt_port, 60 );
client.loop_start()
# Configure MQTT Client (Ends)

# Opening the Camera
camera_number = 1 # Change this to your camera number
print (f"Opening camera number: {camera_number}")
# Initialize the video capture
# Use cv2.CAP_DSHOW for Windows to avoid camera issues
cap = cv2.VideoCapture(camera_number, cv2.CAP_DSHOW)
if not cap.isOpened():
    print ("Error: Could not open video.")
    exit()

print ("Camera Sensor Warming Up")
time.sleep(2.0)  # Allow the camera to warm up

# Using OpenCV Cascade Classifier to detect cars
car_cascade = cv2.CascadeClassifier('cars.xml')

while True:

  # Read a frame from the camera
  # todo

  # Generate Message for Serial Port and MQTT
  generate_message()

  # increment time logic
  for idx in range(4):
    # only one time is positive at a time
    if green[idx] and yellow[idx] == 0 and red[idx] == 0:
      green[idx] = green[idx] - 1
      # if green time reaches zero, then we need to reload the yellow time instantly
      if green[idx] == 0:
        yellow[idx] = YELLOW_TIME
    elif yellow[idx] and green[idx] == 0 and red[idx] == 0:
      yellow[idx] = yellow[idx] -1
      # if yellow reaches zero, it means time to switch to the yellow time instantly
      if yellow[idx] == 0:
        red[idx] = (GREEN_TIME + YELLOW_TIME ) * 3
    elif red[idx] and yellow[idx] == 0 and green[idx] == 0:
      red[idx] = red[idx] - 1
      if red[idx] == 0:
        green[idx] = GREEN_TIME
    else:
      print ("Invalid Case Shouldn't Happen")
  
  time.sleep(1)  # Wait for 1 second before the next iteration