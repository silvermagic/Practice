### 学习心得

`Libivrt API`可以通过阅读`virsh`代码的方式来学习

### virsh代码阅读

通过阅读virsh代码我们能够了解libvirt api的作用以及调用方法

|文件名|对应vshCmdDef变量|对应virsh命令|
| ------ | ------ | ------ |
|virsh-domain-monitor.c|domMonitoringCmds|virsh XX(虚拟机监控)|
|virsh-domain.c|domManagementCmds|virsh XX(虚拟机操作)|
|virsh-host.c|hostAndHypervisorCmds|virsh XX(虚拟机配置)|
|virsh-interface.c|ifaceCmds|virsh iface-XX|
|virsh-network.c|networkCmds|virsh net-XX|
|virsh-nodedev.c|nodedevCmds|virsh net-XX|
|virsh-nwfilter.c|nwfilterCmds|virsh nwfilter-XX|
|virsh-pool.c|storagePoolCmds|virsh pool-XX|
|virsh-secret.c|secretCmds|virsh secret-XX|
|virsh-snapshot.c|snapshotCmds|virsh snapshot-XX|
|virsh-volume.c|storageVolCmds|virsh vol-XX|

有了上面的表格我们就能够根据使用的**virsh命令**找到对应文件的对应**vshCmdDef变量**

```
/*
 * vshCmdDef - command definition
 */
struct _vshCmdDef {
    const char *name;           /* name of command, or NULL for list end */
    bool (*handler) (vshControl *, const vshCmd *);    /* command handler */
    const vshCmdOptDef *opts;   /* definition of command options */
    const vshCmdInfo *info;     /* details about command */
    unsigned int flags;         /* bitwise OR of VSH_CMD_FLAG */
};
```
一个`vshCmdDef`结构对应一个`virsh`命令，其中`vshCmdOptDef`定义了命令的参数，`vshCmdInfo`定义了命令的帮助信息，`bool (*handler) (vshControl *, const vshCmd *)`定义了命令的处理函数。

### 实际应用

如何阅读`virsh start`处理代码

- 查看virsh-domain.c文件的domManagementCmds变量，找到
```
{
     .name = "start",
     .handler = cmdStart,
     .opts = opts_start,
     .info = info_start,
     .flags = 0
}
```

- 查看opts_start了解命令参数
```
static const vshCmdOptDef opts_start[] = {
    VIRSH_COMMON_OPT_DOMAIN(N_("name of the inactive domain")),
#ifndef WIN32
    {
     .name = "console",
     .type = VSH_OT_BOOL,
     .help = N_("attach to console after creation")
    },
#endif
    {
     .name = "paused",
     .type = VSH_OT_BOOL,
     .help = N_("leave the guest paused after creation")
    },
    {
     .name = "autodestroy",
     .type = VSH_OT_BOOL,
     .help = N_("automatically destroy the guest when virsh disconnects")
    },
    {
     .name = "bypass-cache",
     .type = VSH_OT_BOOL,
     .help = N_("avoid file system cache when loading")
    },
    {
     .name = "force-boot",
     .type = VSH_OT_BOOL,
     .help = N_("force fresh boot by discarding any managed save")
    },
    {
     .name = "pass-fds",
     .type = VSH_OT_STRING,
     .help = N_("pass file descriptors N,M,... to the guest")
    },
    {.name = NULL}
};
```

- 查看cmdStart了解命令处理代码，即对应libvirt api的调用方法，此处是对`virDomainCreateWithFiles`或`virDomainCreate`的调用
```
static bool cmdStart(vshControl *ctl, const vshCmd *cmd)
```