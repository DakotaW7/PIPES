import sys
import time


def main():

sys.stdout.reconfigure(line_buffering=True)

counter = 0
while True:
    message = f"Data Packet {counter} from python"
    sys.stdout.write(message + "\n")
    sys.stdout.flush()
    counter += 1
    time.sleep(1)
    
if __name__ =="__main__":main()    