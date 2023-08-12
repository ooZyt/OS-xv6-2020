#include "kernel/types.h"
#include "user/user.h"


int main(int argc, char *argv[])
{
    int parent_fd[2], child_fd[2];
    pipe(parent_fd);  //用于父进程将消息传递给子进程
    pipe(child_fd);  //用于子进程传递消息给父进程
    char buf[64];  //缓冲区存放所传递的信息
    if (fork() == 0) {
       //Chlid
        read(parent_fd[0], buf, 4);
        printf("%d: received %s\n", getpid(), buf);
        write(child_fd[1], "pong", strlen("pong"));
    }
    else {
        //Parent
        write(parent_fd[1], "ping", strlen("ping"));
        read(child_fd[0], buf, 4);
        printf("%d: received %s\n", getpid(), buf);
    }
    exit(0);
}