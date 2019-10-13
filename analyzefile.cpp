#include "analyzefile.h"
#include <iostream>
#include <string>
#include <sstream>
#include "mystring.h"
AnalyzeFile::AnalyzeFile()
{
    MyString a = "1234567";
    std::cout<<a<<std::endl;
    a.replace("1","qq");
    std::cout<<a<<std::endl;

}
AnalyzeFile::~AnalyzeFile()
{
    if(inputfile != nullptr)
        delete inputfile;
}
AnalyzeFile::AnalyzeFile(char* inputfilePath)
{
    setInputfile(inputfilePath);
}
AnalyzeFile::AnalyzeFile(std::string inputfilePath)
{
    setInputfile(inputfilePath.data());
}
bool AnalyzeFile::setInputfile(const MyString & inputfilePath)
{
    if(inputfile == nullptr)
        inputfile = new std::ifstream(inputfilePath,std::ios::in );
    else
    {
        inputfile->close();
        inputfile->open(inputfilePath,std::ios::in );
    }
    pushTo_interfaceLib(inputfile);
    return true;
}

bool AnalyzeFile::outputC_file(MyString path )
{
    std::cout<<"filepath :"<<path<<"XXX.c"<<std::endl;
    path.append("protocol.c");
    MyString text = "#include \"protocol.h\"\n";

    for(size_t i = 0;i<interfaceLib.size();i++)
    {
        text.append(interfaceLib[i].getStrToC_file());
    }


    std::ofstream out(path);
    if(out.is_open())
    {
        out<<text<<std::endl;
    }else {
        return  false;
    }
    out.close();
    return true;
}
bool AnalyzeFile::outputH_file(MyString path)
{
    std::cout<<"filepath :"<<path<<"protocol.h"<<std::endl;
    path.append("protocol.h");
    MyString text = "#ifndef _PROTOCOL_H_ \n#define _PROTOCOL_H_\n\n";
    text.append("typedef enum \n{\n\tTRUE = 1,\n\tFALSE = !TRUE\n} Boolean;\n\n");
    text.append("///------------------需要实现的几个功能函数---------------//\n");
    text.append("extern void* PTC_malloc(unsigned long long size);\n");
    text.append("extern void PTC_free(void* f);\n");
    text.append("extern void PTC_memset(void* des,int v,unsigned long long len);\n");
    text.append("extern void* PTC_memcpy(void* des,void* src,unsigned long long len);\n\n\n");

    for(size_t i = 0;i<interfaceLib.size();i++)
    {
        text.append(interfaceLib[i].getStrToH_file());
    }
    text.append("\n\n#endif\n\n");
    std::cout << "out of H file :"<<text<<std::endl;
    std::ofstream out(path);
    if(out.is_open())
    {
        out<<text<<std::endl;
    }else {
        return  false;
    }
    out.close();
    return true;
}
bool AnalyzeFile::outputCPP_file(MyString path )
{
    return true;
}

bool AnalyzeFile::outputHPP_file(MyString path )
{
    return true;
}
bool AnalyzeFile::outputJava_file(MyString path  )
{
    return true;
}


/**
 ***********************************************************************************************
 * @brief pushTo_interfaceLib
 * 函数会将文件流中的内容读取出来,分割成很多段文本,一段文本就是一个接口协议
 * 将代表某一段文本的接口协议灌入到Typedefine 中,Typedifine 应该可以
 * 解析该通讯协议的接口
 * @param ifstream *        统一格式的接口文件流
 * @return int              文件中符合要求规范的接口的数量
 * @author kimalittle@gmail.com
 * @date 2019-10-07
 * @note mark your ideal
 **********************************************************************************************
 */
int AnalyzeFile::pushTo_interfaceLib(std::ifstream* f)
{
    MyString text;
    std::stringstream bf;
    bf << f->rdbuf();
    text = bf.str();
    text.append("\0");
    std::vector<MyString> * facelist = getInterfaceList(text);
    std::cout<<"list size = "<<facelist->size()<< "text content:"<<text<<"\n "<<std::endl;

    for(std::size_t i = 0;i<facelist->size();i++)
    {
        Interface *iface = new Interface();
        iface->fillInterface(facelist->at(i));
        /// @todo 校验接口号是否存在 不存在才能继续加入;
        interfaceLib.push_back(*iface);
        delete iface;
    }
    return static_cast<int>(facelist->size());
}

std::vector<MyString>* AnalyzeFile::getInterfaceList(const MyString& text)
{
    std::vector<MyString>* ret = new std::vector<MyString>();

    size_t blockcomment_star = text.find("/*");
    size_t blockcomment_end = text.find("*/");
    filecomment = text.mid(blockcomment_star,blockcomment_end-blockcomment_star+2);
    std::cout<<"filecomment == "<<filecomment <<std::endl;
    //开始真正寻找接口字符块

    size_t istar = 0,iend = 0;
    int bracketsDeep = 0;
    int commentDeep = 0;
    for(size_t i = blockcomment_end;i < (text.size()-1);i++)
    {
        if(text[i] == '/' && text[i+1] == '*' && commentDeep == 0)
        {
            commentDeep = 1;
            istar = i;
        }
        if(text[i] == '*' && text[i+1] == '/' && commentDeep == 1)
        {
            commentDeep = 0;
        }
        if(commentDeep == 0 && ((text[i]>='A' && text[i] <= 'Z') || (text[i]>='a' && text[i] <= 'z')))
        {
            if(istar == 0)
                istar = i;
        }
        if(text[i] == '{' && commentDeep == 0)
        {
            bracketsDeep++;
        }else if(text[i] == '}' && commentDeep == 0)
        {
            bracketsDeep--;
            if(bracketsDeep == 0)
            {
                iend = i+1;
                ret->push_back(text.mid(istar,iend-istar));
                istar = 0;
                iend = 0;
            }
        }
    }
    return ret;

}
