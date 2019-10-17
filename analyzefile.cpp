#include "analyzefile.h"
#include "mystring.h"
#include <iostream>
#include <sstream>
#include <string>
AnalyzeFile::AnalyzeFile()
{
    MyString a = "1234567";
    std::cout << a << std::endl;
    a.replace("1", "qq");
    std::cout << a << std::endl;
}
AnalyzeFile::~AnalyzeFile()
{
    if (inputfile != nullptr)
        delete inputfile;
}
AnalyzeFile::AnalyzeFile(char *inputfilePath)
{
    setInputfile(inputfilePath);
}
AnalyzeFile::AnalyzeFile(std::string inputfilePath)
{
    setInputfile(inputfilePath.data());
}
bool AnalyzeFile::setInputfile(const MyString &inputfilePath)
{
    if (inputfile == nullptr)
        inputfile = new std::ifstream(inputfilePath, std::ios::in);
    else
    {
        inputfile->close();
        inputfile->open(inputfilePath, std::ios::in);
    }
    pushTo_interfaceLib(inputfile);
    return true;
}

bool AnalyzeFile::outputC_file(MyString path, MyString filename, MyString incfile)
{
    std::cout << "filepath :" << path << "XXX.c" << std::endl;
    path.append(filename);
    MyString text = "#include \"" + incfile + "\"\n";
    text.append("#ifdef __cplusplus\nusing namespace ptc;\nextern \"C\"\n{\n#endif\n");
    text.append("PTC_DECLARE_WRITE(u32,PTC_u32)\n");
    text.append("PTC_DECLARE_WRITE(s32,PTC_s32)\n");
    text.append("PTC_DECLARE_WRITE(u16, PTC_u16 )\n");
    text.append("PTC_DECLARE_WRITE(s16, PTC_s16)\n");
    text.append("PTC_DECLARE_WRITE(u8, PTC_u8 )\n");
    text.append("PTC_DECLARE_WRITE(s8, PTC_s8)\n");
    text.append("PTC_DECLARE_WRITE(u64, PTC_u64 )\n");
    text.append("PTC_DECLARE_WRITE(bool, Boolean)\n");
    text.append("PTC_DECLARE_WRITE(string, PTC_s8)\n");
    text.append("PTC_DECLARE_WRITE(f32, PTC_f32)\n\n\n");
    text.append("PTC_DECLARE_READ(u32,PTC_u32)\n");
    text.append("PTC_DECLARE_READ(s32,PTC_s32)\n");
    text.append("PTC_DECLARE_READ(u16, PTC_u16 )\n");
    text.append("PTC_DECLARE_READ(s16, PTC_s16)\n");
    text.append("PTC_DECLARE_READ(u8, PTC_u8 )\n");
    text.append("PTC_DECLARE_READ(s8, PTC_s8)\n");
    text.append("PTC_DECLARE_READ(u64, PTC_u64 )\n");
    text.append("PTC_DECLARE_READ(bool, Boolean)\n");
    text.append("PTC_DECLARE_READ(string, PTC_s8)\n");
    text.append("PTC_DECLARE_READ(f32, PTC_f32)\n\n\n");
    for (size_t i = 0; i < interfaceLib.size(); i++)
    {
        text.append(interfaceLib[i].getStrToC_file());
    }

    text.append(getCallbackFuncDeclare());

    text.append("#ifdef __cplusplus\n}\n#endif\n");

    std::ofstream out(path);
    if (out.is_open())
    {
        out << text << std::endl;
    }
    else
    {
        return false;
    }
    out.close();
    return true;
}
bool AnalyzeFile::outputH_file(MyString path, MyString filename, MyString incfile)
{
    std::cout << "filepath :" << path << "protocol.h" << std::endl;
    path.append(filename);
    MyString text = "#ifndef PROTOCOL_H_INTERFACE_ \n#define PROTOCOL_H_INTERFACE_\n\n";
    text.append("#ifdef __cplusplus\nnamespace ptc {\nextern \"C\"\n{\n#endif\n\n");
    text.append("#include<string.h>\n#include<" + incfile + ">\n");

    text.append(getDefineBaseHfile());
    text.append(getCallbackFuncDefine());

    for (size_t i = 0; i < interfaceLib.size(); i++)
    {
        text.append(interfaceLib[i].getStrToH_file());
    }
    text.append("#ifdef __cplusplus\n}\n}\n#endif");
    text.append("\n\n#endif\n\n");

    std::cout << "out of H file :" << text << std::endl;

    std::ofstream out(path);
    if (out.is_open())
    {
        out << text << std::endl;
    }
    else
    {
        return false;
    }
    out.close();
    return true;
}
bool AnalyzeFile::outputCPP_file(MyString path)
{
    outputC_file(path, "protocol.cpp", "protocol.hpp");
    return true;
}

bool AnalyzeFile::outputHPP_file(MyString path)
{
    outputH_file(path, "protocol.hpp");

    return true;
}
bool AnalyzeFile::outputJava_file(MyString path)
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
int AnalyzeFile::pushTo_interfaceLib(std::ifstream *f)
{
    MyString text;
    std::stringstream bf;
    bf << f->rdbuf();
    text = bf.str();
    text.append("\0");
    std::vector<MyString> *facelist = getInterfaceList(text);
    std::cout << "list size = " << facelist->size() << "text content:" << text << "\n " << std::endl;

    for (std::size_t i = 0; i < facelist->size(); i++)
    {
        Interface *iface = new Interface();
        iface->fillInterface(facelist->at(i));
        /// @todo 校验接口号是否存在 不存在才能继续加入;
        interfaceLib.push_back(*iface);
        delete iface;
    }
    return static_cast<int>(facelist->size());
}

std::vector<MyString> *AnalyzeFile::getInterfaceList(const MyString &text)
{
    std::vector<MyString> *ret = new std::vector<MyString>();

    size_t blockcomment_star = text.find("/*");
    size_t blockcomment_end = text.find("*/");
    filecomment = text.mid(blockcomment_star, blockcomment_end - blockcomment_star + 2);
    std::cout << "filecomment == " << filecomment << std::endl;
    //开始真正寻找接口字符块

    size_t istar = 0, iend = 0;
    int bracketsDeep = 0;
    int commentDeep = 0;
    for (size_t i = blockcomment_end; i < (text.size() - 1); i++)
    {
        if (text[i] == '/' && text[i + 1] == '*' && commentDeep == 0)
        {
            commentDeep = 1;
            istar = i;
        }
        if (text[i] == '*' && text[i + 1] == '/' && commentDeep == 1)
        {
            commentDeep = 0;
        }
        if (commentDeep == 0 && ((text[i] >= 'A' && text[i] <= 'Z') || (text[i] >= 'a' && text[i] <= 'z')))
        {
            if (istar == 0)
                istar = i;
        }
        if (text[i] == '{' && commentDeep == 0)
        {
            bracketsDeep++;
        }
        else if (text[i] == '}' && commentDeep == 0)
        {
            bracketsDeep--;
            if (bracketsDeep == 0)
            {
                iend = i + 1;
                ret->push_back(text.mid(istar, iend - istar));
                istar = 0;
                iend = 0;
            }
        }
    }
    return ret;
}

const MyString AnalyzeFile::getDefineBaseHfile()
{
    MyString text;
    //reinterpret
    ///< 定义NULL 指针

    text.append("#ifndef PTC_NULL\n#ifdef __cplusplus\n"
                "#define PTC_NULL nullptr\n#else\n/* CPP */\n"
                "#define PTC_NULL ((void *)0)\n#endif\n#endif\n\n");
    text.append("#ifdef __cplusplus\n#define PTC_REPOINT(TYPE,VALUE) reinterpret_cast<TYPE>( VALUE )\n");
    text.append("#else\n#define PTC_REPOINT(TYPE,VALUE) (TYPE )( VALUE )\n#endif \n");
    ///< 定义Boolean 自定义结构类
    text.append("typedef enum \n{\n\tTRUE = 1,\n\tFALSE = !TRUE\n} Boolean;\n\n");

    ///< 自定义各种类型
    text.append("typedef unsigned char  PTC_u8;\n");
    text.append("typedef unsigned short PTC_u16;\n");
    text.append("typedef unsigned int   PTC_u32;\n");
    text.append("typedef signed char    PTC_s8;\n");
    text.append("typedef signed short   PTC_s16;\n");
    text.append("typedef signed int   PTC_s32;\n");
    text.append("typedef unsigned long long PTC_u64;\n");
    text.append("typedef float PTC_f32;\n\n\n");

    text.append("#define PTC_PRINTF printf\n\n");

    ///< 需要实现的几个接口
    text.append("///------------------需要实现的几个功能函数---------------//\n");
    text.append("extern void* PTC_malloc(PTC_u64 size);\n");
    text.append("extern void PTC_free(void* f);\n");
    text.append("extern void PTC_memset(void* des,PTC_s32 v,PTC_u64 len);\n");
    text.append("extern void* PTC_realloc(void* src,PTC_u64 new_size);\n");
    text.append("extern void* PTC_memcpy(void* des,void* src,PTC_u64 len);\n\n\n");

    ///< 宏定义展开的函数
    text.append("#define PTC_DECLARE_WRITE(T, TureT)                           \\\n");
    text.append("    inline void PTC_write##T(PTC_u8 *des, TureT val) \\\n");
    text.append("    {                                                         \\\n");
    text.append("        union {                                               \\\n");
    text.append("            TureT v;                                        \\\n");
    text.append("            PTC_u8 ch[sizeof( val )];                    \\\n");
    text.append("        } un;                                                 \\\n");
    text.append("        un.v = val;                                           \\\n");
    text.append("        PTC_memcpy(des, &un, sizeof( un ));                     \\\n");
    text.append("    } //The code bloats out how to write individual elements;\n");

    ///< 宏定义展开的函数
    text.append("#define PTC_DECLARE_READ(T, TureT)                \\\n");
    text.append("    inline TureT PTC_read##T(PTC_u8 *src) \\\n");
    text.append("    {                                             \\\n");
    text.append("        union {                                   \\\n");
    text.append("            TureT v;                            \\\n");
    text.append("            PTC_u8 ch[sizeof( TureT )];    \\\n");
    text.append("        } un;                                     \\\n");
    text.append("        PTC_memcpy(un.ch, src, sizeof( un ));       \\\n");
    text.append("        return un.v;                              \\\n");
    text.append("    } //Code bloat, the way individual elements are read at the bloat;\n");

    ///< 宏定义 自定义结构体 todata 函数
    text.append("#define PTC_DECLARE_TODATA(T)                                            \\\n");
    text.append("    inline PTC_u8 *PTC_todata##T(PTC_##T##_t *t, PTC_u32 *len) \\\n");
    text.append("    {                                                                    \\\n");
    text.append("        PTC_u8 *ret = PTC_NULL;                                       \\\n");
    text.append("        union {                                                          \\\n");
    text.append("            PTC_u8 ch[sizeof(*t)];                                \\\n");
    text.append("            PTC_##T##_t v;                                               \\\n");
    text.append("        } un;                                                            \\\n");
    text.append("        un.v = *t;                                                       \\\n");
    text.append("        ret = PTC_REPOINT(PTC_u8 *,PTC_malloc(sizeof(un)));                   \\\n");
    text.append("        PTC_memcpy(ret, un.ch, sizeof(un));                              \\\n");
    text.append("        (*len) += sizeof(un);                                            \\\n");
    text.append("        return ret;                                                      \\\n");
    text.append("    }\n\n");

    ///< 宏定义 fill 结构体函数
    text.append("#define PTC_DECLARE_FILL(T)                                                            \\\n");
    text.append("    inline PTC_##T##_t *PTC_fill##T (PTC_##T##_t *t, PTC_u8 *data, PTC_u32* len) \\\n");
    text.append("    {                                                                                  \\\n");
    text.append("        PTC_##T##_t *ret = t;                                                            \\\n");
    text.append("        union {                                                                        \\\n");
    text.append("            PTC_u8 ch[sizeof(*t)];                                              \\\n");
    text.append("            PTC_##T##_t v;                                                               \\\n");
    text.append("        } un;                                                                          \\\n");
    text.append("        PTC_memcpy(un.ch, data, sizeof(un));                                           \\\n");
    text.append("        if (ret == PTC_NULL)                                                               \\\n");
    text.append("        ret = PTC_REPOINT(PTC_##T##_t *,PTC_malloc(sizeof(un)));                                 \\\n");
    text.append("        *ret = un.v;                                                                   \\\n");
    text.append("        (*len) += sizeof(un);                                                          \\\n");
    text.append("        return ret;                                                                    \\\n");
    text.append("    }\n\n");

    ///< 提前声明需要定义的函数
    text.append("extern void PTC_writeu8(PTC_u8 *des, PTC_u8 val);\n");
    text.append("extern void PTC_writes8(PTC_u8 *des, PTC_s8 val);\n");
    text.append("extern void PTC_writeu16(PTC_u8 *des, PTC_u16 val);\n");
    text.append("extern void PTC_writes16(PTC_u8 *des, PTC_s16 val);\n");
    text.append("extern void PTC_writeu32(PTC_u8 *des, PTC_u32 val);\n");
    text.append("extern void PTC_writes32(PTC_u8 *des, PTC_s32 val);\n");
    text.append("extern void PTC_writef32(PTC_u8 *des, PTC_f32 val);\n");
    text.append("extern void PTC_writebool(PTC_u8 *des, Boolean val);\n");
    text.append("extern void PTC_writestring(PTC_u8 *des, PTC_s8 val);\n");
    text.append("extern void PTC_writeu64(PTC_u8 *des,PTC_u64 val);\n\n");

    text.append("extern PTC_u8 PTC_readu8(PTC_u8 *src);\n");
    text.append("extern PTC_s8 PTC_reads8(PTC_u8 *src);\n");
    text.append("extern PTC_u16 PTC_readu16(PTC_u8 *src);\n");
    text.append("extern PTC_s16 PTC_reads16(PTC_u8 *src);\n");
    text.append("extern PTC_u32 PTC_readu32(PTC_u8 *src);\n");
    text.append("extern PTC_s32 PTC_reads32(PTC_u8 *src);\n");
    text.append("extern PTC_f32 PTC_readf32(PTC_u8 *src);\n");
    text.append("extern Boolean PTC_readbool(PTC_u8 *src);\n");
    text.append("extern PTC_s8 PTC_readstring(PTC_u8 *src);\n");
    text.append("extern PTC_u64 PTC_readu64(PTC_u8 *src);\n\n");

    return text;
}

const MyString AnalyzeFile::getCallbackFuncDefine()
{
    MyString ret;
    MyString fundef = "void PTC_InterfaceProcess(PTC_u8* data,PTC_InterFace_e id,Boolean isSend);\
    \t\t// all data process function enter way;\n\n";
    ret.append("\n\ntypedef enum\n{\n\t");
    for (size_t i = 0; i < interfaceLib.size(); i++)
    {
        ret.append("    PTC_" + interfaceLib[i].getInterfaceName() + "_e\t\t = " + std::to_string(interfaceLib[i].getInterFaceID()) + ",\n");
        fundef.append(interfaceLib[i].getCallbackFuncDefine());
    }
    ret.append("} PTC_InterFace_e;\n\n");
    ret.append(fundef);
    return ret;
}

const MyString AnalyzeFile::getCallbackFuncDeclare()
{
    MyString ret;
    MyString interfaceDeclare;
    ret.append("void PTC_InterfaceProcess(PTC_u8 *data, PTC_InterFace_e id, Boolean isSend)\n");
    ret.append("{\n");
    ret.append("    PTC_u32 len = 0;\n");
    ret.append("    switch (id)\n");
    ret.append("    {\n");
    for (size_t i = 0; i < interfaceLib.size(); i++)
    {
        ret.append("    case PTC_" + interfaceLib[i].getInterfaceName() + "_e:\n");
        ret.append("        PTC_" + interfaceLib[i].getInterfaceName() + "Callback(data, &len, isSend);\n");
        ret.append("        break;\n");
        interfaceDeclare.append(interfaceLib[i].getCallbackFuncDeclare());
    }
    ret.append("    \n");
    ret.append("    default:PTC_PRINTF(\"ERRORTYPE = %d\\n\\n\",id) ;\t// error type occur\n");
    ret.append("        break;\n");
    ret.append("    }\n");
    ret.append("}\n");
    ret.append(interfaceDeclare);
    return ret;
}
