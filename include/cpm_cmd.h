#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define cmd(...) ((CpmCmd){.subcmds = (CpmSubCmd[]){__VA_ARGS__}, \
                           .size = sizeof((CpmSubCmd[]){__VA_ARGS__}) / sizeof(CpmSubCmd)})

#define subcmd(...) ((CpmSubCmd){__VA_ARGS__})
#define args(...) ((CpmCmdArgs){.args = (CpmCmdArg[]){__VA_ARGS__}, \
                                .size = sizeof((CpmCmdArg[]){__VA_ARGS__}) / sizeof(CpmCmdArg)})

typedef enum {
  CPM_ARG_INT,
  CPM_ARG_STRING,
  CPM_ARG_FLAG
} CpmCmdArgType;

typedef struct {
  const char *name;
  int value;
} CpmCmdArgInt;

typedef struct {
  const char *name;
  const char *value;
} CpmCmdArgString;

typedef struct {
  const char *name;
  bool value;
} CpmCmdArgFlag;

typedef union {
  CpmCmdArgInt intArg;
  CpmCmdArgString stringArg;
  CpmCmdArgFlag flagArg;
} CpmCmdArgUnion;

typedef struct {
  CpmCmdArgType type;
  CpmCmdArgUnion data;
} CpmCmdArg;

typedef struct {
  CpmCmdArg *args;
  size_t size; // number of elements
} CpmCmdArgs;

typedef struct {
  const char *name;
  int (*handler)(CpmCmdArgs *args);
  CpmCmdArgs defaultArgs;
} CpmSubCmd;

typedef struct {
  CpmSubCmd *subcmds;
  size_t size;
} CpmCmd;

typedef struct {
  CpmSubCmd *subcmd;
  CpmCmdArgs *args;
} CpmSubCmdArgs;

static inline CpmCmdArg arg_int(const char *name, int value) {
  CpmCmdArg a;
  a.type = CPM_ARG_INT;
  a.data.intArg.name = name;
  a.data.intArg.value = value;
  return a;
}
static inline CpmCmdArg arg_flag(const char *name, bool value) {
  CpmCmdArg a;
  a.type = CPM_ARG_FLAG;
  a.data.flagArg.name = name;
  a.data.flagArg.value = value;
  return a;
}
static inline CpmCmdArg arg_string(const char *name, const char *value) {
  CpmCmdArg a;
  a.type = CPM_ARG_STRING;
  a.data.stringArg.name = name;
  a.data.stringArg.value = value;
  return a;
}

const char *arg_name(CpmCmdArg *arg) {
  if (!arg) {
    return nullptr;
  }
  switch (arg->type) {
  case CPM_ARG_INT:
    return arg->data.intArg.name;
  case CPM_ARG_FLAG:
    return arg->data.flagArg.name;
  case CPM_ARG_STRING:
    return arg->data.stringArg.name;
  default:
    break;
  }
  return nullptr;
}

CpmSubCmd *subcmd_find(CpmCmd *cmd, const char *name) {
  if (!cmd) {
    return nullptr;
  }
  for (size_t i = 0; i < cmd->size; i++) {
    const char *cmdName = cmd->subcmds[i].name;
    if (strcmp(cmdName, name) == 0) {
      return &cmd->subcmds[i];
    }
  }
  return nullptr;
}

CpmCmdArg *args_get(CpmCmdArgs *args, size_t index) {
  if (!args) {
    return nullptr;
  }
  if (index >= args->size) {
    return nullptr;
  }

  return &args->args[index];
}

CpmCmdArg *args_find(CpmCmdArgs *args, const char *name) {
  if (!args) {
    return nullptr;
  }
  for (size_t i = 0; i < args->size; i++) {
    auto arg = args_get(args, i);
    auto argName = arg_name(arg);
    if (strcmp(argName, name) == 0) {
      return arg;
    }
  }
  return nullptr;
}

CpmCmdArgFlag *args_get_flag(CpmCmdArgs *args, const char *name) {
  auto arg = args_find(args, name);
  if (arg) {
    if (arg->type != CPM_ARG_FLAG) {
      return nullptr;
    }
    return &arg->data.flagArg;
  }
  return nullptr;
}

CpmCmdArgInt *args_get_int(CpmCmdArgs *args, const char *name) {
  auto arg = args_find(args, name);
  if (arg) {
    if (arg->type != CPM_ARG_INT) {
      return nullptr;
    }
    return &arg->data.intArg;
  }
  return nullptr;
}

CpmCmdArgString *args_get_string(CpmCmdArgs *args, const char *name) {
  auto arg = args_find(args, name);
  if (arg) {
    if (arg->type != CPM_ARG_STRING) {
      return nullptr;
    }
    return &arg->data.stringArg;
  }
  return nullptr;
}

char **split_equals_argv(int argc, char **argv, int *out_argc) {
  int cap = argc * 2, n = 0;
  char **out = (char **)malloc(sizeof(char *) * cap);
  for (int i = 0; i < argc; i++) {
    char *eq = strchr(argv[i], '=');
    if (!eq) {
      out[n++] = strdup(argv[i]);
      continue;
    }

    size_t klen = (size_t)(eq - argv[i]);
    char *k = (char *)malloc(klen + 1);
    memcpy(k, argv[i], klen);
    k[klen] = '\0';

    char *v = strdup(eq + 1);
    out[n++] = k;
    out[n++] = v;
  }
  *out_argc = n;
  return out;
}

CpmCmdArgs *parse_args(CpmCmdArgs *args, int argc, char **argv) {
  int splitArgc = 0;
  auto splitArgv = split_equals_argv(argc, argv, &splitArgc);

  for (int i = 1; i < argc; i++) {
    const char *s = splitArgv[i];
    auto foundArg = args_find(args, s);
    if (foundArg) {
      switch (foundArg->type) {
      case CPM_ARG_FLAG:
        foundArg->data.flagArg.value = true;
        continue;
      case CPM_ARG_INT:
        if (i + 1 >= argc) {
          continue;
        }
        const char *v = splitArgv[i + 1];
        foundArg->data.intArg.value = atoi(v);
        i += 1;
        continue;
      case CPM_ARG_STRING:
        if (i + 1 >= argc) {
          continue;
        }
        v = splitArgv[i + 1];
        foundArg->data.stringArg.value = splitArgv[i + 1];
        i += 1;
        continue;
      }
    }
  }

  return args;
}

CpmSubCmdArgs parse_subcmd_args(CpmCmd *cmd, int argc, char **argv) {
  auto defaultSubCmd = subcmd_find(cmd, "");
  if (argc < 2) {
    if (defaultSubCmd) {
      return (CpmSubCmdArgs){.subcmd = defaultSubCmd, .args = nullptr};
    }
    return (CpmSubCmdArgs){.subcmd = nullptr, .args = nullptr};
  }

  auto subcmdName = argv[1];
  auto found_subcmd = subcmd_find(cmd, subcmdName);
  if (!found_subcmd) {
    found_subcmd = defaultSubCmd;
    return (CpmSubCmdArgs){.subcmd = found_subcmd, .args = parse_args(&found_subcmd->defaultArgs, argc, argv)};
  }
  return (CpmSubCmdArgs){.subcmd = found_subcmd, .args = parse_args(&found_subcmd->defaultArgs, argc - 1, argv + 1)};
}
