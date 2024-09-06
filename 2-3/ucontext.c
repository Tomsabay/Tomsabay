#include <ucontext.h>
#include <stdio.h>

ucontext_t context1, context2, main_context;

void coroutine1() {
    printf("Coroutine 1 started.\n");
    swapcontext(&context1, &main_context); // 切换回主函数
    printf("Coroutine 1 resumed.\n");
    
}

void coroutine2() {
    printf("Coroutine 2 started.\n");
    swapcontext(&context2, &context1); // 切换回主函数
    printf("Coroutine 2 resumed.\n");
    
}

int main() {
    char stack1[2048] = {0};
    char stack2[2048] = {0};
    // 获取主函数上下文
    getcontext(&main_context);

    // 初始化协程上下文
    getcontext(&context1);
    context1.uc_stack.ss_sp = stack1;
    context1.uc_stack.ss_size = sizeof(stack1);
    context1.uc_link = &context2;
    makecontext(&context1, coroutine1, 0);

    getcontext(&context2);
    context2.uc_stack.ss_sp = stack2;
    context2.uc_stack.ss_size = sizeof(stack2);
    context1.uc_link = &main_context;
    makecontext(&context2, coroutine2, 0);

    // 初始切换到协程1
    swapcontext(&main_context, &context1);

    // 协程1执行完毕后，会切换回主函数
    printf("Main function resumed.\n");
    printf("Main function resumed again.\n");

    // 可以再次切换到协程2
    swapcontext(&main_context, &context2);

    // 协程2执行完毕后，再次切换回主函数
    printf("Main function resumed again.\n");

    return 0;
}