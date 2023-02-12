#include "stdio.h"
 
int main(int argc,char** argv)
{
	FILE *fp;
	char load_file[256];
	char line[1000];
	scanf("%s",load_file);
	fp=fopen(load_file,"r");
	if(fp==NULL)
	{
		printf("can not load file!");
		return 1;
	}
	while(!feof(fp))
	{
		fgets(line,1000,fp);
		
	}
	printf("%s",line);
	fclose(fp);
	return 0;
}

