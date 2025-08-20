# Espressif IoT Development Framework

------

## Project

A `project` is a directory that contains all the files and configuration to build a single `app` (executable), as well as additional supporting elements such as a partition table, data partitions or filesystem partitions, and a bootloader.

An `app` is an executable that is built by ESP-IDF. A single project will usually build two apps - a "project app" (the main executable, ie your custom firmware) and a "bootloader app" (the initial bootloader program which launches the project app).

```
idf.py create-project <project name>
```

This command creates a new ESP-IDF project. Additionally, the folder where the project will be created in can be specified by the `--path` option.

------

## Components

Components` are modular pieces of standalone code that are compiled into static libraries (.a files) and linked to an app. Some are provided by ESP-IDF itself, others may be sourced from other places.

Inside ```project``` "components" directory contains components that are part of the project. "main" directory is a special component that contains source code for the project itself. "main" is a default name.



Components find path is set using `COMPONENT_DIRS` CMake variable. Defaults to `IDF_PATH/components`, `PROJECT_DIR/components`, and `EXTRA_COMPONENT_DIRS`.

When ESP-IDF is collecting all the components to compile, the search precedence is as follows (from highest to lowest):

- Project components
- Components from `EXTRA_COMPONENT_DIRS`
- Project managed components, downloaded by the IDF Component Manager into `PROJECT_DIR/managed_components`, unless the IDF Component Manager is disabled.
- ESP-IDF components (`IDF_PATH/components`)



When compiling each component, the ESP-IDF build system recursively evaluates its dependencies. This means each component needs to declare the components that it depends on ("requires").

```
idf_component_register(...
                       REQUIRES mbedtls
                       PRIV_REQUIRES console spiffs)
```

- `REQUIRES` should be set to all components whose header files are #included from the *public* header files of this component.
- `PRIV_REQUIRES` should be set to all components whose header files are #included from *any source files* in this component, unless already listed in `REQUIRES`. Also, any component which is required to be linked in order for this component to function correctly.



To avoid duplication, every component automatically requires some "common" IDF components even if they are not mentioned explicitly. Headers from these components can always be included.

The list of common components is: cxx, newlib, freertos, esp_hw_support, heap, log, soc, hal, esp_rom, esp_common, esp_system, xtensa/riscv.

- By default, every component is included in the build.
- If you set the `COMPONENTS` variable to a minimal list of components used directly by your project, then the build will expand to also include required components. The full list of components will be:
  - Components mentioned explicitly in `COMPONENTS`.
  - Those components' requirements (evaluated recursively).
  - The common components that every component depends on.
- Setting `COMPONENTS` to the minimal list of required components can significantly reduce compile times.
- The `MINIMAL_BUILD` build property can be set to `ON`, which acts as a shortcut to configure the `COMPONENTS` variable to include only the `main` component. This means that the build will include only the common components, the `main` component, and all dependencies associated with it, both direct and indirect. If the `COMPONENTS` variable is defined while the `MINIMAL_BUILD` property is enabled, `COMPONENTS` will take precedence.

```
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.

idf_build_set_property(MINIMAL_BUILD ON)
project(blink)
```



```
idf.py create-component <component name>
```

This command creates a new component, which will have a minimum set of files necessary for building. The `-C` option can be used to specify the directory the component will be created in.

------

## CMakeLists Files

### Project CMakeLists File

Each project has a single top-level `CMakeLists.txt` file that contains build settings for the entire project. By default, the project CMakeLists can be quite minimal.

Minimal project:

```
cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(myProject)
```

The inclusion of these three lines, in the order shown above, is necessary for every project:

- `cmake_minimum_required(VERSION 3.16)` tells CMake the minimum version that is required to build the project. ESP-IDF is designed to work with CMake 3.16 or newer. This line must be the first line in the CMakeLists.txt file.
- `include($ENV{IDF_PATH}/tools/cmake/project.cmake)` pulls in the rest of the CMake functionality to configure the project, discover all the components, etc.
- `project(myProject)` creates the project itself, and specifies the project name. The project name is used for the final binary output files of the app - ie `myProject.elf`, `myProject.bin`. Only one project can be defined per CMakeLists file.

### Component CMakeLists Files

Each project contains one or more components. Components can be part of ESP-IDF, part of the project's own components directory, or added from custom component directories.

A component is any directory in the `COMPONENT_DIRS` list which contains a `CMakeLists.txt` file.

The minimal component `CMakeLists.txt` file simply registers the component to the build system using `idf_component_register`:

```
idf_component_register(SRCS "foo.c" "bar.c"
                       INCLUDE_DIRS "include"
                       REQUIRES mbedtls)
```

- `SRCS` is a list of source files (`*.c`, `*.cpp`, `*.cc`, `*.S`). These source files will be compiled into the component library.
- `INCLUDE_DIRS` is a list of directories to add to the global include search path for any component which requires this component, and also the main source files.
- `REQUIRES` is not actually required, but it is very often required to declare what other components this component will use.

A library with the name of the component will be built and linked to the final app.

------

## Project Configuration

### Introduction

`Project configuration` is held in a single file called `sdkconfig` in the root directory of the project. This configuration file is modified via `idf.py menuconfig` to customize the configuration of the project. A single project contains exactly one project configuration.

**Values currently assigned** to the configuration options are then stored in one [sdkconfig file](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c3/api-guides/kconfig/configuration_structure.html#sdkconfig-file) in the project's root folder. This file is specific for the given project and is changed every time user makes a change in any of the configuration option's value. For the purpose of editing the configuration, the [menuconfig tool](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c3/api-guides/kconfig/project-configuration-guide.html#project-configuration-menu) is used. The values of the configuration options are also propagated to the CMake scripts and C code via `sdkconfig.h` and `sdkconfig.cmake` files.

For the purpose of setting **user-defined default values** for configuration options, the [sdkconfig.defaults file](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c3/api-guides/kconfig/configuration_structure.html#sdkconfig-defaults-file) can be used. This approach is also preferred in the situation when the project is under version control. This file is used to replace default values set by `Kconfig` files when the project is configured the first time and no `sdkconfig` is present, but those values can still be changed by the user via configuration tools (e.g. `idf.py menuconfig`).

If the user wants to redefine the default values of the configuration options set in `Kconfig` files, the `sdkconfig.defaults` file can be used. The values set there overwrite those from `Kconfig` files, but user can still change those via `idf.py menuconfig`. Those user-set values for the configuration options will be stored in the `sdkconfig` file, `sdkconfig.defaults` will remain unchanged.

Independently on the tool used, configuration values are loaded in the following order, values from later steps **may override** those from previous ones. These steps are done automatically and are described here only in order to explain the behavior:

1. Default configuration option values from `Kconfig` files are loaded, together with the relationships between them.
2. If `sdkconfig.defaults` file is found, default values from this file are loaded.
3. If present, `sdkconfig` file is loaded.

When the configuration is saved, the values in the `sdkconfig` file are updated, as well as those in `sdkconfig.h`, `sdkconfig.cmake` and `sdkconfig.json`.

In context of ESP-IDF, configuration consists of several files, most importantly:

- `Kconfig` files, defining the configuration options and their relationships, together with their default values (if any).
- `sdkconfig` file, containing the currently used values of configuration options.
- `sdkconfig.defaults` file, containing user-defined default values for the configuration options.
- `sdkconfig.rename` file, containing `OLD_NAME NEW_NAME` pairs of configuration names to ensure backward compatibility.

Configuration files can be divided into two groups: those primarily **defining** the configuration options and those containing the **values** of these options. First group includes `Kconfig`, `Kcofnig.projbuild` and `sdkconfig.rename` files, second group includes `sdkconfig`, `sdkconfig.defaults`, `sdkconfig.h` and `sdkconfig.cmake` files

### Kconfig and Kconfig.projbuild Files

The `Kconfig.*` files store the configuration options, together with their properties and relationships. They also may contain default values for the configuration options. Every component has its own `Kconfig` file, which is used to define the configuration options for that component.

The only difference between `Kconfig` and `Kconfig.projbuild` files is where the their content will appear in the configuration interface (menuconfig):

- `Kconfig`: The content of this file will appear in the `Component config` window of the configuration interface.
- `Kconfig.projbuild`: The content of this file will appear in the root window of the configuration interface.

### sdkconfig and sdkconfig.old

In the `sdkconfig` file, **values currently assigned** to the configuration options are stored. It is generated automatically and is not to be edited, because configuration options can have relationships between one another (dependencies and reverse dependencies), that can be broken by manual editing.

The `sdkconfig.old` file is a backup of the previous configuration. It is generated every time the `sdkconfig` file is generated.

### sdkconfig.rename and sdkconfig.rename."chip"

The `sdkconfig.rename` files are used by the build system to ensure backward compatibility. These files are created and maintained by component or ESP-IDF developers and application developer should have no need to edit them.

### sdkconfig.defaults and sdkconfig.defaults."chip"

The ```sdkconfig.defaults``` file structure is the same as `sdkconfig` file; on every line, there is a full config name (including the `CONFIG_` prefix) and its value. This value has precedence over the default value in the Kconfig file by `default` option.

It is also possible to override the default values only for specific target. In this case, you can create `sdkconfig.defaults.<chip>` file, where `<chip>` is the target name (e.g. `esp32s2`). In this case, it is mandatory to create the `sdkconfig.defaults` file as well, otherwise the `sdkconfig.defaults.<chip>` file will be ignored. However, the `sdkconfig.defaults` file can be empty.

How to generate `sdkconfig.defaults` file:

1. `cd` into your project folder.
2. Configure everything you need in `idf.py menuconfig`.
3. Run `idf.py save-defconfig`. This will generate `sdkconfig.defaults` file with all the values different from the default ones

### sdkconfig.ci

Some of the IDF examples include a `sdkconfig.ci` file. This is part of the continuous integration (CI) test framework and is ignored by the normal build process

### Commands

```
idf.py set-target <target>
```

This command sets the current project target.

`idf.py set-target` will clear the build directory and re-generate the `sdkconfig` file from scratch. The old `sdkconfig` file will be saved as `sdkconfig.old`.



```
idf.py menuconfig
```

This command starts menuconfig.



```
idf.py reconfigure
```

This command forces [CMake](https://cmake.org/) to be rerun regardless of whether it is necessary. It is unnecessary during normal usage, but can be useful after adding/removing files from the source tree, or when modifying CMake cache variables