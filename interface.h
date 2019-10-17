#ifndef INTERFACE_H
#define INTERFACE_H
#include "mystring.h"
#include "typedefine.h"

class Interface
{
public:
    Interface();

    bool fillInterface(std::string str);

    const MyString getStrToC_file(void);
    const MyString getStrToH_file(void);
    const std::string getStrToCPP_file(void);
    const std::string getStrToHPP_file(void);
    const std::string getStrToJAVA_file(void);

    const MyString getCallbackFuncDefine() const;
    const MyString getCallbackFuncDeclare() const;

    const MyString getInterfaceName(void) const;
    unsigned short getInterFaceID(void) const { return interfaceID; }

private:
    unsigned short interfaceID;
    TypeDefine retType;
    TypeDefine sendType;
    MyString comment;
    MyString interfaceName;
};

#endif // INTERFACE_H
