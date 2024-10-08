#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ether.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <arpa/inet.h>

#define BUF_SIZE 1500

int 
main () 
{
  int raw_socket;
  struct sockaddr_ll saddr;
  char buf[BUF_SIZE];

  raw_socket = socket (AF_PACKET, SOCK_RAW, htons (ETH_P_ALL));
  if (raw_socket < 0) 
  {
    perror ("\e[0;31mCRITICAL:\e[0m Socket creation failed");
    exit (EXIT_FAILURE);
  }

  printf ("Listening for Ethernet frames...\n");

  while (1) 
  {
    ssize_t frame_len = recvfrom (raw_socket, buf, BUF_SIZE, 0, NULL, NULL);
    if (frame_len < 0) 
    {
      perror ("\e[0;31mCRITICAL: Failed to receive frame\e[0m");
      break;
    }

    printf ("------------------------------------------\n\n");

    printf ("Received Ethernet frame, length: %ld bytes\n", frame_len);

    struct ethhdr *eth_header = (struct ethhdr *)buf;
    printf ("Source MAC: %s\n", ether_ntoa ((struct ether_addr *) eth_header->h_source));
    printf ("Destination MAC: %s\n", ether_ntoa ((struct ether_addr *) eth_header->h_dest));
    printf ("Ethertype: 0x%04x\n", ntohs (eth_header->h_proto));

    int eth_header_len = sizeof (struct ethhdr);
    char *custom_protocol_data = buf + eth_header_len;

    int data_len = frame_len - eth_header_len;
    printf("Custom protocol data (%d bytes):", data_len);

    /* Print packet data */

    for (int i = 0; i < data_len; i++)
    {
      if (i % 16 == 0) printf ("\n");
      if ("%02X ", (unsigned char *)custom_protocol_data[i]);
    }

    printf ("\n\n------------------------------------------");
  }

  close(raw_socket);
  return 0;
}
