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
            ele.back().comment = buff[i];
        }else if(bufftype == TYPE_other || bufftype == TYPE_others)
        {
            TypeDefine sub;
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
            TypeElement newele;
            newele.typebase = typeConvert[bufftype>>1][0];
            std::vector<MyString> b = buff[i].splits(" ");
            newele.type_name = b.back();
            if((bufftype&0x01) == 1 || bufftype == TYPE_string)
                newele.isArray = 1;
            ele.push_back(newele);
        }
    }

    for(size_t i = 0;i<ele.size();i++)
    {
        std::cout<< tpName <<": ele ["<<i<<"] = base:"<<ele[i].typebase<<"; name:"<<
                    ele[i].type_name<<"; comm:"<<ele[i].comment<<"; Array:"<<ele[i].isArray<<"\n";
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
