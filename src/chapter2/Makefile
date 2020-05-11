all: startup.bin

startup_lm3s6965.o: startup_lm3s6965.c
	arm-none-eabi-gcc -c -g -mcpu=cortex-m3 -mthumb -o startup_lm3s6965.o startup_lm3s6965.c

uart_drv.o: uart_drv.c uart_drv.h
	arm-none-eabi-gcc -c -g -mcpu=cortex-m3 -mthumb -o uart_drv.o uart_drv.c

serial_print.o: serial_print.c uart_drv.h
	arm-none-eabi-gcc -c -g -mcpu=cortex-m3 -mthumb -o serial_print.o serial_print.c

startup.elf: startup_lm3s6965.o uart_drv.o serial_print.o
	arm-none-eabi-ld -T lm3s6965_layout.ld -o startup.elf startup_lm3s6965.o uart_drv.o serial_print.o

startup.bin: startup.elf
	arm-none-eabi-objcopy -O binary startup.elf startup.bin

run: startup.bin
	qemu-system-arm -M lm3s6965evb -kernel startup.bin -nographic -monitor telnet:127.0.0.1:1234,server,nowait 

clean:
	rm -f startup_lm3s6965.o serial_print.o uart_drv.o startup.elf startup.bin
            
dump: 
	arm-none-eabi-nm -n startup_lm3s6965.o
	arm-none-eabi-objdump -h startup_lm3s6965.o
	arm-none-eabi-nm -n serial_print.o
	arm-none-eabi-objdump -h serial_print.o
	arm-none-eabi-nm -n uart_drv.o
	arm-none-eabi-objdump -h uart_drv.o
	arm-none-eabi-nm -n startup.elf
	arm-none-eabi-objdump -h startup.elf
	arm-none-eabi-size startup.elf
