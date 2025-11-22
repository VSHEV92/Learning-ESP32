#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"

#define MAX7219_SCLK 4
#define MAX7219_MOSI 2
#define MAX7219_CS 3

#define MAX7219_DISPLAY_TEST_REG  0xF
#define MAX7219_DISPLAY_TEST_OFF  0
#define MAX7219_DISPLAY_TEST_ON   1

#define BLINK_PERIOD_MS 500

void app_main(void) {
    // -----------------------------------------------------
    // ------------ configure spi bus handle ---------------
    // -----------------------------------------------------

    spi_bus_config_t spi_bus_cfg = {
        .sclk_io_num = MAX7219_SCLK,           // SCLK pin number
        .mosi_io_num = MAX7219_MOSI,           // MOSI pin number, union to field data0_io_num
        .miso_io_num = -1,                     // MISO pin number, union to field data1_io_num, -1 for not used
        .quadwp_io_num = -1,                   // write protect pin number, union to field data2_io_num, -1 for not used
        .quadhd_io_num = -1,                   // hold pin number, union to field data3_io_num, -1 for not used
        .data4_io_num = -1,                    // octal spi data4 pin number, -1 for not used
        .data5_io_num = -1,                    // octal spi data5 pin number, -1 for not used
        .data6_io_num = -1,                    // octal spi data6 pin number, -1 for not used
        .data7_io_num = -1,                    // octal spi data7 pin number, -1 for not used
        .max_transfer_sz = 2,                  // max transfer size in bytes, 2 for MAX7219
        .isr_cpu_id = ESP_INTR_CPU_AFFINITY_0, // cpu affinity setting, ESP_INTR_CPU_AFFINITY_AUTO affinity to any core
        .intr_flags = 0,                       // interrupt allocation flags 
        .flags = (
              0                              
         // | SPICOMMON_BUSFLAG_SLAVE         /* Initialize I/O in slave mode */
         // | SPICOMMON_BUSFLAG_MASTER        /* Initialize I/O in master mode */
         // | SPICOMMON_BUSFLAG_IOMUX_PINS    /* Check using iomux pins. Or indicates the pins are configured through the IO mux rather than GPIO matrix. */
         // | SPICOMMON_BUSFLAG_GPIO_PINS     /* Force the signals to be routed through GPIO matrix. Or indicates the pins are routed through the GPIO matrix. */
            | SPICOMMON_BUSFLAG_SCLK          /* Check existing of SCLK pin. Or indicates CLK line initialized. */
         // | SPICOMMON_BUSFLAG_MISO          /* Check existing of MISO pin. Or indicates MISO line initialized. */
            | SPICOMMON_BUSFLAG_MOSI          /* Check existing of MOSI pin. Or indicates MOSI line initialized. */
         // | SPICOMMON_BUSFLAG_DUAL          /* Check MOSI and MISO pins can output. Or indicates bus able to work under DIO mode. */
         // | SPICOMMON_BUSFLAG_WPHD          /* Check existing of WP and HD pins. Or indicates WP & HD pins initialized. */
         // | SPICOMMON_BUSFLAG_QUAD          /* Check existing of MOSI/MISO/WP/HD pins as output. Or indicates bus able to work under QIO mode. */
         // | SPICOMMON_BUSFLAG_IO4_IO7       /* Check existing of IO4~IO7 pins. Or indicates IO4~IO7 pins initialized. */
         // | SPICOMMON_BUSFLAG_OCTAL         /* Check existing of MOSI/MISO/WP/HD/SPIIO4/SPIIO5/SPIIO6/SPIIO7 pins as output. Or indicates bus able to work under octal mode. */
         // | SPICOMMON_BUSFLAG_NATIVE_PINS   /* SPICOMMON_BUSFLAG_IOMUX_PINS */
        )
    };

    spi_bus_initialize(
        SPI2_HOST,       // SPI2 is only supported spi host for esp32c3 
        &spi_bus_cfg,    // bus configuration structure
        SPI_DMA_CH_AUTO  // SPI_DMA_DISABLED - CPU controlled mode
    );


    // -----------------------------------------------------
    // ---------- configure spi device handle --------------
    // -----------------------------------------------------

    spi_device_interface_config_t spi_dev_cfg = {
        .command_bits = 0,                   // Default amount of bits in command phase 
        .address_bits = 8,                   // Default amount of bits in address phase
        .dummy_bits = 0,                     // Amount of dummy bits to insert between address and data phase
        .mode = 0,                           // (CPOL, CPHA) configuration 0:(0, 0), 1:(0, 1), 2:(1, 0), 3:(1, 1)
        .clock_source = SPI_CLK_SRC_DEFAULT, // Clock source (SPI_CLK_SRC_APB, SPI_CLK_SRC_XTAL)
        .duty_cycle_pos = 128,               // Duty cycle of positive clock, in 1/256th increments (128 = 50%/50% duty). Setting this to 0 (=not setting it) is equivalent to setting this to 128.
        .cs_ena_pretrans = 0,                // Amount of SPI bit-cycles the cs should be activated before the transmission (0-16). This only works on half-duplex transactions.
        .cs_ena_posttrans = 0,               // Amount of SPI bit-cycles the cs should stay active after the transmission (0-16)
        .clock_speed_hz = 10000000,          // spi clock in Hz, set to 10 MHz
        .spics_io_num = MAX7219_CS,          // CS pin number
        .queue_size = 1,                     // SPI driver transaction queue size. Set to one, so send transaction and wait response
        .pre_cb = NULL,                      // Specify pre-transfer callback to handle D/C line
        .post_cb = NULL,                     // Specify post-transfer callback to handle D/C line
        .flags = (
                 0
            // | SPI_DEVICE_TXBIT_LSBFIRST   /* Transmit command/address/data LSB first instead of the default MSB first */
            // | SPI_DEVICE_RXBIT_LSBFIRST   /* Receive data LSB first instead of the default MSB first */
            // | SPI_DEVICE_BIT_LSBFIRST     /* Transmit and receive LSB first */
            // | SPI_DEVICE_3WIRE            /* Use MOSI (=spid) for both sending and receiving data */
            // | SPI_DEVICE_POSITIVE_CS      /* Make CS positive during a transaction instead of negative */
            // | SPI_DEVICE_HALFDUPLEX       /* Transmit data before receiving it, instead of simultaneously */
            // | SPI_DEVICE_CLK_AS_CS        /* Output clock on CS line if CS is active */
            // | SPI_DEVICE_NO_DUMMY         /* No dummy bits is needed */
            // | SPI_DEVICE_DDRCLK           /* Double data rate transaction */
            // | SPI_DEVICE_NO_RETURN_RESULT /* Don't return the descriptor to the host on completion (use post_cb to notify instead) */
        ),
        .input_delay_ns = 0                  /* Maximum data valid time of slave. The time required between SCLK and MISO
                                                valid, including the possible clock delay from slave to master. The driver uses this value to give an extra
                                                delay before the MISO is ready on the line. Leave at 0 unless you know you need a delay. For better timing
                                                performance at high frequency (over 8MHz), it's suggest to have the right value.
                                             */
    };

    spi_device_handle_t max7219_spi_dev;
    spi_bus_add_device(SPI2_HOST, &spi_dev_cfg, &max7219_spi_dev);


    // -----------------------------------------------------
    // ------------ configure spi transaction --------------
    // -----------------------------------------------------
    spi_transaction_t* spi_trans_ptr;
    spi_transaction_t spi_trans = {
        .cmd = 0,                            // Command phase data
        .addr = MAX7219_DISPLAY_TEST_REG,    // Address phase data, set to max7219 display test register address
        .length = 8,                         // Total data length (read or write), in bits. For max7219 is one byte
        .rxlength = 0,                       // Total data length received, set by driver
        .user = NULL,                        // User-defined (void*) variable. Can be used to store as transaction ID.
        .tx_buffer = NULL,                   // Pointer to transmit buffer, union to tx_data[4] filed
        .rx_buffer = NULL,                   // Pointer to receive buffer, union to rx_data[4] filed
        .flags = (
            0
            // | SPI_TRANS_MODE_DIO                /*  Transmit/receive data in 2-bit mode */
            // | SPI_TRANS_MODE_QIO                /*  Transmit/receive data in 4-bit mode */
            // | SPI_TRANS_USE_RXDATA              /*  Receive into rx_data member of spi_transaction_t instead into memory at rx_buffer. */
               | SPI_TRANS_USE_TXDATA              /*  Transmit tx_data member of spi_transaction_t instead of data at tx_buffer. Do not set tx_buffer when using this. */
            // | SPI_TRANS_MODE_DIOQIO_ADDR        /*  Also transmit address in mode selected by SPI_MODE_DIO/SPI_MODE_QIO */
            // | SPI_TRANS_VARIABLE_CMD            /*  Use the ``command_bits`` in ``spi_transaction_ext_t`` rather than default value in ``spi_device_interface_config_t``. */
            // | SPI_TRANS_VARIABLE_ADDR           /*  Use the ``address_bits`` in ``spi_transaction_ext_t`` rather than default value in ``spi_device_interface_config_t``. */
            // | SPI_TRANS_VARIABLE_DUMMY          /*  Use the ``dummy_bits`` in ``spi_transaction_ext_t`` rather than default value in ``spi_device_interface_config_t``. */
            // | SPI_TRANS_CS_KEEP_ACTIVE          /*  Keep CS active after data transfer */
            // | SPI_TRANS_MULTILINE_CMD           /*  The data lines used at command phase is the same as data phase (otherwise, only one data line is used at command phase) */
            // | SPI_TRANS_MODE_OCT                /*  Transmit/receive data in 8-bit mode */
            // | SPI_TRANS_MULTILINE_ADDR          /*  The data lines used at address phase is the same as data phase (otherwise, only one data line is used at address phase) */
            // | SPI_TRANS_DMA_BUFFER_ALIGN_MANUAL /*  By default driver will automatically re-alloc dma buffer if it doesn't meet hardware alignment or dma_capable requirements, this flag is for you to disable this feature, you will need to take care of the alignment otherwise driver will return you error ESP_ERR_INVALID_ARG */
        )
    };

    /*
        For flags, SPI_TRANS_VARIABLE_CMD, SPI_TRANS_VARIABLE_ADDR, SPI_TRANS_VARIABLE_DUMMY
        bit length come from spi_transaction_ext_t fields

        typedef struct {
            struct spi_transaction_t base;  // Transaction data, so that pointer to spi_transaction_t can be converted into spi_transaction_ext_t
            uint8_t command_bits;           // The command length in this transaction, in bits.
            uint8_t address_bits;           // The address length in this transaction, in bits.
            uint8_t dummy_bits;             // The dummy length in this transaction, in bits.
        } spi_transaction_ext_t ;
    */

    // -----------------------------------------------------
    // -------------------- max7219 blink ------------------
    // -----------------------------------------------------

    while(true){
        // --------------------------------------
        // ----------- polling api --------------
        // --------------------------------------
        
        // 1. spi_device_polling_start - lock spi bus 
        // 2. spi_device_polling_start - configure device registers according to transaction setting
        // 3. spi_device_polling_start - start spi transaction on the bus
        // 4. spi_device_polling_start - currentlly portMAX_DELAY is on supported timeout
        // 5. spi_device_polling_end   - poll transaction done interrupt flag is asserted
        // 6. spi_device_polling_end   - also check timer ticks value for timeout detection
        // 7. spi_device_polling_end   - portMAX_DELAY can be set for on infinite timeout
        spi_trans.tx_data[0] = MAX7219_DISPLAY_TEST_OFF;
        spi_device_polling_start(max7219_spi_dev, &spi_trans, portMAX_DELAY);
        spi_device_polling_end(max7219_spi_dev, portMAX_DELAY);
        vTaskDelay( pdMS_TO_TICKS(BLINK_PERIOD_MS/2) );
 
        // 1. spi_device_polling_transmit - same as spi_device_polling_start, then spi_device_polling_end
        //                                  with infinite timeout (portMAX_DELAY)
        spi_trans.tx_data[0] = MAX7219_DISPLAY_TEST_ON;
        spi_device_polling_transmit(max7219_spi_dev, &spi_trans);
        vTaskDelay( pdMS_TO_TICKS(BLINK_PERIOD_MS/2) );

        // --------------------------------------
        // --------- interrupt api --------------
        // --------------------------------------
        
        // 1. spi_device_queue_trans      - send transaction to driver transaction "send queue"
        // 2. spi_device_get_trans_result - block on trying toget transaction from driver transaction "receve queue"
        // 3. spi_device_get_trans_result - ISR put transaction to "receve queue" after bus transaction completed
        spi_trans.tx_data[0] = MAX7219_DISPLAY_TEST_OFF;
        spi_device_queue_trans(max7219_spi_dev, &spi_trans, portMAX_DELAY);
        spi_device_get_trans_result(max7219_spi_dev, &spi_trans_ptr, portMAX_DELAY);
        vTaskDelay( pdMS_TO_TICKS(BLINK_PERIOD_MS/2) );
 
        // 1. spi_device_transmit - same as spi_device_queue_trans, then spi_device_get_trans_result
        //                                  with infinite timeout (portMAX_DELAY)
        spi_trans.tx_data[0] = MAX7219_DISPLAY_TEST_ON;
        spi_device_transmit(max7219_spi_dev, &spi_trans);
        vTaskDelay( pdMS_TO_TICKS(BLINK_PERIOD_MS/2) );
    }
}

