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

class Person {
public:
    Person()
    {
    }

    Person(const char *n, const char *t, int a, std::tm b, bool m): name(n), title(t), age(a), birthday(b), married(m)
    {
    }
public:
    std::string name;
    std::string title;
    int age;
    std::tm birthday;
    bool married;
};

namespace soci
{
template <>
struct type_conversion<Person>
{
    typedef values base_type;

    static void from_base(values const &v, indicator &i, Person &p)
    {
        // 会提示找不到列名name/NAME(大小写都一样)
        /*p.name = v.get<std::string>("name");
        // 如果为空设置默认值hero
        p.title = v.get<std::string>("TITLE", "hero");
        p.age = v.get<int>("AGE");
        p.birthday = v.get<std::tm>("BIRTHDAY");
        p.married = v.get<int>("MARRIED");*/
    }

    static void to_base(const Person &p, values &v, indicator &i)
    {
        v.set("NAME", p.name);
        v.set("TITLE", p.title, p.title.empty() ? i_null : i_ok);
        v.set("AGE", p.age);
        v.set("BIRTHDAY", p.birthday);
        v.set("MARRIED", int(p.married)); // 不支持bool
        i = i_ok;
    }
};
}

int main() {
    try {
        session sql(postgresql, "dbname=mydb user=postgres password=123456");

        // 创建表
        sql << "CREATE TABLE IF NOT EXISTS PERSON (\n"
               "    NAME        VARCHAR(10) PRIMARY KEY NOT NULL,\n"
               "    TITLE       VARCHAR(20),\n"
               "    AGE         INT NOT NULL,\n"
               "    BIRTHDAY    DATE,\n"
               "    MARRIED     INT\n"
               ");";

        // 插入对象数据
        std::tm born = {};
        born.tm_year = 1991 - 1990;
        born.tm_mon = 6 - 1;
        born.tm_mday = 22;
        Person p("fwdssg", "", 28, born, true);
        sql << "INSERT INTO PERSON (NAME, TITLE, AGE, BIRTHDAY, MARRIED) VALUES(:NAME, :TITLE, :AGE, :BIRTHDAY, :MARRIED)", use(p);

        // 查询对象数据
        Person t = {};
        indicator i;
        int married;
        sql << "SELECT NAME, TITLE, AGE, BIRTHDAY, MARRIED FROM PERSON", into(t.name), into(t.title, i), into(t.age), into(t.birthday), into(married);
        t.married = married != 0;
        std::cout << "Name: " << t.name << std::endl;
        std::cout << "Title: " << (i == i_null ? "hero" : t.title) << std::endl;
        std::cout << "Age: " << t.age << std::endl;
        std::cout << "Birthday: " << std::asctime(&t.birthday);
        std::cout << "Married: " << t.married << std::endl;

        // 删除表
        sql << "DROP TABLE PERSON";

    } catch (std::exception const &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}