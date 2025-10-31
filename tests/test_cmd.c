#include "cpm_cmd.h"
#include <criterion/criterion.h>

static int dummy_handler(CpmCmdArgs *args) { return 0; }

Test(parse_subcmd_args, finds_named_subcmd) {
  CpmCmd cmd_def = cmd(
      subcmd(.name = "run", .handler = dummy_handler, .defaultArgs = args(arg_flag("--fast", false), arg_int("--count", 0), arg_string("--msg", ""))));

  char *argv[] = {"prog", "run", "--fast", "--count", "5", "--msg", "hello"};
  CpmSubCmdArgs result = parse_subcmd_args(&cmd_def, 7, argv);

  cr_assert_not_null(result.subcmd);
  cr_assert_str_eq(result.subcmd->name, "run");

  CpmCmdArgFlag *fast = args_get_flag(result.args, "--fast");
  CpmCmdArgInt *count = args_get_int(result.args, "--count");
  CpmCmdArgString *msg = args_get_string(result.args, "--msg");

  cr_assert_not_null(fast);
  cr_assert_not_null(count);
  cr_assert_not_null(msg);
  cr_assert(fast->value);
  cr_assert_eq(count->value, 5);
  cr_assert_str_eq(msg->value, "hello");
}

Test(parse_subcmd_args, falls_back_to_default_subcmd) {
  CpmCmd cmd_def = cmd(
      subcmd(.name = "", .handler = dummy_handler, .defaultArgs = args(arg_flag("--debug", false))),
      subcmd(.name = "run", .handler = dummy_handler, .defaultArgs = args()));

  char *argv[] = {"prog", "--debug"};
  CpmSubCmdArgs result = parse_subcmd_args(&cmd_def, 2, argv);

  cr_assert_not_null(result.subcmd);
  cr_assert_str_eq(result.subcmd->name, "");

  CpmCmdArgFlag *dbg = args_get_flag(result.args, "--debug");
  cr_assert_not_null(dbg);
  cr_assert(dbg->value);
}

Test(parse_subcmd_args, unknown_subcmd_uses_default) {
  CpmCmd cmd_def = cmd(
      subcmd(.name = "", .handler = dummy_handler, .defaultArgs = args(arg_flag("--dry", false))),
      subcmd(.name = "build", .handler = dummy_handler, .defaultArgs = args()));

  char *argv[] = {"prog", "unknown", "--dry"};
  CpmSubCmdArgs result = parse_subcmd_args(&cmd_def, 3, argv);

  cr_assert_not_null(result.subcmd);
  cr_assert_str_eq(result.subcmd->name, "");

  CpmCmdArgFlag *dry = args_get_flag(result.args, "--dry");
  cr_assert_not_null(dry);
  cr_assert(dry->value);
}
