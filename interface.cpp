#include "interface.h"
#include <stdio.h>
#include <iostream>
Interface::Interface()
{
    interfaceID = 0;
    interfaceName.clear();
}

bool Interface::fillInterface(std::string str)
{
    std::cout<<"fillInterface"<<std::endl;
    size_t star = str.find("/*");
    size_t end = str.find("*/");
    if (star != end)
    {
        comment = str.substr(star, end - star + 2);
        str.erase(star, end - star + 2);
    }
    MyString ss = str;
    ss.replace("\t"," ");
    while(ss.replace("  "," ") != -1)
    {

    }
    ss.replace("\n ","\n");
    //    std::cout<<"str = \n\n"<<ss<<"\n\n";

    //    std::vector<MyString> buff = ss.splits("{=}");
    //    for(size_t i = 0;i<buff.size();i++)
    //    {
    //        std::cout << "buff ["<<i<<"] = "<<buff[i]<<";\n";
    //    }
    //    buff = buff[2].splits("{=}");
    //    for(size_t i = 0;i<buff.size();i++)
    //    {
    //        std::cout << "buff[2] ["<<i<<"] = "<<buff[i]<<";\n";
    //    }
    // 这个时候的字符串类似于这个样子:Ping = 0010{Send = {u8 data1;u8 data2;Sutdent{ u8 id;u16 name;} bob;}Return ={u32 errCode;}}

    end = 0;
    star = 0;
    std::vector<MyString> buff = ss.splits("{=\n }");
    if (buff.size() != 3)
        return false;
    buff[0].removeAll(" ");
    buff[1].removeAll(" ");
    interfaceName = buff[0];
    if ((buff[1][0] == '0') && (buff[1][1] == 'X' || buff[1][1] == 'x'))
    {
        std::sscanf(buff[1].c_str() + 2, "%x", &interfaceID);
    }
    else
    {
        std::sscanf(buff[1].c_str(), "%d", &interfaceID);
    }
    buff = buff[2].splits("{= \n}");
    if (buff.size() != 4)
        return false;
    if(buff[0].find("Send") != std::string::npos)
    {
        MyString tname = interfaceName;
        sendType.setTypeName(tname.append("_"+ buff[0]));
        sendType.setTypeENum(TYPE_SendRet);
        sendType.fillTypedef(&buff[1]);
    }
    if(buff[2].find("Return") != std::string::npos)
    {
        MyString tname = interfaceName;
        retType.setTypeName(tname.append("_"+ buff[2]));
        retType.setTypeENum(TYPE_SendRet);
        retType.fillTypedef(&buff[3]);
    }
    std::cout << "interface NAME =" << interfaceName << "ID = " << interfaceID << "\n**\n";
    return true;
}


const MyString Interface::getStrToC_file(void)
{
    MyString ret = sendType.getStrToC_file();
    ret.append(retType.getStrToC_file());
    return ret;
}
const MyString Interface::getStrToH_file(void)
{
    MyString ret = sendType.getStrToH_file();
    ret.append(retType.getStrToH_file());

    return ret;
}
