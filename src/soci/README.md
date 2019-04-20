### 环境搭建

```
### 编译环境
# yum install soci-devel.x86_64 soci-postgresql-devel.x86_64

### 测试环境
# yum install postgresql-server.x86_64 postgresql.x86_64
# postgresql-setup initdb
# systemctl start postgresql
# systemctl enable postgresql
# passwd postgres
# su postgres
# psql
postgres-# create database mydb;
postgres-# \q
# sudo -u postgres psql mydb
ALTER USER postgres WITH PASSWORD '123456';
mydb=# ALTER USER postgres WITH PASSWORD '123456';
mydb-# \q
# vim /var/lib/pgsql/data/pg_hba.conf
### 将peer改为md5
local   all             all                                     md5
# systemctl restart postgresql
```

### 教程简介

Basic

> 如何查询和修改数据

Pool

> 多线程中连接池的使用

Object

> 如何自定义对象类型