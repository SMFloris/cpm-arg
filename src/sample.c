#include "cpm_cmd.h"
#include <stdio.h>

const char *init_usage = "Init usage";
int init_handler(CpmCmdArgs *args) {
  auto helpFlag = args_get_flag(args, "--help");
  if (helpFlag && helpFlag->value == true) {
    printf("Usage: %s", init_usage);
    return 0;
  }
  // TODO
  return 0;
}

int default_handler(CpmCmdArgs *args) {
  if (!args) {
    printf("Usage: %s", init_usage);
    return 0;
  }
  auto helpFlag = args_get_flag(args, "--help");
  if (helpFlag && helpFlag->value == true) {
    printf("Usage: %s", init_usage);
    return 0;
  }

  auto versionFlag = args_get_flag(args, "--version");
  if (versionFlag && versionFlag->value == true) {
    printf("Version: 1.0");
    return 0;
  }

  return 0;
}

const char *help_usage = "Show usage for base";
int help_handler(CpmCmdArgs *args) {
  printf("Usage: %s", help_usage);
  return 0;
}

int main(int argc, char **argv) {
  auto cmd = cmd(
      subcmd(.name = "", .defaultArgs = args(arg_flag("--help", false), arg_flag("--version", false)), .handler = default_handler),
      subcmd(.name = "init", .defaultArgs = args(arg_flag("--help", false)), .handler = init_handler));

  auto subcmd_args = parse_subcmd_args(&cmd, argc, argv);
  if (subcmd_args.subcmd) {
    return subcmd_args.subcmd->handler(subcmd_args.args);
  }
  return 0;
}
