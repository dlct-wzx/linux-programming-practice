/*************************************************************************
    > File Name: client.c
    > Author: DLCT
    > Mail: 18109232165@163.com
    > Created Time: Mon Jun  6 10:29:18 2022
 ************************************************************************/
/*
 * 客户端向服务器端请求 mtype = 1
 * 服务器端向客户端返回指定mtype时，mtype = 0
 * 客户端退出时 mtype = 3
 * */

#include "head.h"

char myname[100];

int oper;
pid_t pid;
struct msg_buf msgb;
struct msg_key msgk;
long intype;
long outtype;
void chatgroup();
void privatechat();
void privatelisten();
//捕获信号
void ctrl_c{
    if(pid == 0)
        exit(0);
    kill(pid, SIGINT);
    exit(0);
}

int main()
{
    //捕获SIGINT信号
    signal(SIGINT, ctrl_c);
    system("clear");
    //获取qid
    qid = msgget(ftok("/", 'a'), IPC_CREAT | 0666);

    //初始化数据
    printf("请输入你的名字：");
    scanf("%s", msgk.name);
    msgk.mtype = 1;

    //向服务器发送请求
    msgsnd(qid, &msgk, sizeof(struct msg_key), 0);
    //服务器相应请求
    msgrcv(qid, &msgk, sizeof(struct msg_key), 2, 0);
    //设置发送与接受的mtype

    intype = msgk.intype;
    outtype = msgk.outtype;
    msgb.mtype = outtype;
    strcpy(msgb.msg_name, msgk.name);

    // printf("in = %ld, out = %ld\n", intype, outtype);
    while (1)
    {
        system("clear");
        printf("\n\n\t\t*********************************************\t\t\n");
        printf("\t\t\t\t单主机聊天室\n");
        printf("\t\t*********************************************\t\t\n");
        printf("\n\n\n\t\t\t1.聊天室.");
        printf("\n\t\t\t2.私聊.");
        printf("\n\t\t\t0.退出程序.");
        printf("\n\n\t\t\t\t选择所要操作:");
        scanf("%d", &oper);
        if (oper == 1)
        {
            system("clear");
            printf("\n\n\t\t*********************************************\t\t\n");
            printf("\t\t\t\t欢迎来到集体聊天室\n");
            printf("\t\t*********************************************\t\t\n");
            pid = fork();
            //创建子进程进入聊天室，父进程挂起
            if(pid == 0)
                chatgroup();
            wait();
        }
        else if (oper == 2)
        {
            system("clear");
            printf("\n\n\t\t*********************************************\t\t\n");
            printf("\t\t\t\t请选择私聊对象\n");
            printf("\t\t*********************************************\t\t\n");
            pid = fork();
            //出啊昂见子进程进入私聊，父进程挂起
            if (pid == 0)
                privatechat();
            wait();
        }
        else if (oper == 0)
        {
            //发送退出客户端消息
            msgb.message_type = 4;
            msgsnd(qid, &msgb, sizeof(struct msg_buf), 0);

            exit(0);
        }
        else
        {
            printf("\n\n\t\t\t\terror:操作码错误，请重新输入\n");
            printf("\n\n\t\t\t\t选择所要操作:");
        }
    }
}

void chatgroup()
{

    time_t t;
    pid = fork();
    //子进程监听消息
    if (pid == 0)
    {
        //子进程监听消息
        while (1)
        {
            msgrcv(qid, &msgb, sizeof(struct msg_buf), intype, 0);
            if (msgb.message_type == 1)
            {
                //message_type=1，登录消息
                printf("%s\n**********用户 %s 加入聊天室**********\n", msgb.msg_time, msgb.msg_name);
                continue;
            }
            else if (msgb.message_type == 3)
            {
                //message_type=3，退出消息
                printf("%s\n**********用户 %s 退出聊天室**********\n", msgb.msg_time, msgb.msg_name);
                continue;
            }
            else
            {
                //普通消息
                printf("%s%s: %s\n\n", msgb.msg_time, msgb.msg_name, msgb.msg_text);
                memset(msgb.msg_name, 0, strlen(msgb.msg_name));
                memset(msgb.msg_text, 0, strlen(msgb.msg_text));
            }
        }
    }
    else //父进程发送消息
    {
        //设置消息结构体的内容,加入聊天室

        time(&t);
        //发送进入聊天室消息
        msgb.message_type = 1;
        ctime_r(&t, msgb.msg_time);
        msgsnd(qid, &msgb, sizeof(struct msg_buf), 0);

        getchar();
        while (1)
        {
            //读取输入，并获取时间
            gets(msgb.msg_text);
            time(&t);
            ctime_r(&t, msgb.msg_time);
            //若输入 “exit”
            if (strcmp(msgb.msg_text, "exit") == 0)
            {
                kill(pid, SIGINT); //子进程向子进程发送信号
                memset(msgb.msg_text, 0, strlen(msgb.msg_text));
                msgb.message_type = 3;
                //发送退出聊天室消息
                msgsnd(qid, &msgb, sizeof(struct msg_buf), 0);
                exit(0);
            }
            //发送普通消息
            msgb.message_type = 2;
            msgsnd(qid, &msgb, sizeof(struct msg_buf), 0);
            memset(msgb.msg_text, 0, strlen(msgb.msg_text));
        }
    }
}

void privatechat()
{
    pid = fork();
    if (pid != 0)
    {
        //子进程
        sleep(1);
        //接受消息
        msgrcv(qid, &msgk, sizeof(struct msg_key), intype, 0);
        //向父进程发消息，使其退出
        kill(pid, SIGINT);
        
        system("clear");
        printf("\n\n\t\t*********************************************\t\t\n");
        printf("\t\t\t\t欢迎来到私聊室\n");
        printf("\t\t*********************************************\t\t\n");
        //从消息中获取输出mtype
        msgb.mtype = msgk.outtype;
        pid = fork();
        if (pid == 0)
        {
            //子进程监听消息
            while (1)
            {
                msgrcv(qid, &msgb, sizeof(struct msg_buf), intype, 0);
                if (strcmp(msgb.msg_text, "exit") == 0)
                {
                    //退出
                    kill(getppid(), SIGINT);
                    exit(0);
                }
                printf("%s%s: %s\n\n", msgb.msg_time, msgb.msg_name, msgb.msg_text);
                memset(msgb.msg_name, 0, strlen(msgb.msg_name));
                memset(msgb.msg_text, 0, strlen(msgb.msg_text));
            }
        }
        time_t t;
        while (1)
        {
            //父进程发送消息
            scanf("%s", msgb.msg_text);
            ctime_r(&t, msgb.msg_time);
            msgsnd(qid, &msgb, sizeof(struct msg_buf), 0);
            if (strcmp(msgb.msg_text, "exit") == 0)
            {
                kill(pid, SIGINT);
                exit(0);
            }
        }
    }
    int i;
    int t;
    struct person per[10];
    //发送消息请求用户信息
    msgb.message_type = 5;
    msgsnd(qid, &msgb, sizeof(struct msg_buf), 0);
    printf("\n\n");
    for (i = 1;; i++)
    {
        //展示用户信息
        msgrcv(qid, &msgk, sizeof(struct msg_key), intype, 0);
        per[i].intype = msgk.intype;
        per[i].outtype = msgk.outtype;
        strcpy(per[i].name, msgk.name);
        printf("\n\t\t\t%d. %s", i, per[i].name);
        //退出
        if (msgk.number == 0)
            break;
    }
    printf("\n\t\t\t0.退出私聊.");
    printf("\n\n\t\t\t\t选择所要操作:");
    scanf("%d", &t);
    if (t == 0)
        return;
    else
    {
        //向目标用户和父进程发送消息
        msgk.mtype = per[t].outtype;
        msgk.outtype = intype;

        msgsnd(qid, &msgk, sizeof(struct msg_key), 0);
        msgk.mtype = intype;
        msgk.outtype = per[t].outtype;

        msgsnd(qid, &msgk, sizeof(struct msg_key), 0);
        sleep(1);
    }
}
