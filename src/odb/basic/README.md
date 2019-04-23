### 学习目标

- ORM插入
- ORM查询
- ORM更新
- ORM删除

### 测试方法

```
### 创建表
# psql mydb postgres -f ../person.sql
### 测试ORM插入、查询、更新、删除
# ./app --user postgres --password 123456 --dbname mydb
### 确认执行结果
# psql mydb postgres
mydb=# select * from person;
```