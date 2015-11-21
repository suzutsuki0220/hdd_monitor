#ifndef __INCLUDE_CONFIG_H__
#define __INCLUDE_CONFIG_H__

typedef struct config {
  char *logdir;
  int  span;
} config_t;

int get_option(int argc, char *argv[], config_t *conf, int *extopt);

#endif
