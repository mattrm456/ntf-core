// Copyright 2020-2023 Bloomberg Finance L.P.
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_NTSU_PACKETUTIL
#define INCLUDED_NTSU_PACKETUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_adapter.h>
#include <ntsa_devicetype.h>
#include <ntsa_error.h>
#include <ntsa_ipv4address.h>
#include <ntsa_ipv6address.h>
#include <ntsa_packetfilter.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>
#include <ball_log.h>

namespace BloombergLP {
namespace ntsu {

/* instruction classes */

/// Load a value from the network packet and store it in the accumulator
/// register.
#define NTSU_BPF_LD 0x00

/// Load a value from the network packet and store it in the index register.
#define NTSU_BPF_LDX 0x01

/// Store the value in the accumulator register into scratch memory.
#define NTSU_BPF_ST 0x02

/// Store the value in the index register into scratch memory.
#define NTSU_BPF_STX 0x03

/// Perform an arithmetic or logical operation.
#define NTSU_BPF_ALU 0x04

/// Perform a jump operation.
#define NTSU_BPF_JMP 0x05

/// Perform a return operation.
#define NTSU_BPF_RET 0x06

/// Perform a register copy.
#define NTSU_BPF_MISC 0x07

/* ld/ldx fields */

/// Load a 32-bit word from memory.
#define NTSU_BPF_W 0x00

/// Load a 16-bit half-word from memory.
#define NTSU_BPF_H 0x08

/// Load an 8-bit byte from memory.
#define NTSU_BPF_B 0x10

/// The instruction uses a constant rather than a register.
#define NTSU_BPF_IMM 0x00

/// Load from a fixed, absolute offset.
#define NTSU_BPF_ABS 0x20

/// Load from the offset stored in the index register.
#define NTSU_BPF_IND 0x40

/// Load or store from or to scratch memory instead of the packet.
#define NTSU_BPF_MEM 0x60

/// Load the total length of the packet into the accumulator register.
#define NTSU_BPF_LEN 0x80

/// Load a value, mask it with 0x0F, and multiplies the result by 4.
#define NTSU_BPF_MSH 0xa0

/* alu/jmp fields */

/// Add a constant value or value of the index register to the value of the
/// accumulator register and store the result in the accumulator register.
#define NTSU_BPF_ADD 0x00

/// Subtract a constant value or value of the index register from the value of
/// the accumulator register and store the result in the accumulator register.
#define NTSU_BPF_SUB 0x10

/// Multiply the value of the accumulator register by a constant value or value
/// of the index register and store the result in the accumulator register.
#define NTSU_BPF_MUL 0x20

/// Divide the value of the accumulator register by a constant value or value
/// of the index register and store the result in the accumulator register.
#define NTSU_BPF_DIV 0x30

/// Performa a bitwise-or of the value in the accumulator register with a
/// constant value or the value in the index register and store the result in
/// the accumulator register.
#define NTSU_BPF_OR 0x40

/// Performa a bitwise-and of the value in the accumulator register with a
/// constant value or the value in the index register and store the result in
/// the accumulator register.
#define NTSU_BPF_AND 0x50

/// Left-shift the bits in the accumulator register by a number of bits
/// indicated by a constant value or the value in the index register and store
/// the result in the accumulator register.
#define NTSU_BPF_LSH 0x60

/// Right-shift the bits in the accumulator register by a number of bits
/// indicated by a constant value or the value in the index register and store
/// the result in the accumulator register.
#define NTSU_BPF_RSH 0x70

/// Negate the value of the accumulator register and store the result in the
/// accumulator register.
#define NTSU_BPF_NEG 0x80

/// Jump always.
#define NTSU_BPF_JA 0x00

/// Compare the value of the accumulator register with a constant value or the
/// value of the index register and if the two values are equal jump forward
/// by the relative amount of instructions indicated by the "true" jump target
/// otherwise jump forward by the relative amount of instructions indicated
/// by the "false" jump target.
#define NTSU_BPF_JEQ 0x10

/// Compare the value of the accumulator register with a constant value or the
/// value of the index register and if the value in the accumulator register is
/// greater than the operand jump forward by the relative amount of
/// instructions indicated by the "true" jump target otherwise jump forward by
/// the relative amount of instructions indicated by the "false" jump target.
#define NTSU_BPF_JGT 0x20

/// Compare the value of the accumulator register with a constant value or the
/// value of the index register and if the value in the accumulator register is
/// greater than or equal to the operand jump forward by the relative amount of
/// instructions indicated by the "true" jump target otherwise jump forward by
/// the relative amount of instructions indicated by the "false" jump target.
#define NTSU_BPF_JGE 0x30

/// Perform a bitwise-and comparison of the value of the accumulator register
/// and a constant value or the value of the index register and if the result
/// it not zero jump forward by the relative amount of instructions indicated
/// by the "true" jump target otherwise jump forward by the relative amount of
/// instructions indicated by the "false" jump target.
#define NTSU_BPF_JSET 0x40


/// The instruction uses a constant value as its source operand, instead of
/// a register.
#define NTSU_BPF_K 0x00

/// The instruction uses the value in the index register as its source operand.
#define NTSU_BPF_X 0x08

/// The instruction uses the value in the accumulator register as its source
/// operand.
#define NTSU_BPF_A 0x10


/// Copy the value of the accumulator register to the index register.
#define NTSU_BPF_TAX 0x00

/// Copy the value of the index register to the accumulator register.
#define NTSU_BPF_TXA 0x80

/// @internal @brief
/// Provide utilities for compiling packet filters.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsu
class PacketFilter
{
    BALL_LOG_SET_CLASS_CATEGORY("NTSU.PACKETUTIL");

  public:
    /// Describes the packet filter instruction encoding.
    class Encoding
    {
      public:
        /// Return the class component of the specified operation 'code'.
        bsl::uint16_t getClass(bsl::uint16_t code);

        /// Return the size of the result of the specified load-class operation
        /// 'code'.
        bsl::uint16_t getSize(bsl::uint16_t code);

        /// Return the mode of the specified load-class operation 'code'.
        bsl::uint16_t getMode(bsl::uint16_t code);

        /// Return the operation of the specified arithmetic or jump-class
        /// operation 'code'.
        bsl::uint16_t getOperation(bsl::uint16_t code);

        /// Return the source of the specified arithmetic or jump-class
        /// operation 'code'.
        bsl::uint16_t getSource(bsl::uint16_t code);

        /// Return the return value of the specified return-class operation
        /// 'code'.
        bsl::uint16_t getReturnValue(bsl::uint16_t code);

        /// Return the auxiliary of the specified arithmetic or
        /// miscellanous-class operation 'code'.
        bsl::uint16_t getMisc(bsl::uint16_t code);
    };

    /// Describe a packet filter program instruction.
    class Command
    {
      public:
        Command();

        bsl::vector<bsl::string> label;
        bsl::uint16_t code;
        const char*   jt;
        const char*   jf;
        bsl::uint32_t k;
    };

    /// Define a type alias for a sequence of packet filter commands that
    /// form a packet filter script.
    typedef bsl::vector<Command> Script;

    /// Describe a packet filter program instruction.
    class Instruction
    {
      public:
        Instruction();

        bsl::uint16_t code;
        bsl::uint8_t  jt;
        bsl::uint8_t  jf;
        bsl::uint32_t k;
    };

    /// Define a type alias for a sequence of packet filter instructions that
    /// form a packet filter program.
    typedef bsl::vector<Instruction> Program;

    /// Provide utilities to compile a packet filter program.
    class Compiler
    {
      public:
        /// Defines a map of labels to their absolute instruction positions.
        typedef bsl::map<bsl::string, bsl::size_t> LabelMap;

        /// Label the next instruction in the specified 'script' with the
        /// specified 'label'.
        static void label(Script* script, const char* label);

        /// Add the command to the specified 'script' with the specified
        /// 'label' to execute the specified statement operation 'code' and
        /// operand 'k'.
        static void compile(Script*       script,
                            bsl::uint16_t code,
                            bsl::uint32_t k);

        /// Add the command to the specified 'script' with the specified
        /// 'label' to execute a jump operation 'code' for the
        /// specified operand 'k', jumping to the relative offset 'jt' if true
        /// and the specified relative offset 'jf' if false.
        static void compile(Script*       script,
                            bsl::uint16_t code,
                            bsl::uint32_t k,
                            const char*   jt,
                            const char*   jf);

        /// Load into the specified 'program' the instructions that are
        /// the linked equivalent of commands in the specified 'script'.
        /// Return the error.
        static ntsa::Error link(Program* program, const Script& script);

        /// Load into the specified 'labelMap' the map of labels to absolute
        /// instruction positions in the specified 'script'. Return the error.
        static ntsa::Error analyze(LabelMap* labelMap, const Script& script);

        /// Load into the specified 'jump' target the relative offset from the
        /// specified 'pc' (program counter) to the absolute index of the
        /// instruction having the specified 'label', if any. Return the error.
        static ntsa::Error resolve(bsl::uint8_t*   jump,
                                   bsl::size_t     pc,
                                   const char*     label,
                                   const LabelMap& labelMap);
    };
};

/// @internal @brief
/// Provide utilities for processing packets and packet filters.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsu
class PacketUtil
{
    /// Provide a private, platform-specific implementation of utilities for
    /// processing packets and packet filters.
    class Impl;

    /// The log category.
    BALL_LOG_SET_CLASS_CATEGORY("NTSU.PACKETUTIL");

  public:
    /// Load into the specified 'program' a program to accept all packets.
    /// Return the error.
    static ntsa::Error compile(PacketFilter::Program*    program,
                               ntsa::DeviceType::Value   deviceType,
                               const ntsa::Adapter&      adapter,
                               const ntsa::PacketFilter& filter);

    /// Load into the specified 'program' a program to accept all packets.
    static void acceptAll(PacketFilter::Program* program);

    /// Load into the specified 'program' a program to reject all packets.
    static void rejectAll(PacketFilter::Program* program);
};

}  // end namespace ntsu
}  // end namespace BloombergLP
#endif
