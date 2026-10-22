#pragma once
#include "defines.hpp"
#include "lasm/output_stream.hpp"
#include "luac/array.hpp"
#include "luac/input_stream.hpp"
#include "types.hpp"

namespace Luac {
    class Instruction {
    private:
        static LUACDIS_INLINE void printVKIVKI(Lasm::OutputStream* stream, int arg1, int arg2);
        static LUACDIS_INLINE void printVITVI(Lasm::OutputStream* stream, int arg1, int arg2);
        static LUACDIS_INLINE void printVKI(Lasm::OutputStream* stream, int arg);
        static LUACDIS_INLINE void printVI(Lasm::OutputStream* stream, int arg);
        static LUACDIS_INLINE void printVIVI(Lasm::OutputStream* stream, int arg1, int arg2);
        static LUACDIS_INLINE void printVIVKIVKI(Lasm::OutputStream* stream, int arg1, int arg2, int arg3);
        static LUACDIS_INLINE void printTVI(Lasm::OutputStream* stream, int arg);
        static LUACDIS_INLINE void printI(Lasm::OutputStream* stream, int arg);
        static LUACDIS_INLINE void printUI(Lasm::OutputStream* stream, int arg);
        static LUACDIS_INLINE void printKIF(Lasm::OutputStream* stream, int arg);
        static LUACDIS_INLINE void printKI(Lasm::OutputStream* stream, int arg);

        static LUACDIS_INLINE void printGoto(Lasm::OutputStream* stream, size_t instCount, size_t pc, int arg);

    public:
        enum OP {
            MOVE,
            LOADK,
            LOADKX,
            LOADBOOL,
            LOADNIL,
            GETUPVAL,
            GETTABUP,
            GETTABLE,
            SETTABUP,
            SETUPVAL,
            SETTABLE,
            NEWTABLE,
            SELF,
            ADD,
            SUB,
            MUL,
            DIV,
            MOD,
            POW,
            UNM,
            NOT,
            LEN,
            CONCAT,
            JMP,
            EQ,
            LT,
            LE,
            TEST,
            TESTSET,
            CALL,
            TAILCALL,
            RETURN,
            FORLOOP,
            FORPREP,
            TFORCALL,
            TFORLOOP,
            SETLIST,
            CLOSURE,
            VARARG,
            EXTRAARG,
            IDIV,
            BNOT,
            BAND,
            BOR,
            BXOR,
            SHL,
            SHR
        };
        u32 m_value;
        [[nodiscard]] constexpr int GET_OPCODE() const noexcept {
            return static_cast<int>(m_value & UINT32_C(0x3F));
        }
        [[nodiscard]] constexpr int GETARG_A() const noexcept {
            return static_cast<int>((m_value >> 6) & UINT32_C(0xFF));
        }
        [[nodiscard]] constexpr int GETARG_B() const noexcept {
            return static_cast<int>(m_value >> 23);
        }
        [[nodiscard]] constexpr int GETARG_C() const noexcept {
            return static_cast<int>((m_value >> 14) & UINT32_C(0x1FF));
        }
        [[nodiscard]] constexpr int GETARG_BX() const noexcept {
            return static_cast<int>(m_value >> 14);
        }
        [[nodiscard]] constexpr int GETARG_SBX() const noexcept {
            return static_cast<int>((m_value >> 14) - UINT32_C(0x1ffff));
        }
        [[nodiscard]] constexpr int GETARG_AX() const noexcept {
            return static_cast<int>(m_value >> 6);
        }
        void print(Lasm::OutputStream* stream, size_t instCount = 0, size_t pc = 0);
        static void loadArray(Luac::InputStream* stream, Array<Instruction>* parr);
        static void loadArray(Luac::InputStream* stream, Instruction* instructions, size_t count);
        static void printArray(Lasm::OutputStream* stream, Array<Instruction>* parr);
        static void printArray(Lasm::OutputStream* stream, Instruction* instructions, size_t count);
    };
} // namespace Luac