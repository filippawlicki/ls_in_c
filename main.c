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

void print_file_details(char *filename) {
  struct stat fileStat;
  if(stat(filename,&fileStat) < 0)
    return;

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
    printf(" %s", pw->pw_name);
  }
  if (gr != 0) {
    printf(" %s", gr->gr_name);
  }
  printf(" %ld",fileStat.st_size);

  char* time_str = ctime(&fileStat.st_mtime);
  time_str[strlen(time_str)-1] = '\0'; // remove trailing newline
  printf(" %s",time_str);

  printf(" %s\n",filename);
}

void list_directory(char *dir_path, int lflag, int Rflag) {
  DIR *pDIR;
  struct dirent *pDirEnt;

  pDIR = opendir(dir_path);
  if ( pDIR == NULL ) {
    fprintf( stderr, "%s %d: opendir() failed (%s)\n",
             __FILE__, __LINE__, strerror( errno ));
    exit( -1 );
  }

  if(Rflag) {
    printf("%s:\n", dir_path);
  }

  pDirEnt = readdir( pDIR );
  while ( pDirEnt != NULL ) {
    if(strcmp(pDirEnt->d_name, ".") != 0 && strcmp(pDirEnt->d_name, "..") != 0) {
      if(lflag) {
        print_file_details(pDirEnt->d_name);
      } else {
        printf( "%s  ", pDirEnt->d_name );
      }

      if(Rflag && pDirEnt->d_type == DT_DIR) {
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dir_path, pDirEnt->d_name);
        printf("\n");
        list_directory(path, lflag, Rflag);
      }
    }

    pDirEnt = readdir( pDIR );
  }
  closedir( pDIR );
  if(!Rflag) {
    printf("\n");
  }
}

int main( int argc, char *argv[] ) {
  int c;
  int lflag = 0;
  int Rflag = 0;

  while ((c = getopt (argc, argv, "lR")) != -1)
    switch (c)
    {
      case 'l':
        lflag = 1;
        break;
      case 'R':
        Rflag = 1;
        break;
      default:
        abort ();
    }

  list_directory(".", lflag, Rflag);

  return 0;
}