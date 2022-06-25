/*
 * @Descripttion: 
 * @Author: DLCT
 * @Date: 2022-06-07 18:26:55
 * @e-mail: 18109232165@163.com
 * @LastEditors: DLCT
 * @LastEditTime: 2022-06-10 14:48:17
 */
#include<time.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/mman.h>
#include<fcntl.h>
int qid;
int pid;

//个人信息
struct person{
	char name[100];		//客户端姓名
	long intype;		//接受客户端信息使用的mtype
	long outtype;		//向客户端发送信息使用的mtype
	int has_group;		//1：客户端正在聊天室，0：客户端不在聊天室
	int has_log;		//1：客户端已登陆，0：客户端未登录
	pid_t pid;			//用于存放监听客户端消息的子进程的pid
}; 

//消息结构体
struct msg_buf{
	long mtype;				
	char msg_text[512];		//消息内容
	char msg_name[100];		//发送消息人的姓名
	char msg_time[100];		//发送消息的时间
	int message_type;		//消息类型，1.进入聊天室消息，2.普通消息，3.退出聊天室，4.退出客户端，5.获取用户信息
};

//登陆信息结构体
struct msg_key{	
	long mtype;				
	char name[100];			//客户端名称
	long intype;			//客户端输入mtype
	long outtype;			//客户端输出mtype
	int number;				//客户端序号
};

