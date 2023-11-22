#define _GNU_SOURCE
#include <errno.h>
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct t_Flags {
  int e;
  int v;
  int c;
  int l;
  int n;
  int s;
  int h;
  int o;
  char *patterns;
  size_t pattern_c;
  int check_empty;
} t_Flags;

void flags_hendler(int argc, char *argv[], t_Flags *flags);
void flags_set_value(int ch, t_Flags *flags);
void set_patterns_string(char *pattern, t_Flags *flags);
int set_patterns_file(char *filename, t_Flags *flags);
void *xmalloc(size_t size);
void *xrealloc(char *pat, size_t size);
void stradd(t_Flags *flags, char *pattern);
void stradd_new(t_Flags *flags, char *pattern);
void usage();

void file_pattern_hendler(int argc, char *argv[], t_Flags flags);
int check_many_files(int argc, char *argv[]);
void grep(char *filename, t_Flags flags, regex_t *preg, int many_files);
int grep_lchv(int status, char *filename, t_Flags flags, int many_files);
int grep_nceivh(int status, char *line, int line_number, t_Flags flags,
                char *filename, int many_files);
int grep_ncivho(regmatch_t match, char *line, int line_number, t_Flags flags,
                char *filename, int many_files, regex_t *preg);
void grep_cvh(t_Flags flags, int many_files, char *filename, int count);

int main(int argc, char *argv[]) {
  if (argc == 1) {
    usage();
  }
  t_Flags flags = {0};
  flags_hendler(argc, argv, &flags);
  file_pattern_hendler(argc, argv, flags);
  if (flags.patterns != NULL) free(flags.patterns);
  return 0;
}

int grep_nceivh(int status, char *line, int line_number, t_Flags flags,
                char *filename, int many_files) {
  if (status) {
    if (flags.c) {
      return 1;
    } else if (flags.n) {
      if (many_files && flags.h == 0) {
        printf("%s:%d:%s", filename, line_number, line);
      } else {
        printf("%d:%s", line_number, line);
      }
    } else {
      if (many_files && flags.h == 0) {
        printf("%s:%s", filename, line);
      } else {
        printf("%s", line);
      }
    }
  }
  return 0;
}

void grep_o(regmatch_t match, char *line, regex_t *preg, int len) {
  for (char *remaining = line + match.rm_eo;
       !regexec(preg, remaining, 1, &match, 0); remaining += match.rm_eo) {
    printf("%.*s\n", len, remaining + match.rm_so);
  }
}

int grep_ncivho(regmatch_t match, char *line, int line_number, t_Flags flags,
                char *filename, int many_files, regex_t *preg) {
  int len = match.rm_eo - match.rm_so;
  if (flags.c) {
    return 1;
  } else if (flags.n) {
    if (many_files && flags.h == 0) {
      printf("%s:%d:%.*s\n", filename, line_number, len, line + match.rm_so);
      grep_o(match, line, preg, len);
    } else {
      printf("%d:%.*s\n", line_number, len, line + match.rm_so);
      grep_o(match, line, preg, len);
    }
  } else {
    if (many_files && flags.h == 0) {
      printf("%s:%.*s\n", filename, len, line + match.rm_so);
      grep_o(match, line, preg, len);
    } else {
      printf("%.*s\n", len, line + match.rm_so);
      grep_o(match, line, preg, len);
    }
  }
  return 0;
}

int grep_lchv(int status, char *filename, t_Flags flags, int many_files) {
  int res = 0;
  if (status) {
    if (flags.c) {
      if (many_files) {
        if (flags.h) {
          printf("1\n");
        } else {
          printf("%s:1\n", filename);
        }
      } else {
        printf("1\n");
      }
      printf("%s\n", filename);
      res = 1;
    } else {
      printf("%s\n", filename);
      res = 1;
    }
  }
  return res;
}

void grep_cvh(t_Flags flags, int many_files, char *filename, int count) {
  if (many_files && flags.h == 0)
    printf("%s:%d\n", filename, count);
  else
    printf("%d\n", count);
}

void grep(char *filename, t_Flags flags, regex_t *preg, int many_files) {
  FILE *fp = fopen(filename, "r");
  if (fp != NULL) {
    char *line = 0;
    size_t len = 0;

    regmatch_t match;
    int line_number = 0;
    int match_count = 0;
    int unmatch_count = 0;
    int checker;
    while (getline(&line, &len, fp) != -1) {
      int status = regexec(preg, line, 1, &match, 0);
      len = strlen(line);
      if (line[len - 1] != '\n') {
        line = (char *)xrealloc(line, sizeof(char *) * len + 1);
        strcat(line, "\n");
      }
      if (*line == '\n' && flags.c && flags.check_empty == 1) {
        continue;
      }
      line_number++;
      if (flags.l) {
        if (flags.v) {
          checker = grep_lchv(status, filename, flags, many_files);
          if (checker) break;
        } else {
          checker = grep_lchv(!status, filename, flags, many_files);
          if (checker) break;
        }
      } else {
        if (flags.v) {
          if (grep_nceivh(status, line, line_number, flags, filename,
                          many_files))
            unmatch_count++;
        } else {
          if (flags.o && !status) {
            if (grep_ncivho(match, line, line_number, flags, filename,
                            many_files, preg))
              match_count++;
          } else {
            if (grep_nceivh(!status, line, line_number, flags, filename,
                            many_files))
              match_count++;
          }
        }
      }
    }
    if (!flags.l) {
      if (flags.c && !flags.v) {
        grep_cvh(flags, many_files, filename, match_count);
      } else if (flags.c && flags.v) {
        grep_cvh(flags, many_files, filename, unmatch_count);
      }
    } else {
      if (!checker && flags.c) {
        grep_cvh(flags, many_files, filename, 0);
      }
    }
    free(line);
    fclose(fp);
  } else {
    if (!flags.s) {
      fprintf(stderr, "grep: ");
      perror(filename);
    }
  }
}

int check_many_files(int argc, char *argv[]) {
  int many_files = 0;
  int files_count = 0;

  for (int i = 1; i < argc;) {
    if ((argv[i][0] == '-') && (i + 1 != argc)) {
      if (argv[i][1] == 'e' && strlen(argv[i]) > 2) {
        i++;
      } else if (argv[i + 1][0] != '-') {
        i += 2;
      } else {
        i++;
      }
    } else {
      i++;
      files_count++;
      if (files_count > 1) {
        many_files = 1;
        break;
      }
    }
  }
  return many_files;
}

void reg_comp(regex_t *preg, char *pat, t_Flags flags) {
  // if (preg != NULL) regfree(preg);
  if (regcomp(preg, pat, flags.e)) {
    fprintf(stderr, "regex compile was faild\n");
  }
}

void file_pattern_hendler(int argc, char *argv[], t_Flags flags) {
  regex_t preg_storage;
  regex_t *preg = &preg_storage;

  int many_files = check_many_files(argc, argv);

  for (int i = 1; i < argc;) {
    if ((argv[i][0] == '-') && (i + 1 != argc - 1)) {
      if (argv[i][1] == 'e' && strlen(argv[i]) > 2) {
        reg_comp(preg, flags.patterns, flags);
        i++;
      } else if (argv[i + 1][0] != '-') {
        if (flags.pattern_c != 0) {
          if (flags.patterns[0] == '\0') {
            reg_comp(preg, ".", flags);
          } else {
            reg_comp(preg, flags.patterns, flags);
          }
        } else {
          if (*argv[i + 1] != '\0') {
            if (*argv[i + 1] == '.' && flags.check_empty != 0)
              flags.check_empty = 1;
            reg_comp(preg, argv[i + 1], flags);
          } else {
            flags.check_empty = 0;
            reg_comp(preg, ".", flags);
          }
        }
        i += 2;
      } else
        i++;
    } else {
      grep(argv[i], flags, preg, many_files);
      i++;
    }
  }
  regfree(preg);
}

void usage() {
  printf("usage: grep [options] template [file_name]\n");
  exit(2);
}

void stradd_new(t_Flags *flags, char *pattern) {
  strcpy(flags->patterns, pattern);
  strcat(flags->patterns, "\0");
}

void stradd(t_Flags *flags, char *pattern) {
  strcat(flags->patterns, "|");
  strcat(flags->patterns, pattern);
  strcat(flags->patterns, "\0");
}

void set_patterns_string(char *pattern, t_Flags *flags) {
  if (*pattern != '\0') {
    if (*pattern == '.' && flags->check_empty != 0) flags->check_empty = 1;
    flags->pattern_c += 1;
    size_t pat_len = strlen(pattern);
    if (flags->pattern_c == 1) {
      flags->patterns = (char *)xmalloc(sizeof(char *) * pat_len + 1);
      stradd_new(flags, pattern);
    } else if (flags->pattern_c > 1) {
      flags->patterns =
          (char *)xrealloc(flags->patterns, sizeof(char *) * pat_len + 2);
      stradd(flags, pattern);
    }
  } else {
    flags->check_empty = 0;
    flags->pattern_c += 1;
    if (flags->pattern_c == 1) {
      flags->patterns = (char *)xmalloc(sizeof(char *) * 2);
      stradd_new(flags, ".");
    } else if (flags->pattern_c > 1) {
      flags->patterns = (char *)xrealloc(flags->patterns, sizeof(char *) * 3);
      stradd(flags, ".");
    }
  }
}

int set_patterns_file(char *filename, t_Flags *flags) {
  FILE *fp = fopen(filename, "r");
  if (fp != NULL) fclose(fp);
  fp = fopen(filename, "r");
  if (errno) {
    if (!flags->s) {
      fprintf(stderr, "grep: ");
      perror(filename);
      exit(2);
    }
  } else {
    char *line = 0;
    size_t len = 0;
    while (getline(&line, &len, fp) != -1) {
      size_t pat_len = strlen(line);
      if ((*line == '.' || (line[0] == '.' && line[1] == '\n')) &&
          flags->check_empty != 0)
        flags->check_empty = 1;
      if (line[pat_len - 1] != '\n') {
        set_patterns_string(line, flags);
      } else {
        flags->pattern_c++;
        if (flags->pattern_c == 1) {
          if (*line == '\n') {
            flags->check_empty = 0;
            flags->patterns = (char *)xmalloc(sizeof(char *) * 2);
            stradd_new(flags, ".");
          } else {
            flags->patterns = (char *)xmalloc(sizeof(char *) * pat_len);
            strncpy(flags->patterns, line, pat_len - 1);
            strcat(flags->patterns, "\0");
          }
        } else if (flags->pattern_c > 1) {
          if (*line == '\n') {
            flags->check_empty = 0;
            flags->patterns =
                (char *)xrealloc(flags->patterns, sizeof(char *) * 3);
            stradd(flags, ".");
          } else {
            line[strcspn(line, "\n")] = 0;
            flags->patterns =
                (char *)xrealloc(flags->patterns, sizeof(char *) * pat_len + 1);
            strcat(flags->patterns, "|");
            char *tmp = strdup(line);
            strcat(flags->patterns, tmp);
            free(tmp);
            strcat(flags->patterns, "\0");
          }
        }
      }
    }
    if (line != NULL) free(line);
  }
  fclose(fp);
  return 1;
}

void flags_set_value(int ch, t_Flags *flags) {
  switch (ch) {
    case 'e':
      set_patterns_string(optarg, flags);
      flags->e |= REG_EXTENDED;
      break;
    case 'i':
      flags->e |= REG_ICASE;
      break;
    case 'v':
      flags->v = 1;
      break;
    case 'c':
      flags->c = 1;
      flags->n = 0;
      break;
    case 'n':
      flags->n = 1;
      break;
    case 'l':
      flags->l = 1;
      break;
    case 's':
      flags->s = 1;
      break;
    case 'h':
      flags->h = 1;
      break;
    case 'f':
      if (set_patterns_file(optarg, flags)) flags->e |= REG_EXTENDED;
      break;
    case 'o':
      flags->o = 1;
      break;
    default:
      usage();
  }
}

void *xmalloc(size_t size) {
  void *ptr = malloc(size);
  if (!ptr) {
    exit(errno);
  }
  return ptr;
}

void *xrealloc(char *pat, size_t size) {
  void *ptr = realloc(pat, size);
  if (!ptr) {
    exit(errno);
  }
  return ptr;
}

void flags_hendler(int argc, char *argv[], t_Flags *flags) {
  int ch;
  flags->pattern_c = 0;
  flags->check_empty = -1;
  while ((ch = getopt_long(argc, argv, "e:ivclnhsf:o", NULL, NULL)) != -1) {
    flags_set_value(ch, flags);
  };
}