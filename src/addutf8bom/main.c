#include <stdio.h>
#include <stdlib.h>

FILE *f = NULL;
void *pBuf;
size_t cbBuf;

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("Usage: %s path", argv[0]);
		return 0;
	}
	
	f = fopen(argv[1], "r+b");
    if(!f)
        return -1;
    
    fseek(f, 0, SEEK_END);
    cbBuf = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    pBuf = malloc(cbBuf);
    cbBuf = fread(pBuf, 1, cbBuf, f);
    
    fseek(f, 0, SEEK_SET);
    
    fwrite("\xef\xbb\xbf", 3, 1, f);
    fwrite(pBuf, 1, cbBuf, f);
    
    free(pBuf);
    
    fclose(f);
    
	//printf("Done!\n");
    
	return 0;
}
