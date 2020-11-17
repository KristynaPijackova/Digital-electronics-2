
 | **Push button** | **PC0[A0] voltage** | **ADC value (calculated)** | **ADC value (measured)** |
   | :-: | :-: | :-: | :-: |
   | Right  | 0&nbsp;V | 0   | 0 |
   | Up     | 0.495&nbsp;V | 101 | 101 |
   | Down   | 1.203&nbsp;V | 246 | 245 |
   | Left   | 1.969&nbsp;V | 402 | 402 |
   | Select | 3.182&nbsp;V | 651 | 647 |
   | none   | 5&nbsp;V | 1023 | 1007 |
   
   ### Table for ADC converter register

   | **Operation** | **Register(s)** | **Bit(s)** | **Description** |
   | :-: | :-- | :-- | :-- |
   | Voltage reference | ADMUX | REFS1:0 | 01: AVcc voltage reference, 5V |
   | Input channel | ADMUX | MUX3:0 | 0000: ADC0, 0001: ADC1, ... |
   | ADC enable | ADCSRA | ADEN | 1: ADC Enable, 0: ADC Disable |
   | Start conversion | ADCSRA | ADSC | 1:Start conversion, when conversion complete, returns to zero |
   | ADC interrupt enable | ADCSRA | ADIE | 1: first bit in SREG is set and the ADC Conversion Complete Interrupt is activated |
   | ADC clock prescaler | ADCSRA | ADPS2:0 | 000: Division factor 2, 001: 2, 010: 4, ...|
   | ADC result | ADC | ADCL7:0 and ADCH7:0 | conversion result |
   
### UART description table

   | **Function name** | **Function parameters** | **Description** | **Example** |
   | :-- | :-- | :-- | :-- |
   | `uart_init` | `UART_BAUD_SELECT(9600, F_CPU)` | Initialize UART to 8N1 and set baudrate to 9600&nbsp;Bd | `uart_init(UART_BAUD_SELECT(9600, F_CPU));` |
   | `uart_getc` | void | Get received byte from ringbuffer | `uart_getc();` |
   | `uart_putc` | data(byte) | Put byte to ring buffer | `uart_putc('c');` |
   | `uart_puts` | pointer to string | Put string to ringbuffer | `uart_puts(lcd_string);` |
