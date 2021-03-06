#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>

#define MAC_ADDR_LEN 6
#define BUFFER_SIZE 1600
#define MAX_DATA_SIZE 1500

int main(int argc, char *argv[])
{
        int fd;
        struct ifreq if_idx;
        struct ifreq if_mac;
        struct sockaddr_ll socket_address;
        char ifname[IFNAMSIZ];
        int frame_len = 0;
        char buffer[BUFFER_SIZE];
        char data[MAX_DATA_SIZE];
        char dest_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //broadcast
        short int ethertype = htons(0x0806);

        if (argc != 2) {
                printf("Usage: %s iface\n", argv[0]);
                return 1;
        }
        strcpy(ifname, argv[1]);

        /* Cria um descritor de socket do tipo RAW */
        if ((fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
                perror("socket");
                exit(1);
        }

        /* Obtem o indice da interface de rede */
        memset(&if_idx, 0, sizeof (struct ifreq));
        strncpy(if_idx.ifr_name, ifname, IFNAMSIZ - 1);
        if (ioctl(fd, SIOCGIFINDEX, &if_idx) < 0) {
                perror("SIOCGIFINDEX");
                exit(1);
        }

        /* Obtem o endereco MAC da interface local */
        memset(&if_mac, 0, sizeof (struct ifreq));
        strncpy(if_mac.ifr_name, ifname, IFNAMSIZ - 1);
        if (ioctl(fd, SIOCGIFHWADDR, &if_mac) < 0) {
                perror("SIOCGIFHWADDR");
                exit(1);
        }

        /* Indice da interface de rede */
        socket_address.sll_ifindex = if_idx.ifr_ifindex;

        /* Tamanho do endereco (ETH_ALEN = 6) */
        socket_address.sll_halen = ETH_ALEN;

        /* Endereco MAC de destino */
        memcpy(socket_address.sll_addr, dest_mac, MAC_ADDR_LEN);

        /* Preenche o buffer com 0s */
        memset(buffer, 0, BUFFER_SIZE);

        /* Monta o cabecalho Ethernet */

        /* Preenche o campo de endereco MAC de destino */
        memcpy(buffer, dest_mac, MAC_ADDR_LEN);
        frame_len += MAC_ADDR_LEN;

        /* Preenche o campo de endereco MAC de origem */
        memcpy(buffer + frame_len, if_mac.ifr_hwaddr.sa_data, MAC_ADDR_LEN);
        frame_len += MAC_ADDR_LEN;

        /* Preenche o campo EtherType */
        memcpy(buffer + frame_len, &ethertype, sizeof(ethertype));
        frame_len += sizeof(ethertype);

 	/* ARP: Preenche o campo htype*/ 
        short htype = htons(1);
        memcpy(buffer + frame_len, &htype, sizeof(htype));
        frame_len += sizeof(htype);

        /* ARP: Preenche o campo protocol type*/
        short ptype = htons(0x0800);
        memcpy(buffer + frame_len, &ptype, sizeof(ptype));
        frame_len += sizeof(ptype);
	
	 /* ARP: Preenche o campo HLEN*/
	short hlen = htons(6);
        memcpy(buffer + frame_len, &hlen, sizeof(hlen));
        frame_len += sizeof(hlen);

	/* ARP: Preenche o campo DLEN*/
	short dlen = htons(4);
        memcpy(buffer + frame_len, &dlen, sizeof(dlen));
        frame_len += sizeof(dlen);
	
	/* ARP: Preenche o campo Operation*/ 
	short operation = htons(0x0001);
        memcpy(buffer + frame_len, &operation, sizeof(operation));
        frame_len += sizeof(operation);

	/* ARP: Preenche o campo Sender HA*/ 
	short senderHA = htons(6);
        memcpy(buffer + frame_len, &senderHA, sizeof(senderHA));
        frame_len += sizeof(senderHA);

	/* ARP: Preenche o campo Sender IP*/ 
	short senderIP = htons(4);
        memcpy(buffer + frame_len, &senderIP, sizeof(senderIP));
        frame_len += sizeof(senderIP);

	/* ARP: Preenche o campo Target HA*/ 
	short targetHA = htons(6);
        memcpy(buffer + frame_len, &targetHA, sizeof(targetHA));
        frame_len += sizeof(targetHA);

 	/* ARP: Preenche o campo Target IP*/ 
	short targetIP = htons(6);
        memcpy(buffer + frame_len, &targetIP, sizeof(targetIP));
        frame_len += sizeof(targetIP);	

        /* Obtem uma mensagem do usuario */
        //printf("Digite a mensagem: ");
        //scanf("%[^\n]s", data);

        /* Preenche o campo de dados */
        //memcpy(buffer + frame_len, data, strlen(data));
        //frame_len += strlen(data) + 1;

        /* Envia pacote */
        if (sendto(fd, buffer, frame_len, 0, (struct sockaddr *) &socket_address, sizeof (struct sockaddr_ll)) < 0) {
                perror("send");
                close(fd);
                exit(1);
        }

        printf("Pacote enviado.\n");

        close(fd);
        return 0;
}
