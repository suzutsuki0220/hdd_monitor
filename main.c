#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/file.h>

#include "config.h"
#include "hdhealth.h"
#include "file_operate.h"

#define DEFAULT_CHECK_SPAN 60
static int interrupt = 0;

void usage(const char *command) {
  fprintf(stderr, "Usage: %s [OPTIONS] DEVICES\n", command);
  fprintf(stderr, "OPTIONS\n");
  fprintf(stderr, "  -l: output log directory\n");
  fprintf(stderr, "  -s: check span second\n");
}

void sigcatch(int sig) {
  printf("caught signal: %d\n", sig);
  interrupt = 1;
}

int write_log(const char *filename, const char *fmt, ...) {
  int fd;
  char *dirname = NULL;
  char line_buf[80] = {0};

  get_dirname(&dirname, filename);
  if (dirname) {
    mkdirp(dirname);
    free(dirname);
  }

  const mode_t fperm = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;  /* perm 644 */
  if ((fd = open(filename, O_CREAT|O_RDWR|O_APPEND, fperm)) == -1) {
    fprintf(stderr, "log write failed: %s(%d)\n", strerror(errno), errno);
    return -1;
  }

  flock(fd, LOCK_EX);
  lseek(fd, 0, SEEK_END);

  va_list args;
  va_start(args, fmt);
  vsnprintf(line_buf, 80, fmt, args);
  write(fd, line_buf, strlen(line_buf));
  va_end(args);

  flock(fd, LOCK_UN);
  close(fd);

  return 0;
}

static void replace_slash(char *str) {
  while (*str != '\0') {
    if (*str == '/') {
      *str = '-';
    }
    str++;
  }
}

int main(int argc, char *argv[]) {
  int ret = -1;
  int i;
  int extopt;
  time_t timer;
  struct tm *local;
  hd_health hdh;
  const char *hddevice;
  config_t conf = {
    ".", DEFAULT_CHECK_SPAN
  };

  struct sigaction act = {
    .sa_handler = sigcatch,
    .sa_flags = SA_RESTART,
  };
  struct sigaction ign_act = {
    .sa_handler = SIG_IGN,
    .sa_flags = SA_RESTART,
  };

  if (sigaction(SIGINT, &act, NULL)  != 0 ||
      sigaction(SIGTERM, &act, NULL) != 0 ||
      sigaction(SIGHUP, &ign_act, NULL) != 0
  ) {
    fprintf(stderr, "failed to set signal handler: %s(%d)\n", strerror(errno), errno);
    return 1;
  }

  if (get_option(argc, argv, &conf, &extopt) != 0) {
    usage(argv[0]);
    return 1;
  }

  hddevice = argv[extopt];

  if (hddevice == NULL) {
    fprintf(stderr, "please set device\n");
    usage(argv[0]);
    return 1;
  }

  while(!interrupt) {
    char filename[MAX_PATH_LEN] = {0};

    timer = time(NULL);
    local = localtime(&timer);

    for (i=extopt; i<argc; i++) {
      hddevice = argv[i];
      char *hdname = strdup(hddevice);
      if (hdname == NULL) {
        fprintf(stderr, "failed to allocate for hdname buffer\n");
        exit(1);
      }
      replace_slash(hdname);
      snprintf(filename, MAX_PATH_LEN, "%s/%d-%02d-%02d/hd_health%s.txt",
               conf.logdir,
               local->tm_year + 1900, local->tm_mon + 1, local->tm_mday,
               hdname);
      free(hdname);

      ret = get_hd_health(hddevice, &hdh);
      if (ret == 0) {
        const char *status;
        if (hdh.status) {
          if (hdh.caution) {
            status = "CAUTION";
          } else {
            status = "GOOD";
          }
        } else {
          status = "FAIL";
        }

        write_log(
          filename,
          "%02d:%02d:%02d,%d,%zu,%s\n", local->tm_hour, local->tm_min, local->tm_sec,
          hdh.awake ? 1:0, hdh.temperature, status);
      }
    }
    sleep(conf.span);
  }

  return ret;
}
