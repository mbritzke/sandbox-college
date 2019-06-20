# Objetivo: descobrir portas abertas no sistema utilizando usando IPv6

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

    def assemble_tcp_feilds(self):
        self.raw = struct.pack('!HHLLBBHHH',    # Data Structure Representation
        self.tcp_src,                           # Source IP
        self.tcp_dst,                           # Destination IP
        self.tcp_seq,                           # Sequence
        self.tcp_ack_seq,                       # Acknownlegment Sequence
        self.tcp_hdr_len,                       # Header Length
        self.tcp_flags ,                        # TCP Flags
        self.tcp_wdw,                           # TCP Windows
        self.tcp_chksum,                        # TCP cheksum
        self.tcp_urg_ptr                        # TCP Urgent Pointer
        )

        self.calculate_chksum()
        return

    def calculate_chksum(self):
        src_addr     = socket.inet_aton( self.src_ip )
        dest_addr    = socket.inet_aton( self.dst_ip )
        placeholder  = 0
        protocol     = socket.IPPROTO_TCP
        tcp_len      = len(self.raw) + len(self.data)

        psh = struct.pack('!4s4sBBH' , 
         src_addr ,    # Source Address  
         dest_addr ,   # Destination Address 
         placeholder , # Placeholder
         protocol ,    # Protocol 
         tcp_len       # Length of pseudo + Demo TCP header + Data 
         )

        psh = psh + self.raw + self.data
        self.tcp_chksum = self.chksum(psh)
        self.reassemble_tcp_feilds()       

        return

    