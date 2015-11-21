#ifndef __HD_HEALTH__
#define __HD_HEALTH__

#include <string.h>
#include <atasmart.h>

typedef struct hd_health_t {
  SkBool   awake;
  SkBool   status;
  SkBool   caution;
  uint64_t temperature;
  uint64_t ontime;
  uint64_t error_count;
} hd_health;

int get_hd_health(const char *disk, hd_health *hdh);
int get_errorcount(const char *disk, hd_health *hdh);

#endif
