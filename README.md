# C++ Code Samples

A collection of simple code samples and examples for projects in C++.

## Contents
  * `hello_world`
  * `bitset`
  * `expression_tree`
  * `array_view`
  * `scoped_ptr`
  * `list`

## Building

> :information_source: These instructions describe how to run CMake commands manually. You can simply open
the `CMakeLists.txt` in any IDE that supports CMake, e.g., Qt Creator, Visual Studio, KDevelop, CLion.

1. Install `git`, a C++ compiler, `CMake`, and a build system available as a `CMake` generator
(e.g. `Ninja`, `GNU Make`, `MSBuild`, `NMake`, `Jom`).
2. Download this repository.
    ```bash
    $ git clone https://gitlab.com/pzlabs/cpp-code-samples.git
    $ cd cpp-code-samples
    ```
3. Run the configuration step in the build directory. Change the name of this directory if needed.
Multiple build directories are supported and encouraged in case of multiple configurations.
    ```bash
    $ cmake -B build-release -D CMAKE_BUILD_TYPE=Release
    ```
    The following [configurations](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html#variable:CMAKE_BUILD_TYPE)
    specified by the `CMAKE_BUILD_TYPE` option are supported by default: `Debug`, `MinSizeRel`, `RelWithDebInfo`, `Release`.
4. Build every target.
    ```bash
    $ cmake --build build-release
    ```

    To build a specific executable, run `cmake --build ${MY_BUILD_DIR} --target ${MY_TARGET}`:
    ```bash
    $ cmake --build build-release --target hello_world
    ```

5. You can cleanup binaries in the build directory by running the `clean` target:
    ```bash
    $ cmake --build build-release --target clean
    ```

## Running

After being built, the binaries can be found in the build directory:
```bash
$ build/hello_world
