#ifndef ANALYZEFILE_H
#define ANALYZEFILE_H

#include "interface.h"
#include "mystring.h"
#include <fstream>
#include <vector>
class AnalyzeFile
{
public:
    AnalyzeFile();
    ~AnalyzeFile();
    AnalyzeFile(char *inputfilePath);
    AnalyzeFile(std::string inputfilePath);

    bool setInputfile(const MyString &inputfilePath);

    bool outputC_file(MyString path, MyString filename = ("protocol.c"), MyString incfile = ("protocol.h"));
    bool outputH_file(MyString path, MyString filename = ("protocol.h"), MyString incfile = ("stdio.h"));
    bool outputCPP_file(MyString path);
    bool outputHPP_file(MyString path);
    bool outputJava_file(MyString path);

private:
    std::ifstream *inputfile = nullptr;
    std::vector<Interface> interfaceLib;

    MyString filecomment;
    const MyString getDefineBaseHfile();
    const MyString getCallbackFuncDefine();
    const MyString getCallbackFuncDeclare();
    int pushTo_interfaceLib(std::ifstream *f);
    std::vector<MyString> *getInterfaceList(const MyString &text);
};

#endif // ANALYZEFILE_H

/*
/ *
* -----------------
* ----------------------
** /
Ping = 0x01
{
    Send
    {
        u8 m1_u8;   //adfad;fkjad
    }
    Return
    {

    }
}
*/
