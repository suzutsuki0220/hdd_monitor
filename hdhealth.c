#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <atasmart.h>  /* libatasmart */

#include "hdhealth.h"

#define WARN_REALLOCATED    0x1  /* Threshold: Reallocated_Sector_Ct(5) */
#define WARN_PENDING_SECTOR 0x1  /* Threshold: Current_Pending_Sector(197) */
#define WARN_UNCORRECTABLE  0x1  /* Threshold: Offline_Uncorrectable(198)  */

/* 注意する項目の値が上がっていないか確認 */
static void check_caution_cb(SkDisk *d, const SkSmartAttributeParsedData *a, SkBool *value) {
    switch (a->id) {
    case 5:
        if (a->pretty_value >= WARN_REALLOCATED) {
            *value = 1;
	}
        break;
    case 197:
        if (a->pretty_value >= WARN_PENDING_SECTOR) {
            *value = 1;
	}
        break;
    case 198:
        if (a->pretty_value >= WARN_UNCORRECTABLE) {
            *value = 1;
	}
        break;
    default:
        // nothing
        break;
    }
}

int get_hd_health(const char *disk, hd_health *hdh) {
  int ret = -1;
  SkDisk *skdisk;
  SkBool smart_available = 0;
  uint64_t disk_temp = 0;
  uint64_t disk_ontime = 0;

  if (sk_disk_open(disk, &skdisk) < 0) {
    fprintf(stderr, "Failed to open disk %s: %s(%d)\n", disk, strerror(errno), errno);
    return -1;
  }

  if (sk_disk_smart_is_available(skdisk, &smart_available) < 0) {
    fprintf(stderr, "Failed to query whether SMART is available %s: %s(%d)\n", disk, strerror(errno), errno);
    goto done;
  }
  if (!smart_available) {
    fprintf(stderr, "%s is not support SMART\n", disk);
    goto done;
  }

  if (sk_disk_smart_read_data(skdisk) < 0) {
    fprintf(stderr, "Failed to read SMART data %s: %s(%d)\n", disk, strerror(errno), errno);
    goto done;
  }
  ret = 0;

  if (sk_disk_smart_get_temperature(skdisk, &disk_temp) == 0) {
    uint64_t celsius = (disk_temp - 273150) / 1000;  /* convert milli kelvin to celsius */
    hdh->temperature = celsius;
  }

  if (sk_disk_smart_get_power_on(skdisk, &disk_ontime) == 0) {
    uint64_t onhour = disk_ontime / 1000 / 3600;
    hdh->ontime = onhour;
  }

  sk_disk_check_sleep_mode(skdisk, &hdh->awake);
  sk_disk_smart_status(skdisk, &hdh->status);

  hdh->caution = 0;
  if (sk_disk_smart_parse_attributes(skdisk, (SkSmartAttributeParseCallback) check_caution_cb, &hdh->caution) < 0) {
    fprintf(stderr, "Failed to get attribute: %s(%d)\n", strerror(errno), errno);
    goto done;
  }

done:
  sk_disk_free(skdisk);

  return ret;
}

static void get_errorcount_cb(SkDisk *d, const SkSmartAttributeParsedData *a, uint64_t *value) {
    if (a->id != 199)  /* udma-crc-error-count */
            return;

    *value = a->pretty_value;
}

int get_errorcount(const char *disk, hd_health *hdh) {
  int ret = -1;
  SkDisk *skdisk;

  if (sk_disk_open(disk, &skdisk) < 0) {
    fprintf(stderr, "Failed to open disk %s: %s(%d)\n", disk, strerror(errno), errno);
    return -1;
  }

  if (sk_disk_smart_read_data(skdisk) < 0) {
    fprintf(stderr, "Failed to read SMART data %s: %s(%d)\n", disk, strerror(errno), errno);
    goto done;
  }

  if (sk_disk_smart_parse_attributes(skdisk, (SkSmartAttributeParseCallback) get_errorcount_cb, &hdh->error_count) < 0) {
    fprintf(stderr, "Failed to get attribute: %s(%d)\n", strerror(errno), errno);
    goto done;
  }
  ret = 0;

done:
  sk_disk_free(skdisk);

  return ret;
}

/*
static void check_prefailure_cb(SkDisk *d, const SkSmartAttributeParsedData *a, SkBool *ah) {
  if (a->prefailure) {
    
  }
}
*/

