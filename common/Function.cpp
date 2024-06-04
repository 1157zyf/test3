/**
 * @file function.cpp
 * @author zenglj (zenglj@nwpu.edu.cn)
 * @brief 函数形参与函数管理
 * @version 0.1
 * @date 2023-09-24
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <algorithm>

#include "Function.h"
#include "SymbolTable.h"

/// @brief 默认整型参数
/// @param _name 形参的名字
FuncFormalParam::FuncFormalParam(std::string _name) : name(_name)
{
    type.type = BasicType::TYPE_INT;
}

/// @brief 基本类型的参数
/// @param _name 形参的名字
/// @param _type 基本类型
/// @param _val Value
FuncFormalParam::FuncFormalParam(std::string _name, BasicType _type, Value * _val) : name(_name), type(_type), val(_val)
{}

/// @brief 函数形参转字符串输出
/// @return
std::string FuncFormalParam::toString()
{
    std::string typeName;
    typeName = type.toString();

    // 类型名 空格 形参参数名
    return typeName + " " + name;
}

/// @brief 匿名构造函数
Function::Function()
{
    returnType.type = BasicType::TYPE_VOID;
}

/// @brief 指定有函数类型与名字的构造函数
/// @param _name
/// @param _type
/// @param _builtin
Function::Function(std::string _name, BasicType _type, bool _builtin) : name(_name), builtIn(_builtin)
{
    returnType.type = _type;
}

/// @brief 指定函数名字、函数返回类型以及函数形式参数的构造函数
/// @param _name
/// @param _type
/// @param _param
Function::Function(std::string _name, BasicType _type, FuncFormalParam _param, bool _builtin)
    : name(_name), builtIn(_builtin)
{
    returnType.type = _type;
    params.emplace_back(_param);
}

/// @brief 取得函数名字
/// @return 函数名字
std::string & Function::getName()
{
    return name;
}
/// @brief 获取函数返回类型
/// @return 返回类型
ValueType & Function::getReturnType()
{
    return returnType;
}

/// @brief 获取函数的形参列表
/// @return 形参列表
std::vector<FuncFormalParam> & Function::getParams()
{
    return params;
}

/// @brief 获取函数内的IR指令代码
/// @return IR指令代码
InterCode & Function::getInterCode()
{
    return code;
}

/// @brief 判断该函数是否是内置函数
/// @return true: 内置函数，false：用户自定义
bool Function::isBuiltin()
{
    return builtIn;
}

/// @brief 设置函数是否是内置或用户自定义
/// @param _builtin true: 内置函数 false: 用户自定义函数
void Function::setBuiltin(bool _builtin)
{
    builtIn = _builtin;
}

/// @brief 函数指令信息输出
/// @param str 函数指令
void Function::toString(std::string & str)
{
    if (builtIn) {
        // 内置函数则什么都不输出
        return;
    }

    // 输出函数头
    str = "define " + returnType.toString() + " " + name + "(";

    bool firstParam = false;
    for (auto & param: params) {

        if (!firstParam) {
            firstParam = true;
        } else {
            str += ", ";
        }

        std::string param_str = param.toString();

        str += param_str;
    }

    str += ")\n";

    str += "{\n";

    // 遍历所有的线性IR指令，文本输出
    for (auto & inst: code.getInsts()) {

        std::string instStr;
        inst->toString(instStr);

        if (!instStr.empty()) {

            // Label指令不加Tab键
            if (inst->getOp() == IRInstOperator::IRINST_OP_LABEL) {
                str += instStr + "\n";
            } else {
                str += "\t" + instStr + "\n";
            }
        }
    }

    // 输出函数尾部
    str += "}\n";
}

/// @brief 获取下一个Label名字
/// @return 下一个Label名字
std::string Function::getNextLabelName()
{
    std::string name = std::to_string(nextLabelNo);
    nextLabelNo++;
    return name;
}

/// @brief 获取下一个临时变量名字
/// @return 下一个临时变量名字
std::string Function::getNextTempVarName()
{
    std::string name = std::to_string(nextTempVarNo);
    nextTempVarNo++;
    return name;
}

/// @brief 设置函数出口指令
/// @param inst 出口Label指令
void Function::setExitLabel(IRInst * inst)
{
    exitLabel = inst;
}

/// @brief 获取函数出口指令
/// @return 出口Label指令
IRInst * Function::getExitLabel()
{
    return exitLabel;
}

/// @brief 设置函数返回值变量
/// @param val 返回值变量，要求必须是局部变量，不能是临时变量
void Function::setReturnValue(Value * val)
{
    returnValue = val;
}

/// @brief 获取函数返回值变量
/// @return 返回值变量
Value * Function::getReturnValue()
{
    return returnValue;
}

/// @brief 从函数内的局部变量中删除
/// @param val 变量Value
void Function::deleteVarValue(Value * val)
{
    auto pIter = varsMap.find(val->getName());
    if (pIter != varsMap.end()) {
        varsMap.erase(pIter);
    }

    auto pIter2 = std::find(varsVector.begin(), varsVector.end(), val);
    varsVector.erase(pIter2);
}

/// @brief 获取最大栈帧深度
/// @return 栈帧深度
int Function::getMaxDep()
{
    return maxDepth;
}

/// @brief 设置最大栈帧深度
/// @param dep 栈帧深度
void Function::setMaxDep(int dep)
{
    maxDepth = dep;

    // 设置函数栈帧被重定位标记，用于生成不同的栈帧保护代码
    relocated = true;
}

/// @brief 获取本函数需要保护的寄存器
/// @return 要保护的寄存器
std::vector<int32_t> & Function::getProtectedReg()
{
    return protectedRegs;
}

/// @brief 获取本函数需要保护的寄存器字符串
/// @return 要保护的寄存器
std::string & Function::getProtectedRegStr()
{
    return protectedRegStr;
}

/// @brief 获取函数调用参数个数的最大值
/// @return 函数调用参数个数的最大值
int Function::getMaxFuncCallArgCnt()
{
    return maxFuncCallArgCnt;
}

/// @brief 设置函数调用参数个数的最大值
/// @param count 函数调用参数个数的最大值
void Function::setMaxFuncCallArgCnt(int count)
{
    maxFuncCallArgCnt = count;
}

/// @brief 函数内是否存在函数调用
/// @return 是否存在函调用
bool Function::getExistFuncCall()
{
    return funcCallExist;
}

/// @brief 设置函数是否存在函数调用
/// @param exist true: 存在 false: 不存在
void Function::setExistFuncCall(bool exist)
{
    funcCallExist = exist;
}

/// @brief 新建变量型Value
/// @param name 变量ID
/// @param type 变量类型
Value * Function::newVarValue(std::string name, BasicType type)
{
    Value * retVal;

    retVal = findValue(name);
    if (retVal == nullptr) {
        retVal = new VarValue(name, type);
        insertValue(retVal);
    } else {
        // 已存在的Value，返回失败
        retVal = nullptr;
    }

    return retVal;
}

/// @brief Value插入到符号表中
/// @param name Value的名称
/// @param val Value信息
void Function::insertValue(Value * val)
{
    varsMap.emplace(val->name, val);
    varsVector.push_back(val);
}

/// @brief 新建一个匿名变量型的Value，并加入到符号表，用于后续释放空间
/// \param type 类型
/// \return 变量Value
Value * Function::newVarValue(BasicType type)
{
    // 创建匿名变量，肯定唯一，直接插入
    Value * var = new VarValue(type);

    insertValue(var);

    return var;
}

/// 新建一个临时型的Value，并加入到符号表，用于后续释放空间
/// \param intVal 整数值
/// \return 常量Value
Value * Function::newTempValue(BasicType type)
{
    // 肯定唯一存在，直接插入即可
    Value * temp = new TempValue(type);

    insertValue(temp);

    return temp;
}

/// 根据变量名取得当前符号的值。若变量不存在，则说明变量之前没有定值，则创建一个未知类型的值，初值为0
/// \param name 变量名
/// \param create true: 不存在返回nullptr；false：不存在则不创建
/// \return 变量对应的值
Value * Function::findValue(std::string name, bool create)
{
    Value * temp = nullptr;

    // 这里只是针对函数内的变量进行检查，如果要考虑全局变量，则需要继续检查symtab的符号表
    auto pIter = varsMap.find(name);
    if (pIter != varsMap.end()) {

        // 如果考虑作用域、存在重名的时候，需要从varsVector逆序检查到底用那个Value

        temp = pIter->second;
    }

    // 没有找到，并且指定了全局符号表，则继续查找
    if ((!temp) && symtab) {

        temp = symtab->findValue(name, false);
    }

    // 变量名没有找到
    if ((!temp) && create) {
        temp = newVarValue(name);
    }

    return temp;
}

/// @brief 设置符号表，以便全局符号查找
void Function::setSymtab(SymbolTable * _symtab)
{
    symtab = _symtab;
}