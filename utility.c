/**
 * 文件：utility.c
 * 作者：郑天杨 3160102142
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>  // ISO C variable arguments
#include <dirent.h>
#include <signal.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include "utility.h"

int nextjid = 1;

int bg(int argc, char** args) // 后台
{
	int pid, jid;
	struct job_t* job;

	if (argc < 2) { // 最少2参数
		printf("bg requires at least 2 arguments\n");
		return 1;
	} else if (args[1][0] == '%' && argc < 3) { // 没有指定jid
	   	printf("job id not specified!\n");
	   	return 1;
    } else if (!is_number(args[1]) && argc < 3) { // 不是数字
    	printf("argument must be a pid or job id\n");
    	return 1;
    }

    if (is_number(args[1])) { // pid
    	pid = atoi(args[1]);
    	if ((job = getjobpid(joblist, pid)) == NULL) { // 获取job
			printf("No such process\n");
			return 1;
		}
    } else { // jid
    	jid = atoi(args[2]);
    	if ((job = getjobjid(joblist, jid)) == NULL) { // 获取job
    		printf("No such process\n");
			return 1;
    	}
    }

    printf("[%d] (%d) %s\n", job->jid, job->pid, job->cmdline); // 打印job
    job->state = BG; // 切换到后台
    kill(-(job->pid), SIGCONT);
}

/**
 * @description：更改当前目录
 * @param：目标目录
 * @return：无
 */
int cd(int argc, char** args) // 更改当前目录
{
	if (argc > 2) {
		printf("myshell: cd: too many arguments\n"); // 参数太多
		return 1;
	}
	if (chdir(args[1]) < 0) { // 更改目录
		perror("cd failed");
	}
	return 0; // 成功
}

void clr()
{
	printf("\033[H\033[J"); // 打印清屏符号
	return;	
}

int dir(int argc, char** args) // 列出目录<directory>的内容
{
	if (argc > 2) {
		printf("myshell: dir: too many arguments\n"); // 参数太多
		return 1;
	}

	DIR* dir;
	struct dirent* ent; // 目录结构

	if (!(dir = opendir(args[1]))) { // 打开目录
		perror("opendir failed");
	}

	errno = 0; // 出错
	while ((ent = readdir(dir))) {
		puts(ent->d_name); // 打印文件内容
		errno = 0;
	}

	if (errno) {
		perror("readdir"); // 出错
		return 1;
	}
	return 0;
}

int echo(int argc, char** args) // 在屏幕上显示<comment>并换行（多个空格和制表符可能被缩减为一个空格）
{
	int i;

	if (argc == 1) { // 没有参数
		printf("\n");
		return 0;
	}

	for (i = 1; i < argc - 1; i++) {
		printf("%s ", args[i]); // 打印参数，空格分隔
	}
	printf("%s\n", args[argc - 1]); // 换行
	return 0;
}

int exec(int argc, char** args) // 用给定的命令替代shell
{
	if (argc == 1) { // 当前shell
		return 0;
	}

	if (argc > 2 && args[1][0] == '>') { // 所有输出重定向到文件
		int saved_stdout;
	}
}

int environ(char** envp) // 列出所有的环境变量
{
	for (char **env = envp; *env != 0; env++)
	{
	    char *thisEnv = *env; // 遍历环境变量
	    printf("%s\n", thisEnv);    
	}
}

int fg(int argc, char** args) // 前台
{
	int pid, jid;
	struct job_t* job;

	if (argc < 2) { // 至少2参数
		printf("bg requires at least 2 arguments\n");
		return 1;
	} else if (args[1][0] == '%' && argc < 3) { // 没有指定jid
	   	printf("job id not specified!\n");
	   	return 1;
    } else if (!is_number(args[1]) && argc < 3) { // 不是数字
    	printf("argument must be a pid or job id\n");
    	return 1;
    }

    if (is_number(args[1])) { // pid
    	pid = atoi(args[1]);
    	if ((job = getjobpid(joblist, pid)) == NULL) { // 获取job
			printf("No such process\n");
			return 1;
		}
    } else { // jid
    	jid = atoi(args[2]);
    	if ((job = getjobjid(joblist, jid)) == NULL) { // 获取job
    		printf("No such process\n");
			return 1;
    	}
    }

    if (job->state == ST) { // 停止
        job->state = FG; // 切换到前台
        kill(-(job->pid), SIGCONT); //信号SIGCONT
    } else if (job->state == BG) // 后台运行中
        job->state = FG; // 前台
    waitfg(job->pid); // 等待完成
}

int help(int argc, char** args) // 显示用户手册，并且使用more命令过滤
{
	if (!strcmp(args[1], "bg")) { // bg
		printf("bg: bg [pid] | bg %% [jid]\n后台工作。\n\n切换到后台工作。\n");
	
	} else if (!strcmp(args[1], "cd")) { // cd
		printf("cd: cd [dir]\n更改工作目录。\n\n将工作目录更改到dir。\n\n成功更改时返回0。\n");

	} else if (!strcmp(args[1], "clr")) { // clr
		printf("clr: clr\n清屏。\n\n清除屏幕上所有内容，并显示命令提示符。\n\n无返回值。\n");

	} else if (!strcmp(args[1], "dir")) { // dir
		printf("dir: dir [dir]\n显示目录内容。\n\n显示目录dir下所有内容。\n\n成功显示时返回0。\n");

	} else if (!strcmp(args[1], "echo")) { // echo
		printf("echo: echo [comment]\n显示。\n\n在屏幕上显示<comment>并换行（多个空格和制表符可能被缩减为一个空格）。\n\n成功显示时返回0。");

	} else if (!strcmp(args[1], "exec")) { // exec
		printf("exec: exec\n用给定的命令代替shell。");

	} else if (!strcmp(args[1], "exit")) { // exit
		printf("exit: exit\n退出shell。\n");

	} else if (!strcmp(args[1], "environ")) { // environ
		printf("environ: environ\n显示所有环境变量。\n\n显示shell的所有环境变量。\n\n成功显示时返回0。\n");

	} else if (!strcmp(args[1], "fg")) { // fg
		printf("fg: fg [pid] | fg %% [jid]\n前台工作。\n\n切换到前台工作。\n");

	} else if (!strcmp(args[1], "help")) { // help
		printf("help: help [command]\n显示帮助。\n\n显示内置命令command的帮助。\n\n成功显示时返回0。\n");

	} else if (!strcmp(args[1], "jobs")) { // jobs
		printf("jobs: jobs\n显示工作。\n\n显示所有工作。\n\n成功显示时返回0。\n");

	} else if (!strcmp(args[1], "pwd")) { // pwd
		printf("pwd: pwd\n显示工作目录。\n\n显示当前工作目录。\n\n成功显示时返回0。\n");

	} else if (!strcmp(args[1], "quit")) { // quit
		printf("quit: quit\n退出shell。\n");

	} else if (!strcmp(args[1], "set")) { // set
		printf("set: set [variables]\n设置位置变量。\n\n设置shell的位置变量，变量之间以空格分隔。\n\n成功设置时返回0。\n");

	} else if (!strcmp(args[1], "shift")) { // shift
		printf("shift: shift [n]\n位置变量移位。\n\n位置变量向左移位n。\n\n成功操作时返回0。\n");

	} else if (!strcmp(args[1], "test")) { // test
		printf("test [expr]\n计算表达式的值。\n\n比较字符串或者数字的值。\n\n选项：\n字符串-z -n = != < >\n数字 -eq -ne -lt -le -gt -ge\n\n表达式为真时返回1，假时返回0。\n");

	} else if (!strcmp(args[1], "time")) { // time
		printf("time: time\n显示当前时间。\n\n按标准格式显示当前时间。\n\n成功显示时返回0。\n");

	} else if (!strcmp(args[1], "umask")) { // umask
		printf("umask [nnn]\n设置文件格式掩码。\n\n设置文件格式掩码，每一位在0-7之间。\n\n成功设置时返回0。\n");

	} else if (!strcmp(args[1], "unset")) { // unset
		printf("unset [variables]\n重设变量。\n\n重设环境变量。\n\n成功设置时返回0。\n");

	} 
	return 0;
}

int jobs() // 列出工作
{
	int i;
    
    for (i = 0; i < MAXJOBS; i++) { // 遍历工作
		if (joblist[i].pid != 0) {
		    printf("[%d] (%d) ", joblist[i].jid, joblist[i].pid); // 打印工作
		    if (joblist[i].state == BG) {
			    printf("Running ");
		    } else if (joblist[i].state == BG) {
		    	printf("Foreground ");
		    } else if (joblist[i].state == ST) {
			    printf("Stopped ");
		    } else {
			    printf("listjoblist: Internal error: job[%d].state=%d ", 
				   i, joblist[i].state);
		    }
		    printf("%s\n", joblist[i].cmdline);
		}
    }
}

int pwd() // 显示当前目录
{
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
	printf("%s\n", getcwd(buf, len));
}

int test(int argc, char** args) // 计算条件表达式的值
{
	if (!strcmp(args[1], "-z")) // 字符串为空
		return args[2] == NULL ? 1 : 0;
	if (!strcmp(args[1], "-n")) // 字符串非空
		return args[2] == NULL ? 0 : 1;

	if (!strcmp(args[2], "=")) // 字符串相等
		return strcmp(args[1], args[3]) ? 0 : 1;
	if (!strcmp(args[2], "!=")) // 字符串不等
		return strcmp(args[1], args[3]) ? 1 : 0;
	if (!strcmp(args[2], "<")) // 字符串小于
		return strcmp(args[1], args[3]) < 0 ? 1 : 0;
	if (!strcmp(args[2], ">")) // 字符串大于
		return strcmp(args[1], args[3]) > 0 ? 1 : 0;

	int arg1 = atoi(args[1]); // 转换成数字
	int arg2 = atoi(args[3]);
	if (!strcmp(args[2], "-eq")) // 数值相等
		return arg1 == arg2;
	if (!strcmp(args[2], "-ne")) // 数值不等
		return arg1 != arg2;
	if (!strcmp(args[2], "-le")) // 小于等于
		return arg1 < arg2 || arg1 == arg2;
	if (!strcmp(args[2], "-ge")) // 大于等于
		return arg1 > arg2 || arg1 == arg2;
	if (!strcmp(args[2], "-lt")) // 小于
		return arg1 < arg2;
	if (!strcmp(args[2], "-gt")) // 大于
		return arg1 > arg2;
}

int _time() // 显示当前时间
{
	time_t c_time;
    char* time_string;

    c_time = time(NULL); // 绝对时间

    time_string = ctime(&c_time); // 标准时间格式

    printf("%s", time_string); // 打印
}

int _umask(int argc, char** args) // 显示或设置文件模式掩码
{
	umask(atoi(args[1])); // 设置
	return 0;
}

int unset(int argc, char** args) // 把变量的值重设为null
{
	int i;
	for (i = 0; i < argc; i++) {
		unsetenv(args[i]); // 重设
	}
	return 0;
}

/////////////
/////////////
/////////////
// 辅助函数 //
/////////////
/////////////
/////////////

int is_number(const char *s)
{
    while (*s) {
        if (isdigit(*s++)) return 1; // 所有都是数字
    }

    return 0;
}

void waitfg(pid_t pid)
{
    struct job_t* jobtowait;
    jobtowait = getjobpid(joblist, pid);
    while (jobtowait->state == FG) // 等待前台工作完成
        sleep(1);
    return;
}

struct job_t* getjobpid(struct job_t *jobs, pid_t pid) {
    int i;

    if (pid < 1)
		return NULL;
    for (i = 0; i < MAXJOBS; i++)
		if (jobs[i].pid == pid) // 通过pid寻找job
		    return &jobs[i];
    return NULL;
};

struct job_t* getjobjid(struct job_t *jobs, int jid) 
{
    int i;

    if (jid < 1)
		return NULL;
    for (i = 0; i < MAXJOBS; i++)
		if (jobs[i].jid == jid) // 通过jid寻找job
		    return &jobs[i];
    return NULL;
}

int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline)
{
    int i;
    
    if (pid < 1) // 不在范围内
		return 0;

    for (i = 0; i < MAXJOBS; i++) {
		if (jobs[i].pid == 0) { // 逐项初始化
		    jobs[i].pid = pid;
		    jobs[i].state = state;
		    jobs[i].jid = nextjid++;
		    if (nextjid > MAXJOBS) // 超出最大工作数
			nextjid = 1;
		    strcpy(jobs[i].cmdline, cmdline); // 拷贝
	        return 1;
		}
    }
    printf("Tried to create too many jobs\n"); // 出错
    return 0;
}

int pid2jid(pid_t pid)
{
    int i;

    if (pid < 1)
	return 0;
    for (i = 0; i < MAXJOBS; i++)
		if (joblist[i].pid == pid) { // 通过pid寻找jid
            return joblist[i].jid;
        }
    return 0;
}

void sigchld_handler(int sig) 
{
    pid_t pid;
    int status;

    while (1) {
    	pid = waitpid(fgpid(joblist), &status, WUNTRACED|WNOHANG); // 除去僵尸子进程
    	if (pid <= 0) // 除去完成
        	break;

        // 正常退出
    	if(WIFEXITED(status))
            deletejob(joblist, pid); // 从工作列表中删除
    }
    return;
}

int* Signal(int signum, int* handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); // 阻滞信号
    action.sa_flags = SA_RESTART; // 重启syscall

    if (sigaction(signum, &action, &old_action) < 0)
		perror("Signal error");
    return (old_action.sa_handler);
}

pid_t fgpid(struct job_t *jobs)
{
    int i;

    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].state == FG) // 前台进程的pid
	    return jobs[i].pid;
    return 0;
}

int deletejob(struct job_t *jobs, pid_t pid)
{
    int i;
    if (pid < 1) // 不在范围内
		return 0;

    for (i = 0; i < MAXJOBS; i++) {
		if (jobs[i].pid == pid) {
		    clearjob(&jobs[i]); // 清除job
		    nextjid = maxjid(jobs)+1; // 获取下一个jid
		    return 1;
		}
    }
    return 0;
}

void clearjob(struct job_t *job)
{
    job->pid = 0; // 清零
    job->jid = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}

int maxjid(struct job_t *jobs) 
{
    int i, max=0;

    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].jid > max) // 获取当前最大jid
	    max = jobs[i].jid;
    return max;
}
