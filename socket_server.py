import socket
import argparse
import struct

commands = {
    "setMotor": [1, 0, 0, 0]
}

def main(args):
    #print(type(args.msg))
    b = list()
    for i in args.msg[:4]:
        b.append(i)
    b = bytes(b)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((args.ip, args.port))
        s.sendall(b)
        data = s.recv(40)
        print(data[:4])
        if args.msg[0] == 255:
            print(struct.unpack_from('d',data, 4))

parser = argparse.ArgumentParser()
parser.add_argument("-i", "--ip", default="localhost", help="Ip address to the server")
parser.add_argument("-p", "--port", type=int, default=8080, help="Port for the socket server")
parser.add_argument("msg", type=int, nargs="+")
args = parser.parse_args()

if __name__ == '__main__':
    main(args)