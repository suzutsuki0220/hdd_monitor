#ifndef __FILE_OPERATE_H__
#define __FILE_OPERATE_H__

#ifndef MAX_PATH_LEN
#define MAX_PATH_LEN 1024
#endif

typedef enum {
  PATH_NOTFOUND = 0,
  PATH_FILE,
  PATH_DIRECTORY,
  PATH_CHARDEV,
  PATH_BLKDEV,
  PATH_FIFO,
}path_stat;

path_stat is_path_exists(const char *path);
int mkdirp(const char *path);
void get_dirname(char **dirname, const char *path);

#endif
