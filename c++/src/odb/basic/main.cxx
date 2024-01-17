#include <memory>
#include <iostream>

#include <odb/database.hxx>
#include <odb/transaction.hxx>

#include <odb/pgsql/database.hxx>

#include "person.hxx"
#include "person-odb.hxx"

int main(int argc, char *argv[]) {
  try {
    std::unique_ptr<odb::database> db(new odb::pgsql::database(argc, argv));

    odb::transaction t(db->begin());

    // 插入
    person john("John", "John@gmail.com", 33);
    person jane("Jane", "Jane@gmail.com", 32);
    person joe("Joe", "Joe@gmail.com", 30);
    db->persist(john);
    db->persist(jane);
    db->persist(joe);

    // 更新
    john.age(40);
    db->update(john);

    // 查询
    unsigned short age;
    odb::query<person> sql(odb::query<person>::age < odb::query<person>::_ref(age));
    for (age = 30; age < 40; age += 5)
    {
      std::cout << "Person younger than " << age << ":" << std::endl;
      odb::result<person> r(db->query<person>(sql));
      for (person& p : r)
      {
        std::cout << "Name: " << p.name() << " Email: " << p.email() << " Age: " << p.age() << std::endl;
      }
    }

    // 删除
    db->erase(jane);

    t.commit();
  }
  catch (const odb::exception &e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  return 0;
}