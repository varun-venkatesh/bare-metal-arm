all: startup.bin

startup_lm3s6965.o: startup_lm3s6965.c
	arm-none-eabi-gcc -c -g -mcpu=cortex-m3 -mthumb -o startup_lm3s6965.o startup_lm3s6965.c

serial_print.o: serial_print.c
	arm-none-eabi-gcc -c -g -mcpu=cortex-m3 -mthumb -o serial_print.o serial_print.c

startup_lm3s6965.elf: startup_lm3s6965.o serial_print.o
	arm-none-eabi-ld -T lm3s6965_layout.ld -o startup_lm3s6965.elf startup_lm3s6965.o serial_print.o

system.bin: startup_lm3s6965.elf
	arm-none-eabi-objcopy -O binary startup_lm3s6965.elf system.bin

run: system.bin
	qemu-system-arm -M lm3s6965evb -kernel system.bin -nographic -monitor telnet:127.0.0.1:1234,server,nowait 

rundbg: system.bin
	qemu-system-arm -S -M lm3s6965evb -kernel system.bin -gdb tcp::5678 -nographic -monitor telnet:127.0.0.1:1234,server,nowait 

clean:
	rm -f startup_lm3s6965.o serial_print.o startup.elf startup.bin
            
dump: 
	arm-none-eabi-nm -n startup_lm3s6965.o
	arm-none-eabi-objdump -h startup_lm3s6965.o
	arm-none-eabi-nm -n serial_print.o
	arm-none-eabi-objdump -h serial_print.o
	arm-none-eabi-nm -n startup_lm3s6965.elf
	arm-none-eabi-objdump -h startup_lm3s6965.elf
	arm-none-eabi-size startup_lm3s6965.elf

