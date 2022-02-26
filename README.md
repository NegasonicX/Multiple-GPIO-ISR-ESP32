# Multiple GPIO ISR on ESP32
A simple yet insightful program to set multiple gpio interrupts.


## GPIO functions:
 * GPIO2   : output ( built-in led on Devkit-V1 )
 * GPIO34  : output ( externally pulled up )
 * GPIO35  : output ( externally pulled up )

## Test:
 * Connect GPIO34 with simple switch and ground.
 * Connect GPIO35 with simple switch and ground.

## Understanding the Flow:
* This code is developed for ESP32 on Embedded C Language using FreeRTOS.
* There are 32 interrupts available for each core on ESP32 ( Dual Core CPU ).
* Interrupts follow the same basic principle that is whenever ISR is called, programmer entertains the same and then returns to running code. Although with RTOS, multiple running code will be handled along with ISR call.
* "IRAM_ATTR" is used to place a particular piece of code inside the internal RAM of ESP32 instead of much slower flash memory.
* "portMAX_DELAY" is used to block indefinitely and hence by redefining it as 0 at the beginning removes the block.

## Depending on the need, there are 3 egdes for Interrupt:
* GPIO_INTR_ANYEDGE : Triggers at Any Edge
* GPIO_INTR_POSEDGE : Triggers at Positive Edge
* GPIO_INTR_NEGEDGE : Triggers at Negative Edge
