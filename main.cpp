#include <iostream>
#include <analyzefile.h>
#include <string.h>
using namespace std;


int main(int argn,char * args[])
{
    cout << "Hello World!" << endl;
    MyString interfaceFilePath = "u8 x";
    std::vector<MyString> buff = interfaceFilePath.splits(" ");
    std::cout<<buff.size();
    if(argn == 2)
    {
        interfaceFilePath = args[1];

    }else if(argn == 1)
    {

        std::cout<< "please input your file"<<std::endl;
//        std::cin>>interfaceFilePath;
//        interfaceFilePath = "D:/kim_a/Documents/Qt_Pro/AutoAnalyzeInterface/Interface.txt";
        interfaceFilePath = "../AutoAnalyzeInterface/Interface.txt";

    }
    AnalyzeFile af(interfaceFilePath);
    char* indexOfdir = strrchr(interfaceFilePath.data(),'\\');
    if(indexOfdir == nullptr)
        indexOfdir = strrchr(interfaceFilePath.data(),'/');
    if(indexOfdir == nullptr)
        return -1;
    indexOfdir++;               //跳过 文件夹符号  '/'
    interfaceFilePath.erase(static_cast<size_t>(indexOfdir-interfaceFilePath.data()),
                            interfaceFilePath.size()-static_cast<size_t>(indexOfdir-interfaceFilePath.data()));
    af.outputC_file(interfaceFilePath);
    af.outputH_file(interfaceFilePath);
    af.outputCPP_file(interfaceFilePath);
    af.outputHPP_file(interfaceFilePath);
    std::getchar();
    return 0;
}
#include "protocol.hpp"
#include <string.h>
void* ptc::PTC_malloc(unsigned long long size)
{
    return std::malloc(size);
}
void ptc::PTC_free(void* f)
{
    std::free(f);
}
void ptc::PTC_memset(void* des,int v,unsigned long long len)
{
    memset(des,v,len);
}
void* ptc::PTC_realloc(void* src,unsigned long long new_size)
{
    return std::realloc(src,new_size);
}
void* ptc::PTC_memcpy(void* des,void* src,unsigned long long len)
{

    return memcpy(des,src,len);
}
