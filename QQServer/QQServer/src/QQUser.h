#ifndef QQUSER_H
#define QQUSER_H

#include<string>
class QQUser
{
public:
    QQUser();
    QQUser(int qq,
           const std::string& pwd,
           const std::string& name);
    int _qq;
    std::string _pwd;
    std::string _name;

    int _socket_fd=0;

};

#endif // QQUSER_H
