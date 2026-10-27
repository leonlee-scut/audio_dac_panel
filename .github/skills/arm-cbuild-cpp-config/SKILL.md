---
name: arm-cbuild-cpp-config
description: "Use when configuring or repairing VS Code C/C++ IntelliSense for an ARM embedded CMSIS project: find generated out/**/Debug/*.cbuild.yml, map cbuild settings into a c_cpp_properties.json configuration named ARM, and validate compiler, defines, include paths, and compile database."
argument-hint: "Optional target/context name, for example STM32F411CEUx"
---

# Configure ARM C/C++ IntelliSense from cbuild

## Goal

Create or complete the `ARM` configuration in `.vscode/c_cpp_properties.json` using the active CMSIS cbuild output as the source of truth. Preserve all other configurations.

## Procedure

1. Locate candidate generated build descriptions under `out`:

   ```powershell
   Get-ChildItem -Path out -Recurse -File -Filter *.cbuild.yml |
     Where-Object { $_.Directory.Name -eq 'Debug' }
   ```

   The generated filename may contain both project context and target, for example `fw.Debug+STM32F411CEUx.cbuild.yml`; do not assume the filename itself is exactly `*.Debug.cbuild.yml`.

2. Select the correct candidate. Read its `context`, `device`, `compiler`, `define`, `add-path`, `misc`, and `output` fields. Compare its target/context against `audio_dac_panel.csolution.yml` and any supplied active target argument. If several Debug cbuild files exist and the active target cannot be determined, ask the user which target to configure; do not silently choose the first file.

3. Find the matching compilation database in the same generated output directory, normally `compile_commands.json`. Use its compile entries to confirm the actual compiler executable and target-specific options such as `--target`, `-mcpu`, `-mfpu`, and `-mfloat-abi`. The cbuild `compiler` field identifies the toolchain family but may not contain the executable path.

4. Read the current `.vscode/c_cpp_properties.json` before editing. If a configuration named `ARM` exists, update it in place; otherwise add it. Preserve `Win32` and all unrelated configurations.

5. Populate `ARM` from the selected build context:
   - `includePath`: map each cbuild `add-path` entry. Resolve paths relative to the workspace/cbuild output as appropriate; use `${workspaceFolder}/...` for repository paths and `${env:CMSIS_PACK_ROOT}/...` for pack paths. Include the project, generated RTE, device, HAL, CMSIS, RTOS, and middleware directories listed by cbuild. Avoid `${workspaceFolder}/**` as a substitute for the generated include list.
   - `defines`: copy unique C/C++ symbols from cbuild `define`. Do not copy assembler-only `define-asm` values unless they are also in `define`; omit unrelated host defines such as `UNICODE` unless the selected build explicitly has them.
   - `compilerPath`: use the executable found in the matching compile command, with normalized path separators.
   - `compilerArgs`: include the target architecture and ABI options used by that command, especially `--target`, CPU, FPU, and float ABI. Avoid copying output/dependency flags such as `-o`, `-MF`, or `-MT`.
   - `compileCommands`: point to the matching generated compilation database. This provides per-file flags and should not point at a database for a different target.
   - `cStandard` and `cppStandard`: use standards explicitly specified in cbuild `misc` or compile commands. If C++ has no explicit standard, preserve a valid existing setting or the toolchain default rather than inventing one.
   - `intelliSenseMode`: select the Windows Clang ARM mode appropriate for the Armclang executable.
   - `browse.path`: keep it focused on workspace source roots and relevant pack roots; avoid duplicating a recursive workspace glob.

6. Edit only `.vscode/c_cpp_properties.json` for this task. Do not edit generated cbuild files or build output. Use the existing JSONC style if comments are present.

## Validation

1. Run VS Code diagnostics on `.vscode/c_cpp_properties.json` and resolve any JSON or configuration errors.
2. Verify that the selected cbuild file and `compile_commands.json` both exist and belong to the same target/context.
3. Compare the `ARM` include paths, defines, compiler executable, and architecture arguments against the selected cbuild/compile command. Confirm that unrelated configurations remain unchanged.
4. If the build metadata changed or the user requests a build check, run the existing cbuild build command. Otherwise, configuration validation is sufficient; do not rebuild firmware solely to validate IntelliSense metadata.
5. Report the cbuild context used, the ARM settings updated, and any unresolved pack-root or compiler-path assumptions.
