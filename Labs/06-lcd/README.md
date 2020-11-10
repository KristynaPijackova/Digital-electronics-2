| **LCD signal(s)** | **AVR pin(s)** | **Description** |
   | :-: | :-: | :-- |
   | RS | PB0 | Register selection signal. Selection between Instruction register (RS=0) and Data register (RS=1) |
   | R/W | GND | Read/Write, GDN for enabled writing |
   | E | PB1 | Enable signal into HD44780 |
   | D[3:0] | N/A| 8-bit data D[7:0] |
   | D[7:4] | PD[4:7] | 4-bit data |



 | **Function name** | **Function parameters** | **Description** | **Example** |
   | :-- | :-- | :-- | :-- |
   | `lcd_init` | `LCD_DISP_OFF`<br>`LCD_DISP_ON`<br>`LCD_DISP_ON_CURSOR`<br>`LCD_DISP_ON_CURSOR_BLINK` | Display off<br>display on, cursor off;<br>display on, cursor on;<brdisplay on, cursor on flashing | `lcd_init(LCD_DISP_OFF);`<br>&nbsp;<br>&nbsp;<br>&nbsp; |
   | `lcd_clrscr` | `void` | Clear display, set cursor to home position. | `lcd_clrscr();` |
   | `lcd_gotoxy` | `uint8_t x,uint8_t y`| Set cursor to position. | `lcd_gotoxy(0, 1);` |
   | `lcd_putc` | `char c` | Display character at cursor position. | `lcd_putc('')` |
   | `lcd_puts` | `string s` | Display string without auto linefeed. | `lcd_puts("")` |
   | `lcd_command` | `cmd`	 | Send LCD controller instruction command. | `lcd_puts(cmd)` |
   | `lcd_data` | `data`  | Send data to LCD controller. Similar to lcd_putc(), without interpreting LF | `lcd_puts(data)` |
