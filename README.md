# Firmware-ARM

ARM microcontroller firmware for the Courier.

## Installing

### Tools

Developing firmware for this repository will require a full ARM GCC toolchain
for embedded systems and the `newlib` standard library for embedded devices. A
recent version of GNU make is also required to manage compilation. The firmware
should not be compiled manually!  These can be obtained from your system package
manager or installed manually from the sourcecode archives on the websites.

### Common

1. Install git
2. Clone repo
3. Download (dont' clone!) [vendor
   libs](https://github.com/JHU-Delivery-Robot/lib-vendor)
4. Unzip downlaoded vendor libs, rename to `vendor` and copy (or symlink) into
   cloned repository.

### Windows

1. Acquire and install MSYS2 from the [official website](https://www.msys2.org/)
2. Open the `MSYS / MSYS2` terminal environment from the installed shortcuts
3. Edit `/etc/nsswitch.conf` using `nano /etc/nsswitch.conf` and add `db_home:
   windows` to the end.
3. Update the distribution using `pacman -Syu`
5. Close the shell when prompted to do so and run `pacman -Syu` again after
   reopening it.
6. Install the required packages using `pacman -S mingw-w64-x86_64-arm-none-eabi-gcc 
   mingw-w64-x86_64-arm-none-eabi-gdb  mingw-w64-x86_64-arm-none-eabi-newlib
   make git`
7. Use the `MSYS / MINGW64` shell for all operations. By default it will open in
   the `/users/[your username]` folder on your computer. Replace all invocations
   of `make` with `mingw32-make`.

### Linux

Use the system package manager to obtain the following packages:

- `arm-none-eabi-gdb`
- `arm-none-eabi-gcc`
- `arm-none-eabi-newlib`
- `make`
- `git`

Ensure that you also obtain `arm-none-eabi-binutils`. It should be included as a
dependency of `arm-none-eabi-gcc` in most sane distributions.

**Example PM Commands**

- Ubuntu/Debian Distros: `sudo apt install gcc-arm-none-eabi newlib-arm-none-eabi
    gdb-arm-none-eabi make`
- Arch: `pacman -S arm-none-eabi-gcc arm-none-eabi-gdb arm-none-eabi-newlib
    make`

Note: Ubuntu/Debian package may be hopelessly old. Please use the most recent
LTS version of Ubuntu to mitigate this.

## Development guidelines

## Compiling

Note: GNU make must be used. BSD make and other implementation of make will _not_
work. `make` must be run in the top level directory. Even though subdirs have
makefile, they are recursive makefile and rely on variables exported in upper
level makefiles.

- `make all` to compile all firmware and examples
- Regular targets:
  - TBD
- Special targets
 - `make compiledb` - to make the compilation info file for vscode and other LSP
    supporting IDEs. Must be rerun after any makefile change to propagate the
    changes
 - `make clean` - remove all compilation artefacts except for vendor libraries
 - `make libclean` - remove compiled vendor libs
- Options:
  - `CONFIG=[profile]` - Applies sets of compilation flags for specific usecases.
    Valid profiles are:
    - `release` - (default) enables aggressive optimizations
    - `debug` - disables all optimizations (except constant folding) and emits
       debug symbols
 - `NO_LTO=true` - if passed, link-time optimization will be disabled for the
   release profile. Can help with debugging situations where the compiler
   optimizes out necessary functions.

### Style

This project  will be following the [Linux Kernel
Styleguide](https://www.kernel.org/doc/html/latest/process/coding-style.html)
with a few modifications:

- Ignore the function header comment requirements. A different header will be
    used for this project.
- All indentation should be four spaces not 8. The ban on tabs still holds for
    visual alignment purposes.
  - This can be easily adhered to using a clang-format config
- Variables and constants
    - Rigorously mark variables that don't change with `const`. Rationale: this
      enables better compiler optimizations and code placement.
    - Mark globals used to pass state in and out of ISRs as `volatile` in
      general, unless they are read-only in the ISR. Rationale: This prevents
      modifications form being optimized out.
    - Use macro defines instead of magic constants. Rationale: readability.
- Functions
    - Functions may have the open brace on the same line as the signature.
      Rationale: consistent formatting with everything else.
    - All functions should be correctly scoped. This means that helpers using
      in a single compilation unit should not be declared in header. Instead
      forward declare and prepend with `static` to ensure they don't get
      exported. Rationale: cluttering global namespace increases the likelihood
      of linker errors.
    - That being said, do not _duplicate code_. If functionality is shared, it
      should be refactored out in to a shared function with a header and
      implementation file. Rationale: maintainability.
    - Do not hand inline function unless absolutely necessary, or hack around
      using a function-like macro. Use the inline keyword. Note that all release
      firmware will be compile with link-time optimization, which will
      inline functions across files. Rationale: readability.

#### Function Header

The function header comment shall be placed at the point the interface is
declared. This is the header file for publicly visible functions and the forward
declaration for internal linkage helper functions. It must follow the following
format:

```
/**
 * Prereqs:
 *   [List of prereqs usch as hardware initialization/assumed state. None if N/A]
 * Side Effects:
 *   [List of sideffects that this function may cause (i.e. global var
 *    modifications. None if N/A]
 * [Short description]
 * @param [argument description if applicable, one line per arg]
 * @return [return descirption if applicable]
 */
 ```

#### Note: Globals

Globals are unavoidable in embedded programming. To make them more bearable to
deal with, some conventions should be followed:

- Avoid if not absolutely needed.
- Create a dedicated implementation file (e.g. `globals.c`) which will instantiate
  all of them and a corresponding header with the necessary `extern` references
  that will be included by all CUs that require useage of globals.
- Every function that modifies globals must list the modification in the
  function header under "Side Effects". This is to make debugging easier.
