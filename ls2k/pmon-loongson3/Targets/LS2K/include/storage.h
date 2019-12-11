
/** gpio pin number define */
#define LS2K_GPIO0                 0
#define LS2K_GPIO1                 1
#define LS2K_GPIO2                 2
#define LS2K_GPIO3                 3
#define LS2K_GPIO18                18
#define LS2K_GPIO19                19
#define LS2K_GPIO20                20




/** gpio input output define */
#define STORAGE_GPIO_OUTPUT         0
#define STORAGE_GPIO_INPUT          1

#define GPIO0_OEN                   (*(volatile long long *)0xbfe10500)
#define GPIO0_O                     (*(volatile long long *)0xbfe10510)

#define SYS_COMMON_CFG_0            (*(volatile long long *)0xbfe10420)
#define SYS_COMMON_CFG_1            (*(volatile long long *)0xbfe10428)


/** fpga reset pin */
#define STORAGE_FPGA_RESET          LS2K_GPIO3

#define PHY_RESET_PIN               LS2K_GPIO0

#define WATCH_DOG_PIN               LS2K_GPIO20

#define PHY_SPEED_10M               0
#define PHY_SPEED_100M              1
#define PHY_SPEED_1000M             2
#define PHY_SPEED_UNLINK            3

