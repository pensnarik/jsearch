/*
   Image files search utility
   Copyright (c) 2012 Andrey Zhidenkov, <andrey.zhidenkov@gmail.com> 
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   See the COPYING file for a copy of the GNU General Public License. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#define MAX_EXT_SIZE 10

const char marker[3] = { 0xFF, 0xD8, 0x00 };
static unsigned long total_count = 0;

void usage(const char *pname)
{
  fprintf(stdout, "Usage: %s [path]\n", pname);
  exit(0);
}

static int file_ext(const char *filename, char *buf)
{
  const char *ptr;
  char c; int ext_len = 0;
  ptr = filename + strlen(filename) - 1;
  while((c = *ptr--) != '.') {
    //printf("c = %c\n", c);
    if( c == '/' || ptr < filename || ext_len >= MAX_EXT_SIZE ) return 0;
    ext_len++;
    continue;
  }
  if (ptr <= filename) return 0;
  //printf("len: %ld\n", ext_len);
  strncpy(buf, ptr+2, ext_len);  
  buf[ext_len] = 0;
  return 1;
}

void process_file(const char *filename)
{
  FILE *f;
  char buf[10];
  //fprintf(stdout, "%s\n", filename);
  char ext[20];
  total_count++;
  if(file_ext(filename, (char *) ext)) {
    //printf("EXT = %s\n", ext);
    if (!strcasecmp(ext, "jpg") || !strcasecmp(ext, "jpeg")) {
      //printf("Skipping...\n");
      return;
    }
  }
  f = fopen(filename, "rb");
  if(!f) {
    fprintf(stderr, "Error: cannot open file %s\n", filename);
    return;  
  }
  fread(buf, strlen(marker), 1, f);
  buf[strlen(marker)] = 0;
  if(strcmp(buf, marker) == 0) {
    fprintf(stdout, "JPEG file found: %s\n", filename);
  }
  //printf("%s\n", buf);
  fclose(f);
  return;
}

void process_dir(const char *dirname)
{
  DIR *dirp;
  struct dirent* de;
  struct stat sb;
  char fullpath[1024];
  
  dirp = opendir(dirname);
  if(dirp == (DIR *) 0) {
    fprintf(stderr, "Error: cannot open dir %s\n", dirname);
    return;
  }
  int err;
  while((de = readdir(dirp)) != 0) {
    if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;
    //printf("%s\n", de->d_name);
    sprintf(fullpath, "%s/%s", dirname, de->d_name);        
    err = lstat(fullpath, &sb);
    //printf("%s\n", fullpath);
    /* If sym link - do not follow */
    if ( S_ISLNK( sb.st_mode )) {
      printf("SYMLINK!\n");
      continue;
    }
    switch ( sb.st_mode & S_IFMT ) {
      case S_IFDIR:
        //printf("<DIR>\n");
        //printf("fp = %s\n", fullpath);
        process_dir(fullpath);
        break;
      case S_IFREG:
        process_file(fullpath);
        break;
      default:
        break;
    }
  }
  closedir(dirp);
}

int main(int argc, char *argv[])
{
  if (argc < 2) usage(argv[0]);
  int i;
  for(i = 1; i < argc; i++) {
    process_dir(argv[i]);
  }
  
  fprintf(stdout, "All done. Files proceeded: %u\n", total_count);
  
  return 0;
}
