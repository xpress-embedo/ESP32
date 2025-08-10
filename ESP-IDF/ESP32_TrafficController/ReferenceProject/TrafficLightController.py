import cv2
import time
import imutils
from imutils.video import VideoStream
import serial
import serial.tools.list_ports
import paho.mqtt.client as mqtt

# global variables of the project
mqtt_client = None
previous_time = 0
run_algorithm = False

GREEN_TIME = 10
YELLOW_TIME = 3
# extra time added per car detected
_EXTRA_PER_CAR = 1

LINE_POSITION_Y = 75
LINE_IN_X_1 = 300
LINE_IN_X_2 = 600

LINE_OUT_X_1 = 50
LINE_OUT_X_2 = 350
OFFSET = 10

def generate_message():
  global mqqt_client
  message = "<"
  for i in range(4):
    message += f"{i}:"
    if green[i] :
      message += f"G{green[i]:02},"
    elif yellow[i] :
      message += f"Y{yellow[i]:02},"
    elif red[i]:
      message += f"R{red[i]:02}"
      # don't add comma for last range
      if i < 3:
        message += ","
  message += ">"
  print (message)
  mqtt_client.publish("TrafficTopic2", message)


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
mqtt_client = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2)
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message

mqtt_server = "test.mosquitto.org"
mqtt_port = 1883

# in case of username and password, update these fields and un-comment the code
# user_name = "pyptiouq";
# mqtt_pswd = "aQp113ENJeO9";
# client.username_pw_set( user_name, mqtt_pswd );

mqtt_client.connect( mqtt_server, mqtt_port, 60 );
mqtt_client.loop_start()
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
  ret, img = cap.read()
  if not ret:
      print("Error: Could not read frame.")
      break
  if img is None:
      print("Error: Frame is None.")
      break
  
  gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
  cars =  car_cascade.detectMultiScale(gray, 1.1, 1)

  car_no = 0
  for (x,y,w,h) in cars:
    if w > 50 and h > 50 and  w < 300 and h < 300:
      car_no += 1
      #print(x,y,w,h)
      cv2.rectangle(img,(x,y),(x+w,y+h),(0,0,255),2)

  side1_extra_time = car_no * _EXTRA_PER_CAR  # side one extra time for show   
  
  # font 
  font = cv2.FONT_HERSHEY_SIMPLEX 
  # org 
  org = (50, 50) 
  # fontScale 
  fontScale = 1
  # Blue color in BGR 
  color = (255, 0, 0) 
  # Line thickness of 2 px 
  thickness = 2

  img = cv2.putText(img, "Cars Detected: "+str(car_no), org, font, fontScale, color, thickness, cv2.LINE_AA)

  ############################## Draw lane in line & offset##########################################
  cv2.line(img, (LINE_IN_X_1, LINE_POSITION_Y),
          (LINE_IN_X_2, LINE_POSITION_Y), (0, 0, 255), 2)
  cv2.line(img, (LINE_IN_X_1, LINE_POSITION_Y+OFFSET),
          (LINE_IN_X_2, LINE_POSITION_Y+OFFSET), (255, 255, 255), 1)
  cv2.line(img, (LINE_IN_X_1, LINE_POSITION_Y-OFFSET),
          (LINE_IN_X_2, LINE_POSITION_Y-OFFSET), (255, 255, 255), 1)

  # Draw lane out line & offset
  cv2.line(img, (LINE_OUT_X_1, LINE_POSITION_Y),
          (LINE_OUT_X_2, LINE_POSITION_Y), (0, 255, 0), 2)
  cv2.line(img, (LINE_OUT_X_1, LINE_POSITION_Y+OFFSET),
          (LINE_OUT_X_2, LINE_POSITION_Y+OFFSET), (255, 255, 255), 1)
  cv2.line(img, (LINE_OUT_X_1, LINE_POSITION_Y-OFFSET),
          (LINE_OUT_X_2, LINE_POSITION_Y-OFFSET), (255, 255, 255), 1)

  cv2.imshow('Stream', img)

  current_time = int( time.perf_counter() )

  if current_time != previous_time:
    previous_time = current_time
    run_algorithm = True

  if run_algorithm == True:
    run_algorithm = False
    # Generate Message for Serial Port and MQTT
    generate_message()

    # increment time logic
    for idx in range(4):
      if idx == 0:
        green[idx] = GREEN_TIME + side1_extra_time

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

  if cv2.waitKey(33) == 27:  # Press 'ESC' to exit
    print("Exiting...")
    break

cv2.destroyAllWindows()
mqtt_client.loop_stop()
mqtt_client.disconnect()