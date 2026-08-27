# Kernel source navigation

The workspace already uses the clangd VS Code extension. The existing
fallback configuration resolves kernel headers, but a complete kernel compile
database is also required for navigation from a declaration to its C-file
implementation (for example `misc_register` to `drivers/char/misc.c`).

On the Linux build machine, from `/root/LubanCat_SDK/kernel`, run:

```sh
python3 scripts/clang-tools/gen_compile_commands.py -o compile_commands.linux.json
```

Then, in VS Code opened with `drivers.code-workspace`, run the task
`clangd: sync kernel compile database`. It rewrites the Linux `/root/...`
paths for the SMB workspace and creates `kernel/compile_commands.json`.

Reload the VS Code window once after the first sync. Clangd indexes the kernel
in the background; the initial pass over SMB can take several minutes. After
that, use Ctrl+Click or F12 on a kernel symbol. Use Ctrl+Alt+Click or
Ctrl+Shift+F12 when you want all declarations/implementations.
