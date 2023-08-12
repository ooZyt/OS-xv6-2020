#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WT 1
void primes(int p1[],int p2[])
{   
   if(fork()==0)//利用fork创建子进程
   {
      int num[1];	
      if(read(p1[RD],num,sizeof(num))!=sizeof(num))
      {
	       close(p1[RD]);
          close(p2[RD]);
          close(p2[WT]);	  
	       exit(0);
      }
      int p=num[0];
      printf("prime %d\n",num[0]);
     
      while(read(p1[RD],num,sizeof(num))==sizeof(num))
      {
           if(num[0]%p!=0) write(p2[WT],num,sizeof(num));
      }
      
      close(p1[RD]);	
      close(p2[WT]);
   
      pipe(p1);
      primes(p2,p1);
      exit(0);      
   }
   else
   {
      close(p1[RD]);
      close(p2[WT]);
      close(p2[RD]);      
      wait(0);// wait child process to end
      exit(0);
   }
}

int main(int argc,char* argv[])
{
   //创建两个管道
   int p1[1];
   int p2[2];
   pipe(p1);
   pipe(p2);
   //循环写入2-35数字
   int i[1];
   for(i[0]=2;i[0]<=35;i[0]++)
   {
      write(p1[WT],i,sizeof(i));
   }
   close(p1[WT]);
   //调用prime函数判断素数
   primes(p1,p2);
   //退出程序
   exit(0);
}