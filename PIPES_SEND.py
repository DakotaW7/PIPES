import os
import time
import numpy.random as random

print("Python: Starting...")

time.sleep(1)

# Open FIFOs
cmd_fifo = "/tmp/cpp_to_python"
resp_fifo = "/tmp/python_to_cpp"

print("Python: Opening {} for reading...".format(cmd_fifo))
cpp_to_python_fifo = open(cmd_fifo, 'r')  # Read commands from C++
print("Python: Opened {} for reading".format(cmd_fifo))

print("Python: Opening {} for writing...".format(resp_fifo))
python_to_cpp_fifo = open(resp_fifo, 'w')  # Write responses to C++
print("Python: Opened {} for writing".format(resp_fifo))

try:
    # Process commands for 200 iterations
    for i in range(200):
        print("Python: Waiting for command...")
        command = cpp_to_python_fifo.readline().strip()  # Blocks until command received
        if not command:
            print("Python: Received empty command or EOF, exiting loop")
            break
        print("Python: Received command: {}".format(command))

        if command == "SEND":
            # Generate random coordinates
            x = random.random() * 4300
            y = random.random() * 1100
            # Send coordinates
            print("Python: Sending coordinates: {}, {}".format(x, y))
            python_to_cpp_fifo.write("{} {}\n".format(x, y))
            python_to_cpp_fifo.flush()
        else:
            print("Python: Unexpected command: {}".format(command))

        time.sleep(0.001)  # Small delay to prevent tight looping

except KeyboardInterrupt:
    print("Python: Interrupted by user")

finally:
    print("Python: Closing FIFOs...")
    cpp_to_python_fifo.close()
    python_to_cpp_fifo.close()
    print("Python: Done")
