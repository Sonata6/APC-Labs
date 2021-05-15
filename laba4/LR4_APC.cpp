#include <dos.h>
#include <stdio.h>
#include <stdlib.h>


#define Left_OFFS 10
#define TM_SIZE 4
#define SRT_OFFS 0x140/2

#define QUARTZFREQ 1193180L
#define DURATION 400
#define FMAS 8

typedef struct VIDEOMEM
{
	unsigned char ch;
	unsigned char color;
};
struct VIDEOMEM far* outScreen(int offset);
void outText(char* ms, int offset);
void binToMem(unsigned char value, struct VIDEOMEM far* videomem);
void timer_init();
void hexToMem(long value, struct VIDEOMEM far* videomem);
void do_beep(long freq, unsigned delay);

char atr = 0x1F;

unsigned fmas[] = { 659 , 622  ,659 , 622 , 659 , 493 , 622 , 523, 440 };
long fmas1[] = { 200, 200, 200, 200, 200, 200, 200, 200, 200 };
char* m00 = { "STAT" };
char* m01 = { "DIVD" };
char* m0 = { "TMR0" };
char* m1 = { "TMR1" };
char* m2 = { "TMR2" };


void interrupt(far* oldInt)(...);
void interrupt far newInt(...)
{
	unsigned temp;
	static unsigned max1 = 0, max2 = 0, max3 = 0;
	unsigned low, high;
	int i;
	outp(0x43, 0x00);
	low = inp(0x40); // Читаем младшее
	high = inp(0x40);// Старшее
	temp = (high << 8) + low;

	outText(m01, 9 * SRT_OFFS);
	outText(m0, 10 * SRT_OFFS);
	struct VIDEOMEM far* tmp = outScreen(10 * SRT_OFFS + 5);
	for (i = 0; i < 4; i++)
	{
		tmp->ch = ' ';
		tmp->color = atr;
		tmp += 1;
	}
	tmp += 1;
	for (i = 0; i < 4; i++)
	{
		tmp->ch = ' ';
		tmp->color = atr;
		tmp += 1;
	}

	hexToMem(temp, outScreen(10 * SRT_OFFS + 5));
	if (max1 < temp) max1 = temp;
	hexToMem(max1, outScreen(10 * SRT_OFFS + 10));


	outp(0x43, 0x40);
	low = inp(0x41); // Читаем младшее
	high = inp(0x41);// Старшее
	temp = (high << 8) + low;

	outText(m1, 11 * SRT_OFFS);
	tmp = outScreen(11 * SRT_OFFS + 5);
	for (i = 0; i < 4; i++)
	{
		tmp->ch = ' ';
		tmp->color = atr;
		tmp += 1;
	}
	tmp += 1;
	for (i = 0; i < 4; i++)
	{
		tmp->ch = ' ';
		tmp->color = atr;
		tmp += 1;
	}


	hexToMem(temp, outScreen(11 * SRT_OFFS + 5));
	if (max2 < temp) max2 = temp;
	hexToMem(max2, outScreen(11 * SRT_OFFS + 10));
	outp(0x43, 0x80);
	low = inp(0x42); // Читаем младшее
	high = inp(0x42);// Старшее
	temp = (high << 8) + low;

	outText(m2, 12 * SRT_OFFS);

	tmp = outScreen(12 * SRT_OFFS + 5);
	for (i = 0; i < 4; i++)
	{
		tmp->ch = ' ';
		tmp->color = atr;
		tmp += 1;
	}
	tmp += 1;
	for (i = 0; i < 4; i++)
	{
		tmp->ch = ' ';
		tmp->color = atr;
		tmp += 1;
	}
	hexToMem(temp, outScreen(12 * SRT_OFFS + 5));
	if (max3 < temp) max3 = temp;
	hexToMem(max3, outScreen(12 * SRT_OFFS + 10));

	oldInt();
}
void timer_init()
{

	oldInt = _dos_getvect(0x08);
	_dos_setvect(0x08, newInt);

	// ПРограммируем 2й канал для издавания звуков:
	// Режим 3: счётчик непрерывно уменьшается на 2 каждый такт синхронизации, а при достижении нулевого значения или при переполнении состояние выхода канала
	//меняется на противоположное, и счёт начинается с начала.
	// исходная частота синхронизации FREQ делится на значение счётчика
	//счёт в двоичном виде и загрузку начального значения счётчика в 
	//виде двух байтов — сначала младшего, а затем старшего  

	outp(0x43, 0xB6);
	// 10 11 011 0 
	//к2=10 11=мл и ст 011=реж3 0=двоичСчет
}
void do_beep(unsigned freq, unsigned delay1)
{
	// Таймер 2 уже сконфигурирован. Так что пишем просто 
	// значения делителя: сначала младшее, затем старшее
	unsigned whattodec = QUARTZFREQ / freq;
	static t = 0;
	//   & 00000000 11111111
	// сделали старшую часть  = 0, записали ее в мл байт делителя частоты
	// Оно записывае в регистры только младшие байты
	outp(0x42, (whattodec & 0x00FF));
	// cnt & 11111111 00000000 ; -> 8; младшая часть  обнулилась, на ее 
	// место пошла старшая часть. Записали в старший байт канал таймера
	outp(0x42, ((whattodec & 0xFF00) >> 8));
	// включили
	//т.к нужно изменить только 2 младших бита при переключении
	outp(0x61, inp(0x61) | 3); //xxxxxx 01 | 000000 11 
	delay(delay1);
	outp(0x61, inp(0x61) & 0xfc);
	//xxxxxx 11 & 11111100
	// выключили
}
void readState(unsigned* statemas)
{

	//unsigned controlword = 0xE2; //11 1 0 0 0 1 0 
	// читаем=11, не фикс текущ знач счет для счит=1, 
	// фикс состояния счетч=0
	// канал 2 =0, кан1 =0 , кан0 =1, 1й всегда== 0

	// Тут читаем только состояние
	// Состояние 0го
	outp(0x43, 0xE2);
	statemas[0] = inp(0x40); //0x40


	//1го
	outp(0x43, 0xE4);
	statemas[1] = inp(0x41); //0x41
	//2го
	outp(0x43, 0xE8);
	statemas[2] = inp(0x42); //0x42


}
void binToMem(unsigned char value, VIDEOMEM far* videomem)
{
	char i, ch;
	for (i = 0; i < 8; i++)
	{
		ch = value % 2;
		value = value / 2;
		videomem->ch = ch + '0';
		videomem->color = atr;
		videomem += 0x1;
	}
}
void hexToMem(long value, struct VIDEOMEM far* videomem)
{
	long decimalnum, quotient, remainder;
	quotient = value;
	while (quotient != 0)
	{
		remainder = quotient % 16;
		if (remainder < 10)
		{
			videomem->ch = remainder + '0';
		}
		else
		{
			videomem->ch = remainder + 55;
		}
		videomem->color = atr;
		videomem += 0x1;
		quotient = quotient / 16;
	}
}
void readDividers(long* dmass)
{
	int i;
	long j;
	unsigned port, word;
	unsigned max, temp;
	unsigned low, high;
	word = 0x00;
	port = 0x40;
	for (i = 0; i < 3; i++)
	{
		max = 0;
		low = 0;
		high = 0;
		temp = 0;
		for (j = 0; j < 65535; j++)
		{
			outp(0x43, word);
			low = inp(port); // Читаем младшее
			high = inp(port);// Старшее
			temp = (high << 8) + low;
			if (max < temp) max = temp;
		}
		dmass[i] = max;
		word = word + 0x40;
		port = port + 0x01;
	}
}


VIDEOMEM far* outScreen(int offset)// Смещ с начала экрана
{
	unsigned char ch;
	int i;
	VIDEOMEM far* videomem = (VIDEOMEM far*)MK_FP(0xB800, 0);
	videomem += offset;
	//СмещениеПеред
	return videomem;
}

void outText(char* ms, int offset)
{
	int i;
	unsigned char ch;
	VIDEOMEM far* videomem = (VIDEOMEM far*)MK_FP(0xB800, 0);
	videomem += offset;
	for (i = 0; i < TM_SIZE; i++)
	{
		videomem->ch = ms[i];
		videomem->color = atr;
		videomem += 0x1;
	}

}

void main()
{

	int i;
	unsigned tstates[3];
	long divs[3];
	int ch;
	timer_init();// настраиваем timer 2 
	do
	{
		fflush(stdin);
		puts("1. Play\n2. Show stats\n3. Show dividers\n0. Exit");
		printf(">");
		scanf("%d", &ch);
		system("cls");
		switch (ch)
		{
		case 1:
		{
			for (i = 0; i < FMAS; i++)
			{
				do_beep(fmas[i], 400);
			}
			break;
		}
		case 2:
		{
			readState(tstates);
			outText(m00, 4 * SRT_OFFS);
			outText(m0, 5 * SRT_OFFS);
			binToMem(tstates[0], outScreen(5 * SRT_OFFS + 5));
			outText(m1, 6 * SRT_OFFS);
			binToMem(tstates[1], outScreen(6 * SRT_OFFS + 5));
			outText(m2, 7 * SRT_OFFS);
			binToMem(tstates[2], outScreen(7 * SRT_OFFS + 5));
			break;
		}
		case 3:
		{
			readDividers(divs);
			printf("0.%ld 1. %ld 2. %ld", divs[0], divs[1], divs[2]);
			hexToMem(divs[0], outScreen(10 * SRT_OFFS + 20));
			hexToMem(divs[1], outScreen(11 * SRT_OFFS + 20));
			hexToMem(divs[2], outScreen(12 * SRT_OFFS + 20));
			break;
		}
		}
	} while (ch != 0);

	return;
}



