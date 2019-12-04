#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main (void)
{
    int fd;
    char buffer[50];
    int cnt = 0;
    int runtime = -1;
    
    if (0 > (fd = open("/proc/uptime", O_RDONLY)))
    {
        printf("open  /proc/uptime failed.\n");
        return runtime;
    }
    do
    {
        if (1 != read(fd, buffer + cnt, 1))
        {
            printf("read /proc/uptime failed.\n");
            goto exit;
        }
        cnt++;
    }while((buffer[cnt - 1] != ' ') && (buffer[cnt - 1] != EOF) && (cnt < sizeof(buffer)));
    
    if ((cnt < sizeof(buffer)) && (buffer[cnt - 1] == ' '))
    {
        buffer[cnt - 1] = 0;
        
        runtime = atoi(buffer);
        printf("runtime0 : %s\nruntime1 : %d\n", buffer, runtime);
    }
    else
    {
        printf("failed to read runtime from /proc/uptime\n");
    }
exit:    
    close(fd);
    return runtime;
}