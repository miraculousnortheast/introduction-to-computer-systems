//南希 2000012824


#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include "cachelab.h"
#include<stdio.h>
#include<math.h>
struct lines
{
	int valid;//有效位
	unsigned tag;//标记位
	int waittime;//waittime越大，表示这个块距离上次用到的时间越长
};
char trace[1000];
unsigned s, b, S, E;
int hit = 0, miss = 0, eviction = 0;
struct lines** cache = NULL;
//cache是一个二维数组，cache[i]表示第i组,cache[i][j]表示第i组的第j行

void changestatus(unsigned long long address)
{
	unsigned add_tag = address >> (b + s);//这个地址的标记位
	unsigned add_s = (address >> b) & ((1l << s) - 1);//这个地址在第几组
	int finish = 0;//判断是否完成

	//判断是否命中
	for (int i = 0; i < E; i++)
		if (cache[add_s][i].valid && cache[add_s][i].tag == add_tag)
		{
			hit++;
			cache[add_s][i].waittime = 0;
			finish = 1;
			break;
		}
	if (!finish)
	{
		//判断是否有空白的行
		miss++;
		for (int i = 0; i < E; i++)
			if (!cache[add_s][i].valid)//找到了一个空白的行
			{
				cache[add_s][i].valid = 1;
				cache[add_s][i].tag = add_tag;
				cache[add_s][i].waittime = 0;
				finish = 1;
				break;
			}
	}
	if (!finish)
	{
		//判断要驱逐哪一行
		eviction++;
		int maxwait = 0;
		int eviction_index;

		for (int i = 0; i < E; i++)
			if (maxwait < cache[add_s][i].waittime)
			{
				maxwait = cache[add_s][i].waittime;
				eviction_index = i;
			}
		cache[add_s][eviction_index].valid = 1;
		cache[add_s][eviction_index].tag = add_tag;
		cache[add_s][eviction_index].waittime = 0;
	}
	//更新等待时间
	for (int i = 0; i < S; i++)
		for (int j = 0; j < E; j++)
			if (cache[i][j].valid)
				cache[i][j].waittime++;
	return;
}

//读出文件每一行的操作以及地址，并用地址去访问高速缓存
void deal(char* filepath)
{
	char buf[1000] = { 0 };
	FILE* file = fopen(filepath, "r");
	char type;
	unsigned long long  address;
	unsigned bytes;
	while (fgets(buf, 1000, file) != NULL) {
		type = buf[1];
		if (type == 'L' || type == 'S' || type == 'M')
		{
			sscanf(buf + 2, "%llx, %u", &address, &bytes);
			changestatus(address);
		}
		//M要访问两遍
		if (type == 'M') {
			changestatus(address);
		}
	}
	fclose(file);
}
int main(int argc, char** argv) {
	int option;
	extern char* optarg;
	while ((option = getopt(argc, argv, "s:E:b:t:vh")) != -1) {
		if (option == 's')
			sscanf(optarg, "%u", &s);
		else if (option == 'E')
			sscanf(optarg, "%u", &E);
		else if (option == 'b')
			sscanf(optarg, "%u", &b);
		else if (option == 't')			
			snprintf(trace, sizeof(trace), "%s", optarg);

	}
	//这里用malloc分配S组，每组E行的cache，并且对有效位进行初始化
	S = (int)(pow(2,s));//一共有多少组
	cache = malloc(sizeof(struct lines*) * S);
	for (int i = 0; i < S; i++)
		cache[i] = malloc(sizeof(struct lines) * E);
	for (int i = 0; i < S; i++)
		for (int j = 0; j < E; j++)
			cache[i][j].valid = 0;

	deal(trace);

	//释放空间
	for (unsigned i = 0; i < S; i++)
		free(cache[i]);
	free(cache);
	printSummary(hit, miss, eviction);
}
