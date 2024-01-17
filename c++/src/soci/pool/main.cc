//
// Created by 范炜东 on 2019/4/12.
//

#include <iostream>
#include <thread>
#include <chrono>
#include <exception>
#include <ctime>
#include <soci.h>
#include <postgresql/soci-postgresql.h>

using namespace soci;

int main() {
    try {
        const std::size_t poolSize = 3;
        connection_pool pool(poolSize);
        for (size_t i = 0; i < poolSize; ++i)
        {
            session & sql = pool.at(i);
            sql.open(postgresql, "dbname=mydb user=postgres password=123456");
        }

        session sql(pool);
        sql << "CREATE TABLE IF NOT EXISTS PERSON (\n"
               "    NAME        VARCHAR(10) PRIMARY KEY NOT NULL,\n"
               "    TITLE       VARCHAR(20),\n"
               "    AGE         INT NOT NULL,\n"
               "    BIRTHDAY    DATE,\n"
               "    MARRIED     BOOL\n"
               ");";
        sql << "INSERT INTO PERSON (NAME, AGE, BIRTHDAY, MARRIED) VALUES ('fwdssg', 28, date '1991-06-22', true)";
        std::cout << "End of initialization" << std::endl;

        std::thread query_a([&pool](){
            session sql(pool);
            std::cout << "Thread A query" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            std::tm t;
            sql << "SELECT BIRTHDAY FROM PERSON WHERE NAME = 'fwdssg'", into(t);
            std::cout << "Thead A query result: " << std::asctime(&t);
        });
        std::thread query_b([&pool](){
            session sql(pool);
            std::cout << "Thread B query" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            std::tm t;
            sql << "SELECT BIRTHDAY FROM PERSON WHERE NAME = 'fwdssg'", into(t);
            std::cout << "Thead B query result: " << std::asctime(&t);
        });
        std::thread query_c([&pool](){
            session sql(pool);
            std::cout << "Thread C query" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            std::tm t;
            sql << "SELECT BIRTHDAY FROM PERSON WHERE NAME = 'fwdssg'", into(t);
            std::cout << "Thead C query result: " << std::asctime(&t);
        });

        query_a.join();
        query_b.join();
        query_c.join();

        // 删除表
        sql << "DROP TABLE PERSON";

    } catch (std::exception const &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}