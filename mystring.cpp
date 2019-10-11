#include "mystring.h"
#include <string>
MyString::MyString()
{

}
MyString::MyString(const MyString& other)
{
    std::string::append(other);
}
MyString::MyString(const char* str)
{
    std::string::append(str);
}
MyString::MyString(const unsigned char* str)
{
    std::string::append(reinterpret_cast<const char*> (str));
}

MyString::MyString(const std::string & other)
{
    std::string::append(other);
}


/**
 ***********************************************************************************************
 * @brief replace
 * 函数实现了 寻找自身字符串中含有 des字符串的位置 将其替换为 src,同时index 表示替换其中的第几个,0表示替换所有
 * this = "123456" des = "23" ,src = "---" index = 0;      ---->return 1 && this = "1---456"
 * @param des           想要替换的字符串
 * @param src           替换之后的字符串
 * @param index         需要替换第几个找到的des字符串的地方.
 * @return int          总共替换了几处地方 ,如果没有替换则返回-1
 * @author kimalittle@gmail.com
 * @date 2019-10-07
 * @note mark your ideal
 **********************************************************************************************
 */
int MyString::replace(const std::string & des ,const std::string & src,int index)
{
    std::string::size_type pos = 0;
    int cnt = 1;
    int ret = -1;
    while((pos = this->find(des)) != std::string::npos)   //替换所有指定子串
    {
       // replace();
        if(ret == -1)
            ret = 0;
        if(cnt == index || index == 0)
        {
            std::string::replace(pos, des.length(), src);
            ret++;
        }
        cnt++;

    }
    return ret;
}

/**
 ***********************************************************************************************
 * @brief replace           重写函数 参考
 * @see int MyString::replace(const std::string & des ,const std::string & src,int index)
 * @param des
 * @param src
 * @param index
 * @return  int
 * @author kimalittle@gmail.com
 * @date 2019-10-07
 * @note mark your ideal
 **********************************************************************************************
 */
int MyString::replace(const char * des,const char * src,int index)
{
    return replace(std::string(des),std::string(src),index);
}

/**
 ***********************************************************************************************
 * @brief replace           重写函数 参考
 * @see int MyString::replace(const std::string & des ,const std::string & src,int index)
 * @param des
 * @param src
 * @param index
 * @return  int
 * @author kimalittle@gmail.com
 * @date 2019-10-07
 * @note mark your ideal
 **********************************************************************************************
 */
int MyString::replace(const std::string & des ,const char* src,int index)
{
    return replace(des,std::string(src),index);
}

/**
 ***********************************************************************************************
 * @brief replace           重写函数 参考
 * @see int MyString::replace(const std::string & des ,const std::string & src,int index)
 * @param des
 * @param src
 * @param index
 * @return  int
 * @author kimalittle@gmail.com
 * @date 2019-10-07
 * @note mark your ideal
 **********************************************************************************************
 */
int MyString::replace(const char* des ,const std::string & src,int index)
{
    return replace(std::string(des),src,index);
}

/**
 ***********************************************************************************************
 * @brief mid  切割函数,@see std::string::substr
 * @param start 开始的位置
 * @param lengh 长度
 * @return void
 * @author kimalittle@gmail.com
 * @date 2019-10-07
 * @note mark your ideal
 **********************************************************************************************
 */
MyString MyString::mid(int start,int lengh) const
{
    return mid(static_cast<size_t>( start),static_cast<size_t>(lengh));
}
MyString MyString::mid(std::size_t start,std::size_t lengh) const
{
    return MyString(this->substr(start,lengh));
}
MyString MyString::mid(int start,std::size_t lengh) const
{
    return mid(static_cast<size_t>( start),lengh);
}
MyString MyString::mid(std::size_t start,int lengh) const
{
    return mid(start,static_cast<size_t>(lengh));
}
