all: system.bin

startup_lm3s6965.o: startup_lm3s6965.c irq.h
	arm-none-eabi-gcc -c -g -mcpu=cortex-m3 -mthumb -o startup_lm3s6965.o startup_lm3s6965.c

nvic.o: nvic.c nvic.h lm3s6965_memmap.h
	arm-none-eabi-gcc -c -g -mcpu=cortex-m3 -mthumb -o nvic.o nvic.c

sysctl.o: sysctl.c sysctl.h lm3s6965_memmap.h
	arm-none-eabi-gcc -c -g -mcpu=cortex-m3 -mthumb -o sysctl.o sysctl.c 

uart_drv.o: uart_drv.c uart_drv.h lm3s6965_memmap.h sysctl.h
	arm-none-eabi-gcc -c -g -mcpu=cortex-m3 -mthumb -o uart_drv.o uart_drv.c

serial_print.o: serial_print.c uart_drv.h serial_print.h
	arm-none-eabi-gcc -c -g -mcpu=cortex-m3 -mthumb -o serial_print.o serial_print.c

systick.o: systick.c sysctl.h systick.h uart_drv.h serial_print.h lm3s6965_memmap.h
	arm-none-eabi-gcc -c -g -mcpu=cortex-m3 -mthumb -o systick.o systick.c

init.o: init.c irq.h nvic.h sysctl.h systick.h uart_drv.h serial_print.h
	arm-none-eabi-gcc -c -g -mcpu=cortex-m3 -mthumb -o init.o init.c

system.elf: startup_lm3s6965.o nvic.o uart_drv.o serial_print.o sysctl.o systick.o init.o 
	arm-none-eabi-ld -T lm3s6965_layout.ld -o system.elf startup_lm3s6965.o nvic.o sysctl.o uart_drv.o serial_print.o systick.o init.o

system.bin: system.elf
	arm-none-eabi-objcopy -O binary system.elf system.bin

run: system.bin
	qemu-system-arm -M lm3s6965evb -kernel system.bin -nographic -monitor telnet:127.0.0.1:1234,server,nowait 

rundbg: system.bin
	qemu-system-arm -S -M lm3s6965evb -kernel system.bin -gdb tcp::5678 -nographic -monitor telnet:127.0.0.1:1234,server,nowait 

clean:
	rm -f startup_lm3s6965.o serial_print.o uart_drv.o nvic.o sysctl.o systick.o init.o system.elf system.bin
            
dump: 
	arm-none-eabi-nm -n startup_lm3s6965.o
	arm-none-eabi-objdump -h startup_lm3s6965.o
	arm-none-eabi-nm -n uart_drv.o
	arm-none-eabi-objdump -h uart_drv.o
	arm-none-eabi-nm -n serial_print.o
	arm-none-eabi-objdump -h serial_print.o
	arm-none-eabi-nm -n nvic.o
	arm-none-eabi-objdump -h nvic.o
	arm-none-eabi-nm -n sysctl.o
	arm-none-eabi-objdump -h sysctl.o
	arm-none-eabi-nm -n systick.o
	arm-none-eabi-objdump -h systick.o
	arm-none-eabi-nm -n init.o
	arm-none-eabi-objdump -h init.o
	arm-none-eabi-nm -n system.elf
	arm-none-eabi-objdump -h system.elf
	arm-none-eabi-size system.elf
