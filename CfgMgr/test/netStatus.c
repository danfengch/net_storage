#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>

struct ethtool_value {
        __uint32_t      cmd;
        __uint32_t      data;
};
 
 
/*
	function:	get network status (link up or down)
	params:
			network_name	IN	device name of network
	return:
			0	link up
			1	link down
			-1	get link status error
*/
int networkStatus(char *network_name){
	struct ethtool_value edata;
	int fd = -1;
	int err = 0;
	struct ifreq ifr;
 
 
	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, network_name);
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
	        perror("Cannot get control socket");
	        return -1;
	}
 
 
	edata.cmd = 0x0000000a;
	ifr.ifr_data = (caddr_t)&edata;
	err = ioctl(fd, 0x8946, &ifr);
	close(fd);
	
	if (err == 0) {
		//printf("Link detected: %s\n", edata.data ? "yes":"no");
		return (edata.data ? 0 : 1);
	} else if (errno != EOPNOTSUPP) {
		perror("Cannot get link status");
		return -1;
	}
}
 
 
int main(int argc, char* argv[])
{
	int status;
	
	status = networkStatus("eth0");
	if(status != -1){
		printf("Link status is %s\n", status  == 0 ? "up" : "down");
	}
	return 0;
}