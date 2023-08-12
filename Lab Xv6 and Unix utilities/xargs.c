#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[])
{
    // 如果参数个数小于 2
    if (argc < 2) {
        // 打印参数错误提示
        fprintf(2, "usage: xargs command\n");
        // 异常退出
        exit(1);
    }
    // 存放子进程 exec 的参数
    char *argvs[MAXARG];
    // 索引
    int index = 0;
    // 略去 xargs ，用来保存命令行参数
    for (int i = 1; i < argc; ++i) {
        argvs[index++] = argv[i];
    }
    // 缓冲区存放从管道读出的数据
    char buf[1024] = {"\0"};
    
    int n;
    // 0 代表的是管道的 0，也就是从管道循环读取数据
    while((n = read(0, buf, 1024)) > 0 ) {
        // 计数器
        int count = 0;
        // 内循环获取追加的参数并创建子进程执行命令
        for(int i = 0; i < n; ++i) {
            // 如果是空格或换行符，代表一个参数结束，执行命令
            if (buf[i] == ' ' || buf[i] == '\n') {
                argvs[index] = buf + i - count;
                buf[i] = '\0'; // 将空格或者换行符改为字符串结束符
                count = 0; // 重置计数器
                // 创建子线程执行命令
                if (fork() == 0) {
                    exec(argv[1], argvs);
                }
                // 等待子线程执行完毕
                wait(0);
            } else {
                count++;
            }
        }
    }
    // 正常退出
    exit(0);
}