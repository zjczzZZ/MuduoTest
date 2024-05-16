#include "UserModel.hpp"

#include <iostream>

#include "MySQL.hpp"

using namespace std;

// 增加User
bool UserModel::insert(User &user) {
    // 组装SQL语句
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name,password) values('%s','%s')",
            user.get_name().c_str(), user.get_password().c_str());

    MySQL mysql;
    if (mysql.connet()) {
        if (mysql.update(sql)) {
            // 获取自增的用户ID
            user.set_id(mysql_insert_id(mysql.get_connection()));
            return true;
        }
    }
    return false;
}

// 根据ID查询user信息，失败返回一个默认构造User，id = -1
User UserModel::query(const std::string &name) {
    // 组装SQL语句
    std::cout << name << std::endl;
    std::string sql = "select * from user where name ='";
    sql.append(name);
    sql.append("'");
    std::cout << sql << std::endl;

    MySQL mysql;
    if (mysql.connet()) {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr) {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr) {
                User user;
                user.set_id(atoi(row[0]));
                user.set_name(row[1]);
                user.set_password(row[2]);

                // 释放资源，否则内存不断泄露
                mysql_free_result(res);

                return user;
            }
        }
    }
    return User();
}
