//
// Created by 范炜东 on 2019/4/12.
//

#include <iostream>
#include <string>
#include <exception>
#include <ctime>
#include <soci.h>
#include <postgresql/soci-postgresql.h>

using namespace soci;

int main() {
    try {
        session sql(postgresql, "dbname=mydb user=postgres password=123456");

        // 创建表
        sql << "CREATE TABLE IF NOT EXISTS PERSON (\n"
               "    NAME        VARCHAR(10) PRIMARY KEY NOT NULL,\n"
               "    TITLE       VARCHAR(20),\n"
               "    AGE         INT NOT NULL,\n"
               "    BIRTHDAY    DATE,\n"
               "    MARRIED     BOOL\n"
               ");";

        // 所有的SQL语句都可以使用字符串拼接的方式构建，但是为了显示如何复用SQL语句，这边采用变量的方式构建SQL语句

        // 插入数据
        sql << "INSERT INTO PERSON (NAME, AGE, BIRTHDAY, MARRIED) VALUES ('fwdssg', 28, date '1991-06-22', true)";
        sql << "INSERT INTO PERSON (NAME, AGE, BIRTHDAY, MARRIED) VALUES ('xb', 26, date '1993-09-22', true)";

        // 查询数据
        int age = 30;
        std::vector <std::string> names(30);
        sql << "SELECT NAME FROM PERSON WHERE AGE < :AGE", into(names), use(age, "AGE");
        std::cout << "These people are younger than " << age << " years old:";
        for (auto name : names) {
            std::cout << " " << name;
        }
        std::cout << std::endl;

        // 删除数据
        sql << "DELETE FROM PERSON WHERE NAME = 'xb'";

        // 更新数据
        sql << "UPDATE PERSON SET AGE = 29 WHERE NAME = 'fwdssg'";

        // 日期数据读取
        std::tm t;
        sql << "SELECT BIRTHDAY FROM PERSON WHERE NAME = 'fwdssg'", into(t);
        std::cout << "Person fwdssg's birthday: " << std::asctime(&t);
        t.tm_mday = 23;
        sql << "UPDATE PERSON SET BIRTHDAY = :birthday WHERE NAME = 'fwdssg'", use(t);
        sql << "SELECT BIRTHDAY FROM PERSON WHERE NAME = 'fwdssg'", into(t);
        std::cout << "Person fwdssg's birthday after update: " << std::asctime(&t);

        // NULL值检测
        std::string title;
        indicator i;
        std::string name = "fwdssg";
        //sql << "SELECT TITLE FROM PERSON WHERE NAME = 'fwdssg'", into(title, i);
        sql << "SELECT TITLE FROM PERSON WHERE NAME = :name", into(title, i), use(name);
        if (sql.got_data()) {
            if (i == i_null)
                std::cout << "Person fwdssg has no title" << std::endl;
            else if (i == i_ok)
                std::cout << "Person fwdssg's title: " << title << std::endl;
            else
                std::cout << "Unknow error" << std::endl;
        } else {
            std::cout << "Can't find person with name fwdssg!" << std::endl;
        }

        // 删除表
        sql << "DROP TABLE PERSON";

        // 触发异常
        int c;
        sql << "SELECT COUNT(*) FROM PERSON", into(c);

    } catch (std::exception const &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}