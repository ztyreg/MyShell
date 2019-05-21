/**
 * 文件：utility.h
 * 作者：郑天杨 3160102142
 */
#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

#define BUF_SIZE 50
#define MAXJOBS 16
#define MAXLINE 1024  // max line length
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */

struct job_t {              /* The job struct */
    pid_t pid;              /* job PID */
    int jid;                /* job ID [1, 2, ...] */
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[80];  /* command line */
};

struct job_t joblist[MAXJOBS]; /* The job list */

// 工作操作的辅助函数
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline);
int is_number(const char *s);
void waitfg(pid_t pid);
struct job_t* getjobpid(struct job_t *jobs, pid_t pid);
struct job_t* getjobjid(struct job_t *jobs, int jid);
int pid2jid(pid_t pid);
void sigchld_handler(int sig);
int *Signal(int signum, int *handler);
pid_t fgpid(struct job_t *jobs);
int deletejob(struct job_t *jobs, pid_t pid);
void clearjob(struct job_t *job);
int maxjid(struct job_t *jobs);


// 内部命令
int bg(int argc, char** args); // 后台
int cd(int argc, char** args); // 更改当前目录
void clr();
int dir(int argc, char** args); // 列出目录<directory>的内容
int echo(int argc, char** args); // 在屏幕上显示<comment>并换行（多个空格和制表符可能被缩减为一个空格）
int exec(int argc, char** args); // 用给定的命令替代shell
int environ(char** envp); // 列出所有的环境变量
int fg(int argc, char** args); // 前台
int help(int argc, char** args); // 显示用户手册，并且使用more命令过滤
int jobs(); // 列出工作
int pwd(); // 显示当前目录
int test(int argc, char** args); // 计算条件表达式的值
int _time(); // 显示当前时间
int _umask(int argc, char** args); // 显示或设置文件模式掩码
int unset(int argc, char** args); // 把变量的值重设为null

#endif
