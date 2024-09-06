#include <setjmp.h>
#include <stdio.h>


jmp_buf env[2];
int current = 0;

void co1() {
    printf("co1 start\n");
    // ...
    longjmp(env[0], 1); // 执行到此处的时候(原代码是longjump(env[1], 1),此处以纠正），env[1]由于还未setjmp，即
    //还未保存栈指针等程序参数，此时直接从env[1]缓冲区中的初始值恢复
    //系统状态就导致了内存越界访问，直接段错误。若是此处调用的是env[0],
    //由于env[0]被保存了（即在setjmp(env[0]那句话)，因此状态恢复就会
    //回到setjmp那句话的状态开始执行。
    // ...
}

void co2() {
    printf("co2 start\n");
    // ...
    longjmp(env[0], 0); // 切换到co1
    // ...
}

int main() {
    if (setjmp(env[0])) {//开始数组为0，默认初始化。调用co1
        // co1被切换回来
    } else {
        co1();
    }

    if (setjmp(env[1])) {
        // co2被切换回来
    } else {
        co2();
    }

    return 0;
}
