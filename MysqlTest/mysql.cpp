#include <mysql/mysql.h>

#include <iostream>
#include <string>

using namespace std;

#define SERVER "localhost"
#define USER "root"
#define PASSWORD "123456"
#define DBNAME "test"

class MySQL {
public:
    // 初始化数据库连接
    MySQL() {
        conn_ = mysql_init(nullptr);
    }

    // 释放连接
    ~MySQL() {
        if (conn_ != nullptr) {
            mysql_close(conn_);
        }
    }

    // 连接数据库
    bool connet() {
        MYSQL *p = mysql_real_connect(conn_, SERVER, USER, PASSWORD, DBNAME, 3306, nullptr, 0);
        if (p != nullptr) {
            // 代码支持中文，因为默认是ASCII
            mysql_query(conn_, "set names gbk");
            std::cout << "connect mysql success!" << std::endl;
        } else {
            std::cout << "connect mysql fail!" << mysql_errno(conn_) << std::endl;
        }

        return p;
    }

private:
    MYSQL *conn_;
};

int main() {
    MySQL mysql;
    mysql.connet();
}