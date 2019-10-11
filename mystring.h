#ifndef MYSTRING_H
#define MYSTRING_H
#include<string>
#include <vector>
class MyString :public std::string
{
public:
    MyString();
    MyString(const MyString& other);
    MyString(const char* str);
    MyString(const unsigned char* str);
    MyString(const std::string & other);

    int replace(const char * des,const char * src,int index = 0);
    int replace(const std::string & des ,const std::string & src,int index = 0);
    int replace(const std::string & des ,const char* src,int index = 0);
    int replace(const char* des ,const std::string & src,int index = 0);

    MyString mid(int start,int lengh)   const;
    MyString mid(std::size_t start,std::size_t lengh) const;
    MyString mid(int start,std::size_t lengh) const;
    MyString mid(std::size_t start,int lengh) const;

    std::vector<MyString> splits(const MyString & ftm);

    void removeAll(const char * str);
    void removeAll(const MyString & str);

};

#endif // MYSTRING_H
