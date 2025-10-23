# ungpt

[![CI](https://github.com/ryouze/ungpt/actions/workflows/ci.yml/badge.svg)](https://github.com/ryouze/ungpt/actions/workflows/ci.yml)
[![Release](https://github.com/ryouze/ungpt/actions/workflows/release.yml/badge.svg)](https://github.com/ryouze/ungpt/actions/workflows/release.yml)
![Release version](https://img.shields.io/github/v/release/ryouze/ungpt)

ungpt is a cross-platform GUI app that converts ChatGPT's smart punctuation and symbols to plain ASCII.


## Motivation

I often use ChatGPT to correct grammar and spelling errors. It's smarter than a regular spell checker.

However, it often produces text containing "smart" punctuation and typographic Unicode symbols, such as:

* Em dashes (`—`) instead of ASCII dashes (`-`)
* Curly quotes (`“ ” ‘ ’`) instead of straight quotes (`" '`)
* Ellipses (`…`) instead of three dots (`...`)

Manually fixing these with Find & Replace is tedious. I do it so frequently that I decided to build a small app to automate the process.

All you have to do is paste text from ChatGPT and copy the clean result.


## Features

- Written in modern C++ (C++20).
- Comprehensive documentation with Doxygen-style comments.
- Automatic third-party dependency management using CMake's [FetchContent](https://www.foonathan.net/2022/06/cmake-fetchcontent/).
- No missing STL headers thanks to [header-warden](https://github.com/ryouze/header-warden).


## Tested Systems

This project has been tested on the following systems:

- macOS 15.7 (Sequoia)
<!-- - Manjaro 24.0 (Wynsdey)
- Windows 11 23H2 -->

Automated testing is also performed on the latest versions of macOS, GNU/Linux, and Windows using GitHub Actions.


## Pre-built Binaries

Pre-built binaries are available for macOS (ARM64), GNU/Linux (x86_64), and Windows (x86_64). You can download the latest version from the [Releases](../../releases) page.

To remove macOS quarantine, use the following commands:

```sh
xattr -d com.apple.quarantine ungpt-macos-arm64.app
chmod +x ungpt-macos-arm64.app
```

On Windows, the OS might complain about the binary being unsigned. You can bypass this by clicking on "More info" and then "Run anyway".


## Requirements

To build and run this project, you'll need:

- C++20 or higher
- CMake


## Build

Follow these steps to build the project:

1. **Clone the repository**:

    ```sh
    git clone https://github.com/ryouze/ungpt.git
    ```

2. **Generate the build system**:

    ```sh
    cd ungpt
    mkdir build && cd build
    cmake ..
    ```

    The default configuration (`cmake ..`) is recommended for most users and is also used by CI/CD to build the project.

    However, the build configuration can be customized using the following options:

    - `ENABLE_COMPILE_FLAGS` (default: ON) - Enables strict compiler warnings and treats warnings as errors. When ON, any code warnings will cause compilation to fail, ensuring clean code. When OFF, warnings are allowed and compilation continues. Disable if you encounter compilation issues due to warnings (although CI should catch such issues).
    - `ENABLE_STRIP` (default: ON) - Strips debug symbols from Release builds to reduce binary size. When ON, creates smaller executables but removes debugging information. When OFF, keeps full debugging symbols (useful for debugging crashes). Only affects Release builds.
    - `ENABLE_LTO` (default: ON) - Enables Link Time Optimization for Release builds, producing smaller and faster binaries. When ON, performs cross-module optimizations during linking. When OFF, skips LTO (faster compilation but larger/slower binary). Automatically disabled if compiler doesn't support LTO.
    - `ENABLE_CCACHE` (default: ON) - Optionally uses ccache to cache compilation results for faster rebuilds. When ON and ccache is installed, dramatically speeds up recompilation. When ON but ccache not installed, silently continues without ccache. When OFF, never uses ccache even if available.
    - `BUILD_TESTS` (default: OFF) - Builds unit tests alongside the main executable. When ON, creates test binaries that can be run with `ctest`. When OFF, skips test compilation for faster builds. See [Testing](#testing) for usage.

    Example command to disable strict compile flags and LTO:

    ```sh
    cmake .. -DENABLE_COMPILE_FLAGS=OFF -DENABLE_LTO=OFF
    ```

3. **Compile the project**:

    To compile the project, use the following command:

    ```sh
    cmake --build . --parallel
    ```

After successful compilation, you can run the program using `./ungpt` (`open ungpt.app` on macOS). However, it is highly recommended to install the program, so that it can be run from any directory. Refer to the [Install](#install) section below.

> [!TIP]
> The mode is set to `Release` by default. To build in `Debug` mode, use `cmake .. -DCMAKE_BUILD_TYPE=Debug`.


## Install

If not already built, follow the steps in the [Build](#build) section and ensure that you are in the `build` directory.

To install the program, use the following command:

```sh
sudo cmake --install .
```

On macOS, this will install the program to `/Applications`. You can then run `ungpt.app` from the Launchpad, Spotlight, or by double-clicking the app in Finder.


## Usage

1. Copy text from ChatGPT or another language model.
2. Click **Paste** to insert the text into the input field.
3. Click **Normalize** to modify the text **in place**.
4. Click **Copy** to copy the normalized text to your clipboard.

> [!TIP]
> You do not need to manually select the text area to use the buttons. The text area is only for displaying or optionally editing the text.


## Development

### Debugging

To build with runtime sanitizers and keep debugging symbols, use the following configuration in the `build` directory:

```sh
cmake .. \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DENABLE_STRIP=OFF \
  -DCMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE=OFF \
  -DCMAKE_CXX_FLAGS_RELWITHDEBINFO="-fsanitize=address,undefined -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO="-fsanitize=address,undefined"
cmake --build . --parallel
```

Then, run the program under `lldb` (macOS):

```sh
lldb ./ungpt.app/Contents/MacOS/ungpt
run
```

When a sanitizer detects a fault, it will stop execution and print a full stack trace. Use this to pinpoint the root cause of the issue. You can also use `lldb` commands like `bt` (backtrace) to inspect the call stack.


### Logging

The application uses [spdlog](https://github.com/gabime/spdlog) for logging.

For debug builds, the logging level is set to `debug` by default, which is very verbose. For non-debug (Release) builds, the logging level is kept at the default `info` level, which only shows important messages and warnings.

> [!NOTE]
> While `cmake/External.cmake` defines `SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG` in debug builds, this only affects compile-time filtering. The runtime verbosity is controlled by `spdlog::set_level()`, which is called in `main.cpp` to enable `debug`-level messages during execution.


### Testing

Tests are included in the project but are not built by default.

To enable and build the tests manually, run the following commands from the `build` directory:

```sh
cmake .. -DBUILD_TESTS=ON
cmake --build . --parallel
ctest --verbose
```


## Credits

**Libraries:**
- [Simple and Fast Multimedia Library](https://github.com/SFML/SFML) - Windowing, graphics, input, etc.
- [Dear ImGui](https://github.com/ocornut/imgui) - Immediate-mode GUI.
- [ImGui-SFML](https://github.com/SFML/imgui-sfml) - ImGui-to-SFML binding.
- [snitch](https://github.com/snitch-org/snitch) - Unit-testing framework.
- [spdlog](https://github.com/gabime/spdlog) - Logging library.

**Graphics:**
- [Spelling Alphabet](https://macosicons.com/#/u/plantaclaus) - Application icon.


## Contributing

All contributions are welcome.


## License

This project is licensed under the MIT License.
