## 知识点

> 实时信号和标准信号

- 实时信号会排队，标准信号则合并，即如果进行休眠过程中，收到的信号是按实时信号发送，那么进程恢复后，信号处理器则会被调用多次，而标准信号只会被调用一次
- 实时信号编号从 SIGRTMIN 开始

> sigsuspend 和 sigwaitinfo 的区别

核心区别：异步处理 vs 同步等待

- sigsuspend (异步/回调式)： 它暂时解除屏蔽并让进程休眠，直到捕获到信号。信号发生时，内核会中断当前流程，去执行信号处理函数 (Signal Handler)。处理函数返回后，sigsuspend 才返回。
- sigwaitinfo (同步/阻塞式)： 它像 read() 一样阻塞进程，直到指定的信号发生。它不会调用信号处理函数。信号发生时，它直接"取出"信号并返回信号的信息。

## 函数

### kill

```
int kill(pid_t pid, int sig)
```

- pid > 0: 发送信号给指定进程
- pid == 0: 发送给与调用进程同组的每个进程，包括调用进程自身
- pid < -1: 发送给组 ID == -pid 的进程组内所有下属进程
- pid == -1: 发送给除 init 和调用进程自身外的其他所有进程

### sigsuspend

```
int sigsuspend(const sigset_t *mask)
```

以 mask 所指向的信号集来替换进程的信号掩码，然后挂起进程，直到其捕获到信号，并从信号处理器中返回，同时恢复进程的信号掩码为调用前，其效果等价于

```
### 以不可中断的方式执行
sigprocmask(SIG_SETMASK, &mask, &prevMask);
pause();
sigprocmask(SIG_SETMASK, &prevMask, NULL);
```