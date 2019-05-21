/**
 * 文件：myshell.h
 * 作者：郑天杨 3160102142
 */
#ifndef MYSHELL_H
#define MYSHELL_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

#define MAX_LINE 80 /* 每行字符数上限 */

void init(); // 初始化
void prompt(); // 命令提示符
char* concatpath(const char *s1, const char *s2);
void setpath(char* newpath);   	/*设置搜索路径*/
int readcommand(char* cmd);				/*读取用户输入*/
int sepcommand(char* cmd, char** args);
int is_internal_cmd(char* cmd, int cmdlen, int cmdnum, int m_argc, char** m_argv, char** envp); /*解析内部命令*/
int is_pipe(char* cmd, int cmdlen, char* ppos, int m_argc, char** m_argv, char** envp); 		/*解析管道命令*/
int is_io_redirect(char* cmd, int cmdlen, char* ipos, char* opos, char* apos, int m_argc, char** m_argv, char** envp);   /*解析重定向*/
int normal_cmd(char* cmd, int cmdlen, int infd, int outfd);  /*执行普通命令*/

int runcommand(char* cmd, int cmdlen, int m_argc, char** m_argv, char** envp);
/*其他函数……. */


#endif
