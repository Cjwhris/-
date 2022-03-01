#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#include<Windows.h>

//�ṹ���ýṹ���ʾ����
typedef enum status { running, ready, block };
typedef struct Process
{
	char name[10];
	int priority;
	enum status sta;
	Process *next;
	int req;
};
//�������������е�ͷ��β�ڵ� �� �����ڴ��õ���ʱ������InitΪ����
Process PRH, PBH, PRE, PBE;
Process *Init = NULL, *BlockHead = NULL;
Process *ReadyEnd = NULL, *BlockEnd = NULL;
Process *Pnode = NULL, *Running = NULL;

//�ýṹ���ʾ�ٽ���Դ
struct Resource
{
	bool occupied;
	int name;
	char ProName[10];//����ʹ�ø���Դ�Ľ���
};
Resource R1, R2, R3, R4;

//���̴�����
//���н��̷ŵ�������β
void TimeOut()
{
	if (Running != NULL) {//��ǰ�н�������
		printf("shell>����%s�Ѿ���\n", Running->name);
		Running->sta = ready;
		ReadyEnd->next = Running;
		ReadyEnd = Running;
		Running->next = NULL;
		Running = NULL;
	}
}
//�¸��������̽���
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
			if (t->priority == 1)//Ѱ�����ȼ�Ϊ1�Ľ���
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
				printf("shell>����%s��������\n", Running->name);
				return;
			}
		} while (t->next != NULL);
		//ֻ�����ȼ�Ϊ2�Ľ��̣������׸�����
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
		printf("shell>����%s��������\n", Running->name);
		return;
	}
	else {
		printf("shell>��ǰû�о������̿�ִ��\n");
	}
}
//�¸�������Դ�����Ľ��̽���
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
			if (t->req == R->name)//Ѱ����R�������Ľ���
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
				printf("shell>����%s��������\n", Running->name);
				return;
			}
		} while (t->next != NULL);
		printf("shell>û������ԴR%d�������Ľ���\n", R->name);
	}
	else {
		printf("shell>��ǰû�б���������\n");
	}
}


//����
void Create(char name[10], int priority)
{
	bool check = true;
	Process *temp = Init, *t = NULL;
	if (priority != 1 && priority != 2)
	{
		printf("shell>��������ȼ����Ϸ�\n");
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
		TimeOut();//�����н��̷��ھ�����β
		Pnode = (Process*)malloc(sizeof(Process));
		strcpy_s(Pnode->name, name);
		Pnode->priority = priority;
		Pnode->sta = running;
		Pnode->req = 0;
		Running = Pnode;
		Pnode->next = NULL;
		printf("shell>��������%s�����ȼ�Ϊ%d\nshell>����%s��������\n", name, priority, name);
	}
	else {
		printf("shell>����ʧ�ܣ��������ѱ�ʹ��\n");
	}
}
//ɾ��
void Destroy(char name[10])
{
	bool check = false;
	Process *temp = Init, *t = NULL;
	if (0 == strcmp(Running->name, name)) {
		free(Running);
		printf("shell>����%s��ɾ��\n", name);
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
		printf("shell>����%s��ɾ��\n", name);
		if (0 == strcmp(R1.ProName, name))
		{
			R1.occupied = false;
			strcpy_s(R1.ProName, "");
			printf("shell>��ԴR1�ѱ��ͷ�\n");
		}
		else if (0 == strcmp(R2.ProName, name))
		{
			R2.occupied = false;
			strcpy_s(R2.ProName, "");
			printf("shell>��ԴR2�ѱ��ͷ�\n");
		}
		else if (0 == strcmp(R3.ProName, name))
		{
			R3.occupied = false;
			strcpy_s(R3.ProName, "");
			printf("shell>��ԴR3�ѱ��ͷ�\n");
		}
		else if (0 == strcmp(R4.ProName, name))
		{
			R4.occupied = false;
			strcpy_s(R4.ProName, "");
			printf("shell>��ԴR4�ѱ��ͷ�\n");
		}
		NextR();
	}
	else {
		printf("shell>�Ҳ�������%s\n", name);
	}
}
//����
void Require(Resource *R)
{
	if (Running == NULL)
	{
		printf("shell>��ǰû�����еĽ���\n");
		return;
	}
	if (R->occupied == false) {//��Դ����
		R->occupied = true;
		Running->req = R->name;
		strcpy_s(R->ProName, Running->name);
		printf("shell>����%s��������ԴR%d\n", Running->name, R->name);
	}
	else {//��Դ��ռ��
		printf("shell>����%s������\n", Running->name);
		Running->sta = block;
		Running->req = R->name;
		BlockEnd->next = Running;
		BlockEnd = Running;
		Running = NULL;
		NextR();
	}
}
//�ͷ�
void Release(Resource *R)
{
	if (R->occupied == false) {//��Դ����
		printf("shell>����Դδ��ʹ��\n");
	}
	else {//��Դ��ռ��
		R->occupied = false;
		strcpy_s(R->ProName, "");
		printf("shell>��ԴR%d�ѱ��ͷ�\n", R->name);
		NextB(R);
	}
}

//���ȫ������
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
	printf("shell>��ɾ��ȫ������\n");
}

//��ʾ�����б�
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
//��ʾ�����б�
void ListBlock()
{
	Process *temp = BlockHead;
	if (temp->next == NULL)
	{
		printf("shell>û�б������ĳ���\n");
		return;
	}
	do {
		temp = temp->next;
		printf("shell>����%s���ڵȴ���ԴR%d\n", temp->name, temp->req);
	} while (temp->next != NULL);
}
//��ʾ��Դ״̬
void ListRes()
{
	if (R1.occupied == true){
		printf("shell>R1����\n");
	}
	else {
		printf("shell>R1������\n");
	}
	if (R2.occupied == true) {
		printf("shell>R2����\n");
	}
	else {
		printf("shell>R2������\n");
	}
	if (R3.occupied == true) {
		printf("shell>R3����\n");
	}
	else {
		printf("shell>R3������\n");
	}
	if (R4.occupied == true) {
		printf("shell>R4����\n");
	}
	else {
		printf("shell>R4������\n");
	}
}
//��ʾPCB״̬
void PR(char name[10])
{
	if (0 == strcmp(Running->name, name)) {
		printf("shell>����%s\n", name);
		if (Running->req != 0) {
			printf("shell>������ԴR%d\n", Running->req);
		}
		printf("shell>״̬Ϊ����\n");
		printf("shell>���ȼ�Ϊ%d\n", Running->priority);
		return;
	}
	Process *temp = Init;
	while (temp->next != NULL)
	{
		temp = temp->next;
		if (0 == strcmp(temp->name, name)) {
			printf("shell>����%s��Ϣ��\n", name);
			if (temp->req != 0) {
				printf("shell>������ԴR%d\n", temp->req);
			}
			printf("shell>״̬Ϊ����\n");
			printf("shell>���ȼ�Ϊ%d\n", temp->priority);
			return;
		}
	}
	temp = BlockHead;
	while (temp->next != NULL)
	{
		temp = temp->next;
		if (0 == strcmp(temp->name, name)) {
			printf("shell>����%s\n", name);
			if (temp->req != 0) {
				printf("shell>������ԴR%d\n", temp->req);
			}
			printf("shell>״̬Ϊ����\n");
			printf("shell>���ȼ�Ϊ%d\n", temp->priority);
			return;
		}
	}
	printf("shell>���޴˽���\n");
}

int main(void)
{
	system("color f1");//�׵�����
	SetConsoleTitleA("Text Shell");	

	char commands[30] = "";
	char name[10] = "";
	int priority = 0;
	bool initialized = false;

	printf("shell>");
	scanf_s("%s", commands, 30);

	if (0 == strcmp(commands, "init")) {
		//��ʼ������
		Init = (Process*)malloc(sizeof(Process));
		BlockHead = (Process*)malloc(sizeof(Process));
		Init->next = NULL;
		BlockHead->next = NULL;
		ReadyEnd = Init;
		BlockEnd = BlockHead;
		ReadyEnd->next = NULL;
		BlockEnd->next = NULL;
		initialized = true;

		//��ʼ����Դ
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
		printf("shell>��ʼ�����\n");
	}

	if (initialized)
	{
		while (true) {
			strcpy_s(commands, "");
			printf("shell>");
			scanf_s("%s", commands, 30);
			//ʱ��Ƭ�þ�
			if (0 == strcmp(commands, "to")) {
				if (Init->next == NULL)
				{
					printf("shell>û�о����Ľ���\n");
					continue;
				}
				TimeOut();
				NextR();
				continue;
			}

			//����
			else if (0 == strcmp(commands, "cr")) {
				scanf_s("%s", name, 10);
				scanf_s("%d", &priority);
				Create(name, priority);
				continue;
			}
			//ɾ��
			else if (0 == strcmp(commands, "de")) {
				scanf_s("%s", name, 10);
				Destroy(name);
				continue;
			}
			//����
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
					printf("shell>��������Դ%s\n", name);
				}
				continue;
			}
			//�ͷ�
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
					printf("shell>��������Դ%s\n", name);
				}
				continue;
			}

			//�鿴��������Դ״̬
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

			//��ѡ����pr
			else if (0 == strcmp(commands, "pr")) {
				scanf_s("<%s>", name, 10);
				PR(name);
				continue;
			}

			//��ʼ����ɾ�����н���
			else if (0 == strcmp(commands, "init")) {
				deall();
				Init->next = NULL;
				BlockHead->next = NULL;
				initialized = true;
				printf("shell>��ʼ�����\n");
				continue;
			}

			//ʹ��quitָ���˳�
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
		printf("shell>���ȳ�ʼ��\n");
	}
	printf("\nshell>�����˳�����\n��������˳�");
	getchar();
	getchar();
	return 0;
}