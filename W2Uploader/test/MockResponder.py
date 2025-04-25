import serial
import sys

# Default values
BAUDRATE = 9600
ACK = b'\x06'

def main(port):
    with serial.Serial(port, BAUDRATE, timeout=1) as ser:
        print(f"Listening on {port}...")
        while True:
            if ser.in_waiting:
                data = ser.read(ser.in_waiting)
                print(f"Received: {data}")
                ser.write(ACK)
                print("ACK sent")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python mock_responder.py <serial_port>")
        sys.exit(1)

    port = sys.argv[1]
    main(port)
