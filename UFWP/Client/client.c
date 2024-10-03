#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ether.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#define BUFFER_SIZE 1500

void 
get_mac_address (const char *iface, unsigned char *mac) 
{
  struct ifreq ifr;
  int sock = socket (AF_INET, SOCK_DGRAM, 0);
  strcpy (ifr.ifr_name, iface);
  ioctl (sock, SIOCGIFHWADDR, &ifr);
  close (sock);
  memcpy (mac, ifr.ifr_hwaddr.sa_data, 6);
}

int main() {
  int raw_socket;
  struct sockaddr_ll socket_address;
  char buffer[BUFFER_SIZE];
  unsigned char source_mac[6];
  unsigned char dest_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  get_mac_address ("eth0", source_mac);

  raw_socket = socket (AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (raw_socket < 0) 
  {
    perror ("\e[0;31mCRITICAL:\e[0m Socket creation failed");
    exit (EXIT_FAILURE);
  }

  struct ethhdr *eth_header = (struct ethhdr *)buffer;
  memcpy (eth_header->h_source, source_mac, 6);
  memcpy (eth_header->h_dest, dest_mac, 6);
  eth_header->h_proto = htons (ETH_P_IP);

  char *data = buffer + sizeof(struct ethhdr);
  strcpy (data, "Hello, this is a custom protocol!");

  memset(&socket_address, 0, sizeof(socket_address));
  socket_address.sll_ifindex = if_nametoindex("eth0");
  socket_address.sll_halen = ETH_ALEN;
  memcpy(socket_address.sll_addr, dest_mac, ETH_ALEN);

  if (sendto (raw_socket, buffer, sizeof (struct ethhdr) + strlen (data) + 1, 0,
    (struct sockaddr *)&socket_address, sizeof (socket_address)) < 0) 
  {
    perror ("\e[0;31mCRITICAL:\e[0m Send failed");
    exit (EXIT_FAILURE);
  }

    printf ("Ethernet frame sent\n");

    close (raw_socket);
    return 0;
}
