#ifndef MachineBaseIns_H
#define MachineBaseIns_H
#include <assert.h>
#include <cstring>
#include <iostream>
#include <map>
#include <set>
#include <vector>

// #define ENABLE_PRINT
#ifdef ENABLE_PRINT
#ifndef PRINT
#define PRINT(...)                                                                                                     \
    do {                                                                                                               \
        char message[256];                                                                                             \
        sprintf(message, __VA_ARGS__);                                                                                 \
        std::cerr << message << "\n";                                                                                  \
    } while (0)
#endif
#else
#ifndef PRINT
#define PRINT(...)
#endif
#endif

#ifndef ERROR
#define ERROR(...)                                                                                                     \
    do {                                                                                                               \
        char message[256];                                                                                             \
        sprintf(message, __VA_ARGS__);                                                                                 \
        std::cerr << "\033[;31;1m";                                                                                    \
        std::cerr << "ERROR: ";                                                                                        \
        std::cerr << "\033[0;37;1m";                                                                                   \
        std::cerr << message << "\n";                                                                                  \
        std::cerr << "\033[0;33;1m";                                                                                   \
        std::cerr << "File: \033[4;37;1m" << __FILE__ << ":" << __LINE__ << "\n";                                      \
        std::cerr << "\033[0m";                                                                                        \
        assert(false);                                                                                                 \
    } while (0)
#endif

#ifndef TODO
#define TODO(...)                                                                                                      \
    do {                                                                                                               \
        char message[256];                                                                                             \
        sprintf(message, __VA_ARGS__);                                                                                 \
        std::cerr << "\033[;34;1m";                                                                                    \
        std::cerr << "TODO: ";                                                                                         \
        std::cerr << "\033[0;37;1m";                                                                                   \
        std::cerr << message << "\n";                                                                                  \
        std::cerr << "\033[0;33;1m";                                                                                   \
        std::cerr << "File: \033[4;37;1m" << __FILE__ << ":" << __LINE__ << "\n";                                      \
        std::cerr << "\033[0m";                                                                                        \
        assert(false);                                                                                                 \
    } while (0)
#endif

#define ENABLE_LOG
#ifdef ENABLE_LOG
#ifndef Log
#define Log(...)                                                                                                       \
    do {                                                                                                               \
        char message[256];                                                                                             \
        sprintf(message, __VA_ARGS__);                                                                                 \
        std::cerr << "\033[;35;1m[\033[4;33;1m" << __FILE__ << ":" << __LINE__ << "\033[;35;1m "                       \
                  << __PRETTY_FUNCTION__ << "]";                                                                       \
        std::cerr << "\033[0;37;1m ";                                                                                  \
        std::cerr << message << "\n";                                                                                  \
        std::cerr << "\033[0m";                                                                                        \
    } while (0)
#endif
#else
#ifndef Log
#define Log(...)
#endif
#endif

#define ENABLE_LOG
#ifdef ENABLE_LOG
#ifndef Lazy
#define Lazy(str)                                                                                                      \
    do {                                                                                                               \
        Log("\033[;31;1m%s\033[0m", str);                                                                              \
    } while (0)
#endif
#else
#ifndef Lazy
#define Lazy(str)
#endif
#endif

#ifndef Assert
#define Assert(EXP)                                                                                                    \
    do {                                                                                                               \
        if (!(EXP)) {                                                                                                  \
            ERROR("Assertion failed: %s", #EXP);                                                                       \
        }                                                                                                              \
    } while (0)
#endif

typedef unsigned __int128 Uint128;
typedef unsigned long long Uint64;
typedef unsigned int Uint32;

// 机器数据类型的定义
struct MachineDataType {
    // 枚举值：数据类型和数据长度
    enum { INT, FLOAT };        // 数据类型可以是整数或浮点数
    enum { B32, B64, B128 };    // 数据长度分别表示 32 位、64 位、128 位

    unsigned data_type;      // 数据类型 (INT/FLOAT)
    unsigned data_length;    // 数据长度 (B32/B64/B128)

    // 构造函数
    MachineDataType() {}
    MachineDataType(const MachineDataType &other) {
        this->data_type = other.data_type;
        this->data_length = other.data_length;
    }
    MachineDataType(unsigned data_type, unsigned data_length) : data_type(data_type), data_length(data_length) {}

    // 运算符重载
    MachineDataType operator=(const MachineDataType &other) {
        this->data_type = other.data_type;
        this->data_length = other.data_length;
        return *this;
    }
    bool operator==(const MachineDataType &other) const {
        return this->data_type == other.data_type && this->data_length == other.data_length;
    }

    // 获取数据宽度(字节数), 32位占4字节, 64位占8字节, 128位占16字节
    int getDataWidth() {
        switch (data_length) {
        case B32:
            return 4;
        case B64:
            return 8;
        case B128:
            return 16;
        }
        return 0;
    }
    // i32, i64 ...
    std::string toString() {
        std::string ret;
        if (data_type == INT)
            ret += 'i';
        if (data_type == FLOAT)
            ret += 'f';
        if (data_length == B32)
            ret += "32";
        if (data_length == B64)
            ret += "64";
        if (data_length == B128)
            ret += "128";
        return ret;
    }
};

// 声明一些常见的全局数据类型变量
extern MachineDataType INT32, INT64, INT128, FLOAT_32, FLOAT64, FLOAT128;

// 定义寄存器结构体
struct Register {
public:
    int reg_no;              // 寄存器编号
    bool is_virtual;         // 标识是否为虚拟寄存器
    MachineDataType type;    // 寄存器的数据类型

    // 构造函数
    Register() {}
    Register(bool is_virtual, int reg_no, MachineDataType type, bool save = false)
        : is_virtual(is_virtual), reg_no(reg_no), type(type) {}
    Register(const Register &other) {
        this->is_virtual = other.is_virtual;
        this->reg_no = other.reg_no;
        this->type = other.type;
    }

    // 获取寄存器的数据宽度
    int getDataWidth() { return type.getDataWidth(); }

    // 运算符重载
    Register operator=(const Register &other) {
        this->is_virtual = other.is_virtual;
        this->reg_no = other.reg_no;
        this->type = other.type;
        return *this;
    }
    bool operator<(Register other) const {
        if (is_virtual != other.is_virtual)
            return is_virtual < other.is_virtual;
        if (reg_no != other.reg_no)
            return reg_no < other.reg_no;
        if (type.data_type != other.type.data_type)
            return type.data_type < other.type.data_type;
        if (type.data_length != other.type.data_length)
            return type.data_length < other.type.data_length;
        return false;
    }
    bool operator==(Register other) const {
        return reg_no == other.reg_no && is_virtual == other.is_virtual && type.data_type == other.type.data_type &&
               type.data_length == other.type.data_length;
    }
};

// 定义机器操作数基类
struct MachineBaseOperand {
    MachineDataType type;    // 操作数的数据类型
    enum { REG, IMMI, IMMF, IMMD };    // 操作数类型：寄存器、整型立即数、浮点立即数、双精度浮点数，对应下面4个派生类
    int op_type;    // 当前操作数类型

    MachineBaseOperand(int op_type) : op_type(op_type) {}
    virtual std::string toString() = 0;
};

// 寄存器操作数
struct MachineRegister : public MachineBaseOperand {
    Register reg;
    MachineRegister(Register reg) : MachineBaseOperand(MachineBaseOperand::REG), reg(reg) {}
    std::string toString() {
        if (reg.is_virtual)
            return "%" + std::to_string(reg.reg_no);    // 虚拟寄存器
        else
            return "phy_" + std::to_string(reg.reg_no);    // 物理寄存器
    }
};

// 整型立即数
struct MachineImmediateInt : public MachineBaseOperand {
    int imm32;
    MachineImmediateInt(int imm32) : MachineBaseOperand(MachineBaseOperand::IMMI), imm32(imm32) {}
    std::string toString() { return std::to_string(imm32); }
};

// 浮点立即数
struct MachineImmediateFloat : public MachineBaseOperand {
    float fimm32;
    MachineImmediateFloat(float fimm32) : MachineBaseOperand(MachineBaseOperand::IMMF), fimm32(fimm32) {}
    std::string toString() { return std::to_string(fimm32); }
};

// 双精度浮点立即数
struct MachineImmediateDouble : public MachineBaseOperand {
    double dimm64;
    MachineImmediateDouble(double dimm64) : MachineBaseOperand(MachineBaseOperand::IMMD), dimm64(dimm64) {}
    std::string toString() { return std::to_string(dimm64); }
};

// 定义基础指令类
class MachineBaseInstruction {
public:
    enum { ARM = 0, RiscV, PHI };    // 支持的架构类型
    const int arch;                  // 指令所属架构

private:
    int ins_number;    // 指令编号, 用于活跃区间计算

public:
    void setNumber(int ins_number) { this->ins_number = ins_number; }
    int getNumber() { return ins_number; }

    // 构造函数
    MachineBaseInstruction(int arch) : arch(arch) {}
    virtual std::vector<Register *> GetReadReg() = 0;     // 获得该指令所有读的寄存器
    virtual std::vector<Register *> GetWriteReg() = 0;    // 获得该指令所有写的寄存器
    virtual int GetLatency() = 0;    // 如果你不打算实现指令调度优化，可以忽略该函数
};

// PHI 指令类
class MachinePhiInstruction : public MachineBaseInstruction {
private:
    Register result;                                               // PHI 指令结果寄存器
    std::vector<std::pair<int, MachineBaseOperand *>> phi_list;    // PHI 分支列表

public:
    MachinePhiInstruction(Register result) : result(result), MachineBaseInstruction(MachineBaseInstruction::PHI) {}

    Register GetResult() { return result; }
    void SetResult(Register result) { this->result = result; }
    std::vector<std::pair<int, MachineBaseOperand *>> &GetPhiList() { return phi_list; }

    // 操作 PHI 列表
    MachineBaseOperand *removePhiList(int label) {
        for (auto it = phi_list.begin(); it != phi_list.end(); ++it) {
            if (it->first == label) {
                auto ret = it->second;
                phi_list.erase(it);
                return ret;
            }
        }
        return nullptr;
    }
    void pushPhiList(int label, Register reg) { phi_list.push_back(std::make_pair(label, new MachineRegister(reg))); }
    void pushPhiList(int label, int imm32) {
        phi_list.push_back(std::make_pair(label, new MachineImmediateInt(imm32)));
    }
    void pushPhiList(int label, float immf32) {
        phi_list.push_back(std::make_pair(label, new MachineImmediateFloat(immf32)));
    }
    void pushPhiList(int label, MachineBaseOperand *op) { phi_list.push_back(std::make_pair(label, op)); }
    int GetLatency() { return 0; }

    // // 获取读寄存器列表
    // std::vector<Register *> GetReadReg() override {
    //     std::vector<Register *> readRegs;
    //     for (auto &phi : phi_list) {
    //         if (phi.second->op_type == MachineBaseOperand::REG) {
    //             readRegs.push_back(&((MachineRegister *)(phi.second))->reg);
    //         }
    //     }
    //     return readRegs;
    // }

    // // 获取写寄存器列表
    // std::vector<Register *> GetWriteReg() override { return {&result}; }
};

#endif