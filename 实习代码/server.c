/*************************************************************************
    > File Name: server.c
    > Author: DLCT
    > Mail: 18109232165@163.com
    > Created Time: Mon Jun  6 08:37:45 2022
 ************************************************************************/
/*
 *	server 发送的所有信息mtype=1
 *	客户端 发送的所有信息mtype=0
 * */
#include "head.h"

void daemonize();    //创建守护进程
void creatmmap();    //创建内存映射，用于子父进程之间通信
void userlogin();    //接受用户登陆的消息
void message(int t); //接受消息
void signal_int();  //捕获SIGINT信号

int len;             //用于存放per数组的字节长度
struct person *per;  //用于存放用户信息
int number_per;      //用于记录用户个数

int main()
{
    //生成消息队列
    qid = msgget(ftok("/", 'a'), IPC_CREAT | 0666);
    
    //初始化number_per从10开始
    number_per = 10;

    //捕获SIGINT信号
    signal(SIGINT, signal_int);
    
    //成为守护进程
    //	daemonize();    
    //创建共享内存
    creatmmap();

    //第一个子进程，用来创建用户
    pid = fork();
    if (pid < 0)
    {
        perror("进程创建失败");
        exit(1);
    }
    if (pid == 0)
    {
        printf("开始监听\n");
        userlogin();
    }

    //父进程用来监听退出
    int i;
    while (1)
    {
        //收到-1
        scanf("%d", &i);
        if (i == -1)
        {
            kill(pid, SIGINT);
            int j = 0;
            for (; j < 105; j++)
            {
                //向所有子进程发送SIGKILL
                if (per[j].has_log == 1)
                    kill(per[j].pid, SIGINT);
            }
            break;
        }
    }
    memset(per, 0, sizeof(per));
    //断开共享内存链接
    munmap((void *)per, len);
    return 0;
}

void signal_int()
{
    //是子进程退出
    if (pid == 0)
    {
        //断开共享内存
        munmap((void *)per, len);
        exit(0);
    }
    //父进程向子进程发送信号后退出
    kill(pid, SIGINT);
    //断开共享内存
    munmap((void *)per, len);
    exit(0);
}

void daemonize()    //cheng'wei
{
    if (pid = fork() < 0)
    {
        perror("fork");
        exit(1);
    }
    else if (pid != 0)
        exit(0);
    if (chdir("/") < 0)
    {
        perror("chdir");
        exit(1);
    }
    close(0);
    open("/dev/null", O_RDWR);
    dup2(0, 1);
    dup2(0, 2);
}

void creatmmap()        //创建共享内存
{
    //打开文件
    int fd = open("mmap.dat", O_CREAT | O_RDWR, 0666);
    if (fd == -1)
    {
        perror("文件打开失败");
        exit(1);
    }
    //获取想要的长度
    len = sizeof(struct person) * 105;
    //文件长度设置
    ftruncate(fd, len);
    //共享内存
    per = (struct person *)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (per == MAP_FAILED)
        perror("mmap file");
    int i = 0;
    //设置状态位都为0
    for (; i < 105; i++)
    {
        per[i].has_log = 0;
        per[i].has_group = 0;
    }
    //关闭文件描述符
    close(fd);
}

void userlogin()
{
    
    struct msg_key msg;
    int i;

    while (1)
    {
        //只接受mtype = 0的消息
        //若有登陆请求
        msgrcv(qid, &msg, sizeof(struct msg_key), 1, 0);
        printf("新建用户 %s\n", msg.name);

        //设置第number_per用户的intype和outtype
        per[number_per].intype = number_per;
        per[number_per].outtype = number_per + 200;
        per[number_per].has_log = 1;
        
        //返回对应in/outtype，mtype=2
        msg.intype = number_per + 200;
        msg.outtype = number_per;
        strcpy(per[number_per].name, msg.name);
        printf("%s的intype = %ld, outtype = %ld\n", msg.name, msg.intype, msg.outtype);
        msg.mtype = 2;
        
        msgsnd(qid, &msg, sizeof(struct msg_key), 0);

        //返回消息，创建一个子进程监听消息
        pid = fork();
        if (pid == 0)
        {
            message(number_per);
            break;
        }
        //记录pid
        per[number_per].pid = pid;
        number_per++;
    }
}

void message(int t)
{
    struct msg_buf msg;
    strcpy(msg.msg_name, per[t].name);
    int i;
    while (1)
    {

        while (1)
        {
            //监听消息
            msgrcv(qid, &msg, sizeof(struct msg_buf), per[t].intype, 0);
            printf("收到 %s 的消息:%s\n messagetype=%d\n", msg.msg_name, msg.msg_text, msg.message_type);

            if (msg.message_type == 1)
            {
                // msg.message_type == 1, 进入聊天室
                for (i = 10; i < 20; i++)
                {
                    if (per[i].has_group == 1)
                    {
                        msg.mtype = per[i].outtype;
                        msgsnd(qid, &msg, sizeof(struct msg_buf), 0);
                    }
                }
                per[t].has_group = 1;
            }
            else if (msg.message_type == 2)
            {
                // msg.message_type == 2, 普通消息进行广播消息
                for (i = 10; i < 20; i++)
                {
                    if (per[i].has_group == 1 && i != t)
                    {
                        msg.mtype = per[i].outtype;
                        msgsnd(qid, &msg, sizeof(struct msg_buf), 0);
                        printf("%s 为 %s 发送成功\n", msg.msg_name, per[i].name);
                    }
                }
                memset(msg.msg_name, 0, strlen(msg.msg_name));
                memset(msg.msg_text, 0, strlen(msg.msg_text));
            }
            else if (msg.message_type == 3)
            {
                // msg.message_type == 3, 退出聊天室
                for (i = 10; i < 20; i++)
                {
                    if (per[i].has_group == 1 && i != t)
                    {
                        msg.mtype = per[i].outtype;
                        msgsnd(qid, &msg, sizeof(struct msg_buf), 0);
                    }
                }
                per[t].has_group = 0;
                continue;
            }
            else if (msg.message_type == 4)
            {
                // msg.message_type == 4, 退出程序
                per[t].has_log = 0;

                kill(per[t].pid, SIGINT);
                exit(0);
            }
            else if (msg.message_type == 5)
            {
                //请求私聊，发送所有人的信息
				int i;
                struct msg_key msgk;
				msgk.mtype = per[t].outtype;
				int number = 0;
                //查找在线人数
                for (i = 10; i < 20; i++){
                    if (per[i].has_log == 1){
                        number++;
					}
				}
                msgk.number = 1;
                printf("%d\n", number);
                int s = 0;
				number--;
                for (i = 10; i < 20; i++)
                {
                    if (per[i].has_log == 1 && i != t)
                    {
                        s++;
                        msgk.intype = per[i].intype;
                        msgk.outtype = per[i].outtype;
                        strcpy(msgk.name, per[i].name);
                        if (s == number)
                            msgk.number = 0;
                        msgsnd(qid, &msgk, sizeof(struct msg_key), 0);
                        printf("%s 信息已发送 %ld %d\n", msgk.name, msgk.mtype, msgk.number);
                    }
                }
            }
        }
    }
}
