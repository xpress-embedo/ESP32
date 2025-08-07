import cv2
import time
import imutils
from imutils.video import VideoStream
import serial  #serial communicatio via port
#import argparse
print(cv2.__version__)

LINE_POSITION_Y = 75
LINE_IN_X_1 = 300
LINE_IN_X_2 = 600

LINE_OUT_X_1 = 50
LINE_OUT_X_2 = 350
OFFSET = 10

_CURRENT_GREEN   = "GREEN"
_CURRENT_RED     = "RED"
_CURRENT_YELLOW  = "YELLOW"

current_color   = None
current_side    = 0

_EXTRA_PER_CAR = 1

_GREEN_TIME = 10
_RED_TIME = 10
_YELLOW_TIME = 3


# print alailable serial ports list of computer
import serial.tools.list_ports
ports = serial.tools.list_ports.comports()
for port, desc, hwid in sorted(ports):
     
        print("{}: {}".format(port, desc))

port ='3'

src ="video.mp4"
# src =1
#camera_number = 0

try:
   
    camera_number = int(src)
    print("source is camera")
except:
    camera_number = src
    print("source is video.")

port = port 
baud = 115200
 
#configuring serial communication port
serialPort = serial.Serial(port, baud, timeout=1)
# open the serial port
if serialPort.isOpen():
    print(serialPort.name + ' is open...')

# cascade classifier xml file
cascade_src = 'cars.xml'
#opening camera here
cap = cv2.VideoCapture(camera_number)
print("[INFO] camera sensor warming up...")
time.sleep(2.0)
#using opencv cascade classifier 
car_cascade = cv2.CascadeClassifier(cascade_src)

seconds = 0
seconds_last = 0
tick = False
last_update = 0


while True:
    ret, img = cap.read()
    if not ret:
        continue
   
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    cars = car_cascade.detectMultiScale(gray, 1.1, 1)
    
    car_no = 0
    for (x,y,w,h) in cars:
        if w > 50 and h > 50 and  w < 300 and h < 300:
            car_no += 1
            #print(x,y,w,h)
            cv2.rectangle(img,(x,y),(x+w,y+h),(0,0,255),2)      
    
    side1_extra_time = car_no*_EXTRA_PER_CAR  # side one exxtra time for show 
    
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

    img = cv2.putText(img, "Cars:"+str(car_no)+" seconds: "+str(seconds)[-2:], org, font,  
                   fontScale, color, thickness, cv2.LINE_AA)
    
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
    seconds = int(time.perf_counter())
    
    
    if seconds_last != seconds:
        seconds_last = seconds
        tick = True
        
    
    if tick:
        tick = False
        print("tick: ",seconds)

        if current_color is None:
            current_color = _CURRENT_GREEN
            last_update = seconds
            current_side = 1
            print("SIDE 1 GREEN ","SIDE 2 RED   SIDE 3 RED SIDE 4 RED ")
            serialPort.write("GREEN1 RED2 RED3 RED4\n".encode('ascii'))
            
                    
        if current_color ==  _CURRENT_GREEN and current_side == 1 and (last_update + _GREEN_TIME + side1_extra_time) < seconds:
            #CHANGE TO YELLOW
            current_color = _CURRENT_YELLOW
            last_update = seconds
            print("SIDE 1 GREEN ","SIDE 2 RED   SIDE 3 RED SIDE 4 RED ")
            serialPort.write("GREEN1 RED2 RED3 RED4\n".encode('ascii'))

        if current_color ==  _CURRENT_GREEN and current_side == 1 and (last_update + _YELLOW_TIME) < seconds:
            #CHANGE TO YELLOW
            current_color = _CURRENT_YELLOW
            last_update = seconds
            print("SIDE 1 YELLOW","SIDE 2 YELLOW")
            serialPort.write("YELLOW1 YELLOW2 RED3 RED4\n".encode('ascii'))
            
        
        if current_color ==  _CURRENT_YELLOW and current_side == 1 and last_update + _YELLOW_TIME < seconds:
            #CHANGE TO RED
            current_color = _CURRENT_RED
            last_update = seconds
            print("SIDE 1 RED   ","SIDE 2 GREEN ")
            serialPort.write("RED1 GREEN2 RED3 RED4\n".encode('ascii'))
            
            
        if current_color ==  _CURRENT_RED and current_side == 1 and last_update + _RED_TIME < seconds:
            #CHANGE TO YELLOW
            current_color = _CURRENT_YELLOW
            last_update = seconds
            current_side  = 2
            print("SIDE 2 YELLOW","SIDE 3 YELLOW")
            serialPort.write("RED1 YELLOW2 YELLOW3 RED4\n".encode('ascii'))
            
            
        if current_color ==  _CURRENT_YELLOW and current_side == 2 and last_update + _YELLOW_TIME < seconds:
            #CHANGE TO GREEN
            current_color = _CURRENT_GREEN
            last_update = seconds
            print("SIDE 2 GREEN ","SIDE 3 RED   ")
            serialPort.write("RED1  RED2 GREEN3 RED4\n".encode('ascii'))         

        
        if current_color ==  _CURRENT_GREEN and current_side == 2 and last_update + _GREEN_TIME < seconds:
            #CHANGE TO GREEN
            current_color = _CURRENT_YELLOW
            last_update = seconds
            current_side  = 3
            print("SIDE 1 GREEN ","SIDE 2 RED   ")
            serialPort.write("RED1  YELLOW3 YELLOW4 RED2\n".encode('ascii'))     

          
        if current_color ==  _CURRENT_YELLOW and current_side == 3 and last_update + _YELLOW_TIME < seconds:
            #CHANGE TO GREEN
            current_color = _CURRENT_RED
            last_update = seconds
            print("SIDE 1 GREEN ","SIDE 2 RED   ")
            serialPort.write("RED1  RED2 GREEN4 RED3\n".encode('ascii'))  
        
        if current_color ==  _CURRENT_RED and current_side == 3 and last_update + _RED_TIME < seconds:
            #CHANGE TO GREEN
            current_color = _CURRENT_GREEN
            current_side  = 1
            last_update = seconds
            print("SIDE 1 GREEN ","SIDE 2 RED   ")
            serialPort.write("RED1  YELLOW1 YELLOW4 RED3\n".encode('ascii'))  
    
    if cv2.waitKey(33) == 27:
        break
    
cv2.destroyAllWindows()