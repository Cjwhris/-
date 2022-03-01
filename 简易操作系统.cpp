#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#include<Windows.h>

//结构体用结构体表示进程
typedef enum status { running, ready, block };
typedef struct Process
{
	char name[10];
	int priority;
	enum status sta;
	Process *next;
	int req;
};
//就绪、阻塞队列的头、尾节点 和 分配内存用的临时变量，Init为虚设
Process PRH, PBH, PRE, PBE;
Process *Init = NULL, *BlockHead = NULL;
Process *ReadyEnd = NULL, *BlockEnd = NULL;
Process *Pnode = NULL, *Running = NULL;

//用结构体表示临界资源
struct Resource
{
	bool occupied;
	int name;
	char ProName[10];//正在使用该资源的进程
};
Resource R1, R2, R3, R4;

//进程处理方法
//运行进程放到就绪队尾
void TimeOut()
{
	if (Running != NULL) {//当前有进程运行
		printf("shell>进程%s已就绪\n", Running->name);
		Running->sta = ready;
		ReadyEnd->next = Running;
		ReadyEnd = Running;
		Running->next = NULL;
		Running = NULL;
	}
}
//下个就绪进程进入
void NextR()
{
	Process *temp = NULL;
	Process *t = NULL;
	if (Init->next != NULL)
	{
		t = Init;
		do {
			temp = t;
			t = temp->next;
			if (t->priority == 1)//寻找优先级为1的进程
			{
				if (t->next != NULL) {
					Running = t;
					Running->sta = running;
					temp->next = t->next;
					Running->next = NULL;
				}
				else {
					Running = t;
					Running->sta = running;
					ReadyEnd = temp;
					ReadyEnd->next = NULL;
				}
				printf("shell>进程%s正在运行\n", Running->name);
				return;
			}
		} while (t->next != NULL);
		//只有优先级为2的进程，运行首个进程
		t = Init->next;
		if (t->next != NULL) {
			Running = t;
			Running->sta = running;
			Init->next = t->next;
			Running->next = NULL;
		}
		else {
			Running = t;
			Running->sta = running;
			ReadyEnd = Init;
			ReadyEnd->next = NULL;
		}
		printf("shell>进程%s正在运行\n", Running->name);
		return;
	}
	else {
		printf("shell>当前没有就绪进程可执行\n");
	}
}
//下个被该资源阻塞的进程进入
void NextB(Resource *R)
{
	Process *temp = NULL;
	Process *t = NULL;
	if (BlockHead->next != NULL)
	{
		t = BlockHead;
		do {
			temp = t;
			t = t->next;
			if (t->req == R->name)//寻找因R被阻塞的进程
			{
				Running->sta = ready;
				ReadyEnd->next = Running;
				ReadyEnd = Running;
				if (t->next != NULL) {
					Running = t;
					Running->sta = running;
					temp->next = t->next;
					Running->next = NULL;
				}
				else {
					Running = t;
					Running->sta = running;
					BlockEnd = temp;
					BlockEnd->next = NULL;
				}
				printf("shell>进程%s正在运行\n", Running->name);
				return;
			}
		} while (t->next != NULL);
		printf("shell>没有因资源R%d被阻塞的进程\n", R->name);
	}
	else {
		printf("shell>当前没有被阻塞进程\n");
	}
}


//创建
void Create(char name[10], int priority)
{
	bool check = true;
	Process *temp = Init, *t = NULL;
	if (priority != 1 && priority != 2)
	{
		printf("shell>输入的优先级不合法\n");
		return;
	}
	while (temp->next != NULL)
	{
		t = temp->next;
		if (0 == strcmp(t->name, name)) {
			check = false;
			break;
		}
		temp = t;
	}
	if (check) {
		TimeOut();//将现有进程放在就绪队尾
		Pnode = (Process*)malloc(sizeof(Process));
		strcpy_s(Pnode->name, name);
		Pnode->priority = priority;
		Pnode->sta = running;
		Pnode->req = 0;
		Running = Pnode;
		Pnode->next = NULL;
		printf("shell>创建进程%s，优先级为%d\nshell>进程%s正在运行\n", name, priority, name);
	}
	else {
		printf("shell>创建失败，此名称已被使用\n");
	}
}
//删除
void Destroy(char name[10])
{
	bool check = false;
	Process *temp = Init, *t = NULL;
	if (0 == strcmp(Running->name, name)) {
		free(Running);
		printf("shell>进程%s已删除\n", name);
		return;
	}
	while (temp->next != NULL)
	{
		t = temp;
		temp = temp->next;
		if (0 == strcmp(temp->name, name)) {
			t->next = temp->next;
			free(temp);
			check = true;
			break;
		}
	}
	temp = BlockHead;
	while (temp->next != NULL)
	{
		t = temp;
		temp = temp->next;
		if (0 == strcmp(temp->name, name)) {
			t->next = temp->next;
			free(temp);
			check = true;
			break;
		}
	}
	if (check) {
		printf("shell>进程%s已删除\n", name);
		if (0 == strcmp(R1.ProName, name))
		{
			R1.occupied = false;
			strcpy_s(R1.ProName, "");
			printf("shell>资源R1已被释放\n");
		}
		else if (0 == strcmp(R2.ProName, name))
		{
			R2.occupied = false;
			strcpy_s(R2.ProName, "");
			printf("shell>资源R2已被释放\n");
		}
		else if (0 == strcmp(R3.ProName, name))
		{
			R3.occupied = false;
			strcpy_s(R3.ProName, "");
			printf("shell>资源R3已被释放\n");
		}
		else if (0 == strcmp(R4.ProName, name))
		{
			R4.occupied = false;
			strcpy_s(R4.ProName, "");
			printf("shell>资源R4已被释放\n");
		}
		NextR();
	}
	else {
		printf("shell>找不到进程%s\n", name);
	}
}
//申请
void Require(Resource *R)
{
	if (Running == NULL)
	{
		printf("shell>当前没有运行的进程\n");
		return;
	}
	if (R->occupied == false) {//资源可用
		R->occupied = true;
		Running->req = R->name;
		strcpy_s(R->ProName, Running->name);
		printf("shell>进程%s申请了资源R%d\n", Running->name, R->name);
	}
	else {//资源被占用
		printf("shell>进程%s被阻塞\n", Running->name);
		Running->sta = block;
		Running->req = R->name;
		BlockEnd->next = Running;
		BlockEnd = Running;
		Running = NULL;
		NextR();
	}
}
//释放
void Release(Resource *R)
{
	if (R->occupied == false) {//资源可用
		printf("shell>该资源未被使用\n");
	}
	else {//资源被占用
		R->occupied = false;
		strcpy_s(R->ProName, "");
		printf("shell>资源R%d已被释放\n", R->name);
		NextB(R);
	}
}

//清除全部进程
void deall()
{
	Process *temp = NULL;
	Process *t = NULL;
	if (Init->next != NULL)
	{
		t = Init->next;
		temp = t;
		while (true)
		{
			t = temp;
			if (t->next == NULL)
			{
				free(t);
				break;
			}
			else
			{
				temp = t->next;
				free(t);
			}
		}
	}
	if (BlockHead->next != NULL)
	{
		t = BlockHead->next;
		temp = t;
		while (true)
		{
			t = temp;
			if (t->next == NULL)
			{
				free(t);
				break;
			}
			else
			{
				temp = t->next;
				free(t);
			}
		}
	}
	ReadyEnd = Init;
	BlockEnd = BlockHead;
	R1.occupied = false;
	strcpy_s(R1.ProName, "");
	R2.occupied = false;
	strcpy_s(R2.ProName, "");
	R3.occupied = false;
	strcpy_s(R3.ProName, "");
	R4.occupied = false;
	strcpy_s(R4.ProName, "");
	printf("shell>已删除全部进程\n");
}

//显示就绪列表
void ListReady()
{
	bool first = true;
	Process *temp = Init;
	printf("shell>2:");
	while (temp->next != NULL)
	{
		temp = temp->next;
		if (temp->priority == 2 && first){
			printf("%s", temp->name);
			first = false;
		}
		else if (temp->priority == 2 && !first){
			printf("-%s", temp->name);
		}
	}
	first = true;
	printf("\nshell>1:");
	temp = Init;
	while (temp->next != NULL)
	{
		temp = temp->next;
		if (temp->priority == 1 && first) {
			printf("%s", temp->name);
			first = false;
		}
		else if (temp->priority == 1 && !first) {
			printf("-%s", temp->name);
		}
	}
	first = true;
	printf("\nshell>0:Init\n");
}
//显示阻塞列表
void ListBlock()
{
	Process *temp = BlockHead;
	if (temp->next == NULL)
	{
		printf("shell>没有被阻塞的程序\n");
		return;
	}
	do {
		temp = temp->next;
		printf("shell>进程%s正在等待资源R%d\n", temp->name, temp->req);
	} while (temp->next != NULL);
}
//显示资源状态
void ListRes()
{
	if (R1.occupied == true){
		printf("shell>R1可用\n");
	}
	else {
		printf("shell>R1不可用\n");
	}
	if (R2.occupied == true) {
		printf("shell>R2可用\n");
	}
	else {
		printf("shell>R2不可用\n");
	}
	if (R3.occupied == true) {
		printf("shell>R3可用\n");
	}
	else {
		printf("shell>R3不可用\n");
	}
	if (R4.occupied == true) {
		printf("shell>R4可用\n");
	}
	else {
		printf("shell>R4不可用\n");
	}
}
//显示PCB状态
void PR(char name[10])
{
	if (0 == strcmp(Running->name, name)) {
		printf("shell>进程%s\n", name);
		if (Running->req != 0) {
			printf("shell>请求资源R%d\n", Running->req);
		}
		printf("shell>状态为运行\n");
		printf("shell>优先级为%d\n", Running->priority);
		return;
	}
	Process *temp = Init;
	while (temp->next != NULL)
	{
		temp = temp->next;
		if (0 == strcmp(temp->name, name)) {
			printf("shell>进程%s信息：\n", name);
			if (temp->req != 0) {
				printf("shell>请求资源R%d\n", temp->req);
			}
			printf("shell>状态为就绪\n");
			printf("shell>优先级为%d\n", temp->priority);
			return;
		}
	}
	temp = BlockHead;
	while (temp->next != NULL)
	{
		temp = temp->next;
		if (0 == strcmp(temp->name, name)) {
			printf("shell>进程%s\n", name);
			if (temp->req != 0) {
				printf("shell>请求资源R%d\n", temp->req);
			}
			printf("shell>状态为阻塞\n");
			printf("shell>优先级为%d\n", temp->priority);
			return;
		}
	}
	printf("shell>查无此进程\n");
}

int main(void)
{
	system("color f1");//白底蓝字
	SetConsoleTitleA("Text Shell");	

	char commands[30] = "";
	char name[10] = "";
	int priority = 0;
	bool initialized = false;

	printf("shell>");
	scanf_s("%s", commands, 30);

	if (0 == strcmp(commands, "init")) {
		//初始化链表
		Init = (Process*)malloc(sizeof(Process));
		BlockHead = (Process*)malloc(sizeof(Process));
		Init->next = NULL;
		BlockHead->next = NULL;
		ReadyEnd = Init;
		BlockEnd = BlockHead;
		ReadyEnd->next = NULL;
		BlockEnd->next = NULL;
		initialized = true;

		//初始化资源
		R1.occupied = false;
		R2.occupied = false;
		R3.occupied = false;
		R4.occupied = false;
		strcpy_s(R1.ProName, "");
		strcpy_s(R2.ProName, "");
		strcpy_s(R3.ProName, "");
		strcpy_s(R4.ProName, "");
		R1.name = 1;
		R2.name = 2;
		R3.name = 3;
		R4.name = 4;
		printf("shell>初始化完成\n");
	}

	if (initialized)
	{
		while (true) {
			strcpy_s(commands, "");
			printf("shell>");
			scanf_s("%s", commands, 30);
			//时间片用尽
			if (0 == strcmp(commands, "to")) {
				if (Init->next == NULL)
				{
					printf("shell>没有就绪的进程\n");
					continue;
				}
				TimeOut();
				NextR();
				continue;
			}

			//创建
			else if (0 == strcmp(commands, "cr")) {
				scanf_s("%s", name, 10);
				scanf_s("%d", &priority);
				Create(name, priority);
				continue;
			}
			//删除
			else if (0 == strcmp(commands, "de")) {
				scanf_s("%s", name, 10);
				Destroy(name);
				continue;
			}
			//申请
			else if (0 == strcmp(commands, "req")) {
				scanf_s("%s", name, 10);
				if (0 == strcmp(name, "R1")) {
					Require(&R1);
				}
				else if (0 == strcmp(name, "R2")) {
					Require(&R2);
				}
				else if (0 == strcmp(name, "R3")) {
					Require(&R3);
				}
				else if (0 == strcmp(name, "R4")) {
					Require(&R4);
				}
				else {
					printf("shell>不存在资源%s\n", name);
				}
				continue;
			}
			//释放
			else if (0 == strcmp(commands, "rel")) {
				scanf_s("%s", name, 10);
				if (0 == strcmp(name, "R1")) {
					Release(&R1);
				}
				else if (0 == strcmp(name, "R2")) {
					Release(&R2);
				}
				else if (0 == strcmp(name, "R3")) {
					Release(&R3);
				}
				else if (0 == strcmp(name, "R4")) {
					Release(&R4);
				}
				else {
					printf("shell>不存在资源%s\n", name);
				}
				continue;
			}

			//查看进程与资源状态
			else if (0 == strcmp(commands, "list")) {
				scanf_s("%s", commands, 30);
				if (0 == strcmp(commands, "ready")) {
					ListReady();
					continue;
				}
				else if (0 == strcmp(commands, "block")) {
					ListBlock();
					continue;
				}
				else if (0 == strcmp(commands, "res")) {
					ListRes();
					continue;
				}
				else {
					continue;
				}
			}

			//可选命令pr
			else if (0 == strcmp(commands, "pr")) {
				scanf_s("<%s>", name, 10);
				PR(name);
				continue;
			}

			//初始化、删除所有进程
			else if (0 == strcmp(commands, "init")) {
				deall();
				Init->next = NULL;
				BlockHead->next = NULL;
				initialized = true;
				printf("shell>初始化完成\n");
				continue;
			}

			//使用quit指令退出
			else if (0 == strcmp(commands, "quit")) {
				deall();
				free(Init);
				free(BlockHead);
				break;
			}
			
			else {
				continue;
			}
		}
	}
	else {
		printf("shell>请先初始化\n");
	}
	printf("\nshell>正在退出管理\n按任意键退出");
	getchar();
	getchar();
	return 0;
}