#ifndef INTERFACE_H
#define INTERFACE_H
#include"mystring.h"
#include"typedefine.h"

class Interface
{
public:
    Interface();

    bool fillInterface(std::string str);

    const std::string getStrToC_file(void);
    const std::string getStrToH_file(void);
    const std::string getStrToCPP_file(void);
    const std::string getStrToHPP_file(void);
    const std::string getStrToJAVA_file(void);

private:
    unsigned short interfaceID;
    TypeDefine retType;
    TypeDefine sendType;
    MyString comment;
    MyString interfaceName;
};

#endif // INTERFACE_H
