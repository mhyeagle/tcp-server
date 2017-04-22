//下面是示例代码：
//代码是服务器TCP模型，采用多路复用的select函数实现了循环的监听并接受客户端的功能，其中也包含了上传下载的功能*/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/ioctl.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/types.h>
#include<dirent.h>
 
int main()
{
    struct sockaddr_in seraddr,cliaddr;
    int listenfd,connfd,fd1,fd2,n,m,l,port;
    char user[20],buf[4096];
    fd_set  readfds,tmpfds;//岗哨监控集合
    socklen_t addrlen;
    DIR *dr;
    struct dirent *file;
 
    printf("请输入需要设定的服务器名称:");
    scanf("%s",user);
     
    printf("请输入需要设定的服务器端口:");
    scanf("%d",&port);
    getchar();
 
    if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0)
    {
        perror("创建失败");
       exit(-1);
    }
     
     
 
    /*开始设定服务器的参数地址类型,IP,PORT*/
    memset(&seraddr,0,sizeof(seraddr));//将服务器的初值空间清空，防止转化过程有影响
    seraddr.sin_family=AF_INET;
    seraddr.sin_port=htons(port);//将得到的本地端口转换为网络字节序
    seraddr.sin_addr.s_addr=htonl(INADDR_ANY);//将得到的ip地址字符串转换为网络字节序的ip地址数值
 
    if((bind(listenfd,(struct sockaddr*)&seraddr,sizeof(seraddr))<0))
    {
        perror("绑定失败");
       exit(-1);
    }
    printf("绑定创建\n");
    if((connfd=listen(listenfd,50))<0)
    {
        perror("监听失败");
        exit(-1);
    }
    printf("开始监听\n");
    FD_ZERO(&readfds);//初始化文件集
    FD_SET(listenfd,&readfds);//将需要监视的listenfd放入readfds集中
 
    while(1)//循环监听
    {
        int nread,n;
        tmpfds=readfds;//将监视集传递给临时的监视集中，防止后续的操作修改了监视集
        if(select(FD_SETSIZE,&tmpfds,NULL,NULL,NULL)<0)//设置监视，监视tmpfds内的子fd变化,发生变化的将会被保留在tmpfds中
          {
           perror("监视未成功");
            exit(-1);
         }
         
         for(fd1=0;fd1<FD_SETSIZE;fd1++)//循环找在最大范围内的fd1
        {
           if(FD_ISSET(fd1,&tmpfds))//查找是否fd在tmpfds里面
            {
              if(fd1==listenfd)//判定fd等于监听fd，即监听fd在监视过程中出现变化被发现
              {
                  addrlen=sizeof(cliaddr);
                 connfd=accept(listenfd,(struct sockaddr*)&cliaddr,&addrlen);//开始接收客户
                 FD_SET(connfd,&readfds);//将connfd加入监视集，监视接入的变化
                 printf("接入新的连接\n");
              }
              else
                  {
                      ioctl(fd1,FIONREAD,&nread);//测试在fd中还有nread个字符需要读取
                      if(nread==0)//如果需要读取的数据为0,则关闭检测出来的fd1，并且从监视中移除
                    {
                     close(fd1);
                     FD_CLR(fd1,&readfds);
                      printf("移除\n");
                    }
                      else//如果可读数据不为0，则读出来
                    {
                        int i;
                        char *p=NULL,*q=NULL;
                      n=read(fd1,buf,nread);
                      buf[n]=0;
                      p=buf;
                       
                       
                          if((strncmp(p,"-get",4)==0))
                          {  
                             q=p+5;
                             printf("客户下载文件>%s",q);
                             if((fd2=open(q,O_RDONLY))<0)
                                 perror("打开文件错误");
                             
                             while((m=read(fd2,buf,4096))>0)
                             {
                                 write(connfd,buf,m);
                                  
                             }
                             bzero(buf,sizeof(buf));
                             close(fd1);
                             close(fd2);
                             FD_CLR(fd1,&readfds);
                           
                          }
                           
                          if((strncmp(p,"-up",3)==0))
                          {  
                             q=p+4;
                             printf("客户上传文件%s\n",buf+4);
                             if((fd2=open(q,O_CREAT | O_WRONLY | O_APPEND ,0666))<0)
                             {
                                perror("打开文件写入失败");
                                  
                             }
                              
                             while((m=read(connfd,buf,128))>0)
                             { 
                                 printf("%s",buf);
                                 write(fd2,buf,m);
                             }
                             bzero(buf,sizeof(buf));
                             close(fd1);
                             close(fd2);
                             FD_CLR(fd1,&readfds);
                           
                          }
                           
                          if((strncmp(p,"-ls",3)==0))
                          {  
                             q=p+4;
                             printf("客户查看文件……");
                             if((dr=opendir(q))==NULL)
                                 perror("打开目录失败");
                             while((file=readdir(dr))!=NULL)
                             {
                                   printf("%s    ",file->d_name);
                                  write(connfd,file->d_name,sizeof(file->d_name));
 
                             }
                             close(fd1);
                             close(connfd);
                             closedir(dr);
                             FD_CLR(fd1,&readfds);
                           
                          }
                       
                      printf("从客户收取的信息:%s\n",buf);
                    }
 
 
                  }
 
            }//end if 0
             
          
        }//end for 0
 
     
    }//end while0
 
    exit(0);
}//end main
