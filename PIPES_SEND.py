import os
import time
from numpy import random

print("Python Writer: Starting...")

# Open the FIFO for writing
with open('/tmp/python_to_cpp', 'w') as fifo:
    print("Python Writer: Connected to FIFO")

    # Send messages
    for i in range(1, 200):
        x = random.random()*4300
        y = random.random()*1100
        fifo.write(str(x) + " " + str(y) + "\n")
        fifo.flush()  # Important: make sure the data is sent immediately
        time.sleep(0.001)

print("Python Writer: Done")
