# README

运行程序时，使用make命令，生成sever、client、exitserver三个可执行文件。

server为服务器端，client为客户端，可运行一个服务器端、多个客户端程序。

服务器端输入-1即可退出，客户端在进行群聊或者私聊时，输出`exit`将退出群聊或者私聊

若想将server成为守护进程，在server.c文件中将以下daemonzie()之前的注释删除。退出时运行exitserver程序即可

![image-20220610183129470](https://i0.hdslb.com/bfs/album/5e1344771128028e20e7a1c92f12f51f112d042c.png)