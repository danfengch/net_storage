
/** gpio pin number define */
//#define LS2K_GPIO3                  3

/** gpio input output define */
//#define STORAGE_GPIO_OUTPUT         0
//#define STORAGE_GPIO_INPUT          1

//#define GPIO0_OEN                   (*(volatile long long *)0xbfe10500)
//#define GPIO0_O                     (*(volatile long long *)0xbfe10510)

/** fpga reset pin */
//#define STORAGE_FPGA_RESET          LS2K_GPIO3

#include "target/storage.h"


void storage_delay_us(u32 us)
{
    volatile u32 us_unit = 0;
    
	while (us--)
        for (us_unit = 40; us_unit > 0; us_unit--);
}

void storage_gpio_config(int gpio_num, int dir)
{
    volatile long long temp = 1;       
    
    if (dir == STORAGE_GPIO_OUTPUT)
    {
        temp = ~(temp << gpio_num);
        GPIO0_OEN &= temp;
    }
    else
    {
        temp = temp << gpio_num;
        GPIO0_OEN |= temp;
    }    
}

void storage_gpio_set(int gpio_num, int val)
{
    volatile long long temp = 1;
    
    if (val == 0)
    {
        temp = ~(temp << gpio_num);
        GPIO0_O &= temp;
    }
    else
    {
        temp = temp << gpio_num;
        GPIO0_O |= temp;
    }  
}

#if 0
static void debugInit(int line,uint32 baud, uint8 data, uint8 parity, uint8 stop)
{
	/* disable interrupts */
	UART16550_WRITE(line,OFS_FIFO,FIFO_ENABLE|FIFO_RCV_RST|FIFO_XMT_RST|FIFO_TRIGGER_4);

	/* set up buad rate */
	{
		uint32 divisor;

		/* set DIAB bit */
		UART16550_WRITE(line,OFS_LINE_CONTROL, 0x80);

		/* set divisor */
		divisor = MAX_BAUD / baud;
		UART16550_WRITE(line,OFS_DIVISOR_LSB, divisor & 0xff);
		UART16550_WRITE(line,OFS_DIVISOR_MSB, (divisor & 0xff00) >> 8);

		/* clear DIAB bit */
	//	UART16550_WRITE(line,OFS_LINE_CONTROL, 0x0);
	}

	/* set data format */
	UART16550_WRITE(line,OFS_DATA_FORMAT, data | parity | stop);
	UART16550_WRITE(line,OFS_MODEM_CONTROL,0);
}


static uint8 getDebugChar(int line)
{

	while ((UART16550_READ(line,OFS_LINE_STATUS) & 0x1) == 0);
	return UART16550_READ(line,OFS_RCV_BUFFER);
}

static uint8 testDebugChar(int line)
{

	return (UART16550_READ(line,OFS_LINE_STATUS) & 0x1) ;
}

static int putDebugChar(int line,uint8 byte)
{
	while ((UART16550_READ(line,OFS_LINE_STATUS) & 0x20) == 0);
	UART16550_WRITE(line,OFS_SEND_BUFFER, byte);
	return 1;
}

static int initserial(int line)
{
		debugInit(line,CONS_BAUD,
			  UART16550_DATA_8BIT,
			  UART16550_PARITY_NONE, UART16550_STOP_1BIT);
	return 0;
}
#endif

void storage_board_init1(void)
{
    volatile long long temp = 1;  

    /* set gpio19/18/0/1/2 output */
    storage_gpio_config (LS2K_GPIO19, STORAGE_GPIO_OUTPUT);
    storage_gpio_config (LS2K_GPIO18, STORAGE_GPIO_OUTPUT);
    storage_gpio_config (LS2K_GPIO0, STORAGE_GPIO_OUTPUT);
    storage_gpio_config (LS2K_GPIO1, STORAGE_GPIO_OUTPUT);
    storage_gpio_config (LS2K_GPIO2, STORAGE_GPIO_OUTPUT);    
    
    /* 3.2 gpio 19 set 0 to disable config */
    storage_gpio_set(LS2K_GPIO19, 0);
    /* 3.3 config net 0 to 1Gbps : gpio[18 0 1 2] = 0 1 0 1 */
    storage_gpio_set(LS2K_GPIO18, 0);
    storage_gpio_set(LS2K_GPIO0, 1);
    storage_gpio_set(LS2K_GPIO1, 0);
    storage_gpio_set(LS2K_GPIO2, 1);
    
    /* 3.4 gpio 19 set 1 to enable config */
    storage_gpio_set(LS2K_GPIO19, 1);
    /* 3.5 delay sometime */
    storage_delay_us(1);
	/* 3.6 gpio 19 set 0 to disable config */
    storage_gpio_set(LS2K_GPIO19, 0);
    /* 3.7 config net 0 to 1Gbps : gpio[18 0 1 2] = 1 1 0 1 */
    storage_gpio_set(LS2K_GPIO18, 1);
    storage_gpio_set(LS2K_GPIO0, 1);
    storage_gpio_set(LS2K_GPIO1, 0);
    storage_gpio_set(LS2K_GPIO2, 1);
    /* 3.8 gpio 19 set 1 to enable config */
    storage_gpio_set(LS2K_GPIO19, 1);
    /* 3.9 delay sometime */
    storage_delay_us(1);

	/* 3.10 gpio 19 set 0 to disable config */
    storage_gpio_set(LS2K_GPIO19, 0);

    /* not update mode */
    storage_gpio_set(LS2K_GPIO18, 0);
    storage_gpio_set(LS2K_GPIO0, 0);
    storage_gpio_set(LS2K_GPIO1, 0);
    storage_gpio_set(LS2K_GPIO2, 0);
    storage_gpio_set(LS2K_GPIO19, 1);
    storage_delay_us(1);
    storage_gpio_set(LS2K_GPIO19, 0);
    

}

void storage_watchdog_test(void)
{
    while(1)
    {
//        storage_gpio_set(WATCH_DOG_PIN, 0);
//        storage_delay_us(1000);
//        storage_gpio_set(WATCH_DOG_PIN, 1);
//        storage_delay_us(100);
//        storage_gpio_set(WATCH_DOG_PIN, 0);
//        storage_delay_us(1000);
        storage_gpio_set(WATCH_DOG_PIN, 1);
        storage_delay_us(100);
        storage_gpio_set(WATCH_DOG_PIN, 0);
        storage_delay_us(100000);
    }
}

void storage_fpga_reset(void)
{
    storage_gpio_set(STORAGE_FPGA_RESET, 0);
    
    storage_delay_us(500);
}
