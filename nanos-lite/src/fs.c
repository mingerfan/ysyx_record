#include <fs.h>
#include <string.h>
#include <device.h>
#include <common.h>

#define ARRAY_SIZE(arr)		(sizeof(arr) / sizeof((arr)[0]))

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  int open_offset; 
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  {"/dev/events", 0, 0, events_read, invalid_write},
  {"/dev/fb", 0, 0, invalid_read, fb_write},
  {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
#include "files.h"
};

static int find_file(const char *pathname) {
  // printf("File: %s\n", pathname);
  for (int i = 0; i < ARRAY_SIZE(file_table); i++) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      return i;
    }
  }
  printf("Find path: %s failed\n", pathname);
  panic("Should not reach here");
}

void init_fs() {
  // TODO: initialize the size of /dev/fb
  int idx = find_file("/dev/fb");
  AM_GPU_CONFIG_T am_gpu_cfg;
  am_gpu_cfg = io_read(AM_GPU_CONFIG);
  int width = am_gpu_cfg.width;
  int height = am_gpu_cfg.height;
  file_table[idx].size = width * height * 4;

  // uint32_t buf[400 * 20];
  // for (int i = 0; i < 400*20; i++) {
  //   buf[i] = 0xff00ff;
  // }
  // fb_write(buf, 329999, 400*20*4);
}


int fs_open(const char *pathname, int flags, int mode) {
  int i = find_file(pathname);
  file_table[i].open_offset = 0;
  return i;
}

size_t fs_read(int fd, void *buf, size_t len) {
  int disk_off = file_table[fd].disk_offset;
  if (fd < 0 || fd >= ARRAY_SIZE(file_table)) return -1;
  if (file_table[fd].read != NULL) {
    // copy file offset to offset param
    return file_table[fd].read(buf, file_table[fd].open_offset, len);
  }
  if (len + file_table[fd].open_offset >= file_table[fd].size) {
    len = file_table[fd].size - file_table[fd].open_offset;
  }
  if (len > 0) ramdisk_read(buf, disk_off + file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;
  return len;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  if (fd < 3 || fd >= ARRAY_SIZE(file_table)) return -1;
  int max_size = file_table[fd].size;
  int cur_off = file_table[fd].open_offset;
  if (whence == SEEK_SET) {
    assert(offset >= 0 && offset < max_size);
    file_table[fd].open_offset = offset;
  } else if (whence == SEEK_CUR) {
    assert((offset + cur_off) >= 0 && (offset + cur_off) <= max_size);
    file_table[fd].open_offset = offset + cur_off;
  } else if (whence == SEEK_END) {
    assert((offset + max_size - 1) <= max_size && (offset + max_size - 1) >= 0);
    file_table[fd].open_offset = max_size + offset;
  } else {
    return -1;
  }
  return file_table[fd].open_offset;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  int disk_off = file_table[fd].disk_offset;
  if (fd < 0 || fd >= ARRAY_SIZE(file_table)) return -1;
  if (file_table[fd].write == NULL) {
    assert(len + file_table[fd].open_offset <= file_table[fd].size);
    ramdisk_write(buf, disk_off + file_table[fd].open_offset, len);
    file_table[fd].open_offset += len;
    return len;
  }
  // copy file offset to offset param
  return file_table[fd].write(buf, file_table[fd].open_offset, len);
}

int fs_close(int fd) {
  file_table[fd].open_offset = 0;
  return 0;
}
