# MAX7219 Notes

------

## General Description

The MAX7219 are compact, serial input/output common-cathode display drivers that interface microprocessors to 7-segment numeric LED displays of up to 8 digits, bar-graph displays, or 64 individual LEDs. Included on-chip are a BCD code-B decoder, multiplex scan circuitry, segment and digit drivers, and an 8x8 static RAM that stores each digit.

------

## Serial-Addressing Modes

Serial data at DIN, sent in 16-bit packets, is shifted into the internal 16-bit shift register with each rising edge of CLK regardless of the state of LOAD. The data is then latched into either the digit or control registers on the rising edge of LOAD.

Data at DIN is propagated through the shift register and appears at DOUT 16.5 clock cycles later. This feature is user for chips cascade connection.

Data is clocked out on the falling edge of CLK. Data bits are labeled D0–D15. D8–D11 contain the register address. D0–D7 contain the data, and D12–D15 are “don’t care” bits. The first received is D15, the most significant bit (MSB).

------

## Data registers

The digit registers are realized with an on-chip, 8x8 dual-port SRAM. They are addressed directly so that individual digits can be updated and retain data as long as V+ typically exceeds 2V.

**D0** address is **0x1**, **D1** - **0x2**, and so on **D7** is **0x8**.

------

## Control Registers

### Shutdown

When the MAX7219 is in shutdown mode, the scan oscillator is halted, all segment current sources are pulled to ground, and all digit drivers are pulled to V+, thereby blanking the display. Data in the digit and control registers remains unaltered. Shutdown can be used to save power or as an alarm to flash the display by successively entering and leaving shutdown mode. 

Shutdown register address is **0xC**. 

**D0 = 1** - Normal Operation. 

**D0 = 0** - Shutdown mode. 

### Decode-Mode

The decode-mode register sets BCD code B (0-9, E, H, L, P, and -) or no-decode operation for each digit. Look at Table 4 in datasheet.

Each bit in the register corresponds to one digit. A logic high selects code B decoding while logic low bypasses the decoder. 

When the code B decode mode is used, the decoder looks only at the lower nibble of the data in the digit registers (D3–D0), disregarding bits D4–D6. D7, which sets the decimal point (SEG DP), is independent of the decoder and is positive logic (D7 = 1 turns the decimal point on). Look at Table 5 in datasheet.

When no-decode is selected, data bits D7–D0 correspond to the segment lines of the MAX7219.

Decode-mode register address is **0x9**. 

**Dn = 0** - no decode for digit n. 

**Dn = 1** - code B decode for digit n. 

### Intensity Control

Digital control of display brightness is provided by an internal pulse-width modulator, which is controlled by the lower nibble of the intensity register. The modulator scales the average segment current in 16 steps from a maximum of 31/32 down to 1/32 of the peak current. Look at Table 7 in datasheet.

Intensity control register address is **0xA**. 

**D3-D0** - more the four bit value, brighter the digits.

### Scan-Limit

The scan-limit register sets how many digits are displayed, from 1 to 8. They are displayed in a multiplexed manner with a typical display scan rate of 800Hz with 8 digits displayed. If fewer digits are displayed, the scan rate is 8f OSC /N, where N is the number of digits scanned. Table 8 lists the scan-limit register format.

Scan-Limit register address is **0xB**. 

**D2-D0** - three bit value is number of digits plus one.

### Display Test

The display-test register operates in two modes: normal and display test. Display-test mode turns all LEDs on by overriding, but not altering, all controls and digit registers (including the shutdown register). In display-test mode, 8 digits are scanned and the duty cycle is 31/32.

Display Test register address is **0xF**. 

**D0 = 0** - Normal Operation. 

**D0 = 1** - Display Test. 

------

## No-Op

The no-op register is used when cascading MAX7219s. Connect all devices’ LOAD inputs together and connect DOUT to DIN on adjacent devices. DOUT is a CMOS logic-level output that easily drives DIN of successively cascaded parts.

For example, if four MAX7219s are cascaded, then to write to the fourth chip, sent the desired 16-bit word, followed by three no-op codes. When LOAD goes high, data is latched in all devices. The first three chips receive no-op commands, and the fourth receives the intended data.

No-OP register address is **0x0**. 

------

## Programming Flow

1. Set chip to shutdown mode. Write **0x1** to address **0xC**. 
2. Set number of displayed digits or bars. Using scan-limit register. Register address is **0xB**. 
3. Configure decode mode. Write 1 to n-th bit, if decode should be on, else write 0. Register address is **0x9**. 
4. Configure brightness intensity using Intensity register. Register address is **0xA**.
5. Write data to data registers **D0**-**D7**. Register addresses is **0x1** - **0x8**. 
6. Disable shutdown mode. Write **0x0** to address **0xC**. 



You can test leds using Display test register. Write **0x1** to address **0xF**. Write **0x0** to address **0xF** to disable test mode.

For cascade chips connections use No-Ops.  