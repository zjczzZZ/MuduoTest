#ifndef MYSQL_H
#define MYSQL_H

#include <muduo/base/Logging.h>
#include <mysql/mysql.h>

#include <string>

using namespace std;

#define SERVER "localhost"
#define USER "root"
#define PASSWORD "123456"
#define DBNAME "game"

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
            mysql_query(conn_, "set names utf8");
            LOG_INFO << "connect mysql success!";
        } else {
            LOG_INFO << "connect mysql fail!";
        }

        return p;
    }

    // 更新操作
    bool update(string sql) {
        if (mysql_query(conn_, sql.c_str())) {
            LOG_INFO << __FILE__ << " : " << __LINE__ << " : " << sql << " update error!";
            return false;
        }
        return true;
    }

    // 查询操作
    MYSQL_RES *query(string sql) {
        if (mysql_query(conn_, sql.c_str())) {
            LOG_INFO << __FILE__ << " : " << __LINE__ << " : " << sql << " select error!";
            return nullptr;
        }
        return mysql_use_result(conn_);
    }

    // 获取连接
    MYSQL *get_connection() {
        return conn_;
    }

private:
    MYSQL *conn_;
};
#endif
