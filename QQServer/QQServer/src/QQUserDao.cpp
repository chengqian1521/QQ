#include "QQUserDao.h"

#include<mysql_connection.h>
#include<mysql_driver.h>
#include<mysql_error.h>
#include<cppconn/connection.h>
#include<cppconn/exception.h>
#include<cppconn/statement.h>
#include<cppconn/prepared_statement.h>
#include"QQUser.h"
#include<iostream>
using namespace  std;
QQUserDao::QQUserDao()
{

    using namespace sql;
    using namespace sql::mysql;
    sql::mysql::MySQL_Driver *driver;
    try {

      driver = get_mysql_driver_instance();
      _driver = driver;
      _con = driver->connect("tcp://127.0.0.1:3306", "scott", "tiger");
      _con->setSchema("qq");


    } catch (sql::SQLException &e) {
      cout << "# ERR: SQLException in " << __FILE__;
      cout << "(" << __FUNCTION__ << ") on line "
         << __LINE__ << endl;
      cout << "# ERR: " << e.what();
      cout << " (MySQL error code: " << e.getErrorCode();
      cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
}

QQUserDao::~QQUserDao()
{
    delete _con;
}

int QQUserDao::addQQUser(const QQUser *qqUser)
{
    sql::PreparedStatement *stmt;
    int ret=0;
    try {

      stmt=_con->prepareStatement("insert into qq_user(qq,pwd,name) values(?,?,?)");
      stmt->setInt(1,qqUser->_qq);
      stmt->setString(2,qqUser->_pwd);
      stmt->setString(3,qqUser->_name);
      stmt->execute();

      ret=0;
      goto END;

    } catch (sql::SQLException &e) {

      if(e.getErrorCode()==1062){
          //重复主键
          ret = 1;
          goto END;
      }

      ret = -1;


      cout << "# ERR: SQLException in " << __FILE__;
      cout << "(" << __FUNCTION__ << ") on line "
         << __LINE__ << endl;
      cout << "# ERR: " << e.what();
      cout << " (MySQL error code: " << e.getErrorCode();
      cout << ", SQLState: " << e.getSQLState() << " )" << endl;

    }

END:

 delete stmt;
    return ret; //执行成功

}

int QQUserDao::findQQUserByQQ(int qq, QQUser *qqUser)
{
    using namespace sql;
    using namespace sql::mysql;

    if(!qqUser){
        std::cout<<__FILE__<<" "<<__LINE__<<"qqUser is null"<<std::endl;
        return -1;
    }
    int ret=0;
    static sql::PreparedStatement *stmt=nullptr;
    sql::ResultSet *resSet=nullptr;


    try {

      if(!stmt)
        stmt =_con->prepareStatement("select qq,pwd,name from qq_user where qq=?");
      stmt->setInt(1,qq);
      resSet=stmt->executeQuery();
      if(resSet->next()){
          qqUser->_qq=qq;
          qqUser->_pwd=resSet->getString(2);
          qqUser->_name=resSet->getString(3);
          ret = 0;

      }else{
          ret = 1;//not found
      }
      goto END;

     }catch (sql::SQLException &e) {
          cout << "# ERR: SQLException in " << __FILE__;
          cout << "(" << __FUNCTION__ << ") on line "
             << __LINE__ << endl;
          cout << "# ERR: " << e.what();
          cout << " (MySQL error code: " << e.getErrorCode();
          cout << ", SQLState: " << e.getSQLState() << " )" << endl;
          ret = -1;
          goto END;
    }
END:
     if(resSet)
         delete resSet;

     return ret;
}
