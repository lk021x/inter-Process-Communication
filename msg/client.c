#include "sys/types.h"
#include "sys/msg.h"
#include "signal.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

struct msgbuf {
    long type;
    char voltage[124];
    char ID[4];
};

int main() {
    int msgid;
    struct msgbuf sendbuf;
    struct msgbuf recvbuf;
    int readret;
    int pid;
    int key;
    key = ftok("./b.c", 'c');
    if (key < 0) {
        printf("creat key failure\n");
        return -1;
    }
    msgid = msgget(key,IPC_CREAT | 0777);
    if (msgid < 0) {
        printf("create message queue failure\n");
        return -1;
    }
    printf("create message queue success, msgid = %d\n", msgid);
    system("ipcs -q");
       
    // 在消息队列创建之后，使用 fork 函数
    pid = fork();
    // parent process read type:100 Data
    if (pid > 0) {
        while (1) {
            memset(recvbuf.voltage, 0, 124);
            msgrcv(msgid, (void *)&recvbuf, 124, 100, 0);
            printf("receive data from message queue: %s", recvbuf.voltage); 
            // 可以打印输出后，当前行仍然提示可以输入数据，而不是没有提示
            printf("please input message:\n");
        }   
    } 
    // child process write type:200 Data
    if (pid == 0) {
        sendbuf.type = 200;
        while (1) {
            memset(sendbuf.voltage, 0, 124);
            printf("please input message:\n");
            fgets(sendbuf.voltage, 124, stdin);
            msgsnd(msgid, (void *)&sendbuf, strlen(sendbuf.voltage), 0);
        }
    }
 
    // delete message queue 
    msgctl(msgid, IPC_RMID, NULL);
    system("ipcs -q");
    
    return 0;
}