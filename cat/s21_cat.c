#define _GNU_SOURCE
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct t_Flags {
  int bflag;
  int eflag;
  int nflag;
  int sflag;
  int tflag;
  int vflag;
} t_Flags;

void usage();
void nothing_extra();
void flags_set_value(int ch, t_Flags *flags);
void flags_hendler(int argc, char *argv[], t_Flags *flags);
void cat_vet(int cur, t_Flags flags);
void cat_flags(FILE *file, t_Flags flags);
void print_file_flags(const char *filename, t_Flags flags);
void scan_files(int argc, char *argv[], int checker, t_Flags flags);

int main(int argc, char *argv[]) {
  if (argc == 1) {
    nothing_extra();
  } else {
    t_Flags flags = {0, 0, 0, 0, 0, 0};
    flags_hendler(argc, argv, &flags);
    if (flags.bflag || flags.nflag || flags.eflag || flags.sflag ||
        flags.vflag || flags.tflag) {
      scan_files(argc, argv, 1, flags);
    } else {
      scan_files(argc, argv, 0, flags);
    }
  }

  return 0;
}

void usage() {
  printf("usage: s21_cat [OPTION]... [FILE]...");
  exit(1);
}

void nothing_extra() {
  char buf[4096];
  int fd = STDIN_FILENO;
  int bytes_step = read(fd, buf, 4096);

  while (bytes_step != -1) {
    printf("%.*s", bytes_step, buf);
    bytes_step = read(fd, buf, 4096);
  }
}

void flags_set_value(int ch, t_Flags *flags) {
  switch (ch) {
    case 'b':
      flags->bflag = 1;
      flags->nflag = 1;
      break;
    case 'e':
      flags->eflag = 1;
      flags->vflag = 1;
      break;
    case 'v':
      flags->vflag = 1;
      break;
    case 'E':
      flags->eflag = 1;
      break;
    case 'n':
      flags->nflag = 1;
      break;
    case 's':
      flags->sflag = 1;
      break;
    case 't':
      flags->tflag = 1;
      flags->vflag = 1;
      break;
    case 'T':
      flags->tflag = 1;
      break;
    default:
      usage();
  }
}

void flags_hendler(int argc, char *argv[], t_Flags *flags) {
  int ch;
  static struct option const s_longOptions[] = {
      {"number-nonblank", 0, NULL, 'b'},
      {"number", 0, NULL, 'n'},
      {"squeeze-blank", 0, NULL, 's'},
      {NULL, 0, NULL, 0}};
  while ((ch = getopt_long(argc, argv, "benstvET", s_longOptions, NULL)) !=
         -1) {
    flags_set_value(ch, flags);
  };
}
void print_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("cat: %s: No such file or directory\n", filename);
    exit(1);
  } else {
    char *line = 0;
    size_t len = 0;
    while (getline(&line, &len, file) != -1) {
      printf("%s", line);
    }
    free(line);
  }
}

void cat_vet(int cur, t_Flags flags) {
  if (cur != '\n' && cur != '\t' && flags.vflag) {
    if (cur < 32) {
      printf("^%c", cur + 64);
    } else if (cur == 127) {
      printf("^?");
    } else if (cur > 127 && cur < 160) {
      printf("M-^%c", cur - 64);
    } else {
      printf("%c", cur);
    }
  } else {
    if (flags.eflag && cur == '\n') {
      printf("$%c", cur);
    } else if (flags.tflag && cur == '\t') {
      printf("^I");
    } else {
      printf("%c", cur);
    }
  }
}

void cat_flags(FILE *file, t_Flags flags) {
  int cur, prev, line = 0, gobble = 0;
  for (prev = '\n'; (cur = getc(file)) != EOF; prev = cur) {
    if (prev == '\n') {
      if (flags.sflag) {
        if (cur == '\n') {
          if (gobble) continue;
          gobble = 1;
        } else {
          gobble = 0;
        }
      }
      if (flags.nflag && (!flags.bflag || cur != '\n')) {
        printf("%6d\t", ++line);
      }
    }
    cat_vet(cur, flags);
  }
}

void print_file_flags(const char *filename, t_Flags flags) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    printf("cat: %s: No such file or directory\n", filename);
    exit(1);
  } else {
    cat_flags(file, flags);
    fclose(file);
  }
}

void scan_files(int argc, char *argv[], int checker, t_Flags flags) {
  for (int i = 1; i < argc; i++) {
    if (checker) {
      if (i > 1 && argv[i][0] != '-') {
        print_file_flags(argv[i], flags);
      }
    } else {
      print_file(argv[i]);
    }
  }
}