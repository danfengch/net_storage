#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int getSmartctlAttrValue(FILE *fp, const char *name, char *val, int size)
{
    char line[1024];
    char *pos;
    int  len, copyLen = 0;
    
//    if (0 != fseek (fp, 0, SEEK_SET))
//    {
//        trace (DEBUG_ERR, SYSTEM, "%s : fseek error", __func__);
//        return -1;
//    }

    while (NULL != fgets (line, sizeof(line), fp))
    {
        if (NULL == (pos = strstr(line, name)))
            continue;

        len = strlen(name);
        //take out of ':'        
//        if (pos[len++] != ':')  continue;
        //take out of space
        while(pos[len] == ' ')  len++;

        //copy
        while((pos[len] != '\r') && (pos[len] != '\n') && (copyLen < (size - 1)))
        {
            val[copyLen++] = pos[len++];
        }
        val[copyLen] = 0;

        return 0;
    }

    val[copyLen++] = '-';
    val[copyLen++] = '-';
    val[copyLen++] = 0;
    
    return -1;
}

static int getDiskUseInfo (char *psize, char *pavail, char *pused, int lenMax)
{
    FILE *fp;
    char line[1024];
    int size = 0, avail = 0, used = 0, pos;
    
    if (NULL == (fp = popen ("df -k  |grep /dev/sda", "r")))
    {
        printf("popen df -k  |grep /dev/sda error");
        goto getDiskUseInfoExit;
    }

    while(NULL != fgets(line, sizeof(line), fp))
    {
        if (NULL == (pos = strstr(line, "/dev/sda2")))
            continue;

        pos += strlen("/dev/sda2");
        //take out of space
        while(line[pos] == ' ') pos++;

        size = 1024 * atoi(&line[pos]);
        //take out of szie & space
        while(line[pos] != ' ') pos++;
        while(line[pos] == ' ') pos++;

        used = 1024 * atoi(&line[pos]);
        //take out of used & space
        while(line[pos] != ' ') pos++;
        while(line[pos] == ' ') pos++;

        avail = 1024 * atoi(&line[pos]);

        break;
    }

    pclose (fp);

getDiskUseInfoExit:
    snprintf (psize, lenMax, "%d", size);
    printf ("%s\n", psize);
    snprintf (pavail, lenMax, "%d", avail);
    printf ("%s\n", pavail);
    snprintf (pused, lenMax, "%d", used);
    printf ("%s\n", pused);

    return 0;
}

int main (void)
{
    char temp[30];
    FILE *fp;
    char line[1024];

    printf ("Disk Info start");

    if (NULL == (fp = popen ("smartctl -a /dev/sda", "r")))
    {
        printf("popen smartctl -a /dev/sda error");
        return -1;
    }

    getSmartctlAttrValue(fp, "Device Model:", temp, 30);
    printf ("%s\n", temp);
    getSmartctlAttrValue(fp, "Serial Number:", temp, 30);
    printf ("%s\n", temp);
    getSmartctlAttrValue(fp, "Firmware Version:", temp, 30);  
    printf ("%s\n", temp);
    getSmartctlAttrValue(fp, "194 Temperature_Celsius     0x0000   100   100   000    Old_age   Offline      -", temp, 30);
    printf ("%s\n", temp);
    getSmartctlAttrValue(fp, "cacheBufferSize:", temp, 30);
    printf ("%s\n", temp);
    getSmartctlAttrValue(fp, "rate:", temp, 30);
    printf ("%s\n", temp);
    getSmartctlAttrValue(fp, "formFactor:", temp, 30);
    printf ("%s\n", temp);
    pclose (fp);
    
    getDiskUseInfo (temp, temp, temp, 30);

    printf ("Disk Info succ");

    return 0;
}

