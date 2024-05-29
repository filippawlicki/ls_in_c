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

void print_name(char *filename, char type) {
  if(type == 'd') {
    printf("\033[0;32;44m"); // Set the text color to blue on green background
  } else {
    printf("\033[0;32m"); // Set the text color to green
  }

  printf("%s", filename);

  printf("\033[0m"); // Reset the text color to white
}

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
  printf(" %s ",time_str);

  if(S_ISDIR(fileStat.st_mode)) {
    print_name(filename, 'd');
  } else {
    print_name(filename, 'f');
  }

  printf("\n");
}

void list_directory(char *dir_path, int lflag, int Rflag, int sflag, int aflag) {
  DIR *pDIR;
  struct dirent *pDirEnt;
  struct stat fileStat;
  int totalSize = 0;

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
    if(aflag || pDirEnt->d_name[0] != '.') {
      if(strcmp(pDirEnt->d_name, ".") != 0 && strcmp(pDirEnt->d_name, "..") != 0) {
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dir_path, pDirEnt->d_name);

        if(stat(fullPath,&fileStat) < 0)
          return;

        totalSize += fileStat.st_size;

        if(lflag) {
          print_file_details(fullPath);
        } else {
          char fileType = 'f';
          if(pDirEnt->d_type == DT_DIR) {
            fileType = 'd';
          }
          print_name(pDirEnt->d_name, fileType);
          if(sflag) {
            printf(" %ld", fileStat.st_size / 1024); // Display file size in blocks
          }
          printf("  ");
        }

        if(Rflag && pDirEnt->d_type == DT_DIR) {
          printf("\n");
          list_directory(fullPath, lflag, Rflag, sflag, aflag);
        }
      }
    }

    pDirEnt = readdir( pDIR );
  }
  closedir( pDIR );
  if(sflag) {
    printf("\ntotal %d\n", totalSize / 1024); // Display total size in blocks
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

  while ((c = getopt (argc, argv, "lRsa")) != -1)
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
      default:
        abort ();
    }

  list_directory(".", lflag, Rflag, sflag, aflag);

  return 0;
}
