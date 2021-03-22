#include <dos.h>
#include <stdio.h>
#include <conio.h>

#define PORT1 0x3F8
#define PORT2 0x2E8

void main(void) {
    unsigned char c = 0;
    unsigned char chrctr = 0;


    outportb(PORT1 + 3, 0x80); /* Set DLAB ON */
    outportb(PORT1 + 0, 0x0C); /* Set the baud rate to 9600 */
    outportb(PORT1 + 1, 0x00); /* Set Baud - Divisor latch HIGH */
    outportb(PORT1 + 3, 0x03); /* 8 bits, no parity, 1 stop */
    outportb(PORT1 + 2, 0xC7); /* FIFO Control Register */
    outportb(PORT1 + 4, 0x0B); /* Turn on DTR, RTS, and OUT2) */

    outportb(PORT2 + 3, 0x80); /* Set DLAB ON */
    outportb(PORT2 + 0, 0x0C); /* Set the baud rate to 9600 */
    outportb(PORT2 + 1, 0x00); /* Set Baud - Divisor latch HIGH */
    outportb(PORT2 + 3, 0x03); /* 8 bits, no parity, 1 stop */
    outportb(PORT2 + 2, 0xC7); /* FIFO Control Register */
    outportb(PORT2 + 4, 0x0B); /* Turn on DTR, RTS, and OUT2) */

   
    while (chrctr != 27) {
	c = inportb(PORT1 + 5);
	if (c & 0x01) {
	    chrctr = inportb(PORT1);
	    printf("COM2 says: %c\n", chrctr);

	}
	if (kbhit()) {
	    chrctr = getch();
	    outportb(PORT2, chrctr);
	}
    }
}