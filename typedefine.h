#ifndef TYPEDEFINE_H
#define TYPEDEFINE_H

#include "mystring.h"
#include <string>
#include <vector>

typedef enum
{
    TYPE_u8 = 0,
    TYPE_u8s = 1,
    TYPE_s8 = 2,
    TYPE_s8s = 3,
    TYPE_u16 = 4,
    TYPE_u16s = 5,
    TYPE_s16 = 6,
    TYPE_s16s = 7,
    TYPE_u32 = 8,
    TYPE_u32s = 9,
    TYPE_s32 = 10,
    TYPE_s32s = 11,
    TYPE_u64 = 12,
    TYPE_u64s = 13,
    TYPE_bool = 14,
    TYPE_bools = 15,
    TYPE_f32 = 16,
    TYPE_f32s = 17,
    TYPE_string = 18,
    TYPE_other = 19,
    TYPE_others = 20,
    TYPE_comment = 21,
    TYPE_Err = 22,
    TYPE_SendRet = 23
} TYPE_e;

const static char *typeConvert[][4] = // typestr,Cstr,JavaStr
    {
        {"u8", "unsigned char ", "unsigned char ", reinterpret_cast<const char *>(TYPE_u8)},
        {"s8", "signed char ", "signed char ", reinterpret_cast<const char *>(TYPE_s8)},
        {"u16", "unsigned short ", "unsigned shor ", reinterpret_cast<const char *>(TYPE_u16)},
        {"s16", "signed short ", "signed short ", reinterpret_cast<const char *>(TYPE_s16)},
        {"u32", "unsigned int ", "unsigned int ", reinterpret_cast<const char *>(TYPE_u32)},
        {"s32", "signed int ", "signed int ", reinterpret_cast<const char *>(TYPE_s32)},
        {"u64", "unsigned long long ", "unsigned long long ", reinterpret_cast<const char *>(TYPE_u64)},
        {"bool", "Boolean ", "bool ", reinterpret_cast<const char *>(TYPE_bool)},
        {"f32", "float ", "float ", reinterpret_cast<const char *>(TYPE_f32)},
        {"string", "char ", "String ", reinterpret_cast<const char *>(TYPE_string)}};

typedef struct
{
    std::string typebase;     ///< 基础变量名
    std::string type_name;    ///< 生成文档中的变量名
    std::string comment;      ///< 注释
    unsigned int isArray = 0; ///< 是否是指针
} TypeElement;

class TypeDefine
{
public:
    TypeDefine();
    const MyString getStrToC_file(void);
    const MyString getStrToH_file(void);
    const MyString getStrToCPP_file(void);
    const MyString getStrToHPP_file(void);
    const MyString getStrToJAVA_file(void);

    bool fillTypedef(const MyString *str);
    const MyString &getTypeName() const { return tpName; }
    void setTypeName(const MyString &name) { tpName = name; }
    const MyString &getVarName() const { return varName; }
    void setVarName(const MyString &name) { varName = name; }

    void setComment(const MyString &cmt) { comment = cmt; }
    const MyString &getComment(void) const { return comment; }

    void setArrayType(const bool b) { isArray = b; }
    bool getArrayType(void) const { return isArray; }

    void setTypeENum(TYPE_e t) { typeNum = t; }
    TYPE_e getTypeENum(void) const { return typeNum; }

private:
    //    std::vector<TypeElement> ele;
    std::vector<TypeDefine> subType;
    MyString tpName;
    MyString varName;
    bool isArray = false;
    MyString comment;
    TYPE_e typeNum;
    //    const std::string getTypeBaseStr_Cfamily(const std::string& base);
    //    const std::string getTypeBaseStr_Java(const std::string & base);

    const MyString getTypeName_Cfamily();
    TYPE_e judgeType(const MyString &str) const;

    const MyString getNewfunctionStr();
    const MyString getDeletefunctionStr();
    const MyString getTodatafunctionStr();
    const MyString getFillfunctionStr();

    bool judgeHasArray();
};

#endif // TYPEDEFINE_H
