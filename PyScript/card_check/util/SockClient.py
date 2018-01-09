# Echo client program  
import socket  
import sys  
  
HOST = '127.0.0.1'    # The remote host  
PORT = 8888              # The same port as used by the server  
sock = None  

def Connect(host,port):
    global sock
    sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    sock.connect((host,port))

def Send(buffer):
    sock.sendall(buffer)


if __name__ == '__main__':
    Connect("127.0.0.1",8888)
    Send(b"hello server\nbitch\n")
    print(b'shit')