//��ϣ 2000012824


#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include "cachelab.h"
#include<stdio.h>
#include<math.h>
struct lines
{
	int valid;//��Чλ
	unsigned tag;//���λ
	int waittime;//waittimeԽ�󣬱�ʾ���������ϴ��õ���ʱ��Խ��
};
char trace[1000];
unsigned s, b, S, E;
int hit = 0, miss = 0, eviction = 0;
struct lines** cache = NULL;
//cache��һ����ά���飬cache[i]��ʾ��i��,cache[i][j]��ʾ��i��ĵ�j��

void changestatus(unsigned long long address)
{
	unsigned add_tag = address >> (b + s);//�����ַ�ı��λ
	unsigned add_s = (address >> b) & ((1l << s) - 1);//�����ַ�ڵڼ���
	int finish = 0;//�ж��Ƿ����

	//�ж��Ƿ�����
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
		//�ж��Ƿ��пհ׵���
		miss++;
		for (int i = 0; i < E; i++)
			if (!cache[add_s][i].valid)//�ҵ���һ���հ׵���
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
		//�ж�Ҫ������һ��
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
	//���µȴ�ʱ��
	for (int i = 0; i < S; i++)
		for (int j = 0; j < E; j++)
			if (cache[i][j].valid)
				cache[i][j].waittime++;
	return;
}

//�����ļ�ÿһ�еĲ����Լ���ַ�����õ�ַȥ���ʸ��ٻ���
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
		//MҪ��������
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
	//������malloc����S�飬ÿ��E�е�cache�����Ҷ���Чλ���г�ʼ��
	S = (int)(pow(2,s));//һ���ж�����
	cache = malloc(sizeof(struct lines*) * S);
	for (int i = 0; i < S; i++)
		cache[i] = malloc(sizeof(struct lines) * E);
	for (int i = 0; i < S; i++)
		for (int j = 0; j < E; j++)
			cache[i][j].valid = 0;

	deal(trace);

	//�ͷſռ�
	for (unsigned i = 0; i < S; i++)
		free(cache[i]);
	free(cache);
	printSummary(hit, miss, eviction);
}

