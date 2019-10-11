#include <iostream>
#include <analyzefile.h>
#include <string.h>
using namespace std;


int main(int argn,char * args[])
{
    cout << "Hello World!" << endl;
    MyString interfaceFilePath;
    if(argn == 2)
    {
        interfaceFilePath = args[1];

    }else if(argn == 1)
    {

        std::cout<< "please input your file"<<std::endl;
        std::cin>>interfaceFilePath;
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
    return 0;
}
