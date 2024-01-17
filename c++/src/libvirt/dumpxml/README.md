### 环境搭建

软件安装

```
### 编译环境
# yum install -y libvirt-devel

### 测试环境
# yum install -y libvirt-client libvirt qemu
```

软件配置

```
### 启动服务
# systemctl start libvirtd
# virsh list --all

### 创建测试虚拟机
# wget https://download.cirros-cloud.net/0.3.5/cirros-0.3.5-x86_64-disk.img -O /var/lib/libvirt/images/cirros.qcow2
# tee vm.txt << EOF
/usr/bin/qemu-system-x86_64 -smp 1 -m 128 -hda /var/lib/libvirt/images/cirros.qcow2 -vnc :0 -name cirros
EOF
# virsh domxml-from-native qemu-argv vm.txt > cirros.xml
# virsh define cirros.xml
# virsh dumpxml cirros
```

### 学习目标

- 使用libvirt api接口实现`virsh dumpxml`