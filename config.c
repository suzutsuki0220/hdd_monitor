#include <stdlib.h>
#include <unistd.h>

#include "config.h"

int
get_option(int argc, char *argv[], config_t *conf, int *extopt)
{
  int ch;
  extern char *optarg;
  extern int optind;

  while((ch = getopt(argc, argv, "l:s:h")) != -1) {
    switch (ch) {
    case 'l':
      conf->logdir = optarg;
      break;
    case 's':
      conf->span = atoi(optarg);
      break;
    case 'h':
      return -1;
    default:
      return -1;
    }
  }

  *extopt = optind;

  return 0;  
}
