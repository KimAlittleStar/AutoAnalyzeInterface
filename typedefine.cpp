#include "typedefine.h"
#include <iostream>
#include<string.h>
TypeDefine::TypeDefine()
{

}

bool TypeDefine::fillTypedef(const MyString *str)
{
    // 这里的模式是 u8 ssss; //adsfad
    //              u16 ssss;
    std::cout<<"fillTypedef str = "<<*str<<std::endl;
    std::vector<MyString> buff = str->splits("{\n;}");
    TYPE_e bufftype = TYPE_Err;
    for(size_t i = 0;i<buff.size();i++)
    {
        std::cout<<"str buff ["<<i<<"] = "<<buff[i]<<"\n";
        bufftype = judgeType(buff[i]);
        if(bufftype == TYPE_comment && i != 0)
        {
            subType.back().comment = buff[i];
        }else if(bufftype == TYPE_other || bufftype == TYPE_others)
        {
            TypeDefine sub;
            sub.setTypeENum(bufftype);
            buff[i].removeAll("[");
            buff[i].removeAll("]");
            buff[i].removeAll("*");
            buff[i].removeAll(" ");
            sub.setTypeName(buff[i]);
            sub.setVarName(buff[i+2]);
            sub.fillTypedef(&buff[i+1]);
            if(bufftype == TYPE_others)
                sub.isArray = true;
            subType.push_back(sub);
            i+=2;
        }else if(bufftype == TYPE_Err)
            return  false;
        else {
            TypeDefine newele;
            newele.setTypeName( typeConvert[bufftype>>1][0]);
            std::vector<MyString> b = buff[i].splits(" ");
            newele.setTypeENum(bufftype);
            newele.setVarName(b.back());
            if((bufftype&0x01) == 1 || bufftype == TYPE_string)
                newele.setArrayType(true);
            subType.push_back(newele);
        }
    }

    for(size_t i = 0;i<subType.size();i++)
    {
        std::cout<< tpName <<": subType ["<<i<<"] = base:"<<subType[i].getTypeName()<<"; name:"<<
                    subType[i].getVarName()<<"; comm:"<<subType[i].getComment()<<"; Array:"<<subType[i].getArrayType()<<"\n";
    }

    return true;
}

TYPE_e TypeDefine::judgeType(const MyString& str) const
{
    size_t conversize = sizeof(typeConvert)/sizeof(typeConvert[0]);
    TYPE_e ret = TYPE_Err;
    if(str.find("//") != std::string::npos)
    {
        ret = TYPE_comment;
        return ret;
    }
    std::vector<MyString> buff;
    buff = str.splits(" ");
    for(size_t i = 0;i<conversize;i++)
    {
        //@todo 判断数据类型
        if(buff.size() == 2 && buff[0].find(typeConvert[i][0]) == 0 )
        {//找到了对应的基础类型
            long long  t = 0;
            t = reinterpret_cast<long long>(const_cast<char*>( typeConvert[i][3]));
            if((buff[0].find("*") != std::string::npos || buff[0].find("[]") != std::string::npos ) &&
                    ret != TYPE_string)
                t++;
            ret = static_cast<TYPE_e>(t);
            return ret;
        }else if(buff.size() == 3 && buff[0].find(typeConvert[i][0]) == 0 ){
            long long  t = 0;
            t = reinterpret_cast<long long>(const_cast<char*>( typeConvert[i][3]));
            if((buff[1].find("*") != std::string::npos || buff[1].find("[]") != std::string::npos ) &&
                    ret != TYPE_string)
                t++;
            ret = static_cast<TYPE_e>(t);
            return  ret;
        }

    }
    if(buff.size() == 1 && (buff[0].find("*") != std::string::npos || buff[0].find("[]") != std::string::npos))
        ret = TYPE_others;
    else if(buff.size() == 2 && (buff[1].find("*") != std::string::npos || buff[1].find("[]") != std::string::npos))
        ret = TYPE_others;
    else if(buff.size() <= 2)
        ret = TYPE_other;
    else {
        ret = TYPE_Err;
    }
    return ret;
}

const MyString TypeDefine::getStrToC_file(void)
{
    MyString ret;
    for(size_t i = 0;i<subType.size();i++)
    {
        if(subType[i].getTypeENum() == TYPE_other || subType[i].getTypeENum() == TYPE_others)
        {
            ret.append(subType[i].getStrToC_file());
        }
    }
    ret.append(getNewfunctionStr());
    ret.append(getDeletefunctionStr());
    ret.append(getTodatafunctionStr());
    ret.append(getFillfunctionStr());


    return  ret;
}
const MyString TypeDefine::getStrToH_file(void)
{
    MyString ret;
    for(size_t i = 0;i<subType.size();i++)
    {
        if(subType[i].getTypeENum() == TYPE_other || subType[i].getTypeENum() == TYPE_others)
        {
            ret.append(subType[i].getStrToH_file());
        }
    }
    ret.append("\ntypedef struct \n{\n");
    for(size_t i = 0;i<subType.size();i++)
    {
        if(subType[i].getArrayType() == true)
           ret.append("\tunsigned int "+subType[i].getVarName()+"_lengh ;\n");
        ret.append("\t"+subType[i].getTypeName_Cfamily()+" ");
        if(subType[i].getArrayType() == true)
           ret.append("* ");
        ret.append(subType[i].getVarName()+";"+subType[i].getComment()+"\n");
    }
//    if(typeNum == TYPE_SendRet)
//    {
//        ret.append("\tvoid (*delete)(PTC_"+tpName+"_t* t);    ///< delete this typestruct\n");
//        ret.append("\tunsigned char* (*todata)(PTC_"+tpName+"_t* t,unsigned int* len);    ///< todata this typestruct\n");
//        ret.append("\tPTC_"+tpName+"_t* (*fill)(PTC_"+tpName+"_t* t,unsigned char* data,unsigned int len);    ///< delete this typestruct\n");
//    }
    ret.append("\n} PTC_"+tpName+"_t ;"+comment+"\n");
    if(typeNum == TYPE_others || typeNum == TYPE_SendRet)
    {
        ret.append("\n\n"+getTypeName_Cfamily()+" * PTC_new"+tpName+"(void);\n");
        ret.append("void PTC_delete"+tpName+"("+getTypeName_Cfamily()+" * m);\n\n");
        ret.append("unsigned char* PTC_todata"+tpName+"("+
                   getTypeName_Cfamily()+"* t,unsigned int * len);\n");
        ret.append("PTC_"+tpName+"_t* PTC_fill"+tpName+"("+
                   getTypeName_Cfamily()+"* t,unsigned char * data,unsigned int len);\n");
    }

    return ret;
}

const MyString TypeDefine::getTypeName_Cfamily()
{
    MyString ret;
    if(typeNum == TYPE_Err)
        return ret;
    else if (typeNum == TYPE_other || typeNum == TYPE_others||
             typeNum == TYPE_SendRet) {
        ret = "PTC_"+tpName+"_t ";
    }
    else{
        ret = typeConvert[typeNum>>1][1];
    }
    return  ret;
}

const MyString TypeDefine::getNewfunctionStr()
{
    MyString ret;
    if(typeNum == TYPE_SendRet || typeNum == TYPE_others)
    {
        ret.append(getTypeName_Cfamily()+" * PTC_new"+tpName+"() \n{");
        ret.append("\t"+getTypeName_Cfamily()+"* ret = NULL;\n");
        ret.append("\t ret = PCT_malloc(sizeof("+getTypeName_Cfamily()+"));\n");
        ret.append("\t PCT_memset(ret,0,sizeof("+getTypeName_Cfamily()+"));\n");
        ret.append("\t return ret;\n");
        ret.append("\n}");
    }
    return  ret;
}
const MyString TypeDefine::getDeletefunctionStr()
{
    MyString ret;
    if(typeNum == TYPE_SendRet || typeNum == TYPE_others)
    {

    }
    return  ret;
}
const MyString TypeDefine::getTodatafunctionStr()
{
    MyString ret;
    if(typeNum == TYPE_SendRet || typeNum == TYPE_others)
    {

    }
    return  ret;
}
const MyString TypeDefine::getFillfunctionStr()
{
    MyString ret;
    if(typeNum == TYPE_SendRet || typeNum == TYPE_others)
    {

    }
    return  ret;
}
