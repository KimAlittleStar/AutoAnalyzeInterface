#include "typedefine.h"
#include <iostream>
#include <string.h>

const static char *typeConvert[][4] = // typestr,Cstr,JavaStr
    {
        {"u8", "PTC_u8 ", "unsigned char ", reinterpret_cast<const char *>(TYPE_u8)},
        {"s8", "PTC_s8 ", "signed char ", reinterpret_cast<const char *>(TYPE_s8)},
        {"u16", "PTC_u16 ", "unsigned short ", reinterpret_cast<const char *>(TYPE_u16)},
        {"s16", "PTC_s16 ", "signed short ", reinterpret_cast<const char *>(TYPE_s16)},
        {"u32", "PTC_u32 ", "unsigned int ", reinterpret_cast<const char *>(TYPE_u32)},
        {"s32", "PTC_s32 ", "signed int ", reinterpret_cast<const char *>(TYPE_s32)},
        {"u64", "PTC_u64 ", "unsigned long long ", reinterpret_cast<const char *>(TYPE_u64)},
        {"bool", "Boolean ", "bool ", reinterpret_cast<const char *>(TYPE_bool)},
        {"f32", "PTC_f32 ", "float ", reinterpret_cast<const char *>(TYPE_f32)},
        {"string", "char ", "String ", reinterpret_cast<const char *>(TYPE_string)}};

TypeDefine::TypeDefine()
{
}

bool TypeDefine::fillTypedef(const MyString *str)
{
    // 这里的模式是 u8 ssss; //adsfad
    //              u16 ssss;
    std::cout << "fillTypedef str = " << *str << std::endl;
    std::vector<MyString> buff = str->splits("{\n;}");
    TYPE_e bufftype = TYPE_Err;
    for (size_t i = 0; i < buff.size(); i++)
    {
        std::cout << "str buff [" << i << "] = " << buff[i] << "\n";
        bufftype = judgeType(buff[i]);
        if (bufftype == TYPE_comment && i != 0)
        {
            subType.back().comment = buff[i];
        }
        else if (bufftype == TYPE_other || bufftype == TYPE_others)
        {
            TypeDefine sub;
            sub.setTypeENum(bufftype);
            buff[i].removeAll("[");
            buff[i].removeAll("]");
            buff[i].removeAll("*");
            buff[i].removeAll(" ");
            buff[i + 2].removeAll(" ");
            sub.setTypeName(buff[i]);
            sub.setVarName(buff[i + 2]);
            sub.fillTypedef(&buff[i + 1]);
            if (bufftype == TYPE_others)
                sub.isArray = true;
            subType.push_back(sub);
            i += 2;
        }
        else if (bufftype == TYPE_Err)
            return false;
        else
        {
            TypeDefine newele;
            newele.setTypeName(typeConvert[bufftype >> 1][0]);
            std::vector<MyString> b = buff[i].splits(" ");
            newele.setTypeENum(bufftype);
            newele.setVarName(b.back());
            if ((bufftype & 0x01) == 1 || bufftype == TYPE_string)
                newele.setArrayType(true);
            subType.push_back(newele);
        }
    }

    for (size_t i = 0; i < subType.size(); i++)
    {
        std::cout << tpName << ": subType [" << i << "] = base:" << subType[i].getTypeName() << "; name:" << subType[i].getVarName() << "; comm:" << subType[i].getComment() << "; Array:" << subType[i].getArrayType() << "\n";
    }

    return true;
}

TYPE_e TypeDefine::judgeType(const MyString &str) const
{
    size_t conversize = sizeof(typeConvert) / sizeof(typeConvert[0]);
    TYPE_e ret = TYPE_Err;
    if (str.find("//") != std::string::npos)
    {
        ret = TYPE_comment;
        return ret;
    }
    std::vector<MyString> buff;
    buff = str.splits(" ");
    for (size_t i = 0; i < conversize; i++)
    {
        //@todo 判断数据类型
        if (buff.size() == 2 && buff[0].find(typeConvert[i][0]) == 0)
        { //找到了对应的基础类型
            long long t = 0;
            t = reinterpret_cast<long long>(const_cast<char *>(typeConvert[i][3]));
            if ((buff[0].find("*") != std::string::npos || buff[0].find("[]") != std::string::npos) &&
                ret != TYPE_string)
                t++;
            ret = static_cast<TYPE_e>(t);
            return ret;
        }
        else if (buff.size() == 3 && buff[0].find(typeConvert[i][0]) == 0)
        {
            long long t = 0;
            t = reinterpret_cast<long long>(const_cast<char *>(typeConvert[i][3]));
            if ((buff[1].find("*") != std::string::npos || buff[1].find("[]") != std::string::npos) &&
                ret != TYPE_string)
                t++;
            ret = static_cast<TYPE_e>(t);
            return ret;
        }
    }
    if (buff.size() == 1 && (buff[0].find("*") != std::string::npos || buff[0].find("[]") != std::string::npos))
        ret = TYPE_others;
    else if (buff.size() == 2 && (buff[1].find("*") != std::string::npos || buff[1].find("[]") != std::string::npos))
        ret = TYPE_others;
    else if (buff.size() <= 2)
        ret = TYPE_other;
    else
    {
        ret = TYPE_Err;
    }
    return ret;
}

const MyString TypeDefine::getStrToC_file(void)
{
    MyString ret;
    for (size_t i = 0; i < subType.size(); i++)
    {
        if ((subType[i].getTypeENum() == TYPE_other && judgeHasArray() == true) || subType[i].getTypeENum() == TYPE_others)
        {
            ret.append(subType[i].getStrToC_file());
        }
    }
    ret.append(getNewfunctionStr());
    ret.append(getDeletefunctionStr());
    ret.append(getTodatafunctionStr());
    ret.append(getFillfunctionStr());

    return ret;
}
const MyString TypeDefine::getStrToH_file(void)
{
    MyString ret;
    for (size_t i = 0; i < subType.size(); i++)
    {
        if (subType[i].getTypeENum() == TYPE_other || subType[i].getTypeENum() == TYPE_others)
        {
            ret.append(subType[i].getStrToH_file());
        }
    }
    ret.append("\ntypedef struct \n{\n");
    for (size_t i = 0; i < subType.size(); i++)
    {
        if (subType[i].getArrayType() == true)
            ret.append("\tPTC_u32 " + subType[i].getVarName() + "_lengh ;\n");
        ret.append("\t" + subType[i].getTypeName_Cfamily() + " ");
        if (subType[i].getArrayType() == true)
            ret.append("* ");
        ret.append(subType[i].getVarName() + ";" + subType[i].getComment() + "\n");
    }
    ret.append("\n} PTC_" + tpName + "_t ;" + comment + "\n");
    for (size_t i = 0; i < subType.size(); i++)
    { //判断当前的类型中子类型是不是可以使用固定长度的数组作为存储空间;如果可以,那么就为他声明todata fill 函数
        if ((subType[i].getTypeENum() == TYPE_other || subType[i].getTypeENum() == TYPE_others) && judgeHasArray() == true && subType[i].judgeHasArray() == false)
        {

            ret.append("PTC_u8* PTC_todata" + subType[i].getTypeName() + "(" +
                       subType[i].getTypeName_Cfamily() + "* t,PTC_u32 * len);\n");
            ret.append("PTC_" + subType[i].getTypeName() + "_t* PTC_fill" + subType[i].getTypeName() + "(" +
                       subType[i].getTypeName_Cfamily() + "* t,PTC_u8 * data,PTC_u32* len);\n");
        }
    }
    if (((typeNum == TYPE_other || typeNum == TYPE_others) && judgeHasArray() == true) || typeNum == TYPE_SendRet)
    {
        if (typeNum == TYPE_SendRet)
            ret.append("\n\n" + getTypeName_Cfamily() + " * PTC_new" + tpName + "(void);\n");
        ret.append("void PTC_delete" + tpName + "(" + getTypeName_Cfamily() + " * m);\n\n");
        ret.append("PTC_u8* PTC_todata" + tpName + "(" +
                   getTypeName_Cfamily() + "* t,PTC_u32 * len);\n");
        ret.append("PTC_" + tpName + "_t* PTC_fill" + tpName + "(" +
                   getTypeName_Cfamily() + "* t,PTC_u8 * data,PTC_u32* len);\n");
    }

    return ret;
}

const MyString TypeDefine::getTypeName_Cfamily() const
{
    MyString ret;
    if (typeNum == TYPE_Err)
        return ret;
    else if (typeNum == TYPE_other || typeNum == TYPE_others ||
             typeNum == TYPE_SendRet)
    {
        ret = "PTC_" + tpName + "_t ";
    }
    else
    {
        ret = typeConvert[typeNum >> 1][1];
    }
    return ret;
}

const MyString TypeDefine::getNewfunctionStr()
{
    MyString ret;
    if (typeNum == TYPE_SendRet || typeNum == TYPE_others)
    {
        ret.append(getTypeName_Cfamily() + " * PTC_new" + tpName + "() \n{\n");
        ret.append("\t" + getTypeName_Cfamily() + "* ret = PTC_NULL;\n");
        ret.append("\t ret =  PTC_REPOINT(" + getTypeName_Cfamily() + " *,PTC_malloc(sizeof(" + getTypeName_Cfamily() + ")));\n");
        ret.append("\t PTC_memset(ret,0,sizeof(" + getTypeName_Cfamily() + "));\n");
        ret.append("\t return ret;\n");
        ret.append("\n}\n\n");
    }
    return ret;
}
const MyString TypeDefine::getDeletefunctionStr()
{
    MyString ret;
    if (typeNum == TYPE_SendRet || typeNum == TYPE_others || (typeNum == TYPE_other && judgeHasArray() == true))
    {
        ret.append("void PTC_delete" + tpName + "(" + getTypeName_Cfamily() + "* m)\n");
        ret.append("{\n");
        ret.append("\tif(m != PTC_NULL)\n\t{\n");
        for (size_t i = 0; i < subType.size(); i++)
        {
            if (subType[i].getArrayType() == true)
            {
                ret.append("\t\tif(m->" + subType[i].getVarName() + "!= PTC_NULL)\n\t\t{\n");
                if (subType[i].getTypeENum() == TYPE_others)
                {
                    ret.append("\t\t\tfor(PTC_u32 i = 0;i<m->" + subType[i].getVarName() + "_lengh;i++)\n");
                    ret.append("\t\t\t\tPTC_delete" + subType[i].getTypeName() + "(m->" + subType[i].getVarName() + " + i);\n");
                }
                ret.append("\t\t\tPTC_free(m->" + subType[i].getVarName() + ");\n");
                ret.append("\t\t}\n");
            }
            else if (subType[i].getTypeENum() == TYPE_other && subType[i].judgeHasArray() == true) ///< 筛选出里面有数组的非数据自定义结构体
            {
                ret.append("\t\tPTC_delete" + subType[i].getTypeName() + "(&(m->" + subType[i].getVarName() + "));\n");
            }
        }
        if (typeNum == TYPE_SendRet)
            ret.append("\t\tPTC_free(m);\n");

        ret.append("\t}\n}\n\n");
    }
    return ret;
}
const MyString TypeDefine::getTodatafunctionStr()
{
    MyString ret;
    if (typeNum == TYPE_SendRet || typeNum == TYPE_others || typeNum == TYPE_other)
    {
        if (judgeHasArray() == false)
        {
            ret.append("PTC_DECLARE_TODATA(" + tpName + ")\t\t//declare todata function;\n\n");
            return ret;
        }
        else
        {
            ret.append("PTC_u8* PTC_todata" + tpName + "(" + getTypeName_Cfamily() + "* t,PTC_u32 * len)\n");
            ret.append("{\n");
            ret.append("\tif(t == PTC_NULL)\t {return PTC_NULL;}\n");
            ret.append("\tPTC_u32 lengh = 0;\n\tPTC_u32 memlengh = sizeof(*t);\n\n");
            if (typeNum == TYPE_SendRet)
            {
                ret.append("\tmemlengh += sizeof(Boolean)+2;\n\n");
            }
            ret.append("\tPTC_u8 * ret =  PTC_REPOINT(PTC_u8*,PTC_malloc(memlengh));\n");
            if (judgeHasOther() == true)
                ret.append("\tPTC_u8 * pbuff = ret;\n    PTC_u32 otherLen = 0;\n");

            if (typeNum == TYPE_SendRet)
            {
                MyString iname = tpName;
                if (iname.find("_Send") != std::string::npos)
                {
                    iname.removeAll("_Send");
                    iname = "PTC_" + iname + "_e";
                    ret.append("\tPTC_writeu16(ret," + iname + "&0xFFFF);\n");
                    ret.append("\tPTC_writebool(ret+2,TRUE);\n");
                    ret.append("\tlengh +=3;");
                }
                else if (iname.find("_Return") != std::string::npos)
                {
                    iname.removeAll("_Return");
                    iname = "PTC_" + iname + "_e";
                    ret.append("\tPTC_writeu16(ret," + iname + "&0xFFFF);\n");
                    ret.append("\tPTC_writebool(ret+2,FALSE);\n");
                    ret.append("\tlengh +=3;");
                }
            }
            for (size_t i = 0; i < subType.size(); i++)
            {
                switch (subType[i].getTypeENum())
                { //依据不同的类型写入到内存中去;
                case TYPE_u8:
                case TYPE_s8:
                case TYPE_u16:
                case TYPE_s16:
                case TYPE_u32:
                case TYPE_s32:
                case TYPE_u64:
                case TYPE_f32:
                case TYPE_bool:
                    ret.append("    PTC_write" + subType[i].getTypeName() + "(ret + lengh, t->" + subType[i].getVarName() + ");\n");
                    ret.append("    lengh += sizeof(t->" + subType[i].getVarName() + ");\n\n");
                    break;
                case TYPE_u8s:
                case TYPE_s8s:
                case TYPE_u16s:
                case TYPE_s16s:
                case TYPE_u32s:
                case TYPE_s32s:
                case TYPE_u64s:
                case TYPE_f32s:
                case TYPE_bools:
                case TYPE_string:
                    ret.append("    PTC_writeu32(ret + lengh,t->" + subType[i].getVarName() + "_lengh);\n");
                    ret.append("    lengh += sizeof(t->" + subType[i].getVarName() + "_lengh);\n");
                    ret.append("    memlengh += t->" + subType[i].getVarName() + "_lengh * (sizeof(t->" + subType[i].getVarName() + "[0]));");
                    ret.append("    ret =  PTC_REPOINT(PTC_u8*,PTC_realloc(ret,memlengh));\n");
                    ret.append("    for(PTC_u32 i = 0;i<t->" + subType[i].getVarName() + "_lengh;i++)\n");
                    ret.append("    {\n");
                    ret.append("        PTC_write" + subType[i].getTypeName() + "(ret + lengh, t->" + subType[i].getVarName() + "[i]);\n");
                    ret.append("        lengh += sizeof(t->" + subType[i].getVarName() + "[0]);\n\n");
                    ret.append("    }\n\n");
                    break;
                case TYPE_other:
                    ret.append("    otherLen = 0;\n");
                    ret.append("    pbuff = PTC_todata" + subType[i].getTypeName() + "(&t->" + subType[i].getVarName() + ",&otherLen);\n");
                    ret.append("    memlengh += otherLen;\n");
                    ret.append("    ret =  PTC_REPOINT(PTC_u8*,PTC_realloc(ret,memlengh));\n");
                    ret.append("    PTC_memcpy(ret+lengh,pbuff,otherLen);\n");
                    ret.append("    PTC_free(pbuff);\n\tlengh+=otherLen;\n\n");
                    break;
                case TYPE_others:
                    ret.append("    PTC_writeu32(ret + lengh,t->" + subType[i].getVarName() + "_lengh);\n");
                    ret.append("    lengh += sizeof(t->" + subType[i].getVarName() + "_lengh);\n");
                    ret.append("    for(PTC_u32 i = 0;i<t->" + subType[i].getVarName() + "_lengh;i++)\n");
                    ret.append("    {\n");
                    ret.append("        otherLen = 0;\n");
                    ret.append("        pbuff = PTC_todata" + subType[i].getTypeName() + "(&t->" + subType[i].getVarName() + "[i],&otherLen);\n");
                    ret.append("        memlengh += otherLen;\n");
                    ret.append("        ret =  PTC_REPOINT(PTC_u8*,PTC_realloc(ret,memlengh));\n");
                    ret.append("        PTC_memcpy(ret+lengh,pbuff,otherLen);\n");
                    ret.append("        PTC_free(pbuff);\n\tlengh+=otherLen;\n\n");
                    ret.append("    }\n\n");
                    break;
                default:
                    break;
                }
            }
            ret.append("    ret = PTC_REPOINT(PTC_u8*,PTC_realloc(ret,lengh+1));\n");
            ret.append("    (*len) += lengh;\n\n");
            ret.append("    return ret;\n");
            ret.append("\n}\n\n");
        }
    }
    return ret;
}
const MyString TypeDefine::getFillfunctionStr()
{
    MyString ret;
    if (typeNum == TYPE_SendRet || typeNum == TYPE_others || typeNum == TYPE_other)
    {
        if (judgeHasArray() == false)
        {
            ret.append("PTC_DECLARE_FILL(" + tpName + ")\t\t//declare fill function;\n\n");
            return ret;
        }
        else
        {
            ret.append(getTypeName_Cfamily() + " * PTC_fill" + tpName + "(" + getTypeName_Cfamily() + "* t,PTC_u8 * data,PTC_u32* len)\n");
            ret.append("{\n");
            ret.append("\tif(data == PTC_NULL)\t {return PTC_NULL;}\n");
            ret.append("\t" + getTypeName_Cfamily() + " * ret = t;\n");
            if (typeNum == TYPE_SendRet)
                ret.append("\tif(ret == PTC_NULL)\n\t\tret = PTC_new" + tpName + "();\n");
            ret.append("\tPTC_u32 index = 0;");
            if (typeNum == TYPE_SendRet)
            {
                ret.append("\tindex+=3;");
            }
            for (size_t i = 0; i < subType.size(); i++)
            {
                switch (subType[i].getTypeENum())
                { //依据不同的类型写入到内存中去;
                case TYPE_u8:
                case TYPE_s8:
                case TYPE_u16:
                case TYPE_s16:
                case TYPE_u32:
                case TYPE_s32:
                case TYPE_u64:
                case TYPE_f32:
                case TYPE_bool:
                    ret.append("    ret->" + subType[i].getVarName() + " = PTC_read" + subType[i].getTypeName() + "(data+index);\n");
                    ret.append("    index += sizeof(ret->" + subType[i].getVarName() + ");\n");
                    break;
                case TYPE_u8s:
                case TYPE_s8s:
                case TYPE_u16s:
                case TYPE_s16s:
                case TYPE_u32s:
                case TYPE_s32s:
                case TYPE_u64s:
                case TYPE_f32s:
                case TYPE_bools:
                case TYPE_string:
                    ret.append("    ret->" + subType[i].getVarName() + "_lengh = PTC_readu32(data+index);\n");
                    ret.append("    index += sizeof(ret->" + subType[i].getVarName() + "_lengh);\n");
                    ret.append("    ret->" + subType[i].getVarName() + " = PTC_REPOINT(" + subType[i].getTypeName_Cfamily() + " *,PTC_malloc(ret->" + subType[i].getVarName() +
                               "_lengh * sizeof(ret->" + subType[i].getVarName() + "[0])));\n");
                    ret.append("    for(PTC_u32 i = 0;i<ret->" + subType[i].getVarName() + "_lengh ;i++)\n");
                    ret.append("    {\n");
                    ret.append("        ret->" + subType[i].getVarName() + "[i] = PTC_read" + subType[i].getTypeName() + "(data+index);\n");
                    ret.append("        index += sizeof(ret->" + subType[i].getVarName() + "[0]);\n");
                    ret.append("    }\n\n");
                    break;
                case TYPE_other:
                    ret.append("    PTC_fill" + subType[i].getTypeName() + "(&(ret->" + subType[i].getVarName() + "),data+index,&index);\n");
                    break;
                case TYPE_others:
                    ret.append("    ret->" + subType[i].getVarName() + "_lengh = PTC_readu32(data+index);\n");
                    ret.append("    index += sizeof(ret->" + subType[i].getVarName() + "_lengh);\n");
                    ret.append("    ret->" + subType[i].getVarName() + " = PTC_REPOINT(" + subType[i].getTypeName_Cfamily() + " *,PTC_malloc(ret->" + subType[i].getVarName() +
                               "_lengh * sizeof(ret->" + subType[i].getVarName() + "[0])));\n");
                    ret.append("    for(PTC_u32 i = 0;i<ret->" + subType[i].getVarName() + "_lengh ;i++)\n");
                    ret.append("    {\n");
                    ret.append("        PTC_fill" + subType[i].getTypeName() + "(ret->" + subType[i].getVarName() + "+i,data+index,&index);\n");
                    ret.append("    }\n\n");
                    break;
                default:
                    break;
                }
            }
            ret.append("    (*len) += index;\n");
            ret.append("    return ret;\n");
            ret.append("\n}\n\n");
        }
    }
    return ret;
}

bool TypeDefine::judgeHasArray()
{
    bool ret = false;
    for (size_t i = 0; i < subType.size(); i++)
    {
        if (subType[i].getTypeENum() == TYPE_other)
            ret = subType[i].judgeHasArray();
        else
        {
            if (subType[i].getTypeENum() == TYPE_others ||
                subType[i].getTypeENum() == TYPE_string ||
                (subType[i].getTypeENum() <= TYPE_f32s && ((subType[i].getTypeENum() & 0x01) == 0x01)))
                ret = true;
        }
        if (ret == true)
            return ret;
    }
    return ret;
}

bool TypeDefine::judgeHasOther()
{
    bool ret = false;
    for (size_t i = 0; i < subType.size(); i++)
    {
        if (subType[i].getTypeENum() == TYPE_other || subType[i].getTypeENum() == TYPE_others)
            return true;
    }
    return ret;
}

const MyString TypeDefine::getShowOutFuntion(MyString perStr) const
{
    MyString ret;
    if (typeNum == TYPE_SendRet)
    {
        ret.append("\n" + getTypeName_Cfamily() + "* show = PTC_fill" + tpName + "(PTC_NULL,data,len);\n");
        ret.append("PTC_PRINTF(\"show " + getTypeName_Cfamily() + "\\n\");\n");
        perStr.append("  ");
        for (size_t i = 0; i < subType.size(); i++)
        {
            ret.append(subType[i].getShowOutFuntion(perStr, "show->"));
        }
        ret.append("PTC_PRINTF(\"show " + getTypeName_Cfamily() + "_OVER\\n\");\n");
        ret.append("PTC_delete" + getTypeName() + "(show);\n");
    }
    return ret;
}

const MyString TypeDefine::getShowOutFuntion(MyString perStr, MyString mother) const
{
    MyString ret = "\n";
    std::vector<MyString> subMo = mother.splits("[]");
    if (subMo.size() < 1 || (subMo.size() & 0x01) == 0)
        return ret;
    MyString formatStr = subMo[0];
    MyString argStr;
    for (size_t i = 1; i < subMo.size(); i += 2)
    {
        formatStr.append("[%u]" + subMo[i + 1]);
        argStr.append(subMo[i] + ",");
    }
    switch (typeNum)
    {
    case TYPE_u8:
    case TYPE_u16:
    case TYPE_u32:
        ret.append("PTC_PRINTF(\"" + perStr + formatStr + varName + " = %u\\n\"," + argStr + mother + varName + ");\n");
        break;
    case TYPE_s8:
    case TYPE_s16:
    case TYPE_s32:
        ret.append("PTC_PRINTF(\"" + perStr + formatStr + varName + " = %d\\n\"," + argStr + mother + varName + ");\n");
        break;
    case TYPE_u64:
        ret.append("PTC_PRINTF(\"" + perStr + formatStr + varName + " = %lld\\n\"," + argStr + mother + varName + ");\n");
        break;
    case TYPE_f32:
        ret.append("PTC_PRINTF(\"" + perStr + formatStr + varName + " = %f\\n\",PTC_RETYPE(double," + argStr + mother + varName + "));\n");
        break;
    case TYPE_bool:
        ret.append("PTC_PRINTF(\"" + perStr + formatStr + varName + " = %s\\n\"," + argStr + "(" + mother + varName +
                   " == TRUE)?(\"TRUE\"):(\"FALSE\"));\n");
        break;
    case TYPE_u8s:
    case TYPE_s8s:
    case TYPE_u16s:
        ret.append("PTC_PRINTF(\"" + perStr + formatStr + varName + "_lengh = %u\\n\"," + argStr + mother + varName + "_lengh);\n");
        ret.append("for(PTC_u32 i = 0;i<" + mother + varName + "_lengh;i++)\n");
        ret.append("    PTC_PRINTF(\"" + perStr + formatStr + varName + "[%u] = %8u\\n\", i," + argStr + mother + varName + "[i]);\n");
        break;
    case TYPE_s16s:
    case TYPE_u32s:
    case TYPE_s32s:
        ret.append("PTC_PRINTF(\"" + perStr + formatStr + varName + "_lengh = %u\\n\"," + argStr + mother + varName + "_lengh);\n");
        ret.append("for(PTC_u32 i = 0;i<" + mother + varName + "_lengh;i++)\n");
        ret.append("    PTC_PRINTF(\"" + perStr + formatStr + varName + "[%u] = %8d\\n\", i," + argStr + mother + varName + "[i]);\n");
        break;
    case TYPE_u64s:
        ret.append("PTC_PRINTF(\"" + perStr + formatStr + varName + "_lengh = %u\\n\"," + argStr + mother + varName + "_lengh);\n");
        ret.append("for(PTC_u32 i = 0;i<" + mother + varName + "_lengh;i++)\n");
        ret.append("    PTC_PRINTF(\"" + perStr + formatStr + varName + "[%u] = %8lld\\n\", i," + argStr + mother + varName + "[i]);\n");
        break;
    case TYPE_f32s:
        ret.append("PTC_PRINTF(\"" + perStr + formatStr + varName + "_lengh = %u\\n\"," + argStr + mother + varName + "_lengh);\n");
        ret.append("for(PTC_u32 i = 0;i<" + mother + varName + "_lengh;i++)\n");
        ret.append("    PTC_PRINTF(\"" + perStr + formatStr + varName + "[%u] = %8f\\n\", i,PTC_RETYPE(double," + argStr + mother + varName + "[i]));\n");
        break;

    case TYPE_bools:
        ret.append("PTC_PRINTF(\"" + perStr + formatStr + varName + "_lengh = %u\\n\"," + argStr + mother + varName + "_lengh);\n");
        ret.append("for(PTC_u32 i = 0;i<" + mother + varName + "_lengh;i++)\n");
        ret.append("    PTC_PRINTF(\"" + perStr + formatStr + varName + "[%u] = %5s\\n\"," + argStr + " i,(" + mother + varName +
                   "[i] == TRUE)?(\"TRUE\"):(\"FALSE\"));\n");
        break;

    case TYPE_string:
        ret.append("PTC_PRINTF(\"" + perStr + formatStr + varName + "_lengh = %u\\n\"," + argStr + mother + varName + "_lengh);\n");
        ret.append("PTC_PRINTF(\"" + perStr + formatStr + varName + " = %s\\n\"," + argStr + mother + varName + ");\n");
        break;
    case TYPE_other:
        for (size_t i = 0; i < subType.size(); i++)
        {
            ret.append(subType[i].getShowOutFuntion(perStr + "  ", mother + varName + "."));
        }
        break;
    case TYPE_others:
    {
        MyString item = tpName + "_i";
        MyString tempStr;
        ret.append("PTC_PRINTF(\"" + perStr + formatStr + varName + "_lengh = %u\\n\"," + argStr + mother + varName + "_lengh);\n");
        ret.append("for(PTC_u32 " + item + " = 0;" + item + "<" + mother + varName + "_lengh;" + item + "++)\n");
        ret.append("{\n");
        for (size_t i = 0; i < subType.size(); i++)
        {
            tempStr.append(subType[i].getShowOutFuntion(perStr + "  ", mother + varName + "[" + item + "]."));
        }
        tempStr.replace("\n", "\n\t");
        ret.append(tempStr);
        ret.append("\n}\n");
    }
    break;
    default:
        break;
    }
    return ret;
}
