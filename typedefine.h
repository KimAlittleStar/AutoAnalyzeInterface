#ifndef TYPEDEFINE_H
#define TYPEDEFINE_H

#include<vector>
#include<string>
#include "mystring.h"
const static char* typeConvert[][3] =       // typestr,Cstr,JavaStr
{
    {"u8 "   ,   "unsigned char "  ,  "unsigned char "     },
    {"u16 "  ,   "unsigned short " ,  "unsigned shor t"    },
    {"u32 "  ,   "unsigned int "   ,  "unsigned int "      },
    {"s8 "   ,   "unsigned char "  ,  "signed char "       },
    {"s16 "  ,   "unsigned char "  ,  "signed short "      },
    {"s32 "  ,   "unsigned char "  ,  "signed int "        },
    {"u8 "   ,   "unsigned char "  ,  "unsigned long long "},
    {"u64 "  ,   "unsigned char "  ,  "unsigned char "     },
    {"bool " ,   "Boolean "        ,  "bool "              },
    {"string ",  "char * "         ,  "String "            },
    {"f32 ",     "float "          ,  "float "             }
};

typedef enum
{
    TYPE_u8 =       0,
    TYPE_u8s =      1,
    TYPE_s8 =       2,
    TYPE_s8s =      3,
    TYPE_u16 =      4,
    TYPE_u16s =     5,
    TYPE_s16 =      6,
    TYPE_s16s =     7,
    TYPE_u32 =      8,
    TYPE_u32s =     9,
    TYPE_s32 =      10,
    TYPE_s32s =     11,
    TYPE_u64 =      12,
    TYPE_u64s =     13,
    TYPE_bool =     14,
    TYPE_bools =    15,
    TYPE_f32 =      16,
    TYPE_f32s =     17,
    TYPE_string =   18,
    TYPE_other =    19,
    TYPE_others =   20,
    TYPE_comment =  21
} TYPE_e;

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
    const MyString& getTypeName()   const {return tpName;}
    void setTypeName(const MyString& name) {tpName=name; }
    const MyString& getVarName() const {return varName;}
    void setVarName(const MyString& name) {varName = name;}
private:

    std::vector<TypeElement> ele;
    std::vector<TypeDefine> subType;
    MyString tpName;
    MyString varName;
    const std::string getTypeBaseStr_Cfamily(const std::string& base);
    const std::string getTypeBaseStr_Java(const std::string & base);
    TYPE_e judgeType(const MyString& str) const;
};

#endif // TYPEDEFINE_H
