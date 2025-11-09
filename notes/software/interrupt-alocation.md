# Interrupt Allocation

------

### Abstract

The main purpose of interrupt allocation is to connent interrupt source to interrupt number. ESP32C3 has 31 interrupt lines, nut much more interrupt sources. Interrupt allocator finds interrupt number that match some constraints set by **interrupt flags**.

------

### Interrupt Allocator API

**esp_intr_alloc()** - get interrupt source, flags (shared, iram safe, edge and so on), handler and connect interrupt source to interrupt number (line, vector number). Implemented using **esp_intr_alloc_intrstatus()**.

**esp_intr_alloc_intrstatus()** - same as **esp_intr_alloc()** but also get status register address and mask. For shared interrupts, the handler is only called if a read from the specified register, ANDed with the mask, returns non-zero. By passing an interrupt status register address and a fitting mask, this can be used to accelerate interrupt handling in the case a shared interrupt is triggered; by checking the interrupt statuses first, the code can decide which ISRs can be skipped

------

### esp_intr_alloc_intrstatus()

1. Check flag parameters. Return **ESP_ERR_INVALID_ARG** if invalid flag parameters are set.
2. Allocate in heap memory for interrupt handle data structure, that will contain all information about given interrupt mapping.
3. **get_available_int()** - get available interrupt number. If there is no free interrupt numbers then **ESP_ERR_NOT_FOUND** error returns.
4. **get_desc_for_int()** - Returns a **vector_desc_t** entry for an intno/cpu. Either returns a preexisting one or allocates a new one and inserts it into the list. **vector_desc_t** - data structure that is part of interrupt handle data.
5. Allocate interrupt - configure all interrupt handle data structure fields.
   -  If interrupt is **SHARED** then add shared vector descriptor to its vector descriptor list and configure all it's filed (handler, args, statusreg, statusmask). Call **esp_cpu_intr_set_handler()** with handler for shared interrupts **shared_intr_isr()** and vector descriptor as argument.
   - If interrupt is not **SHARED** then call **esp_cpu_intr_set_handler()** with given handler and given args as argument.
6. Call **esp_rom_route_intr_matrix()** to configure interrupt matrix. **esp_rom_route_intr_matrix()** code is inside ROM.
7. Call **ESP_INTR_ENABLE()** to enable interrupts for given interrupt number. Also enable global interrupts.
8. Return interrupt handle data structure.

------

### Details of get_available_int() call

1. Try to find existing descriptor (vector_desc_t structure) for given source/cpu combination. For that call **find_desc_for_source()**.

   **find_desc_for_source()**:

   - Iterate over all vector descriptor list
   - If vector descriptor not **SHARED** and it's **source** and **cpu** are match with call parameters, then return this descriptor.
   - If vector descriptor is **SHARED**, then compare it's **cpu** value and then iterate over its shared vector descriptor until find match **source** value. If share vector descriptor is found then return vector descriptor that contain it. 

2. If vector descriptor is found, then check, if it is usable (some interrupt numbers are reserved by IDF by internal purpose). If it is OK, then return interrupt number from vector descriptor.

3. If there is no valid vector descriptor, then try to find vector descriptor by force parameter (force values are set for some internal sources - timers, SW interrupts, profile interrupts). Use **find_desc_for_int()**. If there is no vector descriptor, then just return force value.

   **find_desc_for_int()**:

   - Iterate over all vector descriptor list
   - If vector descriptor **interrupt number (intno)** and **cpu** are match with call parameters, then return this descriptor.

4. If there is no any valid vector descriptor (vector_desc_t structure), then try in the loop all interrupt numbers using **find_desc_for_int()**. If there is no descriptor for given vector number, then check if this number is usable (not reserved). If it is OK and interrupt is not **SHARED** then return this number.  

5. if Interrupt is SHARED, then try to find if this number are already marked as **SHARED**. For this iterate over shared vector descriptors (**shared_vector_desc_t** structure) list. Count number of handler for all shared vector numbers, and choose number with minimal handlers number. Return chosen vector number.  

------

### Details of get_desc_for_int() call

1. Try to get vector descriptor for given **interrupt number** and **CPU**
2. If vector descriptor exists then return it
3. If not, then allocate new vector descriptor. 
   - Allocate memory in heap. 
   - Configure descriptor interrupt number and CPU fields
   - Add descriptor to vector descriptors list using **insert_vector_desc()** call 

------

### Details of esp_cpu_intr_set_handler() call



------

### Interrupt Data Structures



#### Interrupt Descriptor Table

Inside **esp_cpu_intr.c** file.

**intr_desc_table** - array of interrupt descriptor (**intr_desc_t**) structures. Each interrupt number has it's on interrupt descriptor.

intr_desc_t - interrupt descriptor structure. It's fields:

- **priority** - for user available: 1 - interrupt 1 to 13: 1,2,3 - interrupts 17 to 23, 3,4,5 - interrupts 25 to 31
-  **type** - LEVEL, EDGE, NA (no available)
- **flags** - RESERVED, SPECIAL (cpu timers)



