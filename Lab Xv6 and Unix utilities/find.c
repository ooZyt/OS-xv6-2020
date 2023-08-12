#include "kernel/types.h"        // 引入系统类型定义
#include "kernel/stat.h"         // 引入文件状态定义
#include "user/user.h"          // 引入用户调用定义
#include "kernel/fs.h"          // 引入文件系统定义

void match(const char* path, const char* name){
    // 匹配当前路径和文件名
    int pp = 0;             // 当前路径指针
    int pa = 0;             // 当前文件名指针
    while(path[pp] != 0){
        pa = 0;
        int np = pp;
        // 尝试匹配文件名和路径
        while(name[pa] != 0){
            if (name[pa] == path[np]){
                pa++;
                np++;
            }
            else
                break;
        }
        // 如果文件名匹配成功，输出路径
        if(name[pa] == 0){
            printf("%s\n", path);
            return;
        }
        pp++;
    }
}

void find(char *path, char *name){
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // 打开目录路径
    if((fd = open(path, 0)) < 0){
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    // 获取文件状态信息
    if(fstat(fd, &st) < 0){
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // 根据文件类型进行处理
    switch(st.type){
        case T_FILE:
            // 如果当前为文件类型，判断是否匹配
            match(path, name);
            break;

        case T_DIR:
            // 如果当前为目录类型
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("ls: path too long\n");
                break;
            }
            // 构建下一级路径，并递归调用find函数
            strcpy(buf, path);
            p = buf+strlen(buf);
            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                if(de.inum == 0)
                    continue;
                // 跳过当前目录和上级目录
                if(de.name[0] == '.' && de.name[1] == 0) continue;
                if(de.name[0] == '.' && de.name[1] == '.' && de.name[2] == 0) continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                // 获取下级文件或目录的状态信息
                if(stat(buf, &st) < 0){
                    printf("ls: cannot stat %s\n", buf);
                    continue;
                }
                // 递归调用find函数
                find(buf, name);
            }
            break;
    }
    // 关闭文件
    close(fd);
}

int main(int argc, char *argv[]){
    // 判断命令行参数
    if (argc < 3){
        printf("Usage: find [path] [filename]\n");
        exit(-1);
    }
    // 调用find函数查找符合条件的文件路径
    find(argv[1], argv[2]);
    // 退出进程
    exit(0);
}