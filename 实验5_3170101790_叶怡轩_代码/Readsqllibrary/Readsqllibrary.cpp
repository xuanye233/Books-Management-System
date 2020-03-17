#include <stdio.h>     
#include <string.h>     
#include <windows.h>     
#include <sql.h>     
#include <sqlext.h>     
#include <sqltypes.h>     
#include <odbcss.h>     
#include<fstream>
#include<iostream>
#include<iomanip>

using namespace std;


void load();
void lookup();
void login();
void input();
void updatecard();
void borrowbook();
void returnbook();
void updatedata();
void lookupborrow();
void lookupmultiple();
void display();
//char havechar(char)

#define MAXSIZE 50

int ifmanager,managernum,booknum,borrownum,cardnum;
int currentmanager;



int bkyearl;
float bkpricel;

SQLRETURN ret;
SQLHENV henv;//SQLHANDLE henv 
SQLHDBC hdbc;//SQLHANDLE hdbc 
SQLHSTMT hstmt;//SQLHANDLE hstmt 

//store the data of book
int bkyear, bktotal, bkstock;
float bkpriceh;
char bkbno[50], bktitle[50], bkpress[50], bkauthor[50],bkcategory[50];
char bkprice[10];
SQLINTEGER len_bkyear,len_bktotal,len_bkstock,len_bkbno, len_bktitle, len_bkpress, len_bkauthor,len_bkprice,len_bkcategory;

//store the data of borrow
//datetime
int bomanager_id;
char bocno[50], bobno[50],boborrow_date[50],boreturn_date[50];
SQLINTEGER len_bocno, len_bobno,len_bomanager_id,len_boborrow_date,len_boreturn_date;

//store the data of card
char cacno[50], caname[50], cadepartment[50], catype[1];
SQLINTEGER len_cacno, len_caname, len_cadepartment, len_catype;

//store the data of manager
int macode, mamanager_id, macontact;
char	maname[50];
SQLINTEGER len_maname,len_macode,len_mamanager_id,len_macontact;

struct book {
	char bno[50];
	char category[50];
	char title[50];
	char press[50];
	int year;
	char author[50];
	char price[10];
	int total;
	int stock;
}cbook[20];

struct borrow{
	char cno[50];
	char bno[50];
	char borrow_date[50];
	char return_date[50] = {0};
	int manager_id;
}cborrow[20];

struct card {
	char cno[50];
	char name[50];
	char department[50];
	char type[1];
	int ifcandelete;
}ccard[20];

struct manager {
	int manager_id;
	int code;
	char name[50];
	int contact;
}cmanager[20];

void load()
{	//连接数据库 并输出界面信息
	//并且把数据库中的book borrow card manager表的内容都分别读入四个结构体
	//booknum borrownum cardnum managernum 中保存了各张表信息的数量

	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//设置环境属性 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄 
	ret = SQLConnect(hdbc, (SQLCHAR*)"library", SQL_NTS, (SQLCHAR*)"sa", SQL_NTS, (SQLCHAR*)"123456", SQL_NTS);//连接数据库 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		printf("---------------------------------------------------------\n");
		printf("------------------您已成功连接数据库---------------------\n");
		printf("--------------------数媒1701 叶怡轩----------------------\n");
		printf("--    请问您想进行什么操作？（按数字与回车进行操作）   --\n");
		printf("--1.管理员登录                                         --\n");
		printf("--2.图书入库                                           --\n");
		printf("--3.图书查询                                           --\n");
		printf("--4.借书                                               --\n");
		printf("--5.还书                                               --\n");
		printf("--6.借书证管理                                         --\n");
		printf("--7.查看借书证借书记录                                 --\n");
		printf("--8.图书复合查询                                       --\n");
		printf("--0.退出                                               --\n");
		printf("---------------------------------------------------------\n");


		////////////////////////////////读取manager数据//////////////////////////////////////////////
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sql[] = "SELECT * FROM manager";
		ret = SQLExecDirect(hstmt,sql,SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			int i = 0;
			while (SQLFetch(hstmt) != SQL_NO_DATA) {//遍历结果
				SQLGetData(hstmt, 1, SQL_C_ULONG, &cmanager[i].manager_id, 0, &len_mamanager_id);
				SQLGetData(hstmt, 2, SQL_C_ULONG, &cmanager[i].code, 0, &len_macode);
				SQLGetData(hstmt, 3, SQL_C_CHAR, cmanager[i].name, 10, &len_maname);
				SQLGetData(hstmt, 4, SQL_C_ULONG, &cmanager[i].contact, 0, &len_macontact);

				//deletespace(cmanager[i].name);
				//printf("%d \n", cmanager[i].manager_id);
				//printf("%d ", cmanager[i].code);
				i++;
			}
			managernum = i;

			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
		}
		else printf("添加manager数据库操作失败！\n");
		///////////////////////////读取manager结束/////////////////////////////////////////////////////
		
		///////////////////////////读取book///////////////////////////////////////////////////////////////
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sqlbook[] = "SELECT * FROM book";
		ret = SQLExecDirect(hstmt, sqlbook, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			int i = 0;
			while (SQLFetch(hstmt) != SQL_NO_DATA) {//遍历结果
				SQLGetData(hstmt, 1, SQL_C_CHAR, cbook[i].bno, 50, &len_bkbno);
				SQLGetData(hstmt, 2, SQL_C_CHAR, cbook[i].category, 50, &len_bkcategory);
				SQLGetData(hstmt, 3, SQL_C_CHAR, cbook[i].title, 50, &len_bktitle);
				SQLGetData(hstmt, 4, SQL_C_CHAR, cbook[i].press, 50, &len_bkpress);
				SQLGetData(hstmt, 5, SQL_C_ULONG, &cbook[i].year, sizeof(int), &len_bkyear);
				SQLGetData(hstmt, 6, SQL_C_CHAR, cbook[i].author, 50, &len_bkauthor);
				SQLGetData(hstmt, 7, SQL_C_CHAR, cbook[i].price, sizeof(cbook[i].price), &len_bkprice);
				SQLGetData(hstmt, 8, SQL_C_ULONG, &cbook[i].total, sizeof(int), &len_bktotal);
				SQLGetData(hstmt, 9, SQL_C_ULONG, &cbook[i].stock, sizeof(int), &len_bkstock);

				/*deletespace(cbook[i].bno);
				deletespace(cbook[i].category);
				deletespace(cbook[i].title);
				deletespace(cbook[i].press);
				deletespace(cbook[i].author);*/
				//printf("%s\n", cbook[i].bno);
				//printf("%d\n", cbook[i].year);
				//printf("%s \n", cbook[i].price);

				i++;
			}
			booknum = i;
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄
		}
		else printf("添加book数据库操作失败！\n");


		////////////////////////////////读取book结束///////////////////////////////////////////////////////

		////////////////////////////////读取borrow///////////////////////////////////////////////////////
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sqlborrow[] = "SELECT * FROM borrow";
		ret = SQLExecDirect(hstmt, sqlborrow, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			int i = 0;
			while (SQLFetch(hstmt) != SQL_NO_DATA) {//遍历结果
				SQLGetData(hstmt, 1, SQL_C_CHAR, cborrow[i].cno, 50, &len_bocno);
				SQLGetData(hstmt, 2, SQL_C_CHAR, cborrow[i].bno, 50, &len_bobno);
				SQLGetData(hstmt, 3, SQL_C_CHAR, cborrow[i].borrow_date ,50, &len_boborrow_date);
				SQLGetData(hstmt, 4, SQL_C_CHAR, cborrow[i].return_date, 50, &len_boreturn_date);
				SQLGetData(hstmt, 5, SQL_C_ULONG, &cborrow[i].manager_id, 0, &len_bomanager_id);

				//deletespace(cborrow[i].bno);
				//deletespace(cborrow[i].cno);

				//printf("%s \n", cborrow[i].borrow_date);

				i++;
			}
			borrownum = i;
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄
		}
		else printf("添加borrow数据库操作失败！\n");


		
		////////////////////////////////读取borrow结束/////////////////////////////////////////////////

		////////////////////////////////读取card/////////////////////////////////////////////////////////

		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sqlcard[] = "SELECT * FROM card";
		ret = SQLExecDirect(hstmt, sqlcard, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			int i = 0;
			while (SQLFetch(hstmt) != SQL_NO_DATA) {//遍历结果
				SQLGetData(hstmt, 1, SQL_C_CHAR, ccard[i].cno, 50, &len_cacno);
				SQLGetData(hstmt, 2, SQL_C_CHAR, ccard[i].name, 50, &len_caname);
				SQLGetData(hstmt, 3, SQL_C_CHAR, ccard[i].department, 50, &len_cadepartment);
				SQLGetData(hstmt, 4, SQL_C_CHAR, ccard[i].type, 1, &len_catype);

				/*deletespace(ccard[i].cno);
				deletespace(ccard[i].name);
				deletespace(ccard[i].department);
				deletespace(ccard[i].type);*/

				//printf("%d \n", cbook[i].total);

				i++;
			}
			cardnum = i;
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄
		}
		else printf("添加card数据库操作失败！\n");


		////////////////////////////////读取card结束///////////////////////////////////////////////////////
		SQLDisconnect(hdbc);//断开与数据库的连接 
	}
	else printf("连接数据库失败!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄

}

void login()
{
	int thisid, thiscode;
	printf("请输入你的账号：\n");
	scanf("%d", &thisid);
	printf("请输入你的密码：\n");
	scanf("%d", &thiscode);
	int j;
	for (j = 0;j < managernum;j++) {
		if (thisid == cmanager[j].manager_id && thiscode == cmanager[j].code) {
			ifmanager = 1;
			currentmanager = thisid;
			printf("登陆成功！\n");
			break;
		}
		if (ifmanager == 0) {
			printf("密码或者账号错误！请重新选择功能！\n");
		}
	}

} 

void input()
{
	int ifhave = 0;//记录输入的图书是否已经拥有
	printf("请输入您需要输入的图书数量：\n");
	int num, i,j;
	scanf("%d", &num);
	printf("请输入图书信息：( 书号, 类别, 书名, 出版社, 年份, 作者, 价格, 数量 )\n");
	printf("Sample:(book_no_1, Computer Science, Computer Architecture, xxx, 2004, xxx, 90.00, 2)\n");


	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//设置环境属性 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄 
	ret = SQLConnect(hdbc, (SQLCHAR*)"library", SQL_NTS, (SQLCHAR*)"sa", SQL_NTS, (SQLCHAR*)"123456", SQL_NTS);//连接数据库 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {	
		
		char c;


		for (i = 0;i < num;i++) {
			///////////////////输入每一行的信息///////////////////////////////////
			ifhave = 0;
			char first, last;
			int j = 0;
			//bno
			c = getchar();//读入空格
			c = getchar();//读入(
			//("%c1\n", c);
			//printf("%c2\n", c);


			c = getchar();
			do {
				bkbno[j++] = c;
				c = getchar();
			} while (c != ',');
			//printf("%s3\n", bkbno);
			//bkcategory
			j = 0;
			c = getchar();
			do {
				bkcategory[j++] = c;
				c = getchar();
			} while (c != ',');
			//printf("%s4\n", bkcategory);
			//bktitle
			j = 0;
			c = getchar();
			do {
				bktitle[j++] = c;
				c = getchar();
			} while (c != ',');
			//printf("%s1\n", bktitle);
			//bkpress
			j = 0;
			c = getchar();
			do {
				bkpress[j++] = c;
				c = getchar();
			} while (c != ',');
			//printf("%s\n", bkpress);
			//bkyear
			scanf("%d", &bkyear);
			c = getchar();
			//("%d\n", bkyear);		
			//bkauthor
			j = 0;
			c = getchar();
			do {
				bkauthor[j++] = c;
				c = getchar();
			} while (c != ',');
			//printf("%s\n", bkauthor);
			//bkprice
			scanf("%f", &bkpricel);
			c = getchar();
			//printf("%c1\n", c);
			//printf("%f\n", bkprice);
			//bktotal
			scanf("%d", &bktotal);
			c = getchar();
			//printf("%c3\n", c);
			//printf("%d\n", bktotal);
			//bkstock
			//scanf("%d", &bkstock);
			//c = getchar();
			//printf("%c3\n", c);
			//printf("%d\n", bkstock);
			////////////////////输入信息结束///////////////////////////////////////

			ifhave = 0;
			for (j = 0;j < booknum;j++) {
				if (strcmp(bkbno, cbook[j].bno) == 0) {
					cbook[j].total += bktotal;
					cbook[j].stock += bktotal;
					ifhave = 1;
					break;
				}
			}

			if (ifhave == 1) {
				//如果已经有了就直接结束这一次的循环
				ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
				SQLCHAR sql[] = "update book set stock = ?,total = ? where bno = ?";
				SQLINTEGER P = SQL_NTS;
				ret = SQLPrepare(hstmt, sql, SQL_NTS);
				ret = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &cbook[j].stock, 0, &P);//绑定参数 
				ret = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &cbook[j].total, 0, &P);//绑定参数 
				ret = SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, bkbno, 50, &P);
				ret = SQLExecute(hstmt);//直接执行SQL语句 
				//ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
				if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
					printf("导入成功！\n");


					SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
				}
				else {
					printf("导入失败！\n");
				}
			}
			else {
				bkstock = bktotal;

				ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
				SQLCHAR sql[] = "insert into book values(?,?,?,?,?,?,?,?,?)";
				SQLINTEGER P = SQL_NTS;
				ret = SQLPrepare(hstmt, sql, SQL_NTS);
				ret = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, bkbno, 50, &P);//绑定参数 
				ret = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, bkcategory, 50, &P);//绑定参数
				ret = SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, bktitle, 50, &P);//绑定参数
				ret = SQLBindParameter(hstmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, bkpress, 50, &P);//绑定参数 
				ret = SQLBindParameter(hstmt, 5, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &bkyear, sizeof(int), &P);
				ret = SQLBindParameter(hstmt, 6, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, bkauthor, 50, &P);
				ret = SQLBindParameter(hstmt, 7, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_DECIMAL, 7, 2, &bkpricel, sizeof(float), &P);
				ret = SQLBindParameter(hstmt, 8, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &bktotal, sizeof(int), &P);
				ret = SQLBindParameter(hstmt, 9, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &bkstock, sizeof(int), &P);
				ret = SQLExecute(hstmt);//直接执行SQL语句 
				//ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
				if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
					printf("插入成功！\n");
					SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
				}
				else {
					printf("插入失败！\n");
				}
			}
		}
		

		SQLDisconnect(hdbc);//断开与数据库的连接 
	}
	else printf("连接数据库失败!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄
	updatedata();
}

void lookup()
{
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//设置环境属性 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄 
	ret = SQLConnect(hdbc, (SQLCHAR*)"library", SQL_NTS, (SQLCHAR*)"sa", SQL_NTS, (SQLCHAR*)"123456", SQL_NTS);//连接数据库 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		//书号, 类别, 书名, 出版社, 年份, 作者, 价格, 总藏书量, 库存
		printf("---------------------------------------------------------\n");
		printf("--1.书号                                               --\n");
		printf("--2.类别                                               --\n");
		printf("--3.书名                                               --\n");
		printf("--4.出版社                                             --\n");
		printf("--5.年份                                               --\n");
		printf("--6.作者                                               --\n");
		printf("--7.价格                                               --\n");
		printf("--8.全部图书                                           --\n");

		int lookorder = 0;
		int lookint;
		char lookchar[50];

		scanf("%d", &lookorder);
		switch (lookorder) {
			case 1:
				printf("请输入书号：\n");
				scanf("%s", bkbno);
				break;
			case 2:
				printf("请输入类别：\n");
				scanf("%s", bkcategory);
				break;
			case 3:
				printf("请输入书名\n");
				scanf("%s", bktitle);
				break;
			case 4:
				printf("请输入出版商：\n");
				scanf("%s", bkpress);
				break;
			case 5:
				printf("请输入年份下限：\n");
				scanf("%d", &bkyearl);
				printf("请输入年份上限：\n");
				scanf("%d", &bkyear);
				break;
			case 6:
				printf("请输入作者：\n");
				scanf("%s", bkauthor);
				break;
			case 7://区间
				printf("请输入价格下限：\n");
				scanf("%f", &bkpricel);
				printf("请输入价格上限：\n");
				scanf("%f", &bkprice);

		}

		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLINTEGER P = SQL_NTS;

		if (lookorder == 1) {
			//printf("%s\n", bkbno);
			SQLCHAR sql[] = "SELECT * FROM book WHERE bno = ?";
			ret = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 50, 0, bkbno, 50, &P);//绑定参数
			ret = SQLPrepare(hstmt, sql, SQL_NTS);
			ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
			if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
				//int i = 0;
				printf("查询链接成功！\n");
				while (SQLFetch(hstmt) != SQL_NO_DATA) {//遍历结果
					SQLGetData(hstmt, 1, SQL_C_CHAR, bkbno, 50, &len_bkbno);
					SQLGetData(hstmt, 2, SQL_C_CHAR, bkcategory, 50, &len_bkcategory);
					SQLGetData(hstmt, 3, SQL_C_CHAR, bktitle, 50, &len_bktitle);
					SQLGetData(hstmt, 4, SQL_C_CHAR, bkpress, 50, &len_bkpress);
					SQLGetData(hstmt, 5, SQL_C_ULONG, &bkyear, sizeof(int), &len_bkyear);
					SQLGetData(hstmt, 6, SQL_C_CHAR, bkauthor, 50, &len_bkauthor);
					SQLGetData(hstmt, 7, SQL_C_CHAR, bkprice, 10, &len_bkprice);
					SQLGetData(hstmt, 8, SQL_C_ULONG, &bktotal, sizeof(int), &len_bktotal);
					SQLGetData(hstmt, 9, SQL_C_ULONG, &bkstock, sizeof(int), &len_bkstock);

					printf("书号：%s\n", bkbno);
					printf("类别：%s\n",bkcategory);
					printf("书名：%s\n", bktitle);
					printf("出版社：%s\n", bkpress);
					printf("年份：%d\n", bkyear);
					printf("作者：%s\n", bkauthor);
					printf("价格：%s\n", bkprice);
					printf("总藏书量：%d\n", bktotal);
					printf("库存：%d\n", bkstock);

					//i++;
				}

				SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 

			}
			else printf("添加数据库操作失败！\n");

		}




		else if (lookorder == 2) {
			SQLCHAR sql[] = "SELECT * FROM book WHERE category = ?";
			ret = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 50, 0, bkcategory, 50, &P);//绑定参数 
			ret = SQLPrepare(hstmt, sql, SQL_NTS);
			ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
			if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
				//int i = 0;
				printf("查询链接成功！\n");
				while (SQLFetch(hstmt) != SQL_NO_DATA) {//遍历结果
					SQLGetData(hstmt, 1, SQL_C_CHAR, bkbno, 50, &len_bkbno);
					SQLGetData(hstmt, 2, SQL_C_CHAR, bkcategory, 50, &len_bkcategory);
					SQLGetData(hstmt, 3, SQL_C_CHAR, bktitle, 50, &len_bktitle);
					SQLGetData(hstmt, 4, SQL_C_CHAR, bkpress, 50, &len_bkpress);
					SQLGetData(hstmt, 5, SQL_C_ULONG, &bkyear, sizeof(int), &len_bkyear);
					SQLGetData(hstmt, 6, SQL_C_CHAR, bkauthor, 50, &len_bkauthor);
					SQLGetData(hstmt, 7, SQL_C_CHAR, &bkprice, 10, &len_bkprice);
					SQLGetData(hstmt, 8, SQL_C_ULONG, &bktotal, sizeof(int), &len_bktotal);
					SQLGetData(hstmt, 9, SQL_C_ULONG, &bkstock, sizeof(int), &len_bkstock);

					printf("书号：%s\n", bkbno);
					printf("类别：%s\n", bkcategory);
					printf("书名：%s\n", bktitle);
					printf("出版社：%s\n", bkpress);
					printf("年份：%d\n", bkyear);
					printf("作者：%s\n", bkauthor);
					printf("价格：%s\n", bkprice);
					printf("总藏书量：%d\n", bktotal);
					printf("库存：%d\n", bkstock);

					//i++;
				}

				SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 

			}
			else printf("添加数据库操作失败！\n");

		}




		else if (lookorder == 3) {
			SQLCHAR sql[] = "SELECT * FROM book WHERE title = ?";
			ret = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 50, 0, bktitle, 50, &P);//绑定参数 
			ret = SQLPrepare(hstmt, sql, SQL_NTS);
			ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
			if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
				//int i = 0;
				printf("查询链接成功！\n");
				while (SQLFetch(hstmt) != SQL_NO_DATA) {//遍历结果
					SQLGetData(hstmt, 1, SQL_C_CHAR, bkbno, 50, &len_bkbno);
					SQLGetData(hstmt, 2, SQL_C_CHAR, bkcategory, 50, &len_bkcategory);
					SQLGetData(hstmt, 3, SQL_C_CHAR, bktitle, 50, &len_bktitle);
					SQLGetData(hstmt, 4, SQL_C_CHAR, bkpress, 50, &len_bkpress);
					SQLGetData(hstmt, 5, SQL_C_ULONG, &bkyear, sizeof(int), &len_bkyear);
					SQLGetData(hstmt, 6, SQL_C_CHAR, bkauthor, 50, &len_bkauthor);
					SQLGetData(hstmt, 7, SQL_C_CHAR, &bkprice, 10, &len_bkprice);
					SQLGetData(hstmt, 8, SQL_C_ULONG, &bktotal, sizeof(int), &len_bktotal);
					SQLGetData(hstmt, 9, SQL_C_ULONG, &bkstock, sizeof(int), &len_bkstock);

					printf("书号：%s\n", bkbno);
					printf("类别：%s\n", bkcategory);
					printf("书名：%s\n", bktitle);
					printf("出版社：%s\n", bkpress);
					printf("年份：%d\n", bkyear);
					printf("作者：%s\n", bkauthor);
					printf("价格：%s\n", bkprice);
					printf("总藏书量：%d\n", bktotal);
					printf("库存：%d\n", bkstock);

					//i++;
				}

				SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 

			}
			else printf("添加数据库操作失败！\n");
		}




		else if (lookorder == 4) {
			SQLCHAR sql[] = "SELECT * FROM book WHERE press = ?";
			printf("%s\n", bkpress);
			ret = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 50, 0, bkpress, 50, &P);//绑定参数 
			ret = SQLPrepare(hstmt, sql, SQL_NTS);
			ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
			if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
				//int i = 0;
				printf("查询链接成功！\n");
				while (SQLFetch(hstmt) != SQL_NO_DATA) {//遍历结果
					SQLGetData(hstmt, 1, SQL_C_CHAR, bkbno, 50, &len_bkbno);
					SQLGetData(hstmt, 2, SQL_C_CHAR, bkcategory, 50, &len_bkcategory);
					SQLGetData(hstmt, 3, SQL_C_CHAR, bktitle, 50, &len_bktitle);
					SQLGetData(hstmt, 4, SQL_C_CHAR, bkpress, 50, &len_bkpress);
					SQLGetData(hstmt, 5, SQL_C_ULONG, &bkyear, sizeof(int), &len_bkyear);
					SQLGetData(hstmt, 6, SQL_C_CHAR, bkauthor, 50, &len_bkauthor);
					SQLGetData(hstmt, 7, SQL_C_CHAR, &bkprice, 10, &len_bkprice);
					SQLGetData(hstmt, 8, SQL_C_ULONG, &bktotal, sizeof(int), &len_bktotal);
					SQLGetData(hstmt, 9, SQL_C_ULONG, &bkstock, sizeof(int), &len_bkstock);

					printf("书号：%s\n", bkbno);
					printf("类别：%s\n", bkcategory);
					printf("书名：%s\n", bktitle);
					printf("出版社：%s\n", bkpress);
					printf("年份：%d\n", bkyear);
					printf("作者：%s\n", bkauthor);
					printf("价格：%s\n", bkprice);
					printf("总藏书量：%d\n", bktotal);
					printf("库存：%d\n", bkstock);

					//i++;
				}

				SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 

			}
			else printf("添加数据库操作失败！\n");
		}




		else if (lookorder == 5) {
			SQLCHAR sql[] = "SELECT * FROM book WHERE year > ? and year < ?";
			ret = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &bkyearl, 0, &P);//绑定参数 
			ret = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &bkyear, 0, &P);//绑定参数 
			ret = SQLPrepare(hstmt, sql, SQL_NTS);
			ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
			if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
				//int i = 0;
				printf("查询链接成功！\n");
				while (SQLFetch(hstmt) != SQL_NO_DATA) {//遍历结果
					SQLGetData(hstmt, 1, SQL_C_CHAR, bkbno, 50, &len_bkbno);
					SQLGetData(hstmt, 2, SQL_C_CHAR, bkcategory, 50, &len_bkcategory);
					SQLGetData(hstmt, 3, SQL_C_CHAR, bktitle, 50, &len_bktitle);
					SQLGetData(hstmt, 4, SQL_C_CHAR, bkpress, 50, &len_bkpress);
					SQLGetData(hstmt, 5, SQL_C_ULONG, &bkyear, sizeof(int), &len_bkyear);
					SQLGetData(hstmt, 6, SQL_C_CHAR, bkauthor, 50, &len_bkauthor);
					SQLGetData(hstmt, 7, SQL_C_CHAR, bkprice, 10, &len_bkprice);
					SQLGetData(hstmt, 8, SQL_C_ULONG, &bktotal, sizeof(int), &len_bktotal);
					SQLGetData(hstmt, 9, SQL_C_ULONG, &bkstock, sizeof(int), &len_bkstock);

					printf("书号：%s\n", bkbno);
					printf("类别：%s\n", bkcategory);
					printf("书名：%s\n", bktitle);
					printf("出版社：%s\n", bkpress);
					printf("年份：%d\n", bkyear);
					printf("作者：%s\n", bkauthor);
					printf("价格：%s\n", bkprice);
					printf("总藏书量：%d\n", bktotal);
					printf("库存：%d\n", bkstock);

					//i++;
				}

				SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 

			}
			else printf("添加数据库操作失败！\n");
		}




		else if (lookorder == 6) {
			SQLCHAR sql[] = "SELECT * FROM book WHERE author = '?'";
			ret = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 50, 0, bkauthor, 50, &P);//绑定参数 
			ret = SQLPrepare(hstmt, sql, SQL_NTS);
			ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
			if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
				//int i = 0;
				printf("查询链接成功！\n");
				while (SQLFetch(hstmt) != SQL_NO_DATA) {//遍历结果
					SQLGetData(hstmt, 1, SQL_C_CHAR, bkbno, 50, &len_bkbno);
					SQLGetData(hstmt, 2, SQL_C_CHAR, bkcategory, 50, &len_bkcategory);
					SQLGetData(hstmt, 3, SQL_C_CHAR, bktitle, 50, &len_bktitle);
					SQLGetData(hstmt, 4, SQL_C_CHAR, bkpress, 50, &len_bkpress);
					SQLGetData(hstmt, 5, SQL_C_ULONG, &bkyear, sizeof(int), &len_bkyear);
					SQLGetData(hstmt, 6, SQL_C_CHAR, bkauthor, 50, &len_bkauthor);
					SQLGetData(hstmt, 7, SQL_C_CHAR, bkprice, 10, &len_bkprice);
					SQLGetData(hstmt, 8, SQL_C_ULONG, &bktotal, sizeof(int), &len_bktotal);
					SQLGetData(hstmt, 9, SQL_C_ULONG, &bkstock, sizeof(int), &len_bkstock);

					printf("书号：%s\n", bkbno);
					printf("类别：%s\n", bkcategory);
					printf("书名：%s\n", bktitle);
					printf("出版社：%s\n", bkpress);
					printf("年份：%d\n", bkyear);
					printf("作者：%s\n", bkauthor);
					printf("价格：%s\n", bkprice);
					printf("总藏书量：%d\n", bktotal);
					printf("库存：%d\n", bkstock);

					//i++;
				}

				SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 

			}
			else printf("添加数据库操作失败！\n");
		}




		else if (lookorder == 7) {
			SQLCHAR sql[] = "SELECT * FROM book WHERE price > ? and price < ?";
			ret = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_DECIMAL, 7, 2, &bkpricel, sizeof(float), &P);//绑定参数
			ret = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_DECIMAL, 7, 2, &bkprice, sizeof(float), &P);//绑定参数
			ret = SQLPrepare(hstmt, sql, SQL_NTS);
			ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
			if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
				//int i = 0;
				printf("查询链接成功！\n");
				while (SQLFetch(hstmt) != SQL_NO_DATA) {//遍历结果
					SQLGetData(hstmt, 1, SQL_C_CHAR, bkbno, 50, &len_bkbno);
					SQLGetData(hstmt, 2, SQL_C_CHAR, bkcategory, 50, &len_bkcategory);
					SQLGetData(hstmt, 3, SQL_C_CHAR, bktitle, 50, &len_bktitle);
					SQLGetData(hstmt, 4, SQL_C_CHAR, bkpress, 50, &len_bkpress);
					SQLGetData(hstmt, 5, SQL_C_ULONG, &bkyear, sizeof(int), &len_bkyear);
					SQLGetData(hstmt, 6, SQL_C_CHAR, bkauthor, 50, &len_bkauthor);
					SQLGetData(hstmt, 7, SQL_C_CHAR, &bkprice, 10, &len_bkprice);
					SQLGetData(hstmt, 8, SQL_C_ULONG, &bktotal, sizeof(int), &len_bktotal);
					SQLGetData(hstmt, 9, SQL_C_ULONG, &bkstock, sizeof(int), &len_bkstock);

					printf("书号：%s\n", bkbno);
					printf("类别：%s\n", bkcategory);
					printf("书名：%s\n", bktitle);
					printf("出版社：%s\n", bkpress);
					printf("年份：%d\n", bkyear);
					printf("作者：%s\n", bkauthor);
					printf("价格：%s\n", bkprice);
					printf("总藏书量：%d\n", bktotal);
					printf("库存：%d\n", bkstock);

					//i++;
				}

				SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 

			}
			else printf("添加数据库操作失败！\n");
		}


		else if (lookorder == 8) {		
			int i;
			for (i = 0;i < booknum;i++) {
				printf("书号：%s\n", cbook[i].bno);
				printf("类别：%s\n", cbook[i].category);
				printf("书名：%s\n", cbook[i].title);
				printf("出版社：%s\n", cbook[i].press);
				printf("年份：%d\n", cbook[i].year);
				printf("作者：%s\n", cbook[i].author);
				printf("价格：%s\n", cbook[i].price);
				printf("总藏书量：%d\n", cbook[i].total);
				printf("库存：%d\n", cbook[i].stock);
			}


		}
	
		


		SQLDisconnect(hdbc);//断开与数据库的连接 
	}
	else printf("连接数据库失败!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄*/

}


void updatecard() {
	printf("请问你要进行什么操作？\n");
	printf("1.增加图书证\n");
	printf("2.删除图书证\n");
	int updateorder;
	scanf("%d", &updateorder);


	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//设置环境属性 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄 
	ret = SQLConnect(hdbc, (SQLCHAR*)"library", SQL_NTS, (SQLCHAR*)"sa", SQL_NTS, (SQLCHAR*)"123456", SQL_NTS);//连接数据库 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		printf("连接借书卡管理数据库成功！\n");
		if (updateorder == 1) {
			printf("请输入卡号 姓名 院系 类型：\n");
			scanf("%s%s%s%s", cacno, caname, cadepartment, catype);
			//printf("%s %s %s %s", cacno, caname, cadepartment, catype);

			ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
			SQLCHAR sql[] = "insert into card values(?,?,?,?)";
			SQLINTEGER P = SQL_NTS;
			ret = SQLPrepare(hstmt, sql, SQL_NTS);
			ret = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, cacno, 50, &P);//绑定参数 
			ret = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, caname, 50, &P);//绑定参数
			ret = SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, cadepartment, 50, &P);//绑定参数
			ret = SQLBindParameter(hstmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 1, 0, catype,1, &P);//绑定参数 

			ret = SQLExecute(hstmt);//直接执行SQL语句 
			//ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
			if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
				printf("插入借书卡成功！\n");
				SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
			}
			else {
				printf("插入借书卡失败！\n");
			}
		}

		if (updateorder == 2) {

			printf("请输入卡号：\n");
			scanf("%s", cacno);
			//printf("%s %s %s %s", cacno, caname, cadepartment, catype);
			int i;
			int ifcandelete = 1;
			for (i = 0;i < borrownum;i++) {
				if (strcmp(cacno, cborrow[i].cno) == 0) {
					if (strcmp(cborrow[i].return_date,cborrow[19].return_date) == 0) {
						ifcandelete = 0;
						printf("删除异常！该卡还有图书未归还\n");
						break;
					}
				}
			}


			if (ifcandelete == 1) {
				ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
				SQLCHAR sql[] = "delete from card where cno = ? ";
				SQLINTEGER P = SQL_NTS;
				ret = SQLPrepare(hstmt, sql, SQL_NTS);
				ret = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, cacno, 50, &P);//绑定参数 

				ret = SQLExecute(hstmt);//直接执行SQL语句 
				//ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
				if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
					printf("删除借书卡成功！\n");
					SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
				}
				else {
					printf("删除借书卡失败！\n");
				}
			}
		}

		SQLDisconnect(hdbc);//断开与数据库的连接 
	}

	else printf("连接数据库失败!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄
	updatedata();
}

void borrowbook() {


	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//设置环境属性 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄 
	ret = SQLConnect(hdbc, (SQLCHAR*)"library", SQL_NTS, (SQLCHAR*)"sa", SQL_NTS, (SQLCHAR*)"123456", SQL_NTS);//连接数据库 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {

			printf("连接借书管理数据库成功！\n");
			printf("请输入你的借书证卡号：\n");
			scanf("%s", bocno);
			//////////////////////////////输出借书证借的书////////////////////////////////////
			ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
			SQLCHAR sql[] = "select bno from borrow where cno =  ? and return_date is NULL";
			SQLINTEGER P = SQL_NTS;
			ret = SQLPrepare(hstmt, sql, SQL_NTS);
			ret = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, bocno, 50, &P);//绑定参数 
			ret = SQLExecute(hstmt);//直接执行SQL语句 
			//ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
			if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
				printf("查询借书卡借书成功！\n");

				while (SQLFetch(hstmt) != SQL_NO_DATA) {//遍历结果

					SQLGetData(hstmt, 1, SQL_C_CHAR, bobno, 50, &len_bkbno);
					printf("%s\n", bobno);
				}

				SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
			}
			else {
				printf("此卡无书借出！\n");
			}
		/////////////////////////////////////更新库存/////////////////
			printf("请输入书号：\n");
			scanf("%s", bobno);

			int i,j;
			char maxreturn[50] = {0};
			//printf("%d\n", borrownum);
			//printf("%s\n", cborrow[0].return_date);
			//printf("%d\n", strcmp(maxreturn, cborrow[0].return_date));
			for (i = 0;i < booknum;i++) {
				if (strcmp(cbook[i].bno, bobno) == 0) {
					if (cbook[i].stock > 0) {
						cbook[i].stock--;



						ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
						SQLCHAR sql[] = "update book set stock =  ? where bno = ?";
						SQLINTEGER P = SQL_NTS;
						ret = SQLPrepare(hstmt, sql, SQL_NTS);
						ret = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &cbook[i].stock, 0, &P);//绑定参数 
						ret = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, bobno, 50, &P);//绑定参数 
						ret = SQLExecute(hstmt);//直接执行SQL语句 
						//ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
						if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
							printf("借书成功！\n");
							SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
							printf("请输入当前日期：\n");
							scanf("%s", boborrow_date);

							ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
							SQLCHAR sql[] = "insert into borrow values(?,?,?,NULL,?)";
							SQLINTEGER P = SQL_NTS;
							ret = SQLPrepare(hstmt, sql, SQL_NTS);
							ret = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, bocno, 50, &P);//绑定参数 
							ret = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, bobno, 50, &P);//绑定参数 
							ret = SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, boborrow_date, 0, &P);//绑定参数 
							ret = SQLBindParameter(hstmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &currentmanager, 0, &P);//绑定参数 
							
							ret = SQLExecute(hstmt);//直接执行SQL语句 

							if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
								printf("插入借书数据成功！\n");
								SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
							}
							else {
								printf("插入借书数据失败\n");
							}
						}
						else {
							printf("借书失败（链接数据库）！\n");
						}
						break;
					}
					else {
						printf("对不起，本书暂无库存！\n");
						for (j = 0;j < borrownum;j++) {
							if (strcmp(cborrow[j].bno, bobno) == 0) {
								//printf("%s\n", cborrow[j].return_date);
								if (strcmp(cborrow[j].return_date, maxreturn) > 0) {
									strcpy(maxreturn, cborrow[j].return_date);
								}
							}
						}
						printf("最近归还的时间是：%s\n", maxreturn);
					}
				}
			}




		SQLDisconnect(hdbc);//断开与数据库的连接 
	}

	else printf("连接数据库失败!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄
	updatedata();
}

void returnbook() {


	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//设置环境属性 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄 
	ret = SQLConnect(hdbc, (SQLCHAR*)"library", SQL_NTS, (SQLCHAR*)"sa", SQL_NTS, (SQLCHAR*)"123456", SQL_NTS);//连接数据库 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {

		printf("连接还书管理数据库成功！\n");
		printf("请输入你的借书证卡号：\n");
		scanf("%s", bocno);
		//////////////////////////////输出借书证借的书////////////////////////////////////
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sql[] = "select bno from borrow where cno = ?";
		SQLINTEGER P = SQL_NTS;
		ret = SQLPrepare(hstmt, sql, SQL_NTS);
		ret = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, bocno, 50, &P);//绑定参数 
		ret = SQLExecute(hstmt);//直接执行SQL语句 
		//ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			printf("查询借书卡借书成功！\n");

			while (SQLFetch(hstmt) != SQL_NO_DATA) {//遍历结果

				SQLGetData(hstmt, 1, SQL_C_CHAR, bobno, 50, &len_bkbno);
				printf("%s\n", bobno);
			}

			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
		}
		else {
			printf("查询借书卡借书失败！\n");
		}
		/////////////////////////////////////更新库存/////////////////
		printf("请输入书号：\n");
		scanf("%s", bobno);

		int i,j;
		int ifhave = 0;
		for (i = 0;i < booknum;i++) {
			if (strcmp(cbook[i].bno, bobno) == 0) {
				cbook[i].stock++;
				ifhave = 1;
				


				ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
				SQLCHAR sql[] = "update book set stock =  ? where bno = ?";
				SQLINTEGER P = SQL_NTS;
				ret = SQLPrepare(hstmt, sql, SQL_NTS);
				ret = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &cbook[i].stock, 0, &P);//绑定参数 
				ret = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, bobno, 50, &P);//绑定参数 
				ret = SQLExecute(hstmt);//直接执行SQL语句 
				//ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
				if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
					printf("还书成功！\n");


					SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 

					printf("请输入当前日期：\n");
					scanf("%s", boborrow_date);

					ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
					SQLCHAR sql[] = "update borrow set return_date = ? where bno = ? and return_date is NULL and cno = ?";
					SQLINTEGER P = SQL_NTS;
					ret = SQLPrepare(hstmt, sql, SQL_NTS);
					ret = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, boborrow_date, 50, &P);//绑定参数 
					ret = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, bobno, 50, &P);//绑定参数 
					ret = SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, bocno, 50, &P);//绑定参数 
					
					ret = SQLExecute(hstmt);//直接执行SQL语句 

					if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
						printf("插入还书数据成功！\n");
						SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
					}
					else {
						printf("插入还书数据失败\n");
					}
				}
				else {
					printf("还书失败（链接数据库）！\n");
				}
				break;
			}
		}
		if (ifhave == 0) {
			printf("异常！图书馆里没有这本书！\n");
		}




		SQLDisconnect(hdbc);//断开与数据库的连接 
	}

	else printf("连接数据库失败!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄
	updatedata();
}


void updatedata()	{
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//设置环境属性 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄 
	ret = SQLConnect(hdbc, (SQLCHAR*)"library", SQL_NTS, (SQLCHAR*)"sa", SQL_NTS, (SQLCHAR*)"123456", SQL_NTS);//连接数据库 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {

		////////////////////////////////读取manager数据//////////////////////////////////////////////
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sql[] = "SELECT * FROM manager";
		ret = SQLExecDirect(hstmt, sql, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			int i = 0;
			while (SQLFetch(hstmt) != SQL_NO_DATA) {//遍历结果
				SQLGetData(hstmt, 1, SQL_C_ULONG, &cmanager[i].manager_id, 0, &len_mamanager_id);
				SQLGetData(hstmt, 2, SQL_C_ULONG, &cmanager[i].code, 0, &len_macode);
				SQLGetData(hstmt, 3, SQL_C_CHAR, cmanager[i].name, 10, &len_maname);
				SQLGetData(hstmt, 4, SQL_C_ULONG, &cmanager[i].contact, 0, &len_macontact);

				//deletespace(cmanager[i].name);
				//printf("%d \n", cmanager[i].manager_id);
				//printf("%d ", cmanager[i].code);
				i++;
			}
			managernum = i;

			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
		}
		else printf("添加manager数据库操作失败！\n");
		///////////////////////////读取manager结束/////////////////////////////////////////////////////

		///////////////////////////读取book///////////////////////////////////////////////////////////////
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sqlbook[] = "SELECT * FROM book";
		ret = SQLExecDirect(hstmt, sqlbook, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			int i = 0;
			while (SQLFetch(hstmt) != SQL_NO_DATA) {//遍历结果
				SQLGetData(hstmt, 1, SQL_C_CHAR, cbook[i].bno, 50, &len_bkbno);
				SQLGetData(hstmt, 2, SQL_C_CHAR, cbook[i].category, 50, &len_bkcategory);
				SQLGetData(hstmt, 3, SQL_C_CHAR, cbook[i].title, 50, &len_bktitle);
				SQLGetData(hstmt, 4, SQL_C_CHAR, cbook[i].press, 50, &len_bkpress);
				SQLGetData(hstmt, 5, SQL_C_ULONG, &cbook[i].year, sizeof(int), &len_bkyear);
				SQLGetData(hstmt, 6, SQL_C_CHAR, cbook[i].author, 50, &len_bkauthor);
				SQLGetData(hstmt, 7, SQL_C_CHAR, cbook[i].price, sizeof(cbook[i].price), &len_bkprice);
				SQLGetData(hstmt, 8, SQL_C_ULONG, &cbook[i].total, sizeof(int), &len_bktotal);
				SQLGetData(hstmt, 9, SQL_C_ULONG, &cbook[i].stock, sizeof(int), &len_bkstock);

				/*deletespace(cbook[i].bno);
				deletespace(cbook[i].category);
				deletespace(cbook[i].title);
				deletespace(cbook[i].press);
				deletespace(cbook[i].author);*/
				//printf("%s\n", cbook[i].bno);
				//printf("%d\n", cbook[i].year);
				//printf("%s \n", cbook[i].price);

				i++;
			}
			booknum = i;
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄
		}
		else printf("添加book数据库操作失败！\n");


		////////////////////////////////读取book结束///////////////////////////////////////////////////////

		////////////////////////////////读取borrow///////////////////////////////////////////////////////
		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sqlborrow[] = "SELECT * FROM borrow";
		ret = SQLExecDirect(hstmt, sqlborrow, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			int i = 0;
			while (SQLFetch(hstmt) != SQL_NO_DATA) {//遍历结果
				SQLGetData(hstmt, 1, SQL_C_CHAR, cborrow[i].cno, 50, &len_bocno);
				SQLGetData(hstmt, 2, SQL_C_CHAR, cborrow[i].bno, 50, &len_bobno);
				SQLGetData(hstmt, 3, SQL_C_CHAR, cborrow[i].borrow_date, 50, &len_boborrow_date);
				SQLGetData(hstmt, 4, SQL_C_CHAR, cborrow[i].return_date, 50, &len_boreturn_date);
				SQLGetData(hstmt, 5, SQL_C_ULONG, &cborrow[i].manager_id, 0, &len_bomanager_id);

				//deletespace(cborrow[i].bno);
				//deletespace(cborrow[i].cno);

				//printf("%s \n", cborrow[i].borrow_date);

				i++;
			}
			borrownum = i;
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄
		}
		else printf("添加borrow数据库操作失败！\n");



		////////////////////////////////读取borrow结束/////////////////////////////////////////////////

		////////////////////////////////读取card/////////////////////////////////////////////////////////

		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLCHAR sqlcard[] = "SELECT * FROM card";
		ret = SQLExecDirect(hstmt, sqlcard, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
			int i = 0;
			while (SQLFetch(hstmt) != SQL_NO_DATA) {//遍历结果
				SQLGetData(hstmt, 1, SQL_C_CHAR, ccard[i].cno, 50, &len_cacno);
				SQLGetData(hstmt, 2, SQL_C_CHAR, ccard[i].name, 50, &len_caname);
				SQLGetData(hstmt, 3, SQL_C_CHAR, ccard[i].department, 50, &len_cadepartment);
				SQLGetData(hstmt, 4, SQL_C_CHAR, ccard[i].type, 1, &len_catype);

				/*deletespace(ccard[i].cno);
				deletespace(ccard[i].name);
				deletespace(ccard[i].department);
				deletespace(ccard[i].type);*/

				//printf("%d \n", cbook[i].total);

				i++;
			}
			cardnum = i;
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄
		}
		else printf("添加card数据库操作失败！\n");


		////////////////////////////////读取card结束///////////////////////////////////////////////////////
		SQLDisconnect(hdbc);//断开与数据库的连接 
	}
	else printf("连接数据库失败!\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄
}

void lookupborrow() {
	int i;
	printf("请输入您想查询的卡号：\n");
	scanf("%s", bocno);
	for (i = 0;i < borrownum;i++) {
		if(strcmp(bocno,cborrow[i].cno) == 0)
		printf("书号：%s\n", cborrow[i].bno);
		printf("借出日期：%s\n", cborrow[i].borrow_date);
		printf("归还日期：%s\n", cborrow[i].return_date);
		printf("经手人：%d\n", cborrow[i].manager_id);
	}
}

void lookupmultiple() {
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//申请环境句柄 
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);//设置环境属性 
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//申请数据库连接句柄 
	ret = SQLConnect(hdbc, (SQLCHAR*)"library", SQL_NTS, (SQLCHAR*)"sa", SQL_NTS, (SQLCHAR*)"123456", SQL_NTS);//连接数据库 
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		

		char sql[100] = "SELECT * FROM book where bno is not NULL ";

		printf("请输入书号（若无请输入0后回车）：\n");
		scanf("%s", bkbno);
		if (bkbno[0] != '0') {
			char sqla[] = "and bno = ? ";
			strcat(sql, sqla);
		}
		printf("请输入种类（若无请输入0后回车））：\n");
		scanf("%s", bkcategory);
		if (bkcategory[0] != '0') {
			char sqlb[] = "and category = ? ";
			strcat(sql, sqlb);
		}
		printf("请输入标题（若无请输入0后回车）：\n");
		scanf("%s", bktitle);
		if (bktitle[0] != '0') {
			char sqlc[] = "and title = ? ";
			strcat(sql, sqlc);
		}
		printf("请输入出版商（若无请输入0后回车）：\n");
		scanf("%s", bkpress);
		if (bkpress[0] != '0') {
			char sqld[] = "and press = ? ";
			strcat(sql, sqld);
		}
		printf("请输入年份下限（若无请输入-1后回车）：\n");
		scanf("%d", &bkyearl);
		if (bkyearl != -1) {
			char sqle[] = "and year > ? ";
			strcat(sql, sqle);
		}
		printf("请输入年份上限（若无请输入-1后回车）：\n");
		scanf("%d", &bkyear);
		if (bkyear != -1) {
			char sqlf[] = "and year < ? ";
			strcat(sql, sqlf);
		}
		printf("请输入作者（若无请输入0后回车）：\n");
		scanf("%s", bkauthor);
		if (bkauthor[0] != '0') {
			char sqlg[] = "and author = ? ";
			strcat(sql, sqlg);
		}
		printf("请输入价格下限（若无请输入-1后回车）：\n");
		scanf("%f", &bkpricel);
		if (bkpricel != -1) {
			char sqlh[] = "and price > ? ";
			strcat(sql, sqlh);
		}
		printf("请输入价格上限（若无请输入-1后回车）：\n");
		scanf("%f", &bkpriceh);
		if (bkpriceh != -1) {
			char sqli[] = "and price < ? ";
			strcat(sql, sqli);
		}

		//printf("%s\n", sql);

		ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//申请SQL语句句柄 
		SQLINTEGER P = SQL_NTS;
		ret = SQLPrepare(hstmt, (SQLCHAR*)sql, SQL_NTS);

		int i = 1;
		if (bkbno[0] != '0') {
			ret = SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, bkbno, 50, &P);//绑定参数 
		}
		if (bkcategory[0] != '0') {
			ret = SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, bkcategory, 50, &P);//绑定参数
		}
		if (bktitle[0] != '0') {
			ret = SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, bktitle, 50, &P);//绑定参数
		}
		if (bkpress[0] != '0'){
			ret = SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, bkpress, 50, &P);//绑定参数 
		}
		if (bkyearl != -1) {
			ret = SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &bkyearl, sizeof(int), &P);
		}
		if (bkyear != -1) {
			ret = SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &bkyear, sizeof(int), &P);
		}
		if (bkauthor[0] != '0') {
			ret = SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, bkauthor, 50, &P);
		}
		if (bkpricel != -1) {
			ret = SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_DECIMAL, 7, 2, &bkpricel, sizeof(float), &P);
		}
		if (bkpriceh != -1) {
			ret = SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_DECIMAL, 7, 2, &bkpriceh, sizeof(float), &P);
		}

		ret = SQLExecute(hstmt);//直接执行SQL语句 


		//ret = SQLExecDirect(hstmt, (SQLCHAR *)sql, SQL_NTS);//直接执行SQL语句 
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {

			while (SQLFetch(hstmt) != SQL_NO_DATA) {//遍历结果

				SQLGetData(hstmt, 1, SQL_C_CHAR, bkbno, 50, &len_bkbno);
				SQLGetData(hstmt, 2, SQL_C_CHAR, bkcategory, 50, &len_bkcategory);
				SQLGetData(hstmt, 3, SQL_C_CHAR, bktitle, 50, &len_bktitle);
				SQLGetData(hstmt, 4, SQL_C_CHAR, bkpress, 50, &len_bkpress);
				SQLGetData(hstmt, 5, SQL_C_ULONG, &bkyear, sizeof(int), &len_bkyear);
				SQLGetData(hstmt, 6, SQL_C_CHAR, bkauthor, 50, &len_bkauthor);
				SQLGetData(hstmt, 7, SQL_C_CHAR, bkprice, 10, &len_bkprice);
				SQLGetData(hstmt, 8, SQL_C_ULONG, &bktotal, sizeof(int), &len_bktotal);
				SQLGetData(hstmt, 9, SQL_C_ULONG, &bkstock, sizeof(int), &len_bkstock);

				printf("书号：%s\n", bkbno);
				printf("类别：%s\n", bkcategory);
				printf("书名：%s\n", bktitle);
				printf("出版社：%s\n", bkpress);
				printf("年份：%d\n", bkyear);
				printf("作者：%s\n", bkauthor);
				printf("价格：%s\n", bkprice);
				printf("总藏书量：%d\n", bktotal);
				printf("库存：%d\n", bkstock);

			}

			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//释放语句句柄 
		}
		else printf("复合查询图书失败！\n");
		

		SQLDisconnect(hdbc);//断开与数据库的连接 
	}

	else printf("连接数据库失败!\n");

	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//释放连接句柄 
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//释放环境句柄
}


void display() {
	printf("---------------------------------------------------------\n");
	printf("--    请问您想进行什么操作？（按数字与回车进行操作）   --\n");
	printf("--1.管理员登录                                         --\n");
	printf("--2.图书入库                                           --\n");
	printf("--3.图书查询                                           --\n");
	printf("--4.借书                                               --\n");
	printf("--5.还书                                               --\n");
	printf("--6.借书证管理                                         --\n");
	printf("--7.查看借书证借书记录                                 --\n");
	printf("--8.图书复合查询                                       --\n");
	printf("--0.退出                                               --\n");
	printf("---------------------------------------------------------\n");
}

int main()
{
	ifmanager = 0;
	managernum = 0;
	booknum = 0;
	borrownum = 0;
	cardnum = 0;

	load();//初始化数据库 并把数据库信息读入结构体中
	int order;//order用来表示操作目的
	printf("请输入您的操作，并按回车结束！\n");
	scanf("%d", &order);
	while (1) {
		switch(order) {
			case 0:
				return 0;//quit
			case 1:
				login();
				break;
			case 2:
				if (ifmanager == 0) {
					printf("对不起，您不是管理员，无法进行导入！\n");
					break;
				}
				input();
				break;
			case 3:
				lookup();
				break;
			case 4:
				if (ifmanager == 0) {
					printf("对不起，您不是管理员，无法进行借书！\n");
					break;
				}
				borrowbook();
				break;
			case 5:
				if (ifmanager == 0) {
					printf("对不起，您不是管理员，无法进行还书！\n");
					break;
				}
				returnbook();
				break;
			case 6:
				if (ifmanager == 0) {
					printf("对不起，您不是管理员，无法进行更新借书卡！\n");
					break;
				}
				updatecard();
				break;
			case 7:
				if (ifmanager == 0) {
					printf("对不起，您不是管理员，无法进行更新借书卡！\n");
					break;
				}
				lookupborrow();
				break;
			case 8:
				lookupmultiple();
				break;
		}
		//printf("%");
		printf("请输入您的操作，并按回车结束！\n");
		display();
		scanf("%d", &order);
	}
}
//注销要求