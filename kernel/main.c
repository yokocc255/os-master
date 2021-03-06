
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"
#include "keyboard.h"




/*======================================================================*/

/* global variable of game Tictactoe */
int tmpQP[3][3]; //表示棋盘数据的临时数组，其中的元素0表示该格为空，
//1表示计算机放下的子，-1表示人放下的子。

char snake_Array[17][30] = 
{
{'*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','\n','\0'},
{'*',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','*','\n','\0'},
{'*',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','*','\n','\0'},
{'*',' ',' ',' ',' ',' ',' ',' ',' ','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*',' ',' ',' ','*','\n','\0'},
{'*',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','*','*','*','\n','\0'},
{'*',' ',' ',' ','*','*','*','*','*','*','*','*','*','*','*','*','*',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','*','\n','\0'},
{'*',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','*','*','*','*','*','*','*','*','*','\n','\0'},
{'*',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','*','\n','\0'},
{'*',' ',' ',' ','*','*','*','*','*','*','*','*','*','*','*','*','*',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','*','\n','\0'},
{'*',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','*','*','*','*','*','*','*','*',' ',' ','*','\n','\0'},
{'*',' ',' ','*','*',' ','*','*','*','*',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','*','\n','\0'},
{'*',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','*','\n','\0'},
{'*',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','*','\n','\0'},
{'*',' ','*','*','*','*','*','*',' ',' ',' ','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','\n','\0'},
{'*',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','*','\n','\0'},
{'*',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','*','\n','\0'},
{'*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*',' ','*','*','*','*','\n','\0'}};
int snake_head[2] = {1,2};
int snake_area_width = 30;
int snake_area_height = 17;
int move_direction = 4;
int selectShowProcess = 0;

#define MAX_NUM 1000//扩展生成状态节点的最大数目

//推箱子游戏:

//1.构图 使用字符二维数组 
char BoxMap[LINES][COLUS] = 
{
		"##########",
		"#O ####  #",
		"# X####  #",
		"#        #",
		"######   #",
		"#  ####  #",
		"#        #",
		"#   ######",
		"#         ",
		"##########"
};



const int NO_BLANK=-1001; //表示没有空格
const int TREE_DEPTH=3; //搜索树的最大深度，如果增加此值可以提高计算机的“智力”，
//但同时也需要增加MAX_NUM的值。
const int NIL=1001;    //表示空
static int s_count;     //用来表示当前分析的节点的下标

//calculator
double first_number = 0.0;
double second_number = 0.0;
double cal_result = 0.0;

struct State//该结构表示棋盘的某个状态，也可看做搜索树中的一个节点
{
	int QP[3][3]; //棋盘格局
	int e_fun; //当前状态的评估函数值
	int child[9]; //儿女节点的下标
	int parent; //双亲节点的下标
	int bestChild; //最优节点（评估函数值最大）的儿女节点下标
}States[MAX_NUM]; //用来保存搜索树中状态节点的数组



//init calacultor
void cal_init()
{
	first_number = 0.0;
	second_number = 0.0;
	cal_result = 0.0;
}

void Init()   //初始化函数，当前的棋盘格局总是保存在States[0]中
{
	int i,j;
	s_count=0; 
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
			States[0].QP[i][j]=0; //将棋盘清空
	States[0].parent=NIL;   //初始节点没有双亲节点
}

void PrintQP() //打印当棋盘格局的函数
{
	int i,j;
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
			if (States[0].QP[i][j] == -1)
			{
				printf("%c       ",1);
			}
			else if (States[0].QP[i][j] == 1)
			{
				printf("%c       ",2);
			}
			else
			{
				printf("%d       ",0);
			}

			printf("\n");
	}
}

int IsWin(struct State s) //有人赢了吗？返回0表示没有人赢，返回-1表示人赢了，返回1表示计算机赢了
{
	int i,j;
	for(i=0;i<3;i++)
	{
		if(s.QP[i][0]==1&&s.QP[i][1]==1&&s.QP[i][2]==1)return 1;
		if(s.QP[i][0]==-1&&s.QP[i][1]==-1&&s.QP[i][2]==-1)return -1;
	}
	for(i=0;i<3;i++)
	{
		if(s.QP[0][i]==1&&s.QP[1][i]==1&&s.QP[2][i]==1)return 1;
		if(s.QP[0][i]==-1&&s.QP[1][i]==-1&&s.QP[2][i]==-1)return -1;
	}
	if((s.QP[0][0]==1&&s.QP[1][1]==1&&s.QP[2][2]==1)||(s.QP[2][0]==1&&s.QP[1][1]==1&&s.QP[0][2]==1))return 1;
	if((s.QP[0][0]==-1&&s.QP[1][1]==-1&&s.QP[2][2]==-1)||(s.QP[2][0]==-1&&s.QP[1][1]==-1&&s.QP[0][2]==-1))return -1;
	return 0;
}

int e_fun(struct State s)//评估函数
{
	int flag=1;
	int i,j;
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
			if(s.QP[i][j]==0)flag= FALSE;
	if(flag)return NO_BLANK;

	if(IsWin(s)==-1)return -MAX_NUM;//如果计算机输了，返回最小值
	if(IsWin(s)==1)return MAX_NUM;//如果计算机赢了，返回最大值
	int count=0;//该变量用来表示评估函数的值

	//将棋盘中的空格填满自己的棋子，既将棋盘数组中的0变为1
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
			if(s.QP[i][j]==0)tmpQP[i][j]=1;
			else tmpQP[i][j]=s.QP[i][j];

			//电脑一方
			//计算每一行中有多少行的棋子连成3个的
			for(i=0;i<3;i++)
				count+=(tmpQP[i][0]+tmpQP[i][1]+tmpQP[i][2])/3;
			//计算每一列中有多少列的棋子连成3个的
			for(i=0;i<3;i++)
				count+=(tmpQP[0][i]+tmpQP[1][i]+tmpQP[2][i])/3;
			//斜行有没有连成3个的？
			count+=(tmpQP[0][0]+tmpQP[1][1]+tmpQP[2][2])/3;
			count+=(tmpQP[2][0]+tmpQP[1][1]+tmpQP[0][2])/3;

			//将棋盘中的空格填满对方的棋子，既将棋盘数组中的0变为-1
			for(i=0;i<3;i++)
				for(j=0;j<3;j++)
					if(s.QP[i][j]==0)tmpQP[i][j]=-1;
					else tmpQP[i][j]=s.QP[i][j];

					//对方
					//计算每一行中有多少行的棋子连成3个的
					for(i=0;i<3;i++)
						count+=(tmpQP[i][0]+tmpQP[i][1]+tmpQP[i][2])/3;
					//计算每一列中有多少列的棋子连成3个的
					for(i=0;i<3;i++)
						count+=(tmpQP[0][i]+tmpQP[1][i]+tmpQP[2][i])/3;
					//斜行有没有连成3个的？
					count+=(tmpQP[0][0]+tmpQP[1][1]+tmpQP[2][2])/3;
					count+=(tmpQP[2][0]+tmpQP[1][1]+tmpQP[0][2])/3;

					return count;
}

//计算机通过该函数决定走哪一步，并对当前的棋局做出判断。
int AutoDone()
{

	int 
		MAX_F=NO_BLANK, //保存对自己最有利的棋局（最大）的评估函数值
		parent=-1, //以当前棋局为根生成搜索树，所以当前棋局节点无双亲节点
		count,   //用来计算当前生成的最后一个扩展节点的下标

		tag;   //标示每一层搜索树中最后一个节点的下标
	int 
		max_min=TREE_DEPTH%2, //标识取下一层评估函数的最大值还是最小值？
		//max_min=1取下一层中的最大值，max_min=0取最小值
		IsOK=FALSE;    //有没有找到下一步落子的位置？
	s_count=0;   //扩展生成的节点数初始值为0

	if(IsWin(States[0])==-1)//如果人赢了
	{
		printf("You Win!\n");
		return TRUE;
	}

	int i,j,t,k,i1,j1;
	for(t=0;t<TREE_DEPTH;t++)//依次生成各层节点
	{
		count=s_count;//保存上一层节点生成的最大下标
		for(k=parent+1;k<=count;k++)//生成一层节点
		{
			int n_child=0;//该层节点的孩子节点数初始化为0
			for(i=0;i<3;i++)
				for(j=0;j<3;j++)
					if(States[k].QP[i][j]==0)//如果在位置(i,j)可以放置一个棋子
					{       //则
						s_count++;    //生成一个节点，节点数（最大下标）数加1
						for(i1=0;i1<3;i1++) //该3×3循环将当前棋局复制到新节点对应的棋局结构中
							for(j1=0;j1<3;j1++)
								States[s_count].QP[i1][j1]=States[k].QP[i1][j1];
						States[s_count].QP[i][j]=t%2==0?1:-1;//根据是人下还是计算机下，在空位上落子
						States[s_count].parent=k;   //将父母节点的下标k赋给新生成的节点
						States[k].child[n_child++]=s_count; //下标为k的父母节点有多了个子女

						//如果下一步有一步期能让电脑赢，则停止扩展节点，转向结局打印语句
						if(t==0&&e_fun(States[s_count])==MAX_NUM)
						{
							States[k].e_fun=MAX_NUM;
							States[k].bestChild=s_count;//最好的下一步棋所在的节点的下标为s_count
							goto L2;
						}
					}
		}
		parent=count;   //将双亲节点设置为当前双亲节点的下一层节点
//		printf("%d\n",s_count); //打印生成节点的最大下标
	}

	tag=States[s_count].parent;//设置最底层标志，以便从下到上计算最大最小值以寻找最佳解路径。
	int pos_x,pos_y;//保存计算机落子的位置
	for(i=0;i<=s_count;i++)
	{
		if(i>tag) //保留叶节点的评估函数值
		{
			States[i].e_fun=e_fun(States[i]);
		}
		else //抹去非叶节点的评估函数值
			States[i].e_fun=NIL;
	}
	while(!IsOK)//寻找最佳落子的循环
	{
		for(i=s_count;i>tag;i--)
		{
			if(max_min)//取子女节点的最大值
			{
				if(States[States[i].parent].e_fun<States[i].e_fun||States[States[i].parent].e_fun==NIL)
				{
					States[States[i].parent].e_fun=States[i].e_fun; //设置父母节点的最大最小值
					States[States[i].parent].bestChild=i;   //设置父母节点的最佳子女的下标
				}
			}
			else//取子女节点的最小值
			{
				if(States[States[i].parent].e_fun>States[i].e_fun||States[States[i].parent].e_fun==NIL)
				{
					States[States[i].parent].e_fun=States[i].e_fun; //设置父母节点的最大最小值
					States[States[i].parent].bestChild=i;   //设置父母节点的最佳子女的下标
				}
			}
		}
		s_count=tag; //将遍历的节点上移一层
		max_min=!max_min; //如果该层都是MAX节点，则它的上一层都是MIN节点，反之亦然。
		if(States[s_count].parent!=NIL)//如果当前遍历的层中不包含根节点，则tag标志设为上一层的最后一个节点的下标
			tag=States[s_count].parent;
		else
			IsOK=TRUE; //否则结束搜索
	}
	int x,y;
L2: //取落子的位置，将x,y坐标保存在变量pos_x和pos_y中，并将根（当前）节点中的棋局设为最佳儿子节点的棋局

	for(x=0;x<3;x++)
	{
		for(y=0;y<3;y++)
		{
			if(States[States[0].bestChild].QP[x][y]!=States[0].QP[x][y])
			{
				pos_x=x;
				pos_y=y;    
			}
			States[0].QP[x][y]=States[States[0].bestChild].QP[x][y]; 
		}
	}


	MAX_F=States[0].e_fun;
	//cout<<MAX_F<<endl;

	printf("The computer put a Chessman at: %d,%d\nThe QP now is:\n",pos_x+1,pos_y+1);
	PrintQP();
	if(MAX_F==MAX_NUM) //如果当前节点的评估函数为最大值，则计算机赢了
	{
		printf("The computer WIN! You LOSE! GAME OVER.\n");
		return TRUE;
	}
	if(MAX_F==NO_BLANK) //否则，如果棋盘时候没空可放了，则平局。
	{
		printf("DRAW GAME!\n");
		return TRUE;
	}
	return FALSE;
}

//用户通过此函数来输入落子的位置，
//比如，用户输入31，则表示用户在第3行第1列落子。
void UserInput(int fd_stdin,int fd_stdout)
{

	int n;
	int pos = -1,x,y;
	char szCmd[80]={0};
	L1: printf("Please Input The Line where you put your Chessman (x): "); 
	n = read(fd_stdin,szCmd,80);
	szCmd[1] = 0;
	atoi(szCmd,&x);
	printf("Please Input The Column where you put your Chessman (y): "); 
	n = read(fd_stdin,szCmd,80);
	szCmd[1] = 0;
	atoi(szCmd,&y);
	if(x>0&&x<4&&y>0&&y<4&&States[0].QP[x-1][y-1]==0)
		States[0].QP[x-1][y-1]=-1;
	else
	{
		printf("Input Error!");
		goto L1;
	}

}



/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{

	clear();

	struct task* p_task;
	struct proc* p_proc= proc_table;
	char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
	u16   selector_ldt = SELECTOR_LDT_FIRST;
        u8    privilege;
        u8    rpl;
	int   eflags;
	int   i, j;
	int   prio;
	for (i = 0; i < NR_TASKS+NR_PROCS; i++) {
	        if (i < NR_TASKS) {     /* 任务 */
                        p_task    = task_table + i;
                        privilege = PRIVILEGE_TASK;
                        rpl       = RPL_TASK;
                        eflags    = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1 */
			prio      = 15;
                }
                else {                  /* 用户进程 */
                        p_task    = user_proc_table + (i - NR_TASKS);
                        privilege = PRIVILEGE_USER;
                        rpl       = RPL_USER;
                        eflags    = 0x202; /* IF=1, bit 2 is always 1 */
			prio      = 5;
                }

		strcpy(p_proc->name, p_task->name);	/* name of the process */
		p_proc->pid = i;			/* pid */

		p_proc->run_count = 0;

		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
		       sizeof(struct descriptor));
		p_proc->ldts[0].attr1 = DA_C | privilege << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
		       sizeof(struct descriptor));
		p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
		p_proc->regs.cs	= (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ds	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.es	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.fs	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ss	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = eflags;

		/* p_proc->nr_tty		= 0; */

		p_proc->p_flags = 0;
		p_proc->p_msg = 0;
		p_proc->p_recvfrom = NO_TASK;
		p_proc->p_sendto = NO_TASK;
		p_proc->has_int_msg = 0;
		p_proc->q_sending = 0;
		p_proc->next_sending = 0;

		for (j = 0; j < NR_FILES; j++)
			p_proc->filp[j] = 0;

		p_proc->ticks = p_proc->priority = prio;
		p_proc->run_state = 1;

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}
	proc_table[5].run_state = 0;
	proc_table[6].run_state = 0;

        /* proc_table[NR_TASKS + 0].nr_tty = 0; */
        /* proc_table[NR_TASKS + 1].nr_tty = 1; */
        /* proc_table[NR_TASKS + 2].nr_tty = 1; */

	k_reenter = 0;
	ticks = 0;

	p_proc_ready = proc_table;

	init_clock();
    init_keyboard();

	restart();

	while(1){}
}


/*****************************************************************************
 *                                get_ticks
 *****************************************************************************/
PUBLIC int get_ticks()
{
	MESSAGE msg;
	reset_msg(&msg);
	msg.type = GET_TICKS;
	send_recv(BOTH, TASK_SYS, &msg);
	return msg.RETVAL;
}


/*======================================================================*
                               TestA
 *======================================================================*/
void TestA()
{
	int fd;
	int i, n;

	char tty_name[] = "/dev_tty0";

	char rdbuf[128];


	int fd_stdin  = open(tty_name, O_RDWR);
	assert(fd_stdin  == 0);
	int fd_stdout = open(tty_name, O_RDWR);
	assert(fd_stdout == 1);

//	char filename[MAX_FILENAME_LEN+1] = "zsp01";
	const char bufw[80] = {0};
//	const int rd_bytes = 3;
//	char bufr[rd_bytes];
	

	

	clear();
		printf("        *************************************************************\n");
	 	printf("        *                                                           *\n");
		printf("        *        * * *             A simple OS         * * *        *\n");
		printf("        *      *  = =  *             Welcome!        *  = =  *      *\n");
		printf("        *     *   o o   *     by 1452754 Liu Yang   *   o o   *     *\n");
	    printf("        *      *   o   *                AND          *   o   *      *\n");
		printf("        *        * * *      1452760 Zhang Haoming      * * *        *\n");
		printf("        *                                                           *\n");
		printf("        *************************************************************\n");
	while (1) {
		printl("root@localhost=> ");
		int r = read(fd_stdin, rdbuf, 70);
		rdbuf[r] = 0;
	 	//show();
                if (strcmp(rdbuf, "process") == 0)
                {
			ProcessManage();
                }
           
		else if (strcmp(rdbuf, "filemng") == 0)
		{
			printf("All Files");
			printfiles("/EOF", O_RDWR);
			printf("\nIf you want to manage files, press SHIFT+F2\n\n");
			continue;

		}
		else if(strcmp(rdbuf,"maze") == 0 )
		{
			move_direction = 4;
			snake_head[0] = 1;
			snake_head[1] = 2;
			StartTheSnake();
		}
		else if(strcmp(rdbuf,"pause a") == 0 )
		{
			proc_table[4].run_state = 0 ;
			ProcessManage();
		}
		else if(strcmp(rdbuf,"pause b") == 0 )
		{
			proc_table[5].run_state = 0 ;
			ProcessManage();
		}
		else if(strcmp(rdbuf,"restart b") == 0 )
		{
		}
		else if(strcmp(rdbuf,"restart c") == 0 ){
			
		}
		else if(strcmp(rdbuf,"kill b") == 0 )
		{
			proc_table[5].p_flags = 1;
			ProcessManage();
		}
		else if(strcmp(rdbuf,"kill c") == 0 )
		{
			proc_table[6].p_flags = 1;
			ProcessManage();
		}
		else if(strcmp(rdbuf,"pause c") == 0 )
		{
			proc_table[6].run_state = 0 ;
			ProcessManage();
		}
		else if(strcmp(rdbuf,"resume a") == 0 )
		{
			proc_table[4].run_state = 1 ;
			ProcessManage();
		}
		else if(strcmp(rdbuf,"up a") == 0 )
		{
			proc_table[4].priority = proc_table[4].priority*2;
			ProcessManage();
		}
		else if(strcmp(rdbuf,"up c") == 0 )
		{
			proc_table[6].priority = proc_table[6].priority*2;
			ProcessManage();
		}
		else if(strcmp(rdbuf,"up b") == 0 )
		{
			proc_table[5].priority = proc_table[5].priority*2;
			ProcessManage();
		}		
		else if(strcmp(rdbuf,"resume b") == 0 )
		{
			proc_table[5].run_state = 1 ;
			ProcessManage();
		}
		else if(strcmp(rdbuf,"snake") == 0 )
		{
			game2();
		}
		else if(strcmp(rdbuf,"resume c") == 0 )
		{
			proc_table[6].run_state = 1 ;
			ProcessManage();
		}
        else if (strcmp(rdbuf, "help") == 0)
		{
			clear();
			help();
		}
		else if (strcmp(rdbuf, "calculate") == 0)
		{
			TTT2(fd_stdin, fd_stdout);
		}

		else if (strcmp(rdbuf, "calendar") == 0)
		{
			TTT3(fd_stdin, fd_stdout);
		}		
        else if (strcmp(rdbuf, "show all process") == 0)
		{
			selectShowProcess = 1;
			ProcessManage();
		}
		

		//box game
        else if (strcmp(rdbuf, "box") == 0)
		{
			clear();

			BoxGameStart(fd_stdin, fd_stdout);

		}



		else if (strcmp(rdbuf, "chess") == 0)
		{

			TTT(fd_stdin, fd_stdout);
		}
		
		else if (strcmp(rdbuf, "clear") == 0)
		{
		clear();
		printf("        *************************************************************\n");
	 	printf("        *                                                           *\n");
		printf("        *        * * *             A simple OS         * * *        *\n");
		printf("        *      *  = =  *             Welcome!        *  = =  *      *\n");
		printf("        *     *   o o   *     by 1452754 Liu Yang   *   o o   *     *\n");
	    printf("        *      *   o   *                AND          *   o   *      *\n");
		printf("        *        * * *      1452760 Zhang Haoming      * * *        *\n");
		printf("        *                                                           *\n");
		printf("        *************************************************************\n");
		}
		
		else
			printf("Command not found, please check!\n");
	}
}

/*======================================================================*
                               TestB
 *======================================================================*/
void TestB()
{
	while(1){
	if(proc_table[5].run_state == 1){
	disp_str("b");
	milli_delay(400);
	}
	}  
}





void TestC()
{	
	while(1){
	if(proc_table[6].run_state == 1){
	disp_str("c");
	milli_delay(900);
	}
	}       
}

void TTT3(int fd_stdin,int fd_stdout)
{
	int year, month;

	char buf[80]={0};
	char IsFirst = 0;
	int IsFinish = FALSE;
	while(!IsFinish)
	{
		printf("Hello \n This is a calendar application,press 'y' to continue!\n");
		read(fd_stdin,buf,2);
		IsFirst = buf[0];
		printf("please input the year: \n");
		read(fd_stdin,buf,7);
		year = My_atoi(buf);
		//printf("%s\n",buf);
		printf("please input the month: \n");
		memset(buf,'\0',sizeof(buf));
		read(fd_stdin,buf,5);
		//printf("%s\n",buf);
		month = My_atoi(buf);
		//printf("%d\n",month);
		//printf("hhh\n");
		calendar_control(year,month);
		do{

			if(IsFirst=='y')
			{
				IsFinish=TRUE;
			}
			
		}while(!IsFinish);	

		if (IsFinish)
		{
			printf("Play Again?(y/n)");
			char cResult;
			read(fd_stdin,buf,2);
			cResult = buf[0];
			printf("%c",cResult);
			if (cResult == 'y')
			{
				clear();
				IsFinish = FALSE;
			}
			else
			{
				clear();
			}
		}			
	}
}

void TTT2(int fd_stdin,int fd_stdout)
{
	// int i=0;
	char buf[80]={0};
	char IsFirst = 0;
	char input_number1[80]={0};
	int IsFinish = FALSE;
	int ret1=0;
	int ret2=0;
	int ret3=0;
	char cal_func;
	while(!IsFinish)
	{
		cal_init();
		printf("Hello\nThis is a calculator application, press 'y' to continue!\n");
		read(fd_stdin,buf,2);
		IsFirst = buf[0];
		printf("please input the first number \n");
		read(fd_stdin,buf,7);
		ret1=My_atoi(buf);
		printf("please input the second number \n");
		memset(buf,'\0',sizeof(buf));
		read(fd_stdin,buf,7);
		ret2=My_atoi(buf);
		printf("choose the function! \n");
		printf("please input the '+','-','*','/' for calculate !\n");
		read(fd_stdin,buf,2);
		if(buf[0]=='+')
		{
			ret3 = add_fun(ret1,ret2);
			printf("+\n");
		}
		else if(buf[0]=='-')
		{
			ret3 = sub_fun(ret1,ret2);
			printf("-\n");
		}
		else if(buf[0]=='*')
		{
			ret3 = mul_fun(ret1,ret2);
			printf("*\n");
		}
		else if(buf[0]=='/')
		{
			ret3 = div_fun(ret1,ret2);
			printf("/\n");
		}
		else
		{
			printf("please input the right function to calculate!\n");
		}	
		printf("The formula is:\n");	
		printf("%d %c %d = %d\n", ret1,buf[0],ret2,ret3);		
		do{

			if(IsFirst=='y')
			{
				IsFinish=TRUE;
			}
			
		}while(!IsFinish);	

		if (IsFinish)
		{
			printf("Play Again?(y/n)");
			char cResult;
			read(fd_stdin,buf,2);
			cResult = buf[0];
			printf("%c",cResult);
			if (cResult == 'y')
			{
				clear();
				IsFinish = FALSE;
			}
			else
			{
				clear();
			}
		}			
	}
}



 void BoxGameStart(int fd_stdin,int fd_stdout)
 {

	//定义变量来创建必要的坐标系统 记录当前位置
	int persionX, persionY, boxX, boxY;
	persionX = persionY = 1;
	boxX = boxY = 2;
	char street = ' ';
	char box = 'X';//prototype of box character
	char wall = '#';
	int isWin = 0;//出局条件
	int winX = LINES - 2;//win cordinate
	int winY = COLUS - 2;//win cordinate
	char buf[80]={0};
	int IsFinish = FALSE;
	char direction = 0;
	char IsFirst = 0;
			
	DrawMap(BoxMap);

	//接收用户输入 并不断重绘地图 直到用户获得胜利
	while(!IsFinish)
	{
		//printf("%d persionX,%d persionY,%d boxX,%d boxY\n",persionX,persionY,boxX,boxY);
		int nextpersonX = persionX;
		int nextpersonY = persionY;
		int nextboxX = boxX;
		int nextboxY = boxY;
		printf("choose direction 'a','s','d','w' to move the box Haha!\n");
		memset(buf,'\0',sizeof(buf));
		read(fd_stdin,buf,2);
		direction = buf[0];
		playerChoice(&nextpersonX, &nextpersonY, &nextboxX, &nextboxY,direction);
		if (BoxMap[nextboxX][nextboxY] == street&&
			BoxMap[nextpersonX][nextpersonY] == box) {
			swap(BoxMap, &boxX, &boxY, &nextboxX, &nextboxY);
			swap(BoxMap, &persionX, &persionY, &nextpersonX, &nextpersonY);
		}if (BoxMap[nextpersonX][nextpersonY] == street)
			swap(BoxMap, &persionX, &persionY, &nextpersonX, &nextpersonY);

		clear();
		DrawMap(BoxMap);

		if (nextboxX == winX && nextboxY == winY) {
			isWin = TRUE;
			IsFinish = TRUE;
			printf("Successful!\n");
		}
		if (isLose(BoxMap, boxX, boxY, wall)) {
			printf("Failed!\n");
			break;
		}
			
	}



}

void TTT(int fd_stdin,int fd_stdout)
{

	char buf[80]={0};
	char IsFirst = 0;
	int IsFinish = FALSE;
	while(!IsFinish)
	{
		Init();
		printf("The chesss is: \n");

		PrintQP();
		
		printf("Do you want do first?(y/n):");
		read(fd_stdin,buf,2);
		IsFirst = buf[0];
		do{

			if(IsFirst=='y')
			{
				UserInput(fd_stdin, fd_stdout);
				IsFinish=AutoDone();
			}else{
				IsFinish=AutoDone();
				if(!IsFinish)UserInput(fd_stdin, fd_stdout);
			}
		}while(!IsFinish);
		if (IsFinish)
		{
			printf("Play Again?(y/n)");
			char cResult;
			read(fd_stdin,buf,2);
			cResult = buf[0];
			printf("%c",cResult);
			if (cResult == 'y')
			{
				clear();
				IsFinish = FALSE;

			}
			else
			{
				clear();
			}

		}
	}
	
}

/*****************************************************************************
 *                                panic
 *****************************************************************************/
PUBLIC void panic(const char *fmt, ...)
{
	int i;
	char buf[256];

	/* 4 is the size of fmt in the stack */
	va_list arg = (va_list)((char*)&fmt + 4);

	i = vsprintf(buf, fmt, arg);

	printl("%c !!panic!! %s", MAG_CH_PANIC, buf);

	/* should never arrive here */
	__asm__ __volatile__("ud2");
}

void clear()
{

	
	clear_screen(0,console_table[current_console].cursor);
	console_table[current_console].crtc_start = 0;
	console_table[current_console].cursor = 0;
	
}


void help()
{
	printf("=============================================================================\n");
	printf("   Command List  :\n");
	printf("1. process       : A process manage,show you all process-info here\n");
	printf("2. filemng       : Simple file manager\n");
	printf("3. calendar      : Show you the lovely calendar\n");
	printf("4. help          : Show this help message\n");
    printf("5. box           : Play the Box Game 0.0\n");
	printf("6. chess         : Play chess!\n");
	printf("7. maze          : Play a maze game\n");
	printf("8. snake         : Play a greedy eating Snake\n");
	printf("9. calculate     : Use the min calculator\n");
	printf("10. clear        : Clear the screen");
	printf("==============================================================================\n");		
}


void ProcessManage()
{
	int i;
	clear();
	printf("=============================================================================\n");
	printf("============================Process Manager==================================\n");
	printf("=                                                                           =\n");
	printf("===========     ID      |     name       |  spriority    |  runningState  =========\n");
	//进程号，进程名，优先级，是否是系统进程，是否在运行
	printf("=---------------------------------------------------------------------------=\n");
	for ( i = 0 ; i < NR_TASKS + NR_PROCS ; ++i )//逐个遍历
	{
		 if(!selectShowProcess)
		{ if(proc_table[i].p_flags == 0)
		printf("******         %d              %s             %d           %d  ************\n", proc_table[i].pid, proc_table[i].name, proc_table[i].priority,proc_table[i].run_state);}
		else{
		printf("******         %d              %s             %d           %d  ************\n", proc_table[i].pid, proc_table[i].name, proc_table[i].priority,proc_table[i].run_state);
		}	
	}
	printf("=============================================================================\n");
	printf("=>          'pause a/b/c' to pause one process!\n");
	printf("=>          'resume a or b or c' to resume one process!\n");
	printf("=>          'show all process' to show all process!\n");
	printf("=>          'up a/b/c' to up a process's priority!\n");
	printf("=============================================================================\n");
}


void sleep(int pauseTime){
	int i = 0;
	for(i=0;i<pauseTime*1000000;i++){
		;
	}
}


//show the game
void diplaySnakeArea(){
   clear();
   int i;
   for(i=0;i<snake_area_height;i++){
	printf(snake_Array[i]);
   }
}


//start the game

int snake_state = 0;
void StartTheSnake(){

while(snake_head[0] != snake_area_height - 1 && snake_head[1] != snake_area_width- 3 && snake_head[0] != 0 && snake_head[1] != 0){
     
     snake_Array[snake_head[0]][snake_head[1]] = 'o';
        //up
     diplaySnakeArea();
     snake_Array[snake_head[0]][snake_head[1]] = ' ';
     if(move_direction == 1){
	snake_head[0]--;
      }
	//down
     if(move_direction == 2){
	snake_head[0]++;
      }
	//left
     if(move_direction == 3){
	snake_head[1]--;
      }
	//right
     if(move_direction == 4){
	snake_head[1]++;
      }
     if(snake_Array[snake_head[0]][snake_head[1]] == '*') {
	snake_state = 0;
	break;
	}
     if(snake_head[0] == 16 && snake_head[1] == 23){
	snake_state = 1;
	break;
	}
     sleep(1);
}
if(snake_state)  gameSuccessShow();
else gameOverShow();
sleep(9);
clear();
help();
}

void gameOverShow(){
	printf("=======================================================================\n");
	printf("==============================Game Over================================\n");
	printf("=======================will exit in 3 seconds...=======================\n");
}

void gameSuccessShow(){
	printf("=======================================================================\n");
	printf("============================Congratulation!================================\n");
	printf("=======================will exit in 3 seconds...=======================\n");
}

//listener for key press
PUBLIC void judgeInpt(u32 key)
{
        char output[2] = {'\0', '\0'};
        if (!(key & FLAG_EXT)) {
	        output[0] = key & 0xFF;
	        if(output[0] == 'a') changeToLeft();
			if(output[0] == 's') changeToDown();
			if(output[0] == 'd') changeToRight();
			if(output[0] == 'w') changeToUp();
        }
}

int listenerStart = 0;
struct Snake{   //every node of the snake 
	int x, y;  
	int now;   //0,1,2,3 means left right up down   
}Snake[8*16];  //Snake[0] is the head，and the other nodes are recorded in inverted order，eg: Snake[1] is the tail
//change the direction of circle
void changeToLeft(){
	move_direction = 3;
	if(listenerStart == 1){
		Snake[0].now = 0;
		listenerStart = 0;
	}
}
void changeToDown(){
	move_direction = 2;
	if(listenerStart == 1){
		Snake[0].now = 3;
		listenerStart = 0;
	}
}
void changeToRight(){
	move_direction = 4;
	if(listenerStart == 1){
		Snake[0].now = 1;
		listenerStart = 0;
	}
}
void changeToUp(){
	move_direction = 1;
	if(listenerStart == 1){
		Snake[0].now = 2;
		listenerStart = 0;
	}
}
const int mapH = 8;   
const int mapW = 16;
char sHead = '@';    
char sBody = 'O';   
char sFood = '#';    
char sNode = '.';     
char Map[8][16]; 
int food[8][2] = {{4,3},{6, 1}, {2, 0}, {8, 9}, {3, 4}, {1,12}, {0, 2}, {5, 13}}; 
int foodNum = 0;
int eat = -1;
int win = 8;
 
int sLength = 1;
int overOrNot = 0;
int dx[4] = {0, 0, -1, 1};  
int dy[4] = {-1, 1, 0, 0}; 



void gameInit(); 
void food_init();
void show();
void move();
void checkBorder();
void checkHead(int x, int y);
void action();

void game2(){
	clear();
 	gameInit();  
 	show(); 
}

void gameInit()   
{  
	int i, j;  
	int headx = 0;
	int heady = 0;  
 
	memset(Map, '.', sizeof(Map));  //init our map with '.'  
                                                                                     
	Map[headx][heady] = sHead;  
	Snake[0].x = headx;  
	Snake[0].y = heady;  
	Snake[0].now = -1;  

	food_init();   //init target 
	for(i = 0; i < mapH; i++)   
	{   
		for(j = 0; j < mapW; j++)  
		printf("%c", Map[i][j]);  
		printf("\n");  
	} 
	printf("press 'a''s''d''w' key and start the game\n"); 

	listenerStart =1;
	while(listenerStart);
} 

void food_init(){
	int fx, fy;
	int tick;  
	while(1)  
	{  
		tick = get_ticks();
		fx = tick%mapH;
		fy = tick%mapW;		
		if(Map[fx][fy] == '.')  
		{   
			eat++;
			Map[fx][fy] = sFood;  
			break;  
		}
		foodNum ++;
	}
}

void show(){
	int i, j; 
	printf("init done"); 
	while(1)  
	{
		listenerStart = 1;
		if(eat < 4){
			sleep(3);
		}else if(eat < 7){
			sleep(2);
		}else{
			sleep(1);
		}
		
		//while(listenerStart);

		move();  
		if(overOrNot) 
		{   
			printf("===========================================================\n");
			printf("========================Game Over==========================\n");
			printf("=================will exit in 3 seconds...=================\n");
			sleep(9);
			clear();
			help(); 
			break;  
		} 
		if(eat == win)
		{
			printf("===========================================================\n");
			printf("======================Congratulations======================\n");
			printf("=================will exit in 3 seconds...=================\n"); 
 			sleep(9);
			clear();
			help(); 
			break;
		}
		clear();
		for(i = 0; i < mapH; i++)   
		{   
			for(j = 0; j < mapW; j++)  
			printf("%c", Map[i][j]);  
			printf("\n");  
		}  
		printf("Have fun!\n");
		printf("You have ate:%d\n",eat); 
	}  
}
void move(){
	int i, x, y;  
    	int t = sLength;
	x = Snake[0].x;  
	y = Snake[0].y;  
	Snake[0].x = Snake[0].x + dx[Snake[0].now];  //now the Snake[0] is the head in the next step
	Snake[0].y = Snake[0].y + dy[Snake[0].now];  

	Map[x][y] = '.';  //when the snake only have head, it's necessary
	checkBorder(); 
	checkHead(x, y);   
	if(sLength == t)  //did not eat
		for(i = 1; i < sLength; i++)  //from the tail  
		{  
			if(i == 1)   //tail  
				Map[Snake[i].x][Snake[i].y] = '.';  
     
			if(i == sLength-1)  //the node after the head 
			{  
				Snake[i].x = x;  
				Snake[i].y = y;  
				Snake[i].now = Snake[0].now;  
			}  
			else 
			{  
				Snake[i].x = Snake[i+1].x;  
				Snake[i].y = Snake[i+1].y;  
				Snake[i].now = Snake[i+1].now;  
			}  
			Map[Snake[i].x][Snake[i].y] = 'O';  
		}  
}

void checkBorder()
{
	if(Snake[0].x < 0 || Snake[0].x >= mapH || Snake[0].y < 0 || Snake[0].y >= mapW)  
		overOrNot = 1;  
}
void checkHead(int x, int y){
	if(Map[Snake[0].x][Snake[0].y] == '.')
		Map[Snake[0].x][Snake[0].y] = '@';  
	else if(Map[Snake[0].x][Snake[0].y] == '#')
	{  
		Map[Snake[0].x][Snake[0].y] = '@';    
		Snake[sLength].x = x;      //new node 
		Snake[sLength].y = y;  
		Snake[sLength].now = Snake[0].now;  
		Map[Snake[sLength].x][Snake[sLength].y] = 'O';   
		sLength++;  
		food_init();  
	}  
	else{ 
		overOrNot = 1; 
	}
}



void clearScreen()
{
	int i;
	disp_pos=0;
	for(i=0;i<80*25;i++)
	{
		disp_str(" ");
	}
	disp_pos=0;
}

int My_atoi(char* pstr)
{
    int intVal = 0;              // 返回值
    int sign = 1;                // 符号, 正数为 1, 负数为 -1
    if(pstr == 0) return 0;      // 判断指针是否为空 pstr == NULL
    while(' '== *pstr) pstr++;   // 跳过前面的空格字符 ' ' 的 ascii 值 0x20
    if('-'==*pstr) sign = -1;    // 判断正负号
    if('-'==*pstr || '+'==*pstr) pstr++;// 如果是符号, 指针后移
    while(*pstr >= '0' && *pstr <= '9') {// 逐字符转换成整数
        // 转换说明
        // ascii 的 '0' = 0x30 转换为int以后 - 0x30即为整型的0
        // ascii 的 '1' = 0x31 转换为int以后 - 0x30即为整型的1
        // ...
        intVal = intVal * 10 + (((int)*pstr)-0x30);// 十进制即每位乘10, 结果累加保存
        pstr++;// 指针后移
    }
    return intVal * sign;// 返回结果,int32 范围是: 2147483647 ~ -2147483648, 此处会进行溢出运算
     
}


/*======================================================================*
				calculator
*=======================================================================*/

int add_fun(int x,int y)
{
	return x+y;
}
int sub_fun(int x,int y)
{
	return x-y;
}  
int mul_fun(int x,int y)
{
	return x*y;
}  
int div_fun(int x,int y)
{
	return x/y;
} 

	/*calendar*/
	int duo(int year)
	{
		if(year%4==0&&year%100!=0||year%400==0)
			return 1;
		else
			return 0;
	}

	void print(int day,int tian)
	{
		int a[20][20],i,j,sum=1;
		for(i=0,j=0;j<7;j++)
		{
			if(j<day)
				printf("    ");
			else
			{
				a[i][j]=sum;
				printf("   %d",sum++);
			}
		}
		printf("\n");
		for(i=1;sum<=tian;i++)
		{
			for(j=0;sum<=tian&&j<7;j++)
			{
				a[i][j]=sum;
				if (sum<10)
				{
					printf("   %d", sum++);
				}
				else{
					printf("  %d",sum++);
				}
			}
			printf("\n");
		}
	}

	int calendar_control(int year,int month)
	{
		int day,tian,preday,strday;
		printf("<==** %dmonth   %dyear **==>\n",month,year);
		printf(" SUN MON TUE WED THU FRI SAT\n");
		switch(month)
		{
			case 1:
				tian=31;
				preday=0;
				break;
			case 2:
				tian=28;
				preday=31;
				break;
			case 3:
				tian=31;
				preday=59;
				break;
			case 4:
				tian=30;
				preday=90;
				break;
			case 5:
				tian=31;
				preday=120;
				break;
			case 6:
				tian=30;
				preday=151;
				break;
			case 7:
				tian=31;
				preday=181;
				break;
			case 8:
				tian=31;
				preday=212;
				break;
			case 9:
				tian=30;
				preday=243;
				break;
			case 10:
				tian=31;
				preday=273;
				break;
			case 11:
				tian=30;
				preday=304;
				break;
			default:
				tian=31;
				preday=334;
		}
		if(duo(year)&&month>2)
		preday++;
		if(duo(year)&&month==2)
		tian=29;
		day=((year-1)*365+(year-1)/4-(year-1)/100+(year-1)/400+preday+1)%7;
		print(day,tian);
	}





//Box function



void DrawMap(char BoxMap[][COLUS]) 
{
	int i=0;
	for (i; i<LINES; i++) printf("%s\n", BoxMap[i]);
}

void swap(char BoxMap[][COLUS], int *x1, int *y1, int *x2, int *y2)
{
	char temp;
	temp = BoxMap[*x1][*y1];
	BoxMap[*x1][*y1] = BoxMap[*x2][*y2];
	BoxMap[*x2][*y2] = temp;
	*x1 = *x2;
	*y1 = *y2;
}
int isLose(char BoxMap[][COLUS], int boxX, int boxY, char deadquater)
{
	//printf("(%d,%d)", boxX, boxY);
	if ((BoxMap[boxX + 1][boxY] == deadquater&&BoxMap[boxX][boxY + 1] == deadquater)
		|| (BoxMap[boxX - 1][boxY] == deadquater&&BoxMap[boxX][boxY + 1] == deadquater)
		|| (BoxMap[boxX - 1][boxY] == deadquater&&BoxMap[boxX][boxY - 1] == deadquater)
		|| (BoxMap[boxX + 1][boxY] == deadquater&& BoxMap[boxX][boxY - 1] == deadquater)
		) return 1;
	return 0;
}

void playerChoice(int *nextpersonX, int *nextpersonY, int
	*nextboxX, int* nextboxY,char direction)
{
	//printf("请输入一个方向 [w,a,s,d]分别代表[上下左右]\n\n");
	//scanf(" %c", &direction);
	switch (direction) {
	case 'w':
	case 'W':
		(*nextpersonX)--;
		(*nextboxX)--;
		break;
	case 'a':
	case 'A':
		(*nextpersonY)--;
		(*nextboxY)--;
		break;
	case 's':
	case 'S':
		(*nextpersonX)++;
		(*nextboxX)++;
		break;
	case 'D':
	case 'd':
		(*nextpersonY)++;
		(*nextboxY)++;
		break;
	default:
		printf("\n!!!!Input Error, Try Again:!!!!\n");
		break;
	}
}

void FileSys()
{
	char tty_name[] = "/dev_tty1";
	int fd_stdin  = open(tty_name, O_RDWR);
	assert(fd_stdin  == 0);
	int fd_stdout = open(tty_name, O_RDWR);
	assert(fd_stdout == 1);
	//printf("%d %d\n",fd_stdin,fd_stdout);

	printf("Orders of FileSystem:\n");
	printf("create    write    read     delete    help\n");
	printf("Press SHIFT+F1 to Quit.\n \n");

	while (1) {
		char way[4];//way of writing
		char order[8];//buffer for order
		char filename[128];//buffer for filename
		
		//buffer for input
		char content[128];
		char bufr[128];
		char bufw[128];
		char bufm[128];

		printf("FileSystem:$ ");
		int r = read(fd_stdin, order, 8);
		order[r] = 0;
		
		//create file
		if (strcmp(order, "create") == 0){
			printf("Input Filename> ");
			int r = read(fd_stdin, filename, 70);
			filename[r] = 0;
			
			//[EOF] is the end signal of hd,it can't be changed 
			if(strcmp(order, "EOF") == 0)
			{
				printf("[EOF] File cannot be created!!!\n");
			}
			else{
			int fd = open(filename, O_CREAT | O_RDWR);
			if(fd != -1)
				{printf("File created: %s\n", filename);
				close(fd);}
			else
				printf("File have existed\n");
			}
			printf("\n");
		}
		//read file
		else if(strcmp(order, "read") == 0){	 
			printf("Input Filename> ");
			int r = read(fd_stdin, filename, 70);
			filename[r] = 0;
			if(filename[0] != '\0')
			{
				int fd = open(filename, O_RDWR);
				//tty file cann't be changed
				if(strcmp(order, "dev_tty0") == 0||strcmp(order, "dev_tty1") == 0)
				{
					fd=-1;
					printf("System File cannot be changed\n");	
				}
				if(fd!=-1){
					int n = read(fd, bufr, 70);
					bufr[n] = 0;
					if(bufr[0]!='\0')
						printf("Content: %s\n",bufr);
					close(fd);
				}
				else
					printf("File not exists\n");	
			}
			printf("\n");
		}
		//write file
		else if(strcmp(order, "write") == 0){

			//choose how to write file, rewrite or append?
			printf("Choose how to write (write or append) [w] or [a] > ");
			r = read(fd_stdin, way, 128);
			way[r] = 0;

			printf("Input Filename> ");
			int r = read(fd_stdin, filename, 128);
			filename[r] = 0;
			if(filename[0] != '\0')
			{
				int fd = open(filename, O_RDWR);
				if(strcmp(order, "dev_tty0") == 0||strcmp(order, "dev_tty1") == 0)
				{
					fd=-1;
					printf("System File cannot be changed\n");	
				}
				if(fd!=-1){
					printf("Input New Content> ");
					r=read(fd_stdin, bufw, 128);
					bufw[r]=0;
					if(strcmp(way, "a") == 0){
						read(fd, bufr, 128);
						close(fd);
						int oldlen=0;
						while(bufr[oldlen]!='\0')
							oldlen++;
				
						int ir=0;
						while(bufw[ir]!='\0')
						{
							bufr[oldlen+ir]=bufw[ir];
							ir++;
						}
						bufr[oldlen+ir]='\0';
						fd = open(filename, O_RDWR);
						printf("new content: %s\n",bufr);
						int n=write(fd, bufr, 128);
					}
					else{
						bufw[r]='\0';
						int n=write(fd, bufw, 128);
						printf("new content: %s\n",bufw);
					}
					close(fd);
				}
				else
					printf("File not exists\n");
				
			}
			printf("\n");	
		}
		//delete file
		else if(strcmp(order, "delete") == 0){
			printf("Input Filename> ");
			int r = read(fd_stdin, filename, 128);
			filename[r] = 0;
			//if the file is being used or system file, cannot be removed
			if (unlink(filename) == 0)
				printf("File removed: %s\n", filename);
			else
				printf("Failed to remove file: %s\n", filename);
			printf("\n");		
		}
		//print help informations
		else if(strcmp(order, "help") == 0){
			printf("Orders of FileSystem:\n");
			printf("create    write    read     delete    help\n");
			printf("Press SHIFT+F1 to Quit.\n \n");
		}
		else
			printf("Undefined Order\n \n");
	}
	assert(0); /* never arrive here */
}
