#ifndef QQUSERDAO_H
#define QQUSERDAO_H

class QQUser;
namespace sql {
    class Connection;
    namespace mysql {
      class MySQL_Driver;

    }
}


class QQUserDao
{
public:
    QQUserDao();
    ~QQUserDao();
    /**
     * @brief addQQUser
     * @param qqUser
     * @return on success return 0,on Duplicate qq error return 1,
     * other err return -1
     */
    int addQQUser(const QQUser*qqUser/*in*/);

    /**
     * @brief findQQUserByQQ
     * @param qq
     * @param qqUser
     * @return on find return 0,not fund return 1,on err return -1
     */
    int findQQUserByQQ(int qq,QQUser* qqUser/*out*/);
    sql::Connection* _con;
    sql::mysql::MySQL_Driver * _driver;
};

#endif // QQUSERDAO_H
