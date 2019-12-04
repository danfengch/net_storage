#include <stdio.h>
#include <string.h>

int main (int argc, char **argv)
{
    FILE *fp;
    char line[1024];
    char *fileName;
    int lineNum = 0;

    if (argc != 2)
    {
        printf (" usage : fgets filename \n");
        return lineNum;
    }
    
    fileName = argv[1];
    if (NULL == (fp = fopen(fileName, "r")))
    {
        printf ("fopen %s failed\n", fileName);
        return lineNum;
    }

    while(NULL != fgets(line, sizeof(line), fp))
    {
        printf ("%s", line);
        lineNum++;
    }

    fclose(fp);

    return lineNum;
}
