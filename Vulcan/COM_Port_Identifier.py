import can

# Define the channel (usually vulCAN or your device name) and bitrate
channel = 'COM3'  # This should match your device's name or port, you may need to check the actual name in your system
bitrate = 500000  # 500 kbps, adjust based on your system's configuration

# Create the CAN bus connection using the USB interface (compatible with vulCAN device)
bus = can.interface.Bus(channel=channel, interface='usb', bitrate=bitrate)

print("Listening for CAN messages...")

while True:
    message = bus.recv()  # Block and wait for a message
    if message:
        print(f"Received message: {message}")
