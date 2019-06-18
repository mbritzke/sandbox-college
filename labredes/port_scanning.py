# Objetivo de descobrir portas abertas no sistema utilizando usando IPv6

import socket
import struct

class TCPPacket:
    def __init__(self, dport = 80, sport = 65535, dst='127.0.0.1', src='192.168.1.101', data = 'Nothing'):
        self.dport = dport
        self.sport = sport
        self.src_ip = src
        self.dst_ip = dst
        self.data   = data
        self.raw = None
        self.create_tcp_feilds()