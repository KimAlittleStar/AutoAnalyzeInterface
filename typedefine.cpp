#include "typedefine.h"
#include <iostream>
#include <string.h>
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
            ret.append("\tunsigned int " + subType[i].getVarName() + "_lengh ;\n");
        ret.append("\t" + subType[i].getTypeName_Cfamily() + " ");
        if (subType[i].getArrayType() == true)
            ret.append("* ");
        ret.append(subType[i].getVarName() + ";" + subType[i].getComment() + "\n");
    }
    ret.append("\n} PTC_" + tpName + "_t ;" + comment + "\n");
    for (size_t i = 0; i < subType.size(); i++)
    { //判断当前的类型中子类型是不是可以使用固定长度的数组作为存储空间;如果可以,那么就为他声明todata fill 函数
        if (subType[i].getTypeENum() == TYPE_other && judgeHasArray() == true && subType[i].judgeHasArray() == false)
        {

            ret.append("unsigned char* PTC_todata" + subType[i].getTypeName() + "(" +
                       subType[i].getTypeName_Cfamily() + "* t,unsigned int * len);\n");
            ret.append("PTC_" + subType[i].getTypeName() + "_t* PTC_fill" + subType[i].getTypeName() + "(" +
                       subType[i].getTypeName_Cfamily() + "* t,unsigned char * data,unsigned int len);\n");
        }
    }
    if (((typeNum == TYPE_other || typeNum == TYPE_others) && judgeHasArray() == true) || typeNum == TYPE_SendRet)
    {
        if (typeNum == TYPE_SendRet)
            ret.append("\n\n" + getTypeName_Cfamily() + " * PTC_new" + tpName + "(void);\n");
        ret.append("void PTC_delete" + tpName + "(" + getTypeName_Cfamily() + " * m);\n\n");
        ret.append("unsigned char* PTC_todata" + tpName + "(" +
                   getTypeName_Cfamily() + "* t,unsigned int * len);\n");
        ret.append("PTC_" + tpName + "_t* PTC_fill" + tpName + "(" +
                   getTypeName_Cfamily() + "* t,unsigned char * data,unsigned int len);\n");
    }

    return ret;
}

const MyString TypeDefine::getTypeName_Cfamily()
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
        ret.append("\t" + getTypeName_Cfamily() + "* ret = NULL;\n");
        ret.append("\t ret = PTC_malloc(sizeof(" + getTypeName_Cfamily() + "));\n");
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
        ret.append("\tif(m != NULL)\n\t{\n");
        for (size_t i = 0; i < subType.size(); i++)
        {
            if (subType[i].getArrayType() == true)
            {
                ret.append("\t\tif(m->" + subType[i].getVarName() + "!= NULL)\n\t\t{\n");
                if (subType[i].getTypeENum() == TYPE_others)
                {
                    ret.append("\t\t\tPTC_delete" + subType[i].getTypeName() + "(m->" + subType[i].getVarName() + ");\n");
                }
                else
                {
                    ret.append("\t\t\tPTC_free(m->" + subType[i].getVarName() + ");\n");
                }
                ret.append("\t\t}\n");
            }
            else if (subType[i].getTypeENum() == TYPE_other && subType[i].judgeHasArray() == true) ///< 筛选出里面有数组的非数据自定义结构体
            {
                ret.append("\t\tPTC_delete" + subType[i].getTypeName() + "(&(m->" + subType[i].getVarName() + "));\n");
            }
        }
        if (getArrayType() == true || typeNum == TYPE_SendRet)
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
            ret.append("unsigned char* PTC_todata" + tpName + "(" + getTypeName_Cfamily() + "* t,unsigned int * len)\n");
            ret.append("{\n");
            ret.append("\tif(t == NULL)\t {return NULL;}\n");
            ret.append("\tunsigned int lengh = 0;\n\tunsigned int memlengh = sizeof(*t);\n\n");
            ret.append("\tunsigned char * ret = PTC_malloc(sizeof(memlengh));\n\tunsigned char * pbuff = ret;");
            for (size_t i = 0; i < subType.size(); i++)
            {
                switch (subType[i].getTypeENum())
                { //依据不同的类型写入到内存中去;
                case TYPE_u8:
                case TYPE_s8:
                case TYPE_u8s:
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
                    ret.append("    memlengh += t->" + subType[i].getVarName() + "_lengh * (sizeof(t->" + subType[i].getVarName() + "));");
                    ret.append("    pbuff = PTC_malloc(memlengh);\n");
                    ret.append("    PTC_memcpy(pbuff,ret,lengh);\n");
                    ret.append("    PTC_free(ret);\n\tret = pbuff;\n");
                    ret.append("    for(unsigned int i;i<t->" + subType[i].getVarName() + "_lengh;i++)\n");
                    ret.append("    {\n");
                    ret.append("        PTC_write" + subType[i].getTypeName() + "(ret + lengh, t->" + subType[i].getVarName() + ");\n");
                    ret.append("        lengh += sizeof(t->" + subType[i].getVarName() + ");\n\n");
                    ret.append("    }\n");
                    break;
                case TYPE_other:
                    break;
                case TYPE_others:
                    break;
                default:
                    break;
                }
            }
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
    if (typeNum == TYPE_SendRet || typeNum == TYPE_others)
    {
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
