/*
 * @Descripttion: 
 * @Author: DLCT
 * @Date: 2022-06-10 15:14:46
 * @e-mail: 18109232165@163.com
 * @LastEditors: DLCT
 * @LastEditTime: 2022-06-10 15:17:59
 */
#include"head.h"

int main()
{
    qid = msgget(ftok("/", 'a'), IPC_CREAT | 0666);
    struct msg_key msgk;
    //向服务器发送消息，并返回服务器端pid，发送信号
    msgk.mtype = 3;
    msgsnd(qid, &msgk, sizeof(struct msg_key), 0);
    msgrcv(qid, &msgk, sizeof(struct msg_key), 4, 0);
    kill(msgk.number, SIGINT);
}