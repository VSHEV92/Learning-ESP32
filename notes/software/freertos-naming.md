## FreeRTOS Naming Conventions

------

## Data Types

- **TickType_t** - data type used to hold the tick count value, and to specify times
- **BaseType_t** - the most efficient data type for the architecture. Typically, this is a 64-bit type on a 64-bit architecture, a 32-bit type on a 32-bit architecture, a 16-bit type on a 16-bit architecture, and an 8-bit type on an 8-bit architecture

## Naming

- Variables are prefixed with their type:
  - **c** - for **char**
  - **s** - for **int16_t** (short)
  - **l** - for **int32_t**
  - **x** - for **BaseType_t** and any other **non-standard** types (structures, task handles, queue handles, etc.)
  - **u** - variable is **unsigned** 
  - **p** - variable is a **pointer**
- Functions are prefixed with both the **type** they return and the **file** they are defined within
- Most macros are written in upper case, and prefixed with lower case letters that indicate where the macro is defined
  - **port** - portable.h or portmacro.h
  - **task** - task.h
  - **pd** - projdefs.h
  - **config** - FreeRTOSConfig.h
  - **err** - projdefs.h