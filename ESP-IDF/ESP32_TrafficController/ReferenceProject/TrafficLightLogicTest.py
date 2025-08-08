GREEN_TIME = 10
YELLOW_TIME = 3

# Simulated car detection
num_of_cars_detected = 5 # You can replace this with actual camera input

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

#if __name__ == "__main__":
green = [ GREEN_TIME, 0, 0, 0]
yellow = [0, 0, 0, 0]
red = [ 0, (GREEN_TIME + YELLOW_TIME), (GREEN_TIME + YELLOW_TIME)*2, (GREEN_TIME + YELLOW_TIME)*3 ]

# test for 60 seconds
for i in range (80):
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