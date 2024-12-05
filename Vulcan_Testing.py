import can
import time
import logging

# Enable debug-level logging
logging.basicConfig(level=logging.DEBUG)

# Set the appropriate channel and bitrate for the SLCAN device
channel = 'COM3'  # COM port for SLCAN device
bitrate = 500000  # 500 kbps

try:
    # Configure the CAN bus connection
    bus = can.interface.Bus(channel=channel, interface="slcan", bitrate=bitrate)
    print("Listening for CAN messages for 10 seconds...")

    # Set a timeout for receiving messages (in seconds)
    timeout = 10
    end_time = time.time() + timeout

    while time.time() < end_time:
        message = bus.recv(timeout=1)  # 1-second timeout for each recv call
        if message:
            print(f"Received message: {message}")
        else:
            print("No message received. Checking again...")

except Exception as e:
    print(f"Error: {e}")
finally:
    if 'bus' in locals():
        bus.shutdown()
        print("CAN bus connection closed.")
