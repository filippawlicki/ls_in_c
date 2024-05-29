#!/usr/bin/tcc -run

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#define MAX_PATH 1024
#define BLOCK_SIZE 1024
#define DIR_COLOR "\033[0;32;44m"
#define FILE_COLOR "\033[0;32m"
#define RESET_COLOR "\033[0m"
#define DIR_TYPE 'd'
#define FILE_TYPE 'f'

void print_name(char *filename, char type) {
  if(type == DIR_TYPE) {
    printf(DIR_COLOR); // Set the text color to blue on green background
  } else {
    printf(FILE_COLOR); // Set the text color to green
  }

  printf("%s", filename);

  printf(RESET_COLOR); // Reset the text color to white
}

void print_file_details(char *filename, int iflag) {
  struct stat fileStat;
  if(stat(filename,&fileStat) < 0)
    return;

  if(iflag) {
    printf("%ld ",fileStat.st_ino); // Display inode number
  }

  struct passwd *pw = getpwuid(fileStat.st_uid);
  struct group  *gr = getgrgid(fileStat.st_gid);

  printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
  printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
  printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
  printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
  printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
  printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
  printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
  printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
  printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
  printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
  printf(" %ld",fileStat.st_nlink);
  if (pw != 0) {
    printf(" %s", pw->pw_name); // Display the user name
  }
  if (gr != 0) {
    printf(" %s", gr->gr_name); // Display the group name
  }
  printf(" %ld",fileStat.st_size);

  char* time_str = ctime(&fileStat.st_mtime);
  time_str[strlen(time_str)-1] = '\0'; // remove trailing newline
  printf(" %s ",time_str);

  if(S_ISDIR(fileStat.st_mode)) {
    print_name(filename, 'd');
  } else {
    print_name(filename, 'f');
  }

  printf("\n");
}

void list_directory(char *dir_path, int lflag, int Rflag, int sflag, int aflag, int iflag) {
  DIR *pDIR;
  struct dirent *pDirEnt; // Directory entry
  struct stat fileStat; // File status
  int totalSize = 0;

  pDIR = opendir(dir_path);
  if ( pDIR == NULL ) { // If the directory cannot be opened, print an error message and exit
    fprintf( stderr, "%s %d: opendir() failed (%s)\n",
             __FILE__, __LINE__, strerror( errno ));
    exit( -1 );
  }

  if(Rflag) {
    printf("%s:\n", dir_path);
  }

  pDirEnt = readdir( pDIR );
  while ( pDirEnt != NULL ) {
    if(aflag || pDirEnt->d_name[0] != '.') {
      if(strcmp(pDirEnt->d_name, ".") != 0 && strcmp(pDirEnt->d_name, "..") != 0) { // Skip the current and parent directories
        char fullPath[MAX_PATH];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dir_path, pDirEnt->d_name);

        if(stat(fullPath,&fileStat) < 0) // If the file stat fails, skip the file
          return;

        totalSize += fileStat.st_size;

        if(lflag) {
          print_file_details(fullPath, iflag);
        } else {
          if(iflag) {
            printf("%ld ", fileStat.st_ino); // Print the inode number
          }
          char fileType = FILE_TYPE;
          if(pDirEnt->d_type == DT_DIR) {
            fileType = DIR_TYPE;
          }
          print_name(pDirEnt->d_name, fileType);
          if(sflag) {
            printf(" %ld", fileStat.st_size / BLOCK_SIZE); // Display file size in blocks
          }
          printf("  ");
        }

        if(Rflag && pDirEnt->d_type == DT_DIR) {
          printf("\n");
          list_directory(fullPath, lflag, Rflag, sflag, aflag, iflag); // Recursively list the directory
        }
      }
    }

    pDirEnt = readdir( pDIR ); // Get the next directory entry
  }
  closedir( pDIR );
  if(sflag) {
    printf("\ntotal %d\n", totalSize / BLOCK_SIZE); // Display total size in blocks
  }
  if(!Rflag) {
    printf("\n");
  }
}

int main( int argc, char *argv[] ) {
  int c;
  int lflag = 0;
  int Rflag = 0;
  int sflag = 0;
  int aflag = 0;
  int iflag = 0;

  while ((c = getopt (argc, argv, "lRsai")) != -1)
    switch (c)
    {
      case 'l':
        lflag = 1;
        break;
      case 'R':
        Rflag = 1;
        break;
      case 's':
        sflag = 1;
        break;
      case 'a':
        aflag = 1;
        break;
      case 'i':
        iflag = 1;
        break;
      default:
        abort ();
    }

  char* dir_path = ".";
  if (optind < argc) { // If there are any arguments after the options
    dir_path = argv[optind];
  }

  list_directory(dir_path, lflag, Rflag, sflag, aflag, iflag);

  return 0;
}
