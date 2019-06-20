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
        self.create_tcp_fields()

    def create_tcp_fields(self):
        self.tcp_src = self.sport   # Source port
        self.tcp_dst = self.dport   # Destination Port
        self.tcp_seq = 0            # Sequence Number
        self.tcp_ack_seq = 0        # Acknowledgement Number
        self.tcp_hdr_len = 80       # Header Length
        
        tcp_flags_rsv = (0 << 9)
        tcp_flags_noc = (0 << 8)
        tcp_flags_cwr = (0 << 7)
        tcp_flags_ecn = (0 << 6)
        tcp_flags_urg = (0 << 5)
        tcp_flags_ack = (0 << 4)
        tcp_flags_psh = (0 << 3)
        tcp_flags_rst = (0 << 2)
        tcp_flags_syn = (1 << 1)
        tcp_flags_fin = (0)

        self.tcp_flags = tcp_flags_rsv + tcp_flags_noc + tcp_flags_cwr + \
                tcp_flags_ecn + tcp_flags_urg + tcp_flags_ack + \
                tcp_flags_psh + tcp_flags_rst + tcp_flags_syn + tcp_flags_fin

        self.tcp_wdw = socket.htons (5840)  # Window Size
        self.tcp_chksum = 0                 # CheckSum
        self.tcp_urg_ptr = 0                # Urgent Pointer
        
        return