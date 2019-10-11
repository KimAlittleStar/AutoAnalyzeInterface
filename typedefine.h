#ifndef TYPEDEFINE_H
#define TYPEDEFINE_H

#include<vector>
#include<string>
#include"mystring.h"
const static char* typeConvert[][3] =       // typestr,Cstr,JavaStr
{
    {"u8"   ,   "unsigned char"  ,  "unsigned char"     },
    {"u16"  ,   "unsigned short" ,  "unsigned short"    },
    {"u32"  ,   "unsigned int"   ,  "unsigned int"      },
    {"s8"   ,   "unsigned char"  ,  "signed char"       },
    {"s16"  ,   "unsigned char"  ,  "signed short"      },
    {"s32"  ,   "unsigned char"  ,  "signed int"        },
    {"u8"   ,   "unsigned char"  ,  "unsigned long long"},
    {"u64"  ,   "unsigned char"  ,  "unsigned char"     },
    {"bool" ,   "Boolean"        ,  "bool"              },
    {"string",  "char *"         ,  "String"            }
};

typedef struct
{
    std::string typebase;
    std::string type_name;
    std::string comment;
    unsigned char isArray;
} TypeElement;

class TypeDefine
{
public:
    TypeDefine();
    const std::string getStrToC_file(void);
    const std::string getStrToH_file(void);
    const std::string getStrToCPP_file(void);
    const std::string getStrToHPP_file(void);
    const std::string getStrToJAVA_file(void);

    bool fillTypedef(const MyString* str);
private:

    std::vector<TypeElement> ele;
    std::vector<TypeDefine> subType;

    const std::string getTypeBaseStr_Cfamily(const std::string& base);
    const std::string getTypeBaseStr_Java(const std::string & base);
};

#endif // TYPEDEFINE_H
