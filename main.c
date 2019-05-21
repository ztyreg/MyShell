/**
 * 文件：main.c
 * 作者：郑天杨 3160102142
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include "myshell.h"
#include "utility.h"

int main(int argc, char **argv, char **envp)
{

	char* args[MAX_LINE/2 + 1]; /* command line arguments */
	char* cmd = (char*)malloc(MAX_LINE * sizeof(char) + 1);
	int num_arg = 0;

	int should_run = 1; /* flag to determine when to exit program */
	init();

	int i;
	int status_input = 0;

	char* exit_cmd[2] = {"exit", "quit"};

	Signal(SIGCHLD, sigchld_handler);

	while (should_run) {
		// 测试shift, set
		// for (i = 0; i < argc; i++) {
		// 	printf("argv[%d]: %s\n", i, argv[i]);
		// }

		prompt();
		status_input = readcommand(cmd);

		/**
		 * Built-in: exit, quit
		 * Function: 退出shell
		 * Parameters: 无
		 */
		for (i = 0; i < 2; i++) {
			if (!strcmp(exit_cmd[i], cmd)) {
				printf("Shell quits!\n");
				exit(0);
			}
		}

		/**
		 * Built-in: set
		 * Function: 设置位置参数
		 * Parameters: 位置参数，空格分隔
		 */
		int tmpc = sepcommand(cmd, args);
		if (!strcmp(args[0], "set")) {
			argv = args;
			argc = tmpc;
			continue;
		}

		/**
		 * Built-in: shift
		 * Function: 位置参数移位
		 * Parameters: 移位数
		 */
		if (!strcmp(args[0], "shift")) {
			int digit = atoi(args[1]);
			argv += digit;
			argc -= digit;
			continue;
		}

		

		// if (!strcmp("environ", cmd)) {
		// 	for (char **env = envp; *env != 0; env++)
		// 	{
		// 	    char *thisEnv = *env;
		// 	    printf("%s\n", thisEnv);    
		// 	}
		// }

		runcommand(cmd, strlen(cmd), argc, argv, envp);

		/**
		* After reading user input, the steps are: 
		*内部命令：
		*…..
		*外部命令：
		* (1) fork a child process using fork()
		* (2) the child process will invoke execvp()
		* (3) if command included &, parent will invoke wait()
		*…..
		*/
	}
	return 0;
}

