import socket
import argparse

def main(args):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((args.ip, args.port))
        s.sendall(" ".join(args.msg).encode('ascii'))
        data = s.recv(1024)
        print(data.decode())

parser = argparse.ArgumentParser()
parser.add_argument("-i", "--ip", default="localhost", help="Ip address to the server")
parser.add_argument("-p", "--port", type=int, default=8080, help="Port for the socket server")
parser.add_argument("msg", nargs="*")
args = parser.parse_args()

if __name__ == '__main__':
    main(args)