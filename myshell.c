/**
 * 文件：myshell.c
 * 作者：郑天杨 3160102142
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "myshell.h"
#include "utility.h"

#define NUM_INTERNAL 15

char* internal_cmd[NUM_INTERNAL] = {"bg","cd","clr","dir","echo",
"exec","environ","fg","help","jobs","pwd","test","time","umask","unset"};

void init() // 初始化
{
	// 清屏
	// clr();
	// 图案
	printf("\n\t\t\t\t************\n");
	printf("\t\t\t\t* My Shell *\n");
	printf("\t\t\t\t*   v1.0   *\n");
	printf("\t\t\t\t************\n\n");

	fflush(stdout);
	/*设置默认的搜索路径*/
	setpath("/bin:/usr/bin");

	//增加当前路径
	char* pPath;
	pPath = getenv("PATH");
	int len = 50; // 缓冲区长度
	char* buf;
	buf = malloc(len);
	while (!getcwd(buf, len) && errno == ERANGE) {
		len += 50;
		buf = realloc(buf, len); // 增加缓冲区
	}
	getcwd(buf, len); // 获取当前路径
	setpath(concatpath(pPath, buf)); // 设置<pathname>/myshell
}

void prompt() // 命令提示符
{
	// 用户名
	char* username = getenv("USER");
    printf("%s@myshell:", username);
    // 缓冲区
	char* buf;
	int len = 50;
	// 分配缓冲区空间
	buf = malloc(len);
	// 增加空间
	while (!getcwd(buf, len) && errno == ERANGE) {
		len += 50;
		buf = realloc(buf, len);
	}
	// 打印工作目录
	printf("%s", getcwd(buf, len));
	// 提示符
    printf("> ");
}

char* concatpath(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 10); // 新路径长度
    strcpy(result, s1);
    strcat(result, ":"); // 分隔符
    strcat(result, s2);
    strcat(result, "/myshell");
    return result; // 返回
}

void setpath(char* newpath)   	/*设置搜索路径*/
{
	setenv("PATH", newpath, 1);
}

int readcommand(char* cmd)				/*读取用户输入*/
{
	char buf[10 * MAX_LINE] = {'\0'};

	scanf("%[^\n]%*c", buf); // 读取一行
	while (strlen(buf) == 0) { // 无输入
		prompt(); // 命令提示符
		int ch = fgetc(stdin);
		scanf("%[^\n]%*c", buf);
	}

	if (strlen(buf) > MAX_LINE) // 输入过长
		printf("Warning: input is too long.\n");
	else
		strcpy(cmd, buf); // 返回

	return 0;
}

/**
 * @description：将命令行字符串分割成参数
 * @param：命令行字符串，命令行参数字符串数组
 * @return：命令行参数个数（从1开始计数）
 */
int sepcommand(char* cmd, char** args)
{
    int i = 0;

    char* copy = (char*)malloc(MAX_LINE * sizeof(char) + 1);
    strcpy(copy, cmd); // 复制一份用于分割

	char* token = strtok(copy, " \t"); // 以空白分隔的一段
    while (token) {
    	args[i] = (char*)malloc(MAX_LINE * sizeof(char) + 1);
        strcpy(args[i], token); // 复制到args
        token = strtok(NULL, " \t");
        i++;
    }
    free(copy); // 释放空间

    return i;
}


int is_pipe(char* cmd, int cmdlen, char* ppos, int m_argc, char** m_argv, char** envp) 		/*解析管道命令*/
{
	char* left;
	char* right;
	left = (char*)malloc(MAX_LINE * sizeof(char) + 1);
	right = (char*)malloc(MAX_LINE * sizeof(char) + 1);
	strcpy(left, cmd);
	left[ppos - cmd] = '\0';
	strcpy(right, ppos + 1);

	int p[2];

	if(pipe(p) < 0)      // 创建管道
		exit(0);
    if(fork() ==  0)    // 第一个子进程
    {
        close(1); // 关闭标准输出
        dup(p[1]); // 标准输出到管道写端
        close(p[0]); //
        close(p[1]); //
        runcommand(left, cmdlen, m_argc, m_argv, envp);
    }

    if(fork() == 0)     // 第二个子进程
    {
        close(0);
        dup(p[0]);
        close(p[1]);
        close(p[0]);
        runcommand(right, cmdlen, m_argc, m_argv, envp);
    }

    close(p[0]);
    close(p[1]);
    wait(NULL);      
    wait(NULL);

    free(left);
    free(right);
}

int is_internal_cmd(char* cmd, int cmdlen, int cmdnum, int m_argc, char** m_argv, char** envp) /*解析内部命令*/
{
	char* args[MAX_LINE/2 + 1];
	int argc;
	argc = sepcommand(cmd, args); // 解析命令

	switch(cmdnum) {
		case 0: // bg
		bg(argc, args);
		break;
		case 1: // cd
		cd(argc, args);
		break;
		case 2: // clr
		clr();
		break;
		case 3: // dir
		dir(argc, args);
		break;
		case 4: // echo
		echo(argc, args);
		break;
		case 5: // exec
		exec(argc, args);
		break;
		case 6: // environ
		environ(envp);
		break;
		case 7: // fg
		fg(argc, args);
		break;
		case 8: // help
		help(argc, args);
		break;
		case 9: // jobs
		jobs();
		break;
		case 10: // pwd
		pwd();
		break;
		case 11: // test
		printf("test result is: %d\n", test(argc, args));
		break;
		case 12: // time
		_time();
		break;
		case 13: // umask
		_umask(argc, args);
		break;
		case 14: // unset
		unset(argc, args);
		break;
	}

	int i;
	for (i = 0; i < argc; i++) {
		free(args[i]); // 释放空间
	}
}

int is_io_redirect(char* cmd, int cmdlen, char* ipos, char* opos, char* apos, int m_argc, char** m_argv, char** envp)   /*解析重定向*/
{
	char* iargs[MAX_LINE/2 + 1]; // 重定向符号后的参数
	char* oargs[MAX_LINE/2 + 1];
	char* aargs[MAX_LINE/2 + 1];

	int in_redirect = 0, out_redirect = 0, append_redirect = 0; // 重定向标志
	if (ipos)  // 存在输入重定向符号
		in_redirect = 1;
	if (opos)  // 存在输出重定向符号
		out_redirect = 1;
	if (apos)  // 增加到文件尾
		append_redirect = 1;

	int saved_stdin, saved_stdout; // 保存标准输入输出

	if (in_redirect) {
		saved_stdin = dup(0); // 复制标准输入输出
		close(0);
		sepcommand(ipos + 1, iargs);
		if((open(iargs[0], O_RDONLY)) < 0) // 只读打开文件
            perror("open file error"); 
        chmod(iargs[0], S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP); // 文件模式
        *ipos = '\0'; // 把重定向符号设置为字符串结束
	}

	if (out_redirect && !append_redirect) {
		saved_stdout = dup(1); // 复制标准输入输出
		close(1);
		sepcommand(opos + 1, oargs);
		if((open(oargs[0], O_RDWR|O_CREAT|O_TRUNC)) < 0) // 覆盖文件
            perror("open file error"); 
        chmod(oargs[0], S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
        *opos = '\0'; // 把重定向符号设置为字符串结束
	}

	if (append_redirect) {
		saved_stdout = dup(1); // 复制标准输入输出
		close(1);
		sepcommand(apos + 2, aargs);
		if((open(aargs[0], O_RDWR|O_CREAT|O_APPEND)) < 0) // 增加到文件尾
            perror("open file error"); 
        chmod(aargs[0], S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
        *apos = '\0'; // 把重定向符号设置为字符串结束
	}

	// 检查内部命令
	char* args[MAX_LINE/2 + 1];
	int argc;
	argc = sepcommand(cmd, args); // 解析命令

	int i, j;
	for (i = 0; i < NUM_INTERNAL; i++) {
		if (!strcmp(args[0], internal_cmd[i])) { // 内部命令
			is_internal_cmd(cmd, cmdlen, i, m_argc, m_argv, envp);
			for (j = 0; j < argc; j++) {
				free(args[j]); // 释放空间
			}
			if (in_redirect) {
				dup2(saved_stdin, 0); // 恢复标准输入
			}

			if (out_redirect || append_redirect) {
				dup2(saved_stdout, 1); // 恢复标准输出
			}
			return 0;
		}
	}

	// 执行外部命令
	normal_cmd(cmd, cmdlen, 0, 1);

	if (in_redirect) {
		dup2(saved_stdin, 0); // 恢复标准输入
	}

	if (out_redirect || append_redirect) {
		dup2(saved_stdout, 1); // 恢复标准输出
	}

	return 0;
}

int normal_cmd(char* cmd, int cmdlen, int infd, int outfd)  /*执行普通命令*/
{
	printf("Normal command is: %s\n", cmd);

	char* args[MAX_LINE/2 + 1];
	int argc;
	int is_bg = 0; // 后台命令
	argc = sepcommand(cmd, args);

	if (args[argc - 1][0] == '&') { // 后台命令符号必须在最后
		is_bg = 1;
		argc--; // 去掉后台命令符号
		args[argc] = NULL;
	}

	// 创建子进程
    pid_t pid = fork(); 
 
    if (pid == -1) {
        printf("Failed to fork child\n");
        return 1;
    } else if (pid == 0) {
        // sigprocmask(SIG_UNBLOCK, &set, NULL);   // 阻断SIGCHILD
        setpgrp();  // 设置子进程
        execvp(args[0], args); // 执行外部命令
        printf("%s: Command not found\n", args[0]); // 执行失败
        exit(1);
    } else {
        // 等待子进程终止
        if (is_bg)  // 后台
            addjob(joblist, pid, BG, cmd); // 增加到工作列表
        else    // 前台
            addjob(joblist, pid, FG, cmd); // 增加到工作列表
        // sigprocmask(SIG_UNBLOCK, &set, NULL); // 阻断SIGCHILD
        if (is_bg)
            printf("[%d] (%d) %s\n", pid2jid(pid), pid, cmd); // 打印后台工作信息
        else
            waitfg(pid); // 等待前台工作结束
    }

    return 0;
}

int runcommand(char* cmd, int cmdlen, int m_argc, char** m_argv, char** envp)
{
	// 检查顺序：管道","重定向","内部命令","外部命令
	char* ppos = NULL;

	// 检查管道
	if (ppos = strstr(cmd, "|")) {
		is_pipe(cmd, cmdlen, ppos, m_argc, m_argv, envp); // 管道命令
		return 0;
	}

	// 检查重定向
	char* ipos = NULL; // 重定向符号位置
	char* opos = NULL;
	char* apos = NULL;
	ipos = strstr(cmd, "<"); // 第一次重定向是有效的
	opos = strstr(cmd, ">");
	apos = strstr(cmd, ">>");
	if (ipos || opos || apos) {
		is_io_redirect(cmd, cmdlen, ipos, opos, apos, m_argc, m_argv, envp); // 重定向命令
		return 0;
	}

	// 检查内部命令
	char* args[MAX_LINE/2 + 1];
	int argc;
	sepcommand(cmd, args); // 解析命令

	int i, j;
	for (i = 0; i < NUM_INTERNAL; i++) {
		if (!strcmp(args[0], internal_cmd[i])) { // 内部命令
			is_internal_cmd(cmd, cmdlen, i, m_argc, m_argv, envp);
			for (j = 0; j < argc; j++) {
				free(args[j]); // 释放空间
			}
			return 0;
		}
	}

	// 执行外部命令
	normal_cmd(cmd, cmdlen, 0, 1);
}
