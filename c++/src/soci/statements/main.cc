//
// Created by 范炜东 on 2019/7/9.
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

    // 语句预处理
    statement stmt = (sql.prepare << "INSERT INTO PERSON (NAME, AGE, BIRTHDAY, MARRIED) VALUES (:NAME, :AGE, :BIRTHDAY, :MARRIED)");

    // 插入
    {
      std::string name = "fwdssg";
      int age = 30;
      std::tm birthday = {0};
      birthday.tm_year = 1991 - 1900;
      birthday.tm_mon = 5;
      birthday.tm_mday = 22;
      bool married = true;
      stmt.exchange(use(name, "NAME"));
      stmt.exchange(use(age, "AGE"));
      stmt.exchange(use(birthday, "BIRTHDAY"));
      stmt.exchange(use(int(married), "MARRIED"));

      stmt.define_and_bind();
      stmt.execute(true);
    }

    {
      std::string name = "xb";
      int age = 25;
      std::tm birthday = {0};
      birthday.tm_year = 1993 - 1900;
      birthday.tm_mon = 8;
      birthday.tm_mday = 22;
      bool married = true;
      stmt.exchange(use(name, "NAME"));
      stmt.exchange(use(age, "AGE"));
      stmt.exchange(use(birthday, "BIRTHDAY"));
      stmt.exchange(use(int(married), "MARRIED"));

      stmt.define_and_bind();
      stmt.execute(true);
    }

    // 查询插入信息
    rowset<row> rs = (sql.prepare << "SELECT NAME, AGE FROM PERSON");
    for (auto iter = rs.begin(); iter != rs.end(); iter++) {
      std::cout << "Name: " << iter->get<std::string>(0) << " Age: " << iter->get<int>(1) << std::endl;
    }

    // 删除表
    sql << "DROP TABLE PERSON";
  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }

  return 0;
}