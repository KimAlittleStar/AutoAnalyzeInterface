#include "typedefine.h"
#include <iostream>
TypeDefine::TypeDefine()
{

}

bool TypeDefine::fillTypedef(const MyString *str)
{
    // 这里的模式是 u8 ssss; //adsfad
    //              u16 ssss;
    std::cout<<"fillTypedef str = "<<*str<<std::endl;
    std::vector<MyString> buff = str->splits("{\n;}");
    for(size_t i = 0;i<buff.size();i++)
    {
        std::cout<<"str buff ["<<i<<"] = "<<buff[i]<<"\n";
    }
    judgeType(*str);
    return true;
}

TYPE_e TypeDefine::judgeType(const MyString& str) const
{
    size_t conversize = sizeof(typeConvert)/sizeof(typeConvert[0]);
    size_t compareindex = 0;
    for(size_t i = 0;i<conversize;i++)
    {
        //@todo 判断数据类型
        compareindex = str.find(typeConvert[i][0]);

    }
    return TYPE_s8;
}
