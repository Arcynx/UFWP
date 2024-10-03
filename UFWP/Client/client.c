#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "/tmp/UFWP.sock"

int
main ()
{
  int sock;
  struct sockaddr_un server_addr;

  /* implement message sending */

  const char *msg = "Hello, World from the Client";

  if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
  {
    perror ("\e[0;31mCRITICAL:\e[0m Socket creation failed");
    exit (EXIT_FAILURE);
  }

  write (sock, msg, strlen (msg));

  close (sock);

  return 0;
}