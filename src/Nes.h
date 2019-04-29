#pragma once

#include "Hydra/Kernel/Lib.h"
#include "BlipBuffer/blip_buf.h"

#include "Nes_Apu.h"
#include "Blip_Buffer.h"


#define NES_SHOW_ASM
#define NES_TEST_ASM
#if defined(HY_DEBUG)
    #define NES_TEST_PPU_CYCLE
#endif // HY_DEBUG

#define NES_EXTERNAL_APU

namespace hydra {
    struct Nes {

        //////////////////////////////////////////////////////////////////////////
        // Defines
        static const uint32 kPrgBankSize = Kilo( 16 );
        static const uint32 kChrBankSize = Kilo( 8 );
        static const uint32 kPrgRamSize = Kilo( 8 );

        //////////////////////////////////////////////////////////////////////////
        // Forward declarations
        struct Ppu;
        struct Cpu;
        struct Controller;
        struct Mapper;
        struct Apu;

        //////////////////////////////////////////////////////////////////////////
        // INes format
        struct RomHeader {
            uint8               nesLetters[4];
            uint8               prgRomPages;    // 16 kb page
            uint8               chrRomPages;    // 8 kb page
            uint8               flag0;
            uint8               flag1;
            uint8               prgRamPages;
            uint8               tail[7];

        }; // struct RomHeader

        //////////////////////////////////////////////////////////////////////////
        struct Cart {

            static const uint32 kInvalidMapper = 0xffffffff;

            Nes::RomHeader  romHeader;
            Buffer          prgRom, chrRom, prgRam;

            uint8           chrRam[kChrBankSize];     // Ram used mostly by custom ROM with Mapper 0.

            String          filename;
            uint32          mapperIndex;

            void            Init();
            void            Reset();

            bool            IsCartridgeInserted() const { return mapperIndex != kInvalidMapper; }

            bool            LoadRom( cstring filename );
        }; // struct Cart

        //////////////////////////////////////////////////////////////////////////
        struct Mapper {

            enum Mirroring {
                MirrorHorizontal = 0,
                MirrorVertical,
                MirrorSingleLow,
                MirrorSingleHigh,
                Mirror4
            };

            Mirroring       mirroring;
            uint8           prgRamWrite = false;
            Cpu*            cpu;

            Mapper( Cpu* cpu ) : cpu(cpu) { }

            virtual uint8   ChrRead( uint16 address ) = 0;
            virtual void    ChrWrite( uint16 address, uint8 data ) = 0;

            virtual uint8   PrgRead( uint16 address ) = 0;
            virtual void    PrgWrite( uint16 address, uint8 data ) = 0;

            virtual void    PpuAddress12Rise() { }
        }; // struct Mapper

        // NROM128/NROM256
        struct Mapper0 : public Mapper {

            Mapper0( Cpu* cpu, Cart& cart );

            uint8           ChrRead( uint16 address ) override;
            void            ChrWrite( uint16 address, uint8 data ) override;

            uint8           PrgRead( uint16 address ) override;
            void            PrgWrite( uint16 address, uint8 data ) override;

            uint16          prgAddressMask;
            uint8*          prg;
            uint8*          chr;
        }; // struct Mapper0

        // MMC1
        struct Mapper1 : public Mapper {

            Mapper1( Cpu* cpu, Cart& cart );

            uint8           ChrRead( uint16 address ) override;
            void            ChrWrite( uint16 address, uint8 data ) override;

            uint8           PrgRead( uint16 address ) override;
            void            PrgWrite( uint16 address, uint8 data ) override;

            void            UpdateBanks();

            // Internal registers
            enum Registers {
                Register_Control = 0,
                Register_Chr0,
                Register_Chr1,
                Register_Prg,
                Register_Count
            };
            uint8           registers[Register_Count];
            uint8           writeCounter, temporaryRegister;
            uint8           prgRomBankCount, chrRomBankCount;

            uint8*          prgRomBank0;
            uint8*          prgRomBank1;
            uint8*          chrRomBank0;
            uint8*          chrRomBank1;

            uint8*          prgRam;
            uint8*          prg;
            uint8*          chr;
        }; // struct Mapper0

        // UNROM
        struct Mapper2 : public Mapper {

            Mapper2( Cpu* cpu, Cart& cart );

            uint8           ChrRead( uint16 address ) override;
            void            ChrWrite( uint16 address, uint8 data ) override;

            uint8           PrgRead( uint16 address ) override;
            void            PrgWrite( uint16 address, uint8 data ) override;

            void            UpdatePrgBank();

            uint8           prgBankSelector;
            uint8*          prgBank0;
            uint8*          prgBank1;
            // Memory from the cartridge
            uint8*          prg;
            uint8*          chr;

        }; // struct Mapper2

        // CNROM
        struct Mapper3 : public Mapper {

            Mapper3( Cpu* cpu, Cart& cart );

            uint8           ChrRead( uint16 address ) override;
            void            ChrWrite( uint16 address, uint8 data ) override;

            uint8           PrgRead( uint16 address ) override;
            void            PrgWrite( uint16 address, uint8 data ) override;

            void            UpdateChrBank();

            uint16          prgAddressMask;
            uint8           chrBankSelector;
            uint8*          chrBank;

            // Memory from the cartridge
            uint8*          prg;
            uint8*          chr;

        }; // struct Mapper3

        // MMC3
        struct Mapper4 : public Mapper {

            Mapper4( Cpu* cpu, Cart& cart );

            uint8           ChrRead( uint16 address ) override;
            void            ChrWrite( uint16 address, uint8 data ) override;

            uint8           PrgRead( uint16 address ) override;
            void            PrgWrite( uint16 address, uint8 data ) override;

            void            PpuAddress12Rise() override;

            void            UpdateBanks();
            void            UpdateMirroring();

            static const uint16 kMMC3PrgBankSize = 0x2000;
            static const uint16 kMMC3PrgBankMask = kMMC3PrgBankSize - 1;
            static const uint16 kMMC3ChrBankSize = 0x400;
            static const uint16 kMMC3ChrBankMask = kMMC3ChrBankSize - 1;

            uint8           prgRomBankCount, chrRomBankCount;
            uint8*          prgMemory;
            uint8*          chrMemory;
            uint8*          prgRamMemory;

            uint8*          prgBanks[4];
            uint8*          chrBanks[8];

            uint8           registers[8];
            uint8           bankSelector;
            uint8           mirroringRegister;
            uint8           prgRamProtect;

            uint8           irqReloadPeriod;
            uint8           irqCounter;
            uint8           irqReload;
            uint8           irqEnable;
        };

        //////////////////////////////////////////////////////////////////////////
        struct Screen {
            
            static const uint16 kWidth = 256;
            static const uint16 kHeight = 240;

            enum ZoomFactor {
                Zoom_1x = 0,
                Zoom_2x
            };

            void            Init();
            void            Terminate();

            void            WritePixel( uint16 scanline, uint16 pixel, uint32 color );

            uint8*          paletteIndices;
            uint32*         frameBuffer;
            ZoomFactor      zoomFactor = Zoom_1x;

        }; // struct Screen

        //////////////////////////////////////////////////////////////////////////
        struct MemoryController {

            /*
            +---------+-------+-------+-----------------------+
            | Address | Size  | Flags | Description           |
            +---------+-------+-------+-----------------------+
            | $0000   | $800  |       | RAM                   |
            | $0800   | $800  | M     | RAM                   |
            | $1000   | $800  | M     | RAM                   |
            | $1800   | $800  | M     | RAM                   | ->  8kb
            | $2000   | 8     |       | Registers             |
            | $2008   | $1FF8 |  R    | Registers             | -> 16kb
            | $4000   | $20   |       | Registers             |
            | $4020   | $1FDF |       | Expansion ROM         |
            | $6000   | $2000 |       | SRAM                  |
            | $8000   | $4000 |       | PRG-ROM               |
            | $C000   | $4000 |       | PRG-ROM               |
            +---------+-------+-------+-----------------------+
            Flag Legend: M = Mirror of $0000
            R = Mirror of $2000-2008 every 8 bytes
            (e.g. $2008=$2000, $2018=$2000, etc.)
            */

            enum Type {
                NRom = 0,
            };

            static const uint32 kCpuAddressSpace = Kilo( 64 );
            static const uint32 kCpuPages = 64;
            static const uint32 kCpuPageSize = kCpuAddressSpace / kCpuPages;
            static const uint16 kCpuPageAddressMask = kCpuPageSize - 1; // 0x3FF;
            static const uint16 kRamSize = Kilo( 2 ); // 0x800

            static const uint16 kPrgRomOffsetBank0 = Kilo( 32 );
            static const uint16 kPrgRomOffsetBank1 = Kilo( 48 );

            ForceInline uint16 AddressToPage( uint16 address ) { return address >> 10; }

            Cart*               cart;
            Ppu*                ppu;
            Cpu*                cpu;
            Apu*                apu;
            Controller*         controllers;
            Mapper*             mapper;

            void                Init( Cart* cart, Cpu* cpu, Ppu* ppu, Controller* controllers, Apu* apu );
            void                LoadMapper();

            uint8               CpuRead( uint16 address );
            void                CpuWrite( uint16 address, uint8 data );

            uint8               PpuRead( uint16 address );
            void                PpuWrite( uint16 address, uint8 data );

            void                PpuAddress12Rise();
        }; // struct MemoryController

        // general architecture
        // load a rom
        // test cpu basic stuff
        // test ppu
        // test input
        // http://nikgavalas.com/nesemu/Howto.htm
        // http://www.slack.net/~ant/nes-emu/6502.html
        // http://wiki.nesdev.com/w/index.php/INES
        // http://wiki.nesdev.com/w/index.php/CPU_power_up_state
        //////////////////////////////////////////////////////////////////////////
        struct Cpu {

            // Addresses to jump to for different events.
            static const uint16 kNmiVector = 0xFFFA;
            static const uint16 kResetVector = 0xFFFC;
            static const uint16 kIrqVector = 0xFFFE;

            // Registers
            uint8       A, X, Y, S, P;
            uint16      PC;

            struct Flags {
                uint8 c : 1;    // carry
                uint8 z : 1;    // zero
                uint8 i : 1;    // interrupt
                uint8 d : 1;    // decimal
                uint8 b : 1;    // break/software interrupt
                uint8 pad : 1;
                uint8 v : 1;    // overflow
                uint8 n : 1;    // negative/sign flag
            };

            Flags       flags;

            uint64      cycles;
            uint32      frameCycles;

            uint16      opAddress;
            uint8       opCode;

            uint16      effectiveAddress;
            uint16      tempAddress;
            uint16      tempReg16;
            uint8       tempReg;

            // Interrupt flags: non maskable interrupt, interrupt request.
            uint8       nmistate;
            uint8       irqstate;

            uint8       handleIrq, prevhandleIrq;

            static const uint32 kRamSize = 0x800;

            uint8       ram[kRamSize];

            MemoryController*     memoryController;
            Ppu*        ppu;
            Apu*        apu;

            bool        showAsm;
            bool        testAsm;

#if defined(NES_SHOW_ASM)
            StaticArray<char, 256> asmBuffer;
#endif
            void        Init( Ppu* ppu, Apu* apu, MemoryController* memoryController );
            void        Reset();

            void        Step();
            void        Tick();

            uint8       MemoryRead( uint16 address );
            void        MemoryWrite( uint16 address, uint8 data );

            void        Push( uint8 data );
            uint8       Pop();

            void        SetZeroOrNegativeFlags( uint8 value );
            void        ExpandFlags( uint8 value );
            uint8       CompactFlags();

            void        SetNMI();
            void        ClearNMI();

            void        SetIRQ( uint8 mask );
            void        ClearIRQ( uint8 mask );

            void        HandleInterrupt();

#if defined(NES_SHOW_ASM)
            void        CpuDisassembleInit();
            uint16      CpuDisassemble( char *buffer, uint16 opcodepos );
#endif

            void        DummyRead() { MemoryRead( PC ); }
            void        DummyRead( uint16 address ) { Tick();/* MemoryRead( address );*/ }
            
            uint8       MemoryReadByte() { return MemoryRead( PC++ ); }
            
            uint16      MemoryReadWord() {
                uint16 address = MemoryReadByte();
                address |= MemoryReadByte() << 8;
                return address;
            }

            uint16      MemoryReadWord( uint16 address ) {
                uint8 lowerByte = MemoryRead( address );
                uint8 higherByte = MemoryRead( address + 1 );
                return ( lowerByte | ( higherByte << 8 ) );
            }

            //////////////////////////////////////////////////////////////////////////
            // Address Modes
            //////////////////////////////////////////////////////////////////////////
            // See http://nesdev.com/6502_cpu.txt
            
            ForceInline bool CheckCrossPage( uint16 a, int8 i ) { return ( ( a + i ) & 0xFF00 ) != ( ( a & 0xFF00 ) ); }

            ForceInline bool CheckCrossPage( uint16 a, uint8 i ) { return ( ( a + i ) & 0xFF00 ) != ( ( a & 0xFF00 ) ); }

            // Implied
            ForceInline uint16 AM_IMP() { return MemoryRead( PC ); }

            // Relative
            ForceInline uint16 AM_REL() { return MemoryRead( PC++ ); }

            // Immediate
            ForceInline uint16 AM_IMM() { return PC++; }

            // Absolute
            ForceInline uint16 AM_ABS() { return MemoryReadWord(); }

            // Absolute x addressing READ
            ForceInline uint16 AM_AXR() {
                uint16 effectiveAddress = AM_ABS();
                
                if ( CheckCrossPage( effectiveAddress, X ) ) {
                    uint16 tempReg16 = ( effectiveAddress & 0xFF ) + X;
                    DummyRead( ( effectiveAddress & 0xFF00 ) | (uint8)tempReg16 );
                }

                return effectiveAddress + X;
            }

            // Absolute x addressing WRITE
            ForceInline uint16 AM_AXW() {
                uint16 effectiveAddress = AM_ABS();

                uint16 tempReg16 = ( effectiveAddress & 0xFF ) + X;
                DummyRead( ( effectiveAddress & 0xFF00 ) | (uint8)tempReg16 );

                return effectiveAddress + X;
            }

            // Absolute y addressing READ
            ForceInline uint16 AM_AYR() {
                uint16 effectiveAddress = AM_ABS();

                if ( CheckCrossPage( effectiveAddress, Y ) ) {
                    uint16 tempReg16 = ( effectiveAddress & 0xFF ) + Y;
                    DummyRead( ( effectiveAddress & 0xFF00 ) | (uint8)tempReg16 );
                }
                return effectiveAddress + Y;
            }

            // Absolute y addressing WRITE
            ForceInline uint16 AM_AYW() {
                uint16 effectiveAddress = AM_ABS();

                uint16 tempReg16 = ( effectiveAddress & 0xFF ) + Y;
                DummyRead( ( effectiveAddress & 0xFF00 ) | (uint8)tempReg16 );

                return effectiveAddress + Y;
            }

            // Zero page
            ForceInline uint16 AM_ZPG() {
                return MemoryRead( PC++ );
            }

            // Zero page X
            ForceInline uint16 AM_ZPX() {
                uint16 effectiveAddress = AM_ZPG();
                DummyRead( effectiveAddress );
                return ( effectiveAddress + X ) & 0xFF;
            }

            // Zero page Y
            ForceInline uint16 AM_ZPY() {
                uint16 effectiveAddress = AM_ZPG();
                DummyRead( effectiveAddress );
                return ( effectiveAddress + Y ) & 0xFF;
            }

            // Indirect
            ForceInline uint16 AM_IND() {
                uint16 tempAddress = MemoryRead( PC++ );
                tempAddress |= MemoryRead( PC++ ) << 8;
                // save lower 8 bits
                uint16 effectiveAddress = MemoryRead( tempAddress );
                tempAddress = ( tempAddress & 0xFF00 ) | ( ( tempAddress + 1 ) & 0xFF );
                effectiveAddress |= MemoryRead( tempAddress ) << 8;
                return effectiveAddress;
            }

            // Indirect x
            ForceInline uint16 AM_INX() {
                uint8 tempReg = MemoryRead( PC++ );
                DummyRead( tempReg );
                tempReg += X;
                uint16 effectiveAddress = MemoryRead( tempReg++ );
                effectiveAddress |= MemoryRead( tempReg ) << 8;

                return effectiveAddress;
            }

            // Indirect y write
            ForceInline uint16 AM_INY() {
                uint8 tempReg = MemoryRead( PC++ );
                uint16 effectiveAddress = MemoryRead( tempReg++ );
                effectiveAddress |= MemoryRead( tempReg ) << 8;
                uint16 tempReg16 = ( effectiveAddress & 0xFF ) + Y;
                MemoryRead( ( effectiveAddress & 0xFF00 ) | (uint8)tempReg16 );
                effectiveAddress += Y;

                return effectiveAddress;
            }

            // Indirect y read
            ForceInline uint16 AM_IYR() {
                uint8 tempReg = MemoryRead( PC++ );
                uint16 effectiveAddress = MemoryRead( tempReg++ );
                effectiveAddress |= MemoryRead( tempReg ) << 8;
                uint16 tempReg16 = ( effectiveAddress & 0xFF ) + Y;
                if ( CheckCrossPage( effectiveAddress, Y ) ) {
                    DummyRead( ( effectiveAddress & 0xFF00 ) | (uint8)tempReg16 );
                }
                effectiveAddress += Y;

                return effectiveAddress;
            }

            ForceInline uint16 AM_UNK() {
                return 0;
            }

            //////////////////////////////////////////////////////////////////////////
            // Operations
            //////////////////////////////////////////////////////////////////////////

            // load/store
            ForceInline void OP_LDX() {
                X = MemoryRead( effectiveAddress );
                SetZeroOrNegativeFlags( X );
            }

            ForceInline void OP_LDA() {
                A = MemoryRead( effectiveAddress );
                SetZeroOrNegativeFlags( A );
            }

            ForceInline void OP_LDY() {
                Y = MemoryRead( effectiveAddress );
                SetZeroOrNegativeFlags( Y );
            }

            ForceInline void OP_STX() {
                MemoryWrite( effectiveAddress, X );
            }

            ForceInline void OP_STY() {
                MemoryWrite( effectiveAddress, Y );
            }

            ForceInline void OP_STA() {
                MemoryWrite( effectiveAddress, A );
            }

            // comparison
            ForceInline void OP_BIT() {
                tempReg = MemoryRead( effectiveAddress );
                flags.v = ( tempReg >> 6 ) & 1;
                flags.n = ( tempReg >> 7 ) & 1;
                flags.z = ( A & tempReg ) ? 0 : 1;
            }

            ForceInline void OP_CMP() {
                tempReg16 = A - MemoryRead( effectiveAddress );
                flags.c = !( tempReg16 & 0x100 );
                SetZeroOrNegativeFlags( (uint8)tempReg16 );
            }

            ForceInline void OP_CPX() {
                tempReg16 = X - MemoryRead( effectiveAddress );
                flags.c = !( tempReg16 & 0x100 );
                SetZeroOrNegativeFlags( (uint8)tempReg16 );
            }

            ForceInline void OP_CPY() {
                tempReg16 = Y - MemoryRead( effectiveAddress );
                flags.c = !( tempReg16 & 0x100 );
                SetZeroOrNegativeFlags( (uint8)tempReg16 );
            }

            // branch
            ForceInline void OP_BRANCH( uint8 flag ) {
                if ( !flag )
                    return;
                // Dummy read
                MemoryRead( PC );
                tempAddress = PC + (int8)effectiveAddress;
                if ( CheckCrossPage( PC, (int8)effectiveAddress ) ) {
                    // Dymmy read
                    MemoryRead( ( PC & 0xFF00 ) | ( tempAddress & 0xFF ) );
                }
                PC = tempAddress;
            }

            ForceInline void OP_BCS() {
                OP_BRANCH( flags.c );
            }

            ForceInline void OP_BCC() {
                OP_BRANCH( !flags.c );
            }

            ForceInline void OP_BEQ() {
                OP_BRANCH( flags.z );
            }

            ForceInline void OP_BNE() {
                OP_BRANCH( !flags.z );
            }

            ForceInline void OP_BPL() {
                OP_BRANCH( !flags.n );
            }

            ForceInline void OP_BMI() {
                OP_BRANCH( flags.n );
            }

            ForceInline void OP_BVC() {
                OP_BRANCH( !flags.v );
            }

            ForceInline void OP_BVS() {
                OP_BRANCH( flags.v );
            }

            // stack
            ForceInline void OP_PHA() {
                Push( A );
            }

            ForceInline void OP_PHP() {
                uint8 p = CompactFlags();
                Push( p | ( 1 << 4 ) ); // b flag set
            }

            ForceInline void OP_PLA() {
                MemoryRead( S | 0x100 );
                A = Pop();
                SetZeroOrNegativeFlags( A );
            }

            ForceInline void OP_PLP() {
                DummyRead();
                uint8 p = Pop();
                ExpandFlags( p );
            }

            ForceInline void OP_JSR() {
                PC++;
                DummyRead();
                Push( (uint8)( PC >> 8 ) );
                Push( (uint8)PC );
                PC = MemoryRead( PC ) << 8 | effectiveAddress;
            }

            ForceInline void OP_RTI() {
                OP_PLP();
                PC = Pop();
                PC |= Pop() << 8;
            }

            ForceInline void OP_RTS() {
                DummyRead();

                PC = Pop();
                PC |= Pop() << 8;
                MemoryRead( PC++ );
            }

            ForceInline void OP_JMP() {
                PC = effectiveAddress;
            }

            ForceInline void OP_BRK() {
                
                Push( (uint8)((PC + 1)>> 8 ) );
                Push( (uint8)(PC + 1) );

                OP_PHP();

                if ( nmistate ) {
                    nmistate = 0;
                    PC = MemoryReadWord( kNmiVector );
                }
                else {
                    PC = MemoryReadWord( kIrqVector );
                }
                OP_SEI();
                
                prevhandleIrq = 0;
            }

            // flags
            ForceInline void OP_SEC() {
                flags.c = 1;
            }

            ForceInline void OP_SEI() {
                flags.i = 1;
            }

            ForceInline void OP_SED() {
                flags.d = 1;
            }

            ForceInline void OP_CLC() {
                flags.c = 0;
            }

            ForceInline void OP_CLI() {
                flags.i = 0;
            }

            ForceInline void OP_CLD() {
                flags.d = 0;
            }

            ForceInline void OP_CLV() {
                flags.v = 0;
            }

            // transfer
            ForceInline void OP_TAX() {
                X = A;
                SetZeroOrNegativeFlags( X );
            }

            ForceInline void OP_TAY() {
                Y = A;
                SetZeroOrNegativeFlags( Y );
            }

            ForceInline void OP_TXA() {
                A = X;
                SetZeroOrNegativeFlags( A );
            }

            ForceInline void OP_TYA() {
                A = Y;
                SetZeroOrNegativeFlags( A );
            }

            ForceInline void OP_TSX() {
                X = S;
                SetZeroOrNegativeFlags( X );
            }

            ForceInline void OP_TXS() {
                S = X;
            }

            // increment/decrement
            ForceInline void OP_DEX() {
                X--;
                SetZeroOrNegativeFlags( X );
            }

            ForceInline void OP_DEY() {
                Y--;
                SetZeroOrNegativeFlags( Y );
            }

            ForceInline void OP_INX() {
                X++;
                SetZeroOrNegativeFlags( X );
            }

            ForceInline void OP_INY() {
                Y++;
                SetZeroOrNegativeFlags( Y );
            }

            ForceInline void OP_DEC() {
                tempReg = MemoryRead( effectiveAddress );
                MemoryWrite( effectiveAddress, tempReg-- );
                MemoryWrite( effectiveAddress, tempReg );
                SetZeroOrNegativeFlags( tempReg );
            }

            ForceInline void OP_INC() {
                tempReg = MemoryRead( effectiveAddress );
                MemoryWrite( effectiveAddress, tempReg++ );
                MemoryWrite( effectiveAddress, tempReg );
                SetZeroOrNegativeFlags( tempReg );
            }

            // math
            ForceInline void OP_ORA() {
                A |= MemoryRead( effectiveAddress );
                SetZeroOrNegativeFlags( A );
            }

            ForceInline void OP_AND() {
                A &= MemoryRead( effectiveAddress );
                SetZeroOrNegativeFlags( A );
            }

            ForceInline void OP_EOR() {
                A ^= MemoryRead( effectiveAddress );
                SetZeroOrNegativeFlags( A );
            }

            ForceInline void OP_ASL() {
                tempReg = MemoryRead( effectiveAddress );
                MemoryWrite( effectiveAddress, tempReg );
                flags.c = ( tempReg >> 7 ) & 1;
                tempReg <<= 1;
                MemoryWrite( effectiveAddress, tempReg );
                SetZeroOrNegativeFlags( tempReg );
            }

            ForceInline void OP_ASLA() {
                flags.c = ( A >> 7 ) & 1;
                A <<= 1;
                SetZeroOrNegativeFlags( A );
            }

            ForceInline void OP_ROL() {
                tempReg = MemoryRead( effectiveAddress );
                MemoryWrite( effectiveAddress, tempReg );
                uint8 tmp8 = flags.c;
                flags.c = ( tempReg >> 7 ) & 1;
                tempReg = ( tempReg << 1 ) | tmp8;
                MemoryWrite( effectiveAddress, tempReg );
                SetZeroOrNegativeFlags( tempReg );
            }

            ForceInline void OP_ROLA() {
                tempReg = flags.c;
                flags.c = ( A >> 7 ) & 1;
                A = ( A << 1 ) | tempReg;
                SetZeroOrNegativeFlags( A );
            }

            ForceInline void OP_ROR() {
                tempReg = MemoryRead( effectiveAddress );
                MemoryWrite( effectiveAddress, tempReg );
                uint8 tmp8 = flags.c;
                flags.c = tempReg & 1;
                tempReg = ( tempReg >> 1 ) | ( tmp8 << 7 );
                MemoryWrite( effectiveAddress, tempReg );
                SetZeroOrNegativeFlags( tempReg );
            }

            ForceInline void OP_RORA() {
                tempReg = flags.c;
                flags.c = A & 1;
                A = ( A >> 1 ) | ( tempReg << 7 );
                SetZeroOrNegativeFlags( A );
            }

            ForceInline void OP_LSR() {
                tempReg = MemoryRead( effectiveAddress );
                MemoryWrite( effectiveAddress, tempReg );
                flags.c = tempReg & 1;
                tempReg >>= 1;
                MemoryWrite( effectiveAddress, tempReg );
                SetZeroOrNegativeFlags( tempReg );
            }

            ForceInline void OP_LSRA() {
                flags.c = A & 1;
                A >>= 1;
                SetZeroOrNegativeFlags( A );
            }

            ForceInline void OP_ADC() {
                tempReg = MemoryRead( effectiveAddress );
                int16 tmpi = A + tempReg + flags.c;
                flags.c = ( tmpi & 0xFF00 ) ? 1 : 0;
                flags.v = ( ( ( A ^ tmpi ) & ( tempReg ^ tmpi ) ) & 0x80 ) ? 1 : 0;
                A = (uint8)tmpi;
                SetZeroOrNegativeFlags( A );
            }

            ForceInline void OP_SBC() {
                tempReg = MemoryRead( effectiveAddress );
                int16 tmpi = A - tempReg - ( 1 - flags.c );
                flags.c = ( ( tmpi & 0xFF00 ) == 0 ) ? 1 : 0;
                flags.v = ( ( ( A ^ tempReg ) & ( A ^ tmpi ) ) & 0x80 ) ? 1 : 0;
                A = (uint8)tmpi;
                SetZeroOrNegativeFlags( A );
            }

            ForceInline void OP_NOP() {
                Tick();
            }

            ForceInline void OP_UNK1() {
                DummyRead();
            }

            ForceInline void OP_UNK2() {
                DummyRead();
                DummyRead();
            }

            ForceInline void OP_UNK3() {
                DummyRead();
                DummyRead();
                DummyRead();
            }

            ForceInline void OP_UNK4() {
                DummyRead();
                DummyRead();
                DummyRead();
                DummyRead();
            }

            ForceInline void OP_UNK() {
                //PrintFormat( "Opcode %02X not implemented yet.\n", opCode );
                //Assert( false && "operation not supported!" );
            }
        }; // struct Cpu

        //////////////////////////////////////////////////////////////////////////
        struct Ppu {

            static const uint16 kMaxScanlines = 262;
            static const uint16 kMaxPixels = 341;
            static const uint8 kMaxSprites = 64;
            static const uint8 kMaxPerLineSprites = 8;

            static const uint16 kNameTableRamSize = 0x800;
            static const uint16 kPaletteRamSize = 32;
            static const uint16 kOAMRamSize = 0x100;

            enum Registers {
                R2000_PPUCTRL = 0,
                R2001_PPUMASK = 1,
                R2002_PPUSTATUS = 2,
                R2003_OAMADDR = 3,
                R2004_OAMDATA = 4,
                R2005_PPUSCROLL = 5,
                R2006_PPUADDR = 6,
                R2007_PPUDATA = 7,
                R4014_OAMDMA = 0x4014
            };

            enum IoModes {

            };

            enum ControlFlags {
                ControlFlag_NametableMask = 0x03,
                ControlFlag_VRAM_AddressIncrement = 0x04,
                ControlFlag_SpriteAddress = 0x08,
                ControlFlag_BackgroundAddress = 0x10,
                ControlFlag_SpriteSize = 0x20,
                ControlFlag_MasterSlave = 0x40,
                ControlFlag_GenerateNMI = 0x80
            };

            enum MaskFlags {
                MaskFlag_GreyScale      = 0x01,
                MaskFlag_LeftColumnBackground = 0x02,
                MaskFlag_LeftColumnSprite = 0x04,
                MaskFlag_EnableBackground = 0x08,
                MaskFlag_EnableSprite     = 0x10,
                MaskFlag_ColorEmphasis    = 0x20,
                MaskFlag_EnableRendering = MaskFlag_EnableBackground | MaskFlag_EnableSprite,
            };

            enum StatusFlags {
                StatusFlag_SpriteOverflow   = 0x20,
                StatusFlag_SpriteHit        = 0x40,
                StatusFlag_VBlank           = 0x80,
                StatusFlag_Mask             = StatusFlag_SpriteOverflow | StatusFlag_SpriteHit |
                                              StatusFlag_VBlank
            };

            enum RenderPhase {
                PreRender,
                Render,
                PostRender,
                VerticalBlank
            };

            // $2000, $2001 and $2002 registers
            uint8                   control;
            uint8                   mask;
            uint8                   status;
            uint8                   register_;

            // Scrolling registers
            uint16                  v, t;
            uint8                   fineX, w, internalReadBuffer;

            // Background data
            uint8                   nameTableByte, attributeTableByte, tileHighByte, tileLowByte;
            // Attribute table cache
            uint8                   previousAT, currentAT, nextAT;
            // Background addresses
            uint16                  nameTableAddress, attributeTableAddress, tileAddress, paletteAddress;

            // Background tiles. They hold previous and current tiles bytes, 1 for low byte and 1 for high bytes.
            uint16                  backgroundTilesLowBits, backgroundTilesHighBits;
#if defined(PPU_SHIFT_REGISTERS)
            uint8                   atShiftL, atShiftH;
            uint8                   atLatchL, atLatchH;
#endif
            

            uint8                   verticalBlank, verticalBlankOccurred;
            uint8                   spriteOverflow, spriteHit;

            uint16                  oamAddress;
            uint16                  scanline, pixelCycle;
            uint32                  frames;

            uint8                   isRendering;

            // http://www.fceux.com/web/help/fceux.html?PPU.html
            // Object Attribute actual memory!
            struct Sprite {
                uint8               x, y, tile, attribute;
                uint8               id, dataH, dataL;
            };

            // Holds all visible sprites.
            uint8                   oamMemory[kOAMRamSize];
            // Used for each scanline to calculate visible sprites.
            Sprite                  oam[kMaxPerLineSprites];
            Sprite                  secOam[kMaxPerLineSprites];

            uint8                   nametableRam[kNameTableRamSize];
            uint8                   paletteRam[kPaletteRamSize];

            // used for external reads.
            MemoryController*       memoryController;
            Cpu*                    cpu;
            Screen*                 screen;

            bool                    debugCycles = false; // Debug cycles for 1 frame. Warning: extremely slow!
            bool                    debugFrame = false;
#if defined(NES_TEST_PPU_CYCLE)
            struct PpuDebugger {
                
                enum Event {
                    Draw        = 1,
                    IncrementX  = 1 << 1,
                    IncrementY  = 1 << 2,
                    CopyX       = 1 << 3,
                    CopyY       = 1 << 4,
                    ReloadShift = 1 << 5,
                    Shift       = 1 << 6
                };

                struct PixelData {
                    uint8           events, paletteIndex;
                    uint8           nameTableByte, attributeTableByte, tileHighByte, tileLowByte;
                    uint16          nameTableAddress, attributeTableAddress, tileAddress, paletteAddress;
                };


                PixelData*           pixelData = nullptr;
            };

            PpuDebugger             debugger;
#endif // NES_TEST_PPU_CYCLE

            void                    Init(Cpu* cpu, Screen* screen, MemoryController* memoryController);
            void                    Reset();

            void                    Tick();
            void                    Step();

            uint8                   CpuRead( uint16 address );
            void                    CpuWrite( uint16 address, uint8 data );

            void                    IncrementPixelCycle();     // Done for every pixel
            void                    IncrementAddressV();       // Executed when reading data from $2007

            void                    IncrementHoriV();
            void                    IncrementVertV();
            void                    CopyHoriV();
            void                    CopyVertV();

            bool                    IsRendering();
            uint8                   SpriteHeight();

            void                    ReadNameTable();
            void                    ReadAttributeTable();
            void                    ReadTileHigh();
            void                    ReadTileLow();

            void                    ReloadShiftRegisters();

            void                    ClearSecondaryOAM();
            void                    EvaluateSprites();
            void                    LoadSprites();

            void                    DrawPixel();

            uint32                  GetPaletteColor( uint8 index );

        }; // struct Ppu

        //////////////////////////////////////////////////////////////////////////
        struct Apu {

            struct Pulse {

                union Control {
                    uint8           data;

                    struct {
                        uint8       envelopeOrVolume : 4;
                        uint8       volumeOrEnvelopeDecay : 1;
                        uint8       envelopeLoopOrLengthCounter : 1;
                        uint8       dutyCycles : 2;
                    };
                };

                union Sweep {
                    uint8           data;

                    struct {
                        uint8       shiftCount : 3;
                        uint8       negateFlag : 1;
                        uint8       dividerPeriod : 3;
                        uint8       enabled : 1;
                    };
                };

                void                WriteControl( uint8 data );
                void                WriteSweep( uint8 data );

				void				TickTimer();
				void				TickLengthCounter();
				void				TickEnvelope();
				void				TickSweep();

                uint8               Output();

                Control             control;
                Sweep               sweep;
                uint8               sweepCycle;
                uint8               sweepStart;

                uint16              timerPeriod;
				uint16				timer;
                
                uint8               lengthCounter;

				uint8				envelopeCounter;
                uint8               envelopeStart;
                uint8               volume;

                uint8               dutyCycle;
            }; // struct Pulse

            struct Triangle {

            };

            struct Noise {

            };

            struct DeltaModulationChannel {

            };

            union EnabledChannels {
                uint8               data;

                struct {
                    uint8           pulse1 : 1;
                    uint8           pulse2 : 1;
                    uint8           triangle : 1;
                    uint8           noise : 1;
                    uint8           dmc : 1;
                    uint8           padding : 3;
                };
            };

            union FrameCounter {
                uint8               data;

                struct {
                    uint8           padding : 6;
                    uint8           inhibitIRQ : 1;
                    uint8           mode : 1;
                };
            };

            void                    Init(Cpu* cpu);
            void                    Reset();
            void                    Tick();

            void                    EndFrame(uint32 count);

            void                    WriteControl( uint8 data );
            void                    WriteFrameCounter( uint8 data );

            uint8                   ReadStatus();

            uint8                   CpuRead( uint16 address );
            void                    CpuWrite( uint16 address, uint8 data );

            float                   OutputSound();

            void                    Mute( bool value );
            void                    SetVolume( float value );

            Pulse                   pulse1, pulse2;
            Triangle                triangle;
            Noise                   noise;
            DeltaModulationChannel  dmc;

            Cpu*                    cpu;

            EnabledChannels         enabledChannels;
            FrameCounter            frameCounter;

			uint16					frameCycle;			// count cycles from the CPU

            float                   volume;
            bool                    mute;

#if defined (NES_EXTERNAL_APU)
			Blip_Buffer*            blipBuffer = nullptr;
			Nes_Apu*				externalApu = nullptr;
#endif

        }; // struct Apu

        //////////////////////////////////////////////////////////////////////////
        struct Controller {

            enum Buttons {
                Button_A = 0,
                Button_B,
                Button_Select,
                Button_Start,
                Button_Up,
                Button_Down,
                Button_Left,
                Button_Right
            };

            uint32   portdata;
            uint8    registers[2];
            uint8    state[2];
            uint8    readBitIndex;
            bool     strobeToggle;

            void     Reset();
            void     NewFrame(); // Clear cached button state

            void     SetButton( uint8 controller, Buttons button );

            uint8    ReadState();
            void     WriteState( uint8 strobe );
        }; // struct Controller

        void        Init();
        void        Reset();
        void        LoadRom( cstring path );
        void        SaveSRAM();

        // Memory mapping:
        // Both cpu and ppu read/write through a mapper
        Cpu         cpu;
        Ppu         ppu;
        Apu         apu;
        Screen      screen;
        Controller  controllers;

        Cart        cart;
        MemoryController      memoryController;

        int       screenTexture;
    }; // struct Nes

} // namespace hydra
