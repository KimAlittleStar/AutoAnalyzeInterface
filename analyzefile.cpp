#include "analyzefile.h"
#include <iostream>
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
    char txt[500];
    inputfile->read(txt,500);
    std::cout<<txt<<std::endl;
    return true;
}

bool AnalyzeFile::outputC_file(MyString path )
{
    std::cout<<"filepath :"<<path<<"XXX.c"<<std::endl;
    return true;
}
bool AnalyzeFile::outputH_file(MyString path)
{
    std::cout<<"filepath :"<<path<<"XXX.h"<<std::endl;
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
    *f>>text ;
    std::vector<MyString> * facelist = getInterfaceList(text);

    return static_cast<int>(facelist->size());
}

std::vector<MyString>* AnalyzeFile::getInterfaceList(const MyString& text)
{
    std::vector<MyString>* ret = new std::vector<MyString>();

    size_t blockcomment_star = text.find_first_of("/*");
    size_t blockcomment_end = text.find_first_of("*/");
    filecomment = text.mid(blockcomment_star,blockcomment_end-blockcomment_star+2);

    //开始真正寻找接口字符块

    size_t istar = 0,iend = 0;
    for(size_t i = blockcomment_end;i < (text.size()-1);i++)
    {
        if(text[i] == '/' && text[i+1] == '*')
        {
            istar = i;
        }
    }


}
