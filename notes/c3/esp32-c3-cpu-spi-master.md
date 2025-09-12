# ESP32-C3 CPU SPI Master

------

## Programming Model

1. Configure GPIO Pins and GPIO Matrix
2. Configure bit order using **SPI_WR_BIT_ORDER** bit in **SPI_CTRL_REG** register
   - 0 - MSB (default)
   - 1 - LSB
3. Configure transfer mode to CPU-Controlled Master Mode. Configure **SPI_USR_MISO_HIGHPART** and **SPI_USR_MOSI_HIGHPART** bits in SPI_USER_REG register (1: only high part enabled; 0: all register are used). Use registers **SPI_W0_REG** to **SPI_W15_REG** send and receive data.
4. Disable DMA transfer. For that clear bit **SPI_DMA_RX_ENA** and **SPI_DMA_TX_ENA** in **SPI_DMA_CONF_REG** register
5. Set FSPI in Master mode by clearing bit **SPI_SLAVE_MODE** in **SPI_SLAVE_REG** register
6. Configure full-duplex or hlf-duplex mode bit **SPI_DOUTDIN** in register **SPI_USER_REG**
   - 1: full-duplex
   - 0: hlf-duplex
7. Configure SPI clock
   - When the bit **SPI_CLK_EQU_SYSCLK** in register **SPI_CLOCK_REG** is set to 1, the output clock frequency of GP-SPI2 will be fclk_spi_mst
   - When the bit **SPI_CLK_EQU_SYSCLK** in register **SPI_CLOCK_REG** is set to 0, the output clock frequency of GP-SPI2 will fclk_spi_mst divided by product (**SPI_CLKCNT_N** + 1) and (**SPI_CLKDIV_PRE** + 1) values in register **SPI_CLOCK_REG**
   - Also configure **SPI_CLKCNT_L** and **SPI_CLKCNT_H** values in register **SPI_CLOCK_REG**
   - Choose master clock source using **SPI_MST_CLK_SEL** bit in **SPI_CLK_GATE_REG** register. 1: PLL_F80M_CLK. 0: XTAL_CLK.
   -  Enable master clock using **SPI_MST_CLK_ACTIVE** bit in **SPI_CLK_GATE_REG** register
8. Configure CPOL and CPHA using bit **SPI_CK_IDLE_EDGE** in register **SPI_MISC_REG** and the bit **SPI_CK_OUT_EDGE** in register **SPI_USER_REG**
9. Select when SPI clock is enabled (always or during transfer with some delay) using field **SPI_CLK_MODE** in **SPI_SLAVE_REG** register
10. If needed configure timing compensation
   - **SPI_DIN_MODE_REG**: select the latch edge of input data
   - **SPI_DIN_NUM_REG**: select the delay cycles of input data
   - **SPI_DOUT_MODE_REG**: select the latch edge of output data
11. Configure setup and hold CS time:
    1. Prepare an SPI transaction and control SPI CS setup time
       - Set **SPI_CS_SETUP** in **SPI_USER_REG** register to enable this state
       - Set state length (expected -1) in cycles using **SPI_CS_SETUP_TIME** in **SPI_USER1_REG** register
       - If **SPI_CS_SETUP** is cleared, the SPI CS setup time is 0.5 x **T_SPI_CLK**. If **SPI_CS_SETUP** is set, the SPI CS setup time is (**SPI_CS_SETUP_TIME** + 1.5) x **T_SPI_CLK**
    2. Prepare an SPI transaction and control SPI CS hold time
       - Set **SPI_CS_HOLD** in **SPI_USER_REG** register to enable this state
       - Set state length (expected -1) in cycles using **SPI_CS_HOLD_TIME** in **SPI_USER1_REG** register
       - If **SPI_CS_HOLD** is cleared, the SPI CS hold time is 0.5 x **T_SPI_CLK**. If **SPI_CS_HOLD** is set, the SPI CS hold time is (**SPI_CS_HOLD_TIME** + 1.5) x **T_SPI_CLK**
12. Configure Master State Machine. States:
    1. configuration (CONF). Only used in DMA-controlled configurable segmented transfer
       - Set **SPI_USR_CONF** in **SPI_USER_REG** register to enable this state. Clear for single transfer (CPU mode)
    3. command (CMD). Send command sequence
       - Set **SPI_USR_COMMAND** in **SPI_USER_REG** register to enable this state
       - Set state length (expected -1) in cycles using **SPI_USR_COMMAND_BITLEN** in **SPI_USER2_REG** register
       - if needed set **SPI_FCMD_DUAL** and **SPI_FCMD_QUAD** bits in **SPI_CTRL_REG** for 2 and 4 bit modes
       - Set command data in **SPI_USR_COMMAND_VALUE** in **SPI_USER2_REG** register
    4. address (ADDR). Send address sequence
       - Set **SPI_USR_ADDR** in **SPI_USER_REG** register to enable this state
       - Set state length (expected -1) in cycles using **SPI_USR_ADDR_BITLEN** in **SPI_USER1_REG** register
       - if needed set **SPI_FADDR_DUAL** and **SPI_FADDR_QUAD** bits in **SPI_CTRL_REG** for 2 and 4 bit modes
       - Set address data in **SPI_USR_ADDR_VALUE** in **SPI_ADDR_REG** register
    5. dummy (DUMMY). Send dummy sequence (wait cycle). 
       - Set **SPI_USR_DUMMY** in **SPI_USER_REG** register to enable this state
       - Set state length (expected -1) in cycles using **SPI_USR_DUMMY_CYCLELEN** in **SPI_USER1_REG** register
    6. data out (DOUT). Send data sequence. 
       - Set **SPI_USR_MOSI** in **SPI_USER_REG** register to enable this state
       - Set state length (expected -1) in cycles using **SPI_MS_DATA_BITLEN** in **SPI_MS_DLEN_REG** register
       - if needed set **SPI_FWRITE_DUAL** and **SPI_FWRITE_QUAD** bits in **SPI_CTRL_REG** for 2 and 4 bit modes
    7. data in (DIN). Receive data sequence. 
       - Set **SPI_USR_MISO** in **SPI_USER_REG** register to enable this state
       - Set state length (expected -1) in cycles using **SPI_MS_DATA_BITLEN** in **SPI_MS_DLEN_REG** register
       - if needed set **SPI_FREAD_DUAL** and **SPI_FREAD_QUAD** bits in **SPI_CTRL_REG** for 2 and 4 bit modes
13. Configure interrupts. Set **SPI_TRANS_DONE_INT_ENA** in **SPI_DMA_INT_ENA_REG** to enable end of transacrion interrupt
14. Set **SPI_DMA_AFIFO_RST**, **SPI_BUF_AFIFO_RST**, and **SPI_RX_AFIFO_RST** in **SPI_DMA_CONF_REG** register to reset GP-SPI2 internal  buffers
15. Set **SPI_USR** in **SPI_CMD_REG** to start GP-SPI2 transfer
16. Wait for end for transaction by polling bit **SPI_TRANS_DONE_INT_ST** in **SPI_DMA_INT_ST_REG** register
17. Clear interrupt by toggling bit **SPI_TRANS_DONE_INT_CLR** in **SPI_DMA_INT_CLR_REG** register