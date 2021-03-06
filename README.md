# Myshell

## 零、运行

运行：

```make; ./myshell```

## 一、简介

Unix shell是一种壳层与命令行界面，是UNIX操作系统下传统的用户和计算机的交互界面。第一个用户直接输入命令来执行各种各样的任务。

这是一个简单的shell，实现了I/O 重定向、管道、程序环境和后台程序执行等功能，支持内部命令bg、cd 、clr、dir、echo 、exec 、exit 、environ、fg 、help、jobs 、pwd 、quit、set 、shift 、test 、time 、umask、unset（一部分尚未完成）。

## 二、概念

### I/O 重定向

对于shell来说，有三个基础的流，标准输入流(stdin或者stream 0)，标准输出流(stdout或者stream 1)，标准错误流(stderr或者stream2)。通常，stdout跟stderr都输出到了屏幕上，但对于Linux来说，其实是两种不同的输出。

输出重定向：可以用>大于号将stdout重定向到另一个IO。必须注意的是，默认情况下，该重定向会覆盖已有文件，这个在有时候可能不经意间丢失重要数据。shell提供了选项使得我们可以禁止这种覆盖，set -o noclobber可以打开该选项。

追加输出：可以采用>>将输出重定向到文件并追加在文件结尾，这样就可以避免覆盖文件了。

输入重定向：可以用<小于号将stdin重定向到另一个IO。

### 管道

在Linux中，我们可以使用管道(Pipe)将前一个命令的stdout作为输入给后面一个命令。

### 程序环境

程序必须载入到内存中。在宿主环境中（也就是具有操作系统的环境），这个任务由操作系统完成。那些不是存储在堆栈中的尚未初始化的变量将在这个时候得到初始值。在独立环境中，程序的载入必须由手工安排，也可能是通过把可执行代码置入只读内存（ROM）来完成。

程序的执行便开始。在宿主环境中，通过一个小型的启动程序与程序链接在一起。它负责处理一系列日常事务，如收集命令行参数以便使程序能够访问它们。接着，便调用main函数。

开始执行函数代码。在绝大多数机器里，程序将使用一个运行时堆栈（stack），用于存储函数的局部变量和返回地址。程序同时也可以使用静态（static）内存，存储于静态内存中的变量在程序的整个执行过程中将一直保留它们的值。

终止程序，它可以由多种不同原因引起。“正常”终止就是main函数返回。除此之外，程序也可能是由于用户按下break键或者电话连接的挂起而终止，另外也可能是由于在执行过程中出现错误而自行中断。

### 后台程序执行

程序计算周期通常很长，要几个小时甚至一个星期。我们用的环境是用Xshell远程连接到Linux服务器。所以使程序在后台跑有以下两个好处：

1）关机不影响服务器的程序运行。
2）让程序在后台执行，不会占据终端，我们可以用终端做别的事情。

### 命令手册

请在程序内使用help [command]查询。

## 三、参考

https://blog.csdn.net/shenghuaDay/article/details/78897053
https://blog.csdn.net/sifanchao/article/details/80007755
https://blog.csdn.net/woodcorpse/article/details/79292346
