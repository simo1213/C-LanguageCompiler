#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctype.h>
#include <stdlib.h>
#include <stack>
#include <unordered_map>
using namespace std;
#define Max_Qsize 50


/*结构体定义*/
//四元式
struct Quat
{
	string symbal;  //运算符
	string op_1;   //第一个操作数
	string op_2;   //第二个操作数
	string result;  //结果
};
Quat Total_Q[Max_Qsize];  //四元式生成结果
stack <string> chara;		//运算符栈

//字符串常量表中数据
struct Data
{
	int loca;	//
	string code;
};//表示某个表中的某个元素


//构造链表节点
typedef struct node
{
	Data table;
	node* next;
}LList;

//符号表
typedef struct SYNBL {
	string name;
	string type;
	char CAT;
	string ADDR;
}SYNBL;
SYNBL ALL_SYNBL[50];
int synbl_index = 0;
SYNBL temp_F = { "","",' ',"" };

//标识符和常量的结构体
typedef struct I {
	int num;	//表里的编号
	string content;	//表的内容，标识符和常量都存成string类型
	SYNBL* p;//指向符号表
	I* next;//指向下一个节点
}I;
I* temp_I;//改
//Token串
typedef struct Token
{
	string ch;
	int value;
}Token;

/*函数定义*/
void NextLine();//读入程序
bool MorAnaly(string s);//词法分析
int Act(string buffer);//?状态转换
void GAnaly(string s);//语法分析
void NextString();
void S();//改
void A();
void C();
bool X();
bool Y();
void Z();
void Z_1();
void U();
void U_1();
void T();
void T_1();
bool R();
void L();
void L_1();
bool isID(string ss);      //是否是标识符
bool isNUM(string ss);      //是否是标识符
void SaveToken();//保存Token串
void PrintToken();//打印TOKEN
void transform(string temp, int state);//temp是当前单词 state是当前状态
bool isCHAR(string ss);		//判断是不是字符常量
void F();//推出变量，常量，（算术表达式）
void err();//报错
string Ran_name();//产生新变量名
Quat New_Quat(string symbal, string op_1, string op_2, string result);//产生四元式
void math_quat();//算术表达式的四元式产生
void Create_VALL();		//活动记录的构建
bool Search_SYNBL(string s);		//查符号表 检测未定义的标识符

//拓展函数定义
void B();//根据扫描到的第一个单词调用子程序B1-B4
void B1();//if_else
void B2();//while
void B3();//for
void B4();//printf
void B5();
void P();//被printf调用，扫描printf双引号“”内部内容
void E();//if、while、for括号内条件判定
void E1();//推出比较符号
void Q();//推出 id++/id--/空
void Q1();//推出++/--/空
//拓展-语义-函数声明 
void IF();//if-else
void EL();
void GEQ();
void IE();
void WH();//while
void DO();
void WE();
void For();//for
void For_do();
void GEQ_quat_Q();
void For_q3();
void Print();//print
void q2();//do-while
void q1();//

/*变量定义*/
string LinePro;		//读入文件中的一行程序
string str;			//每一个单词
ifstream fin("procedure.txt");//打开文件
int index = 0;		//LinePro的下标
int len;			//LinePro的长度
int ibuffer = 0;	//buffer的下标
string KTable[10] = { "K","int","main","char","if","else","while","for","printf","do" };
string PTable[20] = { "P","-","/","(",")","==","!=","<=","<","+","*",">=",">","=",",",";","++","--","{","}" };
Token token;
vector <string> TokenTemp;	//储存Token串
I* ITable = new I[sizeof(I)]();		//标识符表
I* C1Table = new I[sizeof(I)]();			//整数表
LList* CTTable = new LList[sizeof(LList)]();//字符常量
LList* STTable = new LList[sizeof(LList)]();//字符串常量
int ran_num = 0;  //新变量名Ti
int Q_num = 0;   //四元式的个数
stack <string>* SEM = new stack<string>;		//语义栈，暂存运算对象的属性值
stack <string>* VALL = new stack<string>;		//活动记录
int lyer = 0;		//层数
int VALL_index = 0;		//活动记录脚标
string Fuhao;
int stateY = 0;		//标识符类型
int for_flag = 0;//for语句标志

//产生新变量名
string Ran_name()
{
	ran_num++;
	string s = "t" + to_string(ran_num);
	return s;
}

//产生四元式
Quat New_Quat(string symbal, string op_1, string op_2, string result)
{
	Quat temp_Quat = { symbal,op_1,op_2,result };
	return temp_Quat;
}
void Create_VALL()		//改
{
	VALL->push("0");
	VALL->push("0");
	VALL->push("?");
	VALL_index += 3;
}
bool Search_SYNBL(string s)		//查符号表，检测未定义的标识符
{
	for (int i = 0; i < synbl_index; i++)
	{
		if (s == ALL_SYNBL[i].name) return 1;
	}
	return 0;
}

bool isCHAR(string ss)		//判断是不是字符常量
{
	LList* p = CTTable;
	string ss_1 = "";
	ss_1 += ss[1];
	while (p)
	{
		if (p->table.code.compare(ss_1) == 0)
		{
			return 1;
		}
		p = p->next;
	}
	return 0;
}
//报错
void err()
{
	delete SEM, VALL;//删除语义栈 改
	cout << "ERROR!" << endl;
	exit(0);
}
//输入程序
void NextLine()
{
	index = 0;
	bool flag = 1;
	getline(fin, LinePro);
	len = LinePro.length();
}

//读入一个词,读到界符停止
void NextString()
{
	str.clear();
	bool legal;
	if (index >= len)//如果读完一个行就读下一行再分词
	{
		NextLine();
		index = 0;
		NextString();//改1
	}
	else {
		while (index < len)
		{
			if ((LinePro[index] >= 'a' && LinePro[index] <= 'z') || (LinePro[index] >= 'A' && LinePro[index] <= 'Z') || (LinePro[index] >= '0' && LinePro[index] <= '9') || LinePro[index] == '_')
				legal = 1;
			else legal = 0;
			if (legal)
			{
				str += LinePro[index];
				index++;
			}
			//合法字符为字母、数字、下划线，其余为界符，if内为如果不合法
			else if (str.length() == 0 && !legal)
			{
				//如果为双引号,则识别为字符串，把引号连带着其中内容都传进str
				if (LinePro[index] == '\"')
				{
					str += LinePro[index];		//把左边的引号传入str
					index++;
					while (index < len && LinePro[index] != '\"')	//一直扫描到另一个引号跳出循环
					{
						str += LinePro[index];
						index++;
					}
					str += LinePro[index];//把右边的双引号加到str内
					index++;
					break;
				}
				//如果是单引号则识别为字符串
				else if (LinePro[index] == '\'')
				{
					str += LinePro[index];		//把左边的引号传入str
					index++;
					while (index < len && LinePro[index] != '\'')	//一直扫描到另一个引号跳出循环
					{
						str += LinePro[index];
						index++;
					}
					str += LinePro[index];//把右边的双引号加到str内
					index++;
					break;
				}
				//如果是空格就跳过
				else if (LinePro[index] == ' ') index++;
				//如果是==、!=、<=、>=、++、--
				else if ((LinePro[index] == '=' && LinePro[index + 1] == '=') ||
					(LinePro[index] == '!' && LinePro[index + 1] == '=') ||
					(LinePro[index] == '<' && LinePro[index + 1] == '=') ||
					(LinePro[index] == '>' && LinePro[index + 1] == '=') ||
					(LinePro[index] == '+' && LinePro[index + 1] == '+') ||
					(LinePro[index] == '-' && LinePro[index + 1] == '-'))
				{
					str += LinePro[index];
					index++;
					str += LinePro[index];
					index++;
					break;
				}
				//其他情况：读入单个界符
				else
				{
					str += LinePro[index];
					index++;
					break;
				}

			}
			else break;
		}
		if (MorAnaly(str)) {
			PrintToken();//新加的
			return;
		}//如果词法分析没问题就返回
		else err();					//不然就输出错误结果
	}
}
//词法分析
bool MorAnaly(string s)
{
	s += '#';

	int state = 0;
	state = Act(s);
	if (state == 2)
	{
		err();
		return 0;
	}
	SaveToken();
	return 1;
}
//标识符和常量表增加新结点
void addInode(I* L, int num, string content)
{
	I* p = new I;
	I* q = L;
	while (q->next)
	{
		q = q->next;
	}
	q->next = p;
	p->next = NULL;
	p->num = num;
	p->content = content;
}
//字符常量表、字符串常量表插入新节点
void addDatanode(LList* L, Data datanode) {
	LList* p = new LList;
	LList* q = L;
	while (q->next)
	{
		q = q->next;
	}
	q->next = p;
	p->next = NULL;
	p->table = datanode;
}
//状态转换
//状态转换
void transform(string temp, int state)//temp是当前单词 state是当前状态
{

	if (state == 8)
	{
		int num = 1; //判断是否为关键字或标识符
		for (num = 1; num < 10; num++)
		{
			if (KTable[num] == temp)
			{
				//Token token;
				token.ch = "K";
				token.value = num;
				//addnewtoken(TOKEN, token);
				break;
			}
		}
		if (num == 10)//关键字表中没有找到
		{
			int judge = 0;
			I* p = ITable->next;
			while (p)
			{
				if (p->content == temp)
				{
					judge = 1;//表示在现有的标识符表中找到了，即该标识符已经在表中
					break;
				}
				p = p->next;
			}
			if (judge == 0)//现有的标识符表中没找到
			{
				//Data data;
				//data.loca = 0;
				int num = 0;
				string s;
				I* p = ITable;
				//p->num = 0;
				while (p)//找到最后一个插入的位置
				{
					num++;
					p = p->next;
				}
				s = temp;
				addInode(ITable, num, s);//标识符表中中新加一个标识符
			//	Token token;
				token.ch = "I";
				token.value = num;
				//addnewtoken(TOKEN, token);
			}
			else if (judge == 1)//现有的标识符表中找到了
			{
				//Token token;
				token.ch = "I";
				token.value = p->num;
				//addnewtoken(TOKEN, token);
			}
		}
	}
	else if (state == 9)
	{
		int judge = 0;
		I* p = C1Table->next;
		while (p)
		{
			if (p->content.compare(temp) == 0)//temp已经在常数表中
			{
				judge = 1;
				break;
			}
			p = p->next;
		}
		if (judge == 0)
		{
			//Data data;
			//data.loca = 0;
			int num = 0;
			string s;
			I* p = C1Table;
			while (p)
			{
				num++;
				p = p->next;
			}
			s = temp;
			addInode(C1Table, num, s);
			//Token token;
			token.ch = "C1";
			token.value = num;
			//addnewtoken(TOKEN, token);
		}
		else if (judge == 1)
		{
			//Token token;
			token.ch = "C1";
			token.value = p->num;
			//addnewtoken(TOKEN, token);
		}
	}
	else if (state == 20) {
		temp = temp.substr(1);//除去字符前的‘
		int judge = 0;
		LList* p = CTTable->next;
		while (p)
		{
			if (p->table.code == temp)
			{
				judge = 1;
				break;
			}
			p = p->next;
		}
		if (judge == 0)
		{
			Data data;
			data.loca = 0;
			LList* p = CTTable;
			while (p)
			{
				data.loca++;
				p = p->next;
			}
			data.code = temp;
			addDatanode(CTTable, data);
			//	Token token;
			token.ch = "CT";
			token.value = data.loca;
			//addnewtoken(TOKEN, token);
		}
		else if (judge == 1)
		{
			//Token token;
			token.ch = "CT";
			token.value = p->table.loca;
			//addnewtoken(TOKEN, token);
		}
	}
	else if (state == 22)
	{
		temp = temp.substr(1);
		int judge = 0;
		LList* p = STTable->next;
		while (p)
		{
			if (p->table.code == temp)
			{
				judge = 1;
				break;
			}
			p = p->next;
		}
		if (judge == 0)
		{
			Data data;
			data.loca = 0;
			LList* p = STTable;
			while (p)
			{
				data.loca++;
				p = p->next;
			}
			data.code = temp;
			addDatanode(STTable, data);
			//Token token;
			token.ch = "ST";
			token.value = data.loca;
			//addnewtoken(TOKEN, token);
		}
		else if (judge == 1)
		{
			//Token token;
			token.ch = "ST";
			token.value = p->table.loca;
			//addnewtoken(TOKEN, token);
		}
	}

	else//是界符
	{
		int num = 1;
		//查找界符对应的位置
		for (int num = 1; num <= 19; num++)
		{
			if (PTable[num] == temp)
			{
				//	Token token;
				token.ch = 'P';
				token.value = num;
				//addnewtoken(TOKEN, token);
				break;
			}
		}
	}
}
//每个状态下的动作
//每个状态下的动作
int Act(string buffer)//?状态转换
{
	ibuffer = 0;
	int state = 1;
	string temp;
	while (buffer[ibuffer])
	{
		switch (state)
		{
		case 1:
		{
			if (buffer[ibuffer] == '#')
				return 1;
			else if (buffer[ibuffer] == ' ' || buffer[ibuffer] == '\n')//空格或换行则跳过
			{
				state = 1;
				ibuffer++;
			}
			else if ((buffer[ibuffer] >= 'a' && buffer[ibuffer] <= 'z') || (buffer[ibuffer] >= 'A' && buffer[ibuffer] <= 'Z'))//????????????2??
			{
				state = 2;//读入字母，转入2状态
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else if (buffer[ibuffer] >= '0' && buffer[ibuffer] <= '9')
			{
				state = 3;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else if (buffer[ibuffer] == '+')
			{
				state = 4;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else if (buffer[ibuffer] == '<')
			{
				state = 5;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else if (buffer[ibuffer] == '=')
			{
				state = 6;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else if (buffer[ibuffer] == '/' || buffer[ibuffer] == '(' || buffer[ibuffer] == ')' || buffer[ibuffer] == '*' || buffer[ibuffer] == '>' || buffer[ibuffer] == ',' || buffer[ibuffer] == ';' || buffer[ibuffer] == '{' || buffer[ibuffer] == '}')
			{
				state = 7;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else if (buffer[ibuffer] == '>')
			{
				state = 13;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else if (buffer[ibuffer] == '!')
			{
				state = 15;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else if (buffer[ibuffer] == '-')
			{
				state = 17;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else if (buffer[ibuffer] == '\'')
			{
				state = 19;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else if (buffer[ibuffer] == '\"')
			{
				state = 21;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else
			{
				return 2;
			}
			break;
		}
		case 2:
		{
			if ((buffer[ibuffer] >= 'a' && buffer[ibuffer] <= 'z') || (buffer[ibuffer] >= 'A' && buffer[ibuffer] <= 'Z') || (buffer[ibuffer] >= '0' && buffer[ibuffer] <= '9'))
			{
				state = 2;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else if (buffer[ibuffer] == '-' || buffer[ibuffer] == '/' || buffer[ibuffer] == '(' || buffer[ibuffer] == ')' || buffer[ibuffer] == '*' || buffer[ibuffer] == '>' || buffer[ibuffer] == ',' || buffer[ibuffer] == ';' || buffer[ibuffer] == '{' || buffer[ibuffer] == '}' || buffer[ibuffer] == '+' || buffer[ibuffer] == '<' || buffer[ibuffer] == '=' || buffer[ibuffer] == ' ' || buffer[ibuffer] == '#')
			{
				state = 8;
				transform(temp, state);
				return 0;
			}
			else
			{
				return 2;
			}
			break;
		}
		case 3:
		{
			if ((buffer[ibuffer] < '0') || (buffer[ibuffer] > '9'))
			{
				if (buffer[ibuffer] >= 'a' && buffer[ibuffer] <= 'z') { err(); }
				state = 9;
				transform(temp, state);
				return 0;
			}
			else if (buffer[ibuffer] >= '0' && buffer[ibuffer] <= '9')
			{
				state = 3;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else
			{
				return 2;
			}
			break;
		}
		case 4:
		{
			if (buffer[ibuffer] != '+')//只有一个+
			{
				//Token token;
				token.ch = 'P';
				token.value = 9;
				//addnewtoken(TOKEN, token);
				return 0;
			}
			else if (buffer[ibuffer] == '+')//++
			{
				state = 10;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else
			{
				return 2;
			}
			break;
		}
		case 5:
		{
			if (buffer[ibuffer] != '=')
			{
				//	Token token;
				token.ch = 'P';
				token.value = 8;
				//addnewtoken(TOKEN, token);
				return 0;
			}
			else if (buffer[ibuffer] == '=')
			{
				state = 11;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else
			{
				return 2;
			}
			break;
		}
		case 6:
		{
			if (buffer[ibuffer] != '=')
			{
				//	Token token;
				token.ch = 'P';
				token.value = 13;
				//	addnewtoken(TOKEN, token);
				return 0;
			}
			else if (buffer[ibuffer] == '=')
			{
				state = 12;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else
			{
				return 2;
			}
			break;
		}
		case 7:
		{
			transform(temp, state);
			return 0;
		}

		case 10://  ??++
		{
			//	Token token;
			token.ch = 'P';
			token.value = 16;
			//addnewtoken(TOKEN, token);
			return 0;
		}
		case 11://  ??<=
		{
			//	Token token;
			token.ch = 'P';
			token.value = 7;
			//addnewtoken(TOKEN, token);
			return 0;
		}
		case 12://  ??==
		{
			//	Token token;
			token.ch = 'P';
			token.value = 5;
			//addnewtoken(TOKEN, token);
			return 0;
		}
		case 13:
		{
			if (buffer[ibuffer] != '=')	//>
			{
				//	Token token;
				token.ch = 'P';
				token.value = 12;
				//addnewtoken(TOKEN, token);
				return 0;
			}
			else if (buffer[ibuffer] == '=')//>=
			{
				state = 14;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else
			{
				return 2;
			}
			break;
		}
		case 14: // ??>=
		{
			//	Token token;
			token.ch = 'P';
			token.value = 11;
			//	addnewtoken(TOKEN, token);
			return 0;
		}
		case 15:
		{

			if (buffer[ibuffer] == '=')
			{
				state = 16;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else
			{
				return 2;
			}
			break;
		}
		case 16: // ???=
		{
			//	Token token;
			token.ch = 'P';
			token.value = 6;
			//addnewtoken(TOKEN, token);
			return 0;
		}
		case 17:
		{
			if (buffer[ibuffer] != '-')//?-
			{
				//	Token token;
				token.ch = 'P';
				token.value = 1;
				//addnewtoken(TOKEN, token);
				return 0;
			}
			else if (buffer[ibuffer] == '-')
			{
				state = 18;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else
			{
				return 2;
			}
			break;
		}
		case 18: // --
		{
			//Token token;
			token.ch = 'P';
			token.value = 17;
			//addnewtoken(TOKEN, token);
			return 0;
		}
		case 19:
		{
			if ((buffer[ibuffer] >= 'a' && buffer[ibuffer] <= 'z') || (buffer[ibuffer] >= 'A' && buffer[ibuffer] <= 'Z'))
			{
				state = 20;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else
			{
				return 2;
			}
			break;
		}
		case 20:
		{
			if (buffer[ibuffer] == '\'')
			{
				transform(temp, state);
				ibuffer++;
				return 0;
			}
			else
			{
				return 2;
			}
			break;
		}
		case 21:
		{
			if ((buffer[ibuffer] >= 'a' && buffer[ibuffer] <= 'z') || (buffer[ibuffer] >= 'A' && buffer[ibuffer] <= 'Z'))
			{
				state = 22;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else
			{
				return 2;
			}
			break;
		}
		case 22:
		{
			if ((buffer[ibuffer] >= 'a' && buffer[ibuffer] <= 'z') || (buffer[ibuffer] >= 'A' && buffer[ibuffer] <= 'Z'))
			{
				state = 22;
				temp += buffer[ibuffer];
				ibuffer++;
			}
			else if (buffer[ibuffer] == '\"')
			{
				transform(temp, state);
				ibuffer++;
				return 0;
			}
			else
			{
				return 2;
			}
			break;
		}
		default:
			break;
		}
	}
	return 1;
}
//保存Token串
void SaveToken()
{
	TokenTemp.push_back("(");
	TokenTemp.push_back(token.ch);
	TokenTemp.push_back(",");
	TokenTemp.push_back(to_string(token.value));
	TokenTemp.push_back(") ");
}
//新加的
void PrintToken()
{
	cout << str << " " << "(" << token.ch << "," << token.value << ")" << endl;
}
//语法分析
void GAnaly(string s)
{

}
bool isID(string ss)      //是否是标识符
{
	I* p = ITable;//查找标识符表
	while (p)
	{
		if (p->content.compare(ss) == 0)
		{
			temp_I = p;//改
			return 1;
		}
		p = p->next;
	}
	return 0;
}
bool isNUM(string ss)
{
	I* p = C1Table;
	while (p)
	{
		if (p->content.compare(ss) == 0)
		{
			return 1;
		}
		p = p->next;
	}
	return 0;

}
void S()   //改
{
	bool flag = 1;
	len = LinePro.length();
	//用空格/换行分隔每一个单词
	//第一行一定为int main(){

	//NextString();
	if (str.compare("int") == 0) NextString();
	else err();

	if (str.compare("main") == 0) NextString();
	else err();

	if (str.compare("(") == 0) NextString();
	else err();

	if (str.compare(")") == 0) NextString();
	else err();
	temp_F = { "main","itp",'f',"" };
	lyer++;
	ALL_SYNBL[synbl_index] = temp_F;
	synbl_index++;
	VALL->push("0");		//参数个数
	VALL->push("?");		//display表
	VALL_index += lyer + 1 + 1;
	if (str.compare("{") == 0) NextString();//执行完后直接跳出循环
	else err();

	A();
	if (LinePro.compare("}") == 0) return;
	else err();
}

void A()//改
{

	if (str == ";")
	{
		NextString();
		if (str != "}") { A(); }
		return;
	}
	else
	{
		C();
		A();
	}
	return;
}

void C()  //改
{
	bool x = 1, r = 1;
	x = X();
	if (x == 0) r = R();
	if (r == 0) B();
	return;
}

bool X()
{
	bool y = 1, z = 1;
	y = Y();

	if (y == 0) return 0;
	Z();
	return 1;
}

bool Y()		//改
{
	len = LinePro.length();
	if (str.compare("int") == 0 || str.compare("char") == 0)
	{
		if (str.compare("int") == 0) { temp_F.type = "itp"; stateY = 1; }
		else if (str.compare("char") == 0) { temp_F.type = "ctp"; stateY = 2; }
		temp_F.CAT = 'v';
		NextString();
		return 1;
	}
	else return 0;
}

void Z()
{
	U();
	Z_1();
}
void U()		//改
{
	int flag;//new
	flag = isID(str);//判断是不是id,在标识符表中查找
	if (flag == 0)err();
	if (for_flag == 0)
	{
		temp_F.name = str;
		temp_F.ADDR = "(" + to_string(lyer) + "," + to_string(VALL_index) + ")";
		ALL_SYNBL[synbl_index] = temp_F;
		temp_I->p = &ALL_SYNBL[synbl_index];//改
		synbl_index++;
		VALL->push(str);
		if (temp_F.type == "itp") { VALL_index += 4; }
		else if (temp_F.type == "rtp") { VALL_index += 1; }
	}
	SEM->push(str);//new
	NextString();
	U_1();
}
void Z_1()   //改
{
	if (str == ",")
	{
		NextString();
		Z();
	}
	return;
}
void U_1()
{
	//string str;
	if (str.compare("=") == 0)
	{
		NextString();
		if (stateY == 1)
		{
			if (isCHAR(str))
			{
				err();
			}
		}
		else if (stateY == 2)
		{
			if (!isCHAR(str) && !(str.length() == 1 && isalnum(str[0])))
			{
				err();
			}
		}
		stateY = 0;
		L();
		string* op_1 = new string;
		string* op_2 = new string;
		*op_1 = SEM->top();
		SEM->pop();
		*op_2 = SEM->top();
		SEM->pop();
		Total_Q[Q_num] = New_Quat("=", *op_1, "", *op_2);
		Q_num++;
		delete op_1, op_2;
	}
	else
	{
		SEM->pop();
		return;
	}
}
void L()
{
	T();
	L_1();
}

bool R()
{
	if (isID(str) && Search_SYNBL(str))//判断是否为id
	{
		SEM->push(str);
		NextString();
		if (str.compare("=") == 0)
		{
			NextString();
			L();
			Quat* temp_Quat = new Quat;
			string* op_1 = new string;
			string* op_2 = new string;
			*op_1 = SEM->top();
			SEM->pop();
			*op_2 = SEM->top();
			SEM->pop();
			*temp_Quat = New_Quat("=", *op_1, "", *op_2);
			Total_Q[Q_num] = *temp_Quat;
			Q_num++;
			delete temp_Quat;
			delete op_1, op_2;
		}
		else
		{
			//err();
			return 0;
		}
	}
	else
	{
		//err();
		return 0;
	}
	return 1;
}

void T()
{
	F();
	T_1();
}
//加
void math_quat()
{
	//GEQ
	Quat* temp_Quat = new Quat;
	string* op_1 = new string;
	string* op_2 = new string;
	*op_1 = SEM->top();
	SEM->pop();
	*op_2 = SEM->top();
	SEM->pop();
	string s = Ran_name();
	*temp_Quat = New_Quat(chara.top(), *op_2, *op_1, s);
	chara.pop();
	SEM->push(s);
	Total_Q[Q_num] = *temp_Quat;
	Q_num++;
	delete temp_Quat, op_1, op_2;
}
//算术表达式四元式生成
void dg(int i)
{
	if (i == 1)
	{
		if (str.compare("+") == 0 || str.compare("-") == 0)//改
		{
			chara.push(str);
			NextString();
			L();
			math_quat();
			dg(i);
		}
	}
	else if (i == 2)
	{
		if (str.compare("*") == 0 || str.compare("/") == 0)//改
		{
			chara.push(str);
			NextString();
			T();
			math_quat();
			dg(i);
		}
	}
}
void L_1()
{
	dg(1);
	return;
}

void F()
{
	if (str.compare("(") == 0)
	{
		NextString();
		L();
		if (str.compare(")") == 0)
		{
			NextString();
		}
	}
	else if ((isID(str) && Search_SYNBL(str)) || isNUM(str) || isCHAR(str))//判断是否为id/num
	{
		SEM->push(str);
		NextString();
	}
	else { err(); }
}

void T_1()
{
	dg(2);
	return;
}

//拓展函数定义
void B()//根据扫描到的第一个单词调用子程序B1-B5
{
	if (str.compare("if") == 0)
	{
		NextString();
		B1();
	}
	else if (str.compare("while") == 0)
	{
		WH();//记录入口
		NextString();
		B2();
	}
	else if (str.compare("for") == 0)
	{
		For();//记录入口
		NextString();
		B3();
	}
	else if (str.compare("printf") == 0)
	{
		NextString();
		B4();
	}
	else if (str.compare("do") == 0)
	{
		q1();//记录入口
		NextString();
		B5();
	}
	else//报错
	{
		err();
	}
}

void B1()//if_else
{
	if (str.compare("(") == 0)
	{
		NextString();
		E();
		if (str.compare(")") == 0)
		{

			IF();

			NextString();
			if (str.compare("{") == 0)
			{
				NextString();
				A();
				if (str.compare("}") == 0)
				{
					NextString();
					if (str.compare("else") == 0)
					{
						EL();

						NextString();
						if (str.compare("{") == 0)
						{
							NextString();
							A();
							if (str.compare("}") == 0)
							{
								IE();
								NextString();
							}
							else err();
						}
						else err();
					}
					else err();
				}
				else err();
			}
			else err();
		}
		else err();
	}
	else err();
}

void B2()//while
{
	if (str.compare("(") == 0)
	{
		NextString();
		E();
		if (str.compare(")") == 0)
		{
			DO();

			NextString();
			if (str.compare("{") == 0)
			{
				NextString();
				A();
				if (str.compare("}") == 0)
				{
					WE();
					NextString();
				}
				else err();
			}
			else err();
		}
		else err();
	}
	else err();
}

void B3()//for
{
	if (str.compare("(") == 0)
	{
		for_flag = 1;
		NextString();
		U();//////////////////////修改//////////////////
		for_flag = 0;
		if (str.compare(";") == 0)
		{
			NextString();
			E();
			For_do();///////////////////////
			if (str.compare(";") == 0)
			{
				NextString();
				Q();

				if (str.compare(")") == 0)
				{
					NextString();
					if (str.compare("{") == 0)
					{
						NextString();
						A();
						GEQ_quat_Q();
						For_q3();
						if (str.compare("}") == 0)
						{
							NextString();
						}
						else err();
					}
					else err();
				}
				else err();
			}
			else err();
		}
		else err();
	}
	else err();
}

void B4()//printf
{
	if (str.compare("(") == 0)
	{
		NextString();
		P();
		Print();//////////////语义
		if (str.compare(")") == 0)
		{
			NextString();
			/*if (str.compare(";") == 0)   //待确认
			{
				NextString();
			}
			else err();*/
		}
		else err();
	}
	else err();
}

void B5()//do-while
{
	if (str.compare("{") == 0)
	{
		NextString();
		A();
		if (str.compare("}") == 0)
		{
			NextString();
			if (str.compare("while") == 0)
			{
				NextString();
				if (str.compare("(") == 0)
				{
					NextString();
					E();
					q2();
					if (str.compare(")") == 0)
					{
						NextString();
					}
					else err();
				}
				else err();
			}
			else err();
		}
		else err();
	}
	else err();
}

void P()//被printf调用，扫描printf双引号“”内部内容
{
	int counti = 0;
	/*if (str.compare("\"") == 0)
	{
		NextString();
		counti++;
		while (str.compare("\"") && counti <= len - 11)
		{
			NextString();
			counti++;
		}
		if (counti > len - 11) { err(); }
		NextString();
	}*/
	if (str[0] == '\"' && str[str.length() - 1] == '\"')
	{
		NextString();
	}
	else
	{
		err();
	}
}

void E()//if、while、for括号内条件判定
{
	F();
	E1();
	F();
	GEQ();//生成四元式
}

void E1()//推出比较符号
{
	if (str.compare(">") == 0)
	{
		//	Fuhao = ">";
		chara.push(str);
		NextString();
	}
	else if (str.compare("<") == 0)
	{
		//Fuhao = "<";
		chara.push(str);
		NextString();
	}
	else if (str.compare("==") == 0)
	{
		chara.push(str);
		//Fuhao = "==";
		NextString();
	}
	else if (str.compare("!=") == 0)
	{
		chara.push(str);
		//Fuhao = "!=";
		NextString();
	}
	else if (str.compare("<=") == 0)
	{
		chara.push(str);
		//Fuhao = "<=";
		NextString();
	}
	else if (str.compare(">=") == 0)
	{
		chara.push(str);
		//Fuhao = ">=";
		NextString();
	}
	else err();
}

void Q()//推出 id++/id--/空
{
	if (isID(str))
	{
		SEM->push(str);
		NextString();
		Q1();
	}
	else {}
}

void Q1()//推出++/--/空
{
	if (str.compare("++") == 0)
	{
		chara.push(str);
		//Fuhao = "++";
		NextString();
	}
	else if (str.compare("--") == 0)
	{
		chara.push(str);
		//Fuhao = "--";
		NextString();
	}
	else {}
}

//加语义函数 
void IF()
{
	Quat* temp_Quat = new Quat;//if 四元式
	string* op_2 = new string;
	*op_2 = SEM->top();
	SEM->pop();
	*temp_Quat = New_Quat("if", *op_2, "", "");
	Total_Q[Q_num] = *temp_Quat;
	Q_num++;
	delete temp_Quat, op_2;
}
void EL()
{
	Quat* temp_Quat = new Quat;// else 四元式
	string* op_1 = new string;
	string* op_2 = new string;
	*temp_Quat = New_Quat("el", "", "", "");
	Total_Q[Q_num] = *temp_Quat;
	Q_num++;
	delete temp_Quat, op_1, op_2;
}
void GEQ()//E部分四元式
{
	Fuhao = chara.top();
	chara.pop();
	Quat* temp_Quat = new Quat;
	string* op_1 = new string;
	string* op_2 = new string;
	*op_2 = SEM->top();
	SEM->pop();
	*op_1 = SEM->top();
	SEM->pop();
	string s = Ran_name();
	SEM->push(s);//把生成的ti压栈
	*temp_Quat = New_Quat(Fuhao, *op_1, *op_2, s);
	Total_Q[Q_num] = *temp_Quat;
	Q_num++;
	delete temp_Quat, op_1, op_2;
}

void IE()
{
	Quat* temp_Quat = new Quat;
	*temp_Quat = New_Quat("ie", "", "", "");
	Total_Q[Q_num] = *temp_Quat;
	Q_num++;
	delete temp_Quat;
}

//while语义
void WH()
{
	Quat* temp_Quat = new Quat;
	*temp_Quat = New_Quat("wh", "", "", "");
	Total_Q[Q_num] = *temp_Quat;
	Q_num++;
	delete temp_Quat;
}
void DO()
{
	Quat* temp_Quat = new Quat;
	string* op_2 = new string;
	*op_2 = SEM->top();
	SEM->pop();
	*temp_Quat = New_Quat("do", *op_2, "", "");
	Total_Q[Q_num] = *temp_Quat;
	Q_num++;
	delete temp_Quat, op_2;
}
void WE()
{
	Quat* temp_Quat = new Quat;
	*temp_Quat = New_Quat("we", "", "", "");
	Total_Q[Q_num] = *temp_Quat;
	Q_num++;
	delete temp_Quat;
}
//for语义
void For()//记录入口
{
	Quat* temp_Quat = new Quat;
	*temp_Quat = New_Quat("for", "", "", "");
	Total_Q[Q_num] = *temp_Quat;
	Q_num++;
	delete temp_Quat;
}
void For_do()
{
	Quat* temp_Quat = new Quat;
	string* op_1 = new string;
	*op_1 = SEM->top();
	SEM->pop();
	*temp_Quat = New_Quat("do", *op_1, "", "");
	Total_Q[Q_num] = *temp_Quat;
	Q_num++;
	delete temp_Quat, op_1;
}
void GEQ_quat_Q()
{
	Fuhao = chara.top();
	chara.pop();
	Quat* temp_Quat = new Quat;
	string* op_1 = new string;
	*op_1 = SEM->top();
	SEM->pop();
	string s = Ran_name();
	if (Fuhao == "++")
	{
		*temp_Quat = New_Quat("+", *op_1, "1", s);
	}
	else if (Fuhao == "--")
	{
		*temp_Quat = New_Quat("-", *op_1, "1", s);
	}
	else
	{
		//cout << "Fuhao错误";
	}

	Total_Q[Q_num] = *temp_Quat;
	Q_num++;
	delete temp_Quat, op_1;
}
void For_q3()
{
	Quat* temp_Quat = new Quat;
	*temp_Quat = New_Quat("jump", "", "", "");//无条件跳转到循环入口
	Total_Q[Q_num] = *temp_Quat;
	Q_num++;
	delete temp_Quat;
}
//printf输出
void Print()
{
	Quat* temp_Quat = new Quat;
	*temp_Quat = New_Quat("print", "", "", "");//输出
	Total_Q[Q_num] = *temp_Quat;
	Q_num++;
	delete temp_Quat;
}
//do-while
void q1()//记录入口
{
	Quat* temp_Quat = new Quat;
	*temp_Quat = New_Quat("do", "", "", "");//输出
	Total_Q[Q_num] = *temp_Quat;
	Q_num++;
	delete temp_Quat;
}
void q2()//根据E，真值跳转到do-while入口
{
	Quat* temp_Quat = new Quat;
	string* op_1 = new string;
	*op_1 = SEM->top();
	SEM->pop();
	*temp_Quat = New_Quat("jump", *op_1, "", "");
	Total_Q[Q_num] = *temp_Quat;
	Q_num++;
	delete temp_Quat, op_1;
}

int main()
{
	cout << "Token:" << endl;//新加的
	NextString();
	S();
	cout << "四元式：" << endl;
	for (int i = 0; i < Q_num; i++)
	{
		cout << "(" << Total_Q[i].symbal << "," << Total_Q[i].op_1 << "," << Total_Q[i].op_2 << "," << Total_Q[i].result << ")" << endl;
	}
	cout << "符号表：" << endl;
	for (int i = 0; i < synbl_index; i++)
	{
		cout << ALL_SYNBL[i].name << "," << ALL_SYNBL[i].type << "," << ALL_SYNBL[i].CAT << "," << ALL_SYNBL[i].ADDR << endl;
	}
	delete SEM, VALL, ITable, C1Table, CTTable, STTable;
}
