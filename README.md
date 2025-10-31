# cpm-arg

A lightweight, header-only C command parser designed for small CLI tools.  
It provides a simple API to define **commands**, **subcommands**, and **typed arguments** using plain C constructs.

---

## ✨ Features

- Header-only (just include `cpm_cmd.h`)
- Supports subcommands and default subcommands
- Typed arguments: `int`, `string`, and `flag`
- Inline utilities for argument lookup
- Simple macro-based DSL for defining commands
- Easily integrates with any C project or Nix build setup

---

## 🚀 Example

```c
#include "cpm_cmd.h"

int run_handler(CpmCmdArgs *args) {
    CpmCmdArgInt *count = args_get_int(args, "--count");
    CpmCmdArgFlag *fast = args_get_flag(args, "--fast");
    printf("Running %d times%s\n", count->value, fast->value ? " fast!" : ".");
    return 0;
}

int main(int argc, char **argv) {
    CpmCmd cmd_def = cmd(
        subcmd(.name = "run", .handler = run_handler, .defaultArgs = args(
            arg_int("--count", 1),
            arg_flag("--fast", false)
        ))
    );

    CpmSubCmdArgs parsed = parse_subcmd_args(&cmd_def, argc, argv);
    if (parsed.subcmd && parsed.subcmd->handler) {
        return parsed.subcmd->handler(parsed.args);
    }
    fprintf(stderr, "Unknown command.\n");
    return 1;
}
````

Run:

```bash
./myapp run --count 5 --fast
# Output: Running 5 times fast!
```

---

## 🧪 Testing

This project uses [Criterion](https://github.com/Snaipe/Criterion) for unit testing.

Run tests with:

```bash
make test
```

This will:

1. Build and run module tests.
2. Build and execute all Criterion-based tests under `tests/`.

---

## 🧰 Build

```bash
make bin     # Build executable sample
make lib     # Build static library
make install # Install to PREFIX
```

---

## 🧪 Powered by nix-c-modular-template

This project is built on top of the [nix-c-modular-template](https://github.com/SMFloris/nix-c-modular-template) system — a modular C/C++ project template powered by Nix, designed for reproducible builds, clean module structure, automatic `pkg-config` handling and integration with `clangd` + `bear`.

* Use `nix-shell` to enter a fully prepared dev environment with correct compiler flags and dependencies.
* Use `nix-build` to produce a deterministic build of the app (including internal modules) with minimal effort.
* Every module (in `modules/`) follows the same structure, so you can reuse or add modules easily — the template’s modular architecture handles linking and flags for you.

---

## 💡 Contributing

Pull requests, feature ideas, and bug reports are welcome!
Please include Criterion tests for new features.
