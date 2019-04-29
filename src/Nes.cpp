#include "Nes.h"

#pragma warning(disable:4996)

/*
- dealing with paged / bankswitched memory.

Because the 6502's memory map is nonlinear, when the 6502 engine needs to 
read / write memory from / to a part of the NES's memory map, use part of the 
target address to index into a table of pointers.This table of pointers
( hereon memory map translation table ) will represent the architecture of the
NES memory map( there will have to be 2 seperate tables of this type for
    handling reads & writes from / to NES memory ).Each entry in the table
    represents a linear page( or bank ) in the NES's memory map. Generally, this 
    table will have to have a page bank granularity as small as 2K( fei.e., 32
        entries ), to emulate the mirrioring behaviour of the NES's internal RAM 
    properly.Reserve a single bit in each pointer entry to indicate if the
    target page pointer points directly to a base memory address( in the x86
        memory map ), or an x86 subroutine( used for emulating hardware behaviour
            when access to that address occurs ).If the entry is a subroutine, then
    program control should be transferred to the pointer's address. Otherwise, 
    no branch should occur, and the code that stores / loads the data via the
    pointer base( the original address will be used as the offset into the page )
    should be inlined within the opcode's code body. Also remember that 6502 
    opcodes that work directly with stack or zero - page memory can avoid doing a
    look - up into the memory map translation table, since these areas are
    hardwired to the NES's internal 2K RAM.


    - fetching instructions.

    Because the 6502 is always fetching instructions, and in a sequential
    mannar, it is more efficient to maintain the 6502's PC as a 32-bit IPC 
    register ( Indexed PC ) which points to the physical address at which the
    instructions reside.This avoids having to test the PC's address (with the 
    memory map pointer table) every time an instruction byte needs to be fetched
    from memory.


    - handling program control transfering

    When the IPC must be modified via an absolute program control xfer
    instruction, using the new 16 - bit PC address, the memory map page table is
    consulted to load the new base address for IPC( this base address is also
        saved for later ).The PC value is then added to IPC to offset it into the
    page.When a PC value must be saved to the stack, the saved base address
    ( mentioned previously ) is subtracted from IPC to calculate the current PC
    value.

    Note that support for executing 6502 instructions residing in a hardware
    register mapped part of memory will requre additional code.


    - detecting when PC exceeds a page / bank boundary.

    Because of the way IPC is implemented, when IPC crosses a page boundary,
    instead of the next virtual page being executed, the next physical page is
    executed.This will result in the CPU taking behaviour it never intended to
    take, and may result in compromising the stability of the game being
    emulated.This problem cannot be overcome, but it may be trapped.By placing
    3 of the same special opcodes at the end of these banks in physical memory,
    the page boundary crossing condition can be detected.Now, this doesn't give 
    much hope towards running games that may potentially do this, but if the
    game did this anyways, it would probably hang the game without much
    explanation.By trapping this occurence, you'll know exactly what happened.

*/


namespace hydra {

#define OP(opcode, operation, addressMode) \
    case 0x##opcode:        \
        effectiveAddress = AM_##addressMode();\
        OP_##operation();   \
        break;


void Nes::Cpu::Step() {

#if defined(NES_SHOW_ASM)
    if ( showAsm )
        CpuDisassemble( asmBuffer.begin(), PC );
#endif

    opAddress = PC;
    opCode = MemoryRead(PC++);

    switch ( opCode ) {

        OP(00, BRK,IMP) OP(01, ORA,INX) OP(02, UNK,UNK) OP(03, UNK3,INX) OP(04, UNK1,ZPG) OP(05, ORA,ZPG) OP(06, ASL,ZPG) OP(07, UNK3,ZPG)
        OP(08, PHP,IMP) OP(09, ORA,IMM) OP(0A,ASLA,IMP) OP(0B, UNK1,IMM) OP(0C, UNK1,ABS) OP(0D, ORA,ABS) OP(0E, ASL,ABS) OP(0F, UNK3,ABS)
        OP(10, BPL,REL) OP(11, ORA,IYR) OP(12, UNK,UNK) OP(13, UNK3,INY) OP(14, UNK1,ZPX) OP(15, ORA,ZPX) OP(16, ASL,ZPX) OP(17, UNK4,ZPG)
        OP(18, CLC,IMP) OP(19, ORA,AYR) OP(1A, UNK1,UNK) OP(1B, UNK3,AXW) OP(1C, UNK1,AXR) OP(1D, ORA,AXR) OP(1E, ASL,AXW) OP(1F, UNK3,AXW)
        OP(20, JSR,IMP) OP(21, AND,INX) OP(22, UNK,UNK) OP(23, UNK3,INX) OP(24, BIT,ZPG) OP(25, AND,ZPG) OP(26, ROL,ZPG) OP(27, UNK3,ZPG)
        OP(28, PLP,IMP) OP(29, AND,IMM) OP(2A,ROLA,IMP) OP(2B, UNK1,IMM) OP(2C, BIT,ABS) OP(2D, AND,ABS) OP(2E, ROL,ABS) OP(2F, UNK3,ABS)
        OP(30, BMI,REL) OP(31, AND,IYR) OP(32, UNK,UNK) OP(33, UNK3,INY) OP(34, UNK1,ZPX) OP(35, AND,ZPX) OP(36, ROL,ZPX) OP(37, UNK4,ZPG)
        OP(38, SEC,IMP) OP(39, AND,AYR) OP(3A, UNK1,UNK) OP(3B, UNK3,AXW) OP(3C, UNK1,AXR) OP(3D, AND,AXR) OP(3E, ROL,AXW) OP(3F, UNK,UNK)
        OP(40, RTI,IMP) OP(41, EOR,INX) OP(42, UNK,UNK) OP(43, UNK,UNK) OP(44, UNK1,ZPG) OP(45, EOR,ZPG) OP(46, LSR,ZPG) OP(47, UNK,UNK)
        OP(48, PHA,IMP) OP(49, EOR,IMM) OP(4A,LSRA,IMP) OP(4B, UNK,UNK) OP(4C, JMP,ABS) OP(4D, EOR,ABS) OP(4E, LSR,ABS) OP(4F, UNK,UNK)
        OP(50, BVC,REL) OP(51, EOR,IYR) OP(52, UNK,UNK) OP(53, UNK,UNK) OP(54, UNK1,ZPX) OP(55, EOR,ZPX) OP(56, LSR,ZPX) OP(57, UNK4,ZPG)
        OP(58, CLI,IMM) OP(59, EOR,AYR) OP(5A, UNK1,UNK) OP(5B, UNK,UNK) OP(5C, UNK1,AXR) OP(5D, EOR,AXR) OP(5E, LSR,AXW) OP(5F, UNK,UNK)
        OP(60, RTS,IMP) OP(61, ADC,INX) OP(62, UNK,UNK) OP(63, UNK,UNK) OP(64, UNK1,ZPG) OP(65, ADC,ZPG) OP(66, ROR,ZPG) OP(67, UNK,UNK)
        OP(68, PLA,IMP) OP(69, ADC,IMM) OP(6A,RORA,IMP) OP(6B, UNK,UNK) OP(6C, JMP,IND) OP(6D, ADC,ABS) OP(6E, ROR,ABS) OP(6F, UNK,UNK)
        OP(70, BVS,REL) OP(71, ADC,IYR) OP(72, UNK,UNK) OP(73, UNK,UNK) OP(74, UNK1,ZPX) OP(75, ADC,ZPX) OP(76, ROR,ZPX) OP(77, UNK,UNK)
        OP(78, SEI,IMP) OP(79, ADC,AYR) OP(7A, UNK1,UNK) OP(7B, UNK,UNK) OP(7C, UNK1,AXR) OP(7D, ADC,AXR) OP(7E, ROR,AXW) OP(7F, UNK,UNK)
        OP(80, UNK1,IMM) OP(81, STA,INX) OP(82, UNK1,IMM) OP(83, UNK,UNK) OP(84, STY,ZPG) OP(85, STA,ZPG) OP(86, STX,ZPG) OP(87, UNK,UNK)
        OP(88, DEY,IMP) OP(89, UNK1,IMM) OP(8A, TXA,IMP) OP(8B, UNK,UNK) OP(8C, STY,ABS) OP(8D, STA,ABS) OP(8E, STX,ABS) OP(8F, UNK,UNK)
        OP(90, BCC,REL) OP(91, STA,INY) OP(92, UNK,UNK) OP(93, UNK,UNK) OP(94, STY,ZPX) OP(95, STA,ZPX) OP(96, STX,ZPY) OP(97, UNK,UNK)
        OP(98, TYA,IMP) OP(99, STA,AYW) OP(9A, TXS,IMP) OP(9B, UNK1,AYW) OP(9C, UNK1,AXW) OP(9D, STA,AXW) OP(9E, UNK1,AYW) OP(9F, UNK1,AYW)
        OP(A0, LDY,IMM) OP(A1, LDA,INX) OP(A2, LDX,IMM) OP(A3, UNK,UNK) OP(A4, LDY,ZPG) OP(A5, LDA,ZPG) OP(A6, LDX,ZPG) OP(A7, UNK,UNK)
        OP(A8, TAY,IMP) OP(A9, LDA,IMM) OP(AA, TAX,IMP) OP(AB, UNK,UNK) OP(AC, LDY,ABS) OP(AD, LDA,ABS) OP(AE, LDX,ABS) OP(AF, UNK,UNK)
        OP(B0, BCS,REL) OP(B1, LDA,IYR) OP(B2, UNK,UNK) OP(B3, UNK,UNK) OP(B4, LDY,ZPX) OP(B5, LDA,ZPX) OP(B6, LDX,ZPY) OP(B7, UNK,UNK)
        OP(B8, CLV,IMP) OP(B9, LDA,AYR) OP(BA, TSX,IMP) OP(BB, UNK,UNK) OP(BC, LDY,AXR) OP(BD, LDA,AXR) OP(BE, LDX,AYR) OP(BF, UNK,UNK)
        OP(C0, CPY,IMM) OP(C1, CMP,INX) OP(C2, UNK1,IMM) OP(C3, UNK,UNK) OP(C4, CPY,ZPG) OP(C5, CMP,ZPG) OP(C6, DEC,ZPG) OP(C7, UNK,UNK)
        OP(C8, INY,IMP) OP(C9, CMP,IMM) OP(CA, DEX,IMP) OP(CB, UNK,UNK) OP(CC, CPY,ABS) OP(CD, CMP,ABS) OP(CE, DEC,ABS) OP(CF, UNK,UNK)
        OP(D0, BNE,REL) OP(D1, CMP,IYR) OP(D2, UNK,UNK) OP(D3, UNK,UNK) OP(D4, UNK1,ZPX) OP(D5, CMP,ZPX) OP(D6, DEC,ZPX) OP(D7, UNK,UNK)
        OP(D8, CLD,IMP) OP(D9, CMP,AYR) OP(DA, UNK1,UNK) OP(DB, UNK,UNK) OP(DC, UNK1,AYR) OP(DD, CMP,AXR) OP(DE, DEC,AXW) OP(DF, UNK,UNK)
        OP(E0, CPX,IMM) OP(E1, SBC,INX) OP(E2, UNK1,IMM) OP(E3, UNK,UNK) OP(E4, CPX,ZPG) OP(E5, SBC,ZPG) OP(E6, INC,ZPG) OP(E7, UNK,UNK)
        OP(E8, INX,IMP) OP(E9, SBC,IMM) OP(EA, UNK1,UNK) OP(EB, UNK1,IMM) OP(EC, CPX,ABS) OP(ED, SBC,ABS) OP(EE, INC,ABS) OP(EF, UNK,UNK)
        OP(F0, BEQ,REL) OP(F1, SBC,IYR) OP(F2, UNK,UNK) OP(F3, UNK,UNK) OP(F4, UNK1,ZPX) OP(F5, SBC,ZPX) OP(F6, INC,ZPX) OP(F7, UNK,UNK)
        OP(F8, SED,IMP) OP(F9, SBC,AYR) OP(FA, UNK,IMP) OP(FB, UNK,UNK) OP(FC, UNK1,AYR) OP(FD, SBC,AXR) OP(FE, INC,AXW) OP(FF, UNK,UNK)

        default:
            PrintFormat( "Opcode %02X not implemented yet.", opCode );
            Assert( false && "operation not supported!");
    }

    P = CompactFlags();
    memcpy( &P, &flags, 1 );

    if ( prevhandleIrq ) {
        HandleInterrupt();
    }
}

void Nes::Cpu::Init( Ppu* ppu, Apu* apu, MemoryController* memoryController ) {

    this->ppu = ppu;
    this->apu = apu;
    this->memoryController = memoryController;
}

void Nes::Cpu::Reset() {
    A = X = Y = 0;
    S = 0xFD;
    P = 0x24;
    PC = 0;
    memset( &flags, 0x24, 1 );
    memset( &ram, 0xFF, kRamSize );

    cycles = frameCycles = 0;
    opCode = 0;
    opAddress = 0;

    nmistate = irqstate = 0;

    Tick();
    Tick();
    Tick();
    Tick();
    Tick();

    OP_SEI();

    PC = MemoryReadWord( kResetVector );
}

void Nes::Cpu::Tick() {
    
    ++cycles;
    ++frameCycles;

    ppu->Tick();
    //apu->Tick();
    // mapper tick

    // update interrupts
    prevhandleIrq = handleIrq;
    handleIrq = nmistate || (flags.i == 0 && irqstate);
}

uint8 Nes::Cpu::MemoryRead( uint16 address ) {    
    Tick();

    return memoryController->CpuRead( address );
}

void Nes::Cpu::MemoryWrite( uint16 address, uint8 data ) {
    Tick();

    memoryController->CpuWrite( address, data );
}

void Nes::Cpu::Push( uint8 data ) {
    MemoryWrite(S | 0x100, data);
    S--;
}

uint8 Nes::Cpu::Pop() {
    S++;
    return MemoryRead(S | 0x100);
}

void Nes::Cpu::SetZeroOrNegativeFlags( uint8 value ) {
    flags.n = ( value >> 7 ) & 1;
    flags.z = value == 0 ? 1 : 0;
}

void Nes::Cpu::ExpandFlags( uint8 value ) {
    flags.c = ( value & 0x01 ) >> 0;
    flags.z = ( value & 0x02 ) >> 1;
    flags.i = ( value & 0x04 ) >> 2;
    flags.d = ( value & 0x08 ) >> 3;
    flags.v = ( value & 0x40 ) >> 6;
    flags.n = ( value & 0x80 ) >> 7;
}

uint8 Nes::Cpu::CompactFlags() {
    uint8 p = 0x20;
    p |= ( flags.c ) << 0;
    p |= ( flags.z ) << 1;
    p |= ( flags.i ) << 2;
    p |= ( flags.d ) << 3;
    p |= ( flags.v ) << 6;
    p |= ( flags.n ) << 7;

    return p;
}


void Nes::Cpu::SetNMI() {
    nmistate = 1;
}

void Nes::Cpu::ClearNMI() {
    nmistate = 0;
}


void Nes::Cpu::HandleInterrupt() {
    DummyRead();
    DummyRead();

    Push( (uint8)( PC >> 8 ) );
    Push( (uint8)PC );

    if ( nmistate ) {
        // Do not use PHP, it always sets the 'b' flag.
        uint8 p = CompactFlags();
        Push( p );

        flags.i = 1;
        // nmi state can be cleared.
        nmistate = 0;

        PC = MemoryReadWord( kNmiVector );
    }
    else {
        // Do not use PHP, it always sets the 'b' flag.
        uint8 p = CompactFlags();
        Push( p );

        flags.i = 1;

        PC = MemoryReadWord( kIrqVector );
    }
}

void Nes::Cpu::SetIRQ( uint8 mask ) {
    irqstate |= mask;
}

void Nes::Cpu::ClearIRQ( uint8 mask ) {
    irqstate &= ~mask;
}


//////////////////////////////////////////////////////////////////////////
void Nes::Cart::Init() {
    Reset();
}

void Nes::Cart::Reset() {
    filename.clear();
    mapperIndex = kInvalidMapper;
}

bool Nes::Cart::LoadRom( cstring filename ) {

    FileHandle romFile;
    OpenFile( filename, "rb", &romFile );
    if ( !romFile ) {
        PrintFormat( "Error loading %s\n", filename );
        Reset();

        return false;
    }

    this->filename.clear();
    this->filename.append( filename );

    ReadFile( (uint8*)&romHeader, 1, 16, romFile );

    // http://wiki.nesdev.com/w/index.php/INES
    bool iNESFormat = false;
    if ( romHeader.nesLetters[0] == 'N' && romHeader.nesLetters[1] == 'E' && romHeader.nesLetters[2] == 'S' && romHeader.nesLetters[3] == 0x1A )
        iNESFormat = true;

    bool NES20Format = false;
    if ( iNESFormat == true && ( romHeader.flag0 & 0x0C ) == 0x08 )
        NES20Format = true;

    // clear data!
    prgRom.clear();
    chrRom.clear();
    prgRam.clear();

    //    76543210
    //    ||||||||
    //    |||||||+-Mirroring: 0 : horizontal( vertical arrangement ) (CIRAM A10 = PPU A11)
    //    |||||||              1 : vertical( horizontal arrangement ) (CIRAM A10 = PPU A10)
    //    ||||||+-- 1 : Cartridge contains battery - backed PRG RAM( $6000 - 7FFF ) or other persistent memory
    //    |||||+-- - 1 : 512 - byte trainer at $7000 - $71FF( stored before PRG data )
    //    ||||+---- 1 : Ignore mirroring control or above mirroring bit; instead provide four - screen VRAM
    //    ++++---- - Lower nybble of mapper number
    const uint8 hasTrainer = (romHeader.flag0 >> 3) & 1;
    if ( hasTrainer ) {
        PrintFormat( "Trainer present, but still not supported\n" );
    }

    const uint8 hasBattery = (romHeader.flag0 >> 2) & 1;
    if ( hasBattery ) {
        PrintFormat( "Battery supported!\n" );
    }

    // Always allocate some SRAM. Some INES headers are incorrect in the sram presence.
    prgRam.resize( romHeader.prgRamPages ? kPrgRamSize * romHeader.prgRamPages : kPrgRamSize );

    // Search for sram file
    String sramFilename( filename );
    sramFilename.append( ".sram" );

    FileHandle sramFile;
    OpenFile( sramFilename.c_str() , "rb", &sramFile );
    if ( sramFile ) {
        // Found sram file, fill the prgRam.
        // TODO: how to handle differences in sram size/prg ram reported size ?
        ReadFile( prgRam.begin(), 1, prgRam.size(), sramFile );
    }
    CloseFile( sramFile );

    // copy prg
    if ( romHeader.prgRomPages ) {
        const uint32 size = romHeader.prgRomPages * kPrgBankSize;
        prgRom.resize( size );
        ReadFile( prgRom.data, 1, size, romFile );
    }
    // copy chr
    if ( romHeader.chrRomPages ) {
        const uint32 size = romHeader.chrRomPages * kChrBankSize;
        chrRom.resize( size );
        ReadFile( chrRom.data, 1, size, romFile );
    }

    // check mapper type
    mapperIndex = ( romHeader.flag1 & 0xF0 ) | ( romHeader.flag0 >> 4 );
    PrintFormat( "Loading rom with mapper %u\n", mapperIndex );

    CloseFile( romFile );

    return true;
}

//////////////////////////////////////////////////////////////////////////
Nes::Mapper0::Mapper0( Cpu* cpu, Cart & cart ) : Mapper(cpu) {
    const uint16 romPrgBanks = cart.prgRom.size() / kPrgBankSize;
    prgAddressMask = romPrgBanks == 1 ? 0x3FFF : 0x7FFF;

    mirroring = (cart.romHeader.flag0 & 1) ? MirrorVertical : MirrorHorizontal;
    
    prg = cart.prgRom.begin();
    chr = cart.chrRom.size() ? cart.chrRom.begin() : cart.chrRam;
}

uint8 Nes::Mapper0::ChrRead( uint16 address ) {
    return chr[address];
}

void Nes::Mapper0::ChrWrite( uint16 address, uint8 data ) {
    chr[address] = data;
}

uint8 Nes::Mapper0::PrgRead( uint16 address ) {
    address = (address - Nes::MemoryController::kPrgRomOffsetBank0) & prgAddressMask;
    return prg[address];
}

void Nes::Mapper0::PrgWrite( uint16 address, uint8 data ) {
    address = (address - Nes::MemoryController::kPrgRomOffsetBank0) & prgAddressMask;
    prg[address] = data;
    
    prgRamWrite = true;
}

//////////////////////////////////////////////////////////////////////////

//#define MMC1_ASSERT
//#define MAPPER_WRITE_ASSERT

Nes::Mapper1::Mapper1( Cpu* cpu, Cart & cart ) : Mapper( cpu ) {

    // Initial state
    writeCounter = 0;
    for ( uint8 i = 0; i < Register_Count; ++i ) {
        registers[i] = 0;
    }
    registers[0] = 0xC;

    prgRomBankCount = cart.prgRom.size() / kPrgBankSize;
    chrRomBankCount = cart.chrRom.size() ? cart.chrRom.size() / kChrBankSize : 1;

    prgRam = cart.prgRam.size() ? cart.prgRam.begin() : nullptr;
    prg = cart.prgRom.begin();
    chr = cart.chrRom.size() ? cart.chrRom.begin() : cart.chrRam;

    UpdateBanks();
}

uint8 Nes::Mapper1::ChrRead( uint16 address ) {
    return address < 0x1000 ? chrRomBank0[address] : chrRomBank1[address - 0x1000];
}

void Nes::Mapper1::ChrWrite( uint16 address, uint8 data ) {

    if ( address < 0x1000 )
        chrRomBank0[address] = data;
    else
        chrRomBank1[address - 0x1000] = data;
}

uint8 Nes::Mapper1::PrgRead( uint16 address ) {
    if ( address < 0x8000 ) {
        return prgRam[address - 0x6000];
    }
    else if ( address < 0xC000 ) {
        return prgRomBank0[address - 0x8000];
    }
    else {
        return prgRomBank1[address - 0xC000];
    }
}

void Nes::Mapper1::PrgWrite( uint16 address, uint8 data ) {
    if ( address < 0x8000 ) {
        prgRam[address - 0x6000] = data;

        prgRamWrite = true;
    }
    else if ( address & 0x8000 ) {

        // Writing a value with bit 7 set ($80 through $FF) to any address in $8000-$FFFF
        // clears the shift register to its initial state.
        if ( data & 0x80 ) {
            writeCounter = 0;
            temporaryRegister = 0;
            registers[Register_Control] |= 0x0C;

            UpdateBanks();
        }
        else {
            // To change a register's value, the CPU writes five times with bit 7 clear and
            // a bit of the desired value in bit 0. On the first four writes, the MMC1 shifts
            // bit 0 into a shift register. On the fifth write, the MMC1 copies bit 0 and 
            // the shift register contents into an internal register selected by bits 14 and 13
            // of the address, and then it clears the shift register.
            // Only on the fifth write does the address matter, and even then, 
            // only bits 14 and 13 of the address matter because the mapper registers are 
            // incompletely decoded like the PPU registers. After the fifth write, the 
            // shift register is cleared automatically, so a write to the shift register with 
            // bit 7 on to reset it is not needed.
            // Writes the 5 bit and shift the register.
            temporaryRegister = ((data & 1) << 4) | (temporaryRegister >> 1);
            if ( ++writeCounter == 5 ) {
                registers[(address >> 13) & 0x3] = temporaryRegister;
                temporaryRegister = 0;
                writeCounter = 0;

                UpdateBanks();
            }
        } 
    }
}

void Nes::Mapper1::UpdateBanks() {
    
    // Prg rom bank switching
    const uint8 prgRomBankMode = (registers[Register_Control] >> 2) & 0x3;
    switch ( prgRomBankMode ) {
        case 0:
        case 1: {
            // 0, 1: switch 32 KB at $8000, ignoring low bit of bank number
            // Register uses 3 bits, ignoring the low with the shift.
            const uint8 bankIndex = (registers[Register_Prg] >> 1) & 0x7;
            prgRomBank0 = prg + kPrgBankSize * bankIndex;
            prgRomBank1 = prgRomBank0 + kPrgBankSize;
#if defined(MMC1_ASSERT)
            if ( bankIndex >= prgRomBankCount )
                Assert( false );
#endif
            break;
        }
        case 2: {
            // 2 : fix first bank at $8000 and switch 16 KB bank at $C000
            // Register uses 4 bits.
            const uint8 bankIndex = registers[Register_Prg] & 0xf;
            prgRomBank0 = prg;
            prgRomBank1 = prg + kPrgBankSize * bankIndex;
#if defined(MMC1_ASSERT)
            if ( bankIndex >= prgRomBankCount )
                Assert( false );
#endif
            break;
        }
        case 3: {
            // 3: fix last bank at $C000 and switch 16 KB bank at $8000
            // Register uses 4 bits.
            uint8 lastBank = prgRomBankCount - 1;
            const uint8 bankIndex = registers[Register_Prg] & 0xf;
            prgRomBank0 = prg + kPrgBankSize * bankIndex;
            prgRomBank1 = prg + kPrgBankSize * lastBank;
#if defined(MMC1_ASSERT)
            if ( bankIndex >= prgRomBankCount )
                Assert( false );
#endif
            break;
        }
    }

    // Chr rom bank switching
    const uint8 chrRomBankMode = (registers[Register_Control] >> 4) & 1;
    switch ( chrRomBankMode ) {
        case 0: {
            // 0: switch 8 KB at a time
            // Select 4 KB or 8 KB CHR bank at PPU $0000 (low bit ignored in 8 KB mode).
            // Uses 4 bits in total (low bit ignored with the shift).
            const uint8 bankIndex = (registers[Register_Chr0] >> 1) & 0xf;
            chrRomBank0 = chr + kChrBankSize * bankIndex;
            chrRomBank1 = chrRomBank0 + (kChrBankSize / 2);
#if defined(MMC1_ASSERT)
            if ( bankIndex >= chrRomBankCount )
                Assert( false );
#endif
            break;
        }
        case 1: {
            // 1: switch two separate 4 KB banks.
            // Bank size is 8k, so address calculations need to use 4kb instead.
            // Register uses 5 bits.
            const uint8 chrBank0 = registers[Register_Chr0] & 0x1f;
            chrRomBank0 = chr + (kChrBankSize / 2) * chrBank0;
#if defined(MMC1_ASSERT)
            if ( chrBank0 >= chrRomBankCount )
                Assert( false );
#endif
            const uint8 chrBank1 = registers[Register_Chr1] & 0x1f;
            chrRomBank1 = chr + (kChrBankSize / 2) * chrBank1;
#if defined(MMC1_ASSERT)
            if ( chrBank1 >= chrRomBankCount )
                Assert( false );
#endif
            break;
        }
    }

    // Mirroring
    switch ( registers[Register_Control] & 0x03 ) {
        case 0:
            mirroring = MirrorSingleLow;
            break;
        case 1:
            mirroring = MirrorSingleHigh;
            break;
        case 2:
            mirroring = MirrorVertical;
            break;
        case 3:
            mirroring = MirrorHorizontal;
            break;
    }
}

//////////////////////////////////////////////////////////////////////////
Nes::Mapper2::Mapper2( Cpu* cpu, Cart & cart ) : Mapper( cpu ) {
    prgBankSelector = 0;

    mirroring = (cart.romHeader.flag0 & 1) ? MirrorVertical : MirrorHorizontal;

    const uint16 romPrgBanks = cart.prgRom.size() / kPrgBankSize;
    // Bank 0 is switchable
    prgBank0 = cart.prgRom.begin();
    // Bank 1 is fixed to last bank
    const uint16 lastBankIndex = romPrgBanks > 1 ? romPrgBanks - 1 : 0;
    prgBank1 = cart.prgRom.begin() + (kPrgBankSize * lastBankIndex);

    prg = cart.prgRom.begin();
    chr = cart.chrRom.size() ? cart.chrRom.begin() : cart.chrRam;
}

uint8 Nes::Mapper2::ChrRead( uint16 address ) {
    return chr[address];
}

void Nes::Mapper2::ChrWrite( uint16 address, uint8 data ) {
    chr[address] = data;
}

uint8 Nes::Mapper2::PrgRead( uint16 address ) {
    return address < 0xC000 ? prgBank0[address - 0x8000] : prgBank1[address - 0xC000];
}

void Nes::Mapper2::PrgWrite( uint16 address, uint8 data ) {

    // Bank switching
    if ( address & 0x8000 ) {
        prgBankSelector = data;

        UpdatePrgBank();
    }
}

void Nes::Mapper2::UpdatePrgBank() {
    const uint8 bankIndex = prgBankSelector & 0xF;
    prgBank0 = prg + (kPrgBankSize * bankIndex);
}

//////////////////////////////////////////////////////////////////////////
Nes::Mapper3::Mapper3( Cpu* cpu, Cart & cart ) : Mapper(cpu) {
    const uint16 romPrgBanks = cart.prgRom.size() / kPrgBankSize;
    prgAddressMask = romPrgBanks == 1 ? 0x3FFF : 0x7FFF;

    mirroring = (cart.romHeader.flag0 & 1) ? MirrorVertical : MirrorHorizontal;

    prg = cart.prgRom.begin();
    chr = cart.chrRom.size() ? cart.chrRom.begin() : cart.chrRam;

    chrBankSelector = 0;
    chrBank = chr;
}

uint8 Nes::Mapper3::ChrRead( uint16 address ) {
    return chrBank[address];
}

void Nes::Mapper3::ChrWrite( uint16 address, uint8 data ) {
    chrBank[address] = data;
}

uint8 Nes::Mapper3::PrgRead( uint16 address ) {
    address = (address - Nes::MemoryController::kPrgRomOffsetBank0) & prgAddressMask;
    return prg[address];
}

void Nes::Mapper3::PrgWrite( uint16 address, uint8 data ) {
    if ( address & 0x8000 ) {
        chrBankSelector = data;
        UpdateChrBank();
    }
}

void Nes::Mapper3::UpdateChrBank() {
    chrBank = chr + (chrBankSelector & 0x3) * kChrBankSize;
}

//////////////////////////////////////////////////////////////////////////
Nes::Mapper4::Mapper4( Cpu* cpu, Cart& cart ) : Mapper(cpu) {

    prgMemory = cart.prgRom.begin();
    chrMemory = cart.chrRom.begin();
    prgRamMemory = cart.prgRam.begin();

    // 8k prg rom bank
    prgRomBankCount = cart.prgRom.size() / kMMC3PrgBankSize;

    registers[0] = 0;
    registers[1] = 2;
    registers[2] = 4;
    registers[3] = 5;
    registers[4] = 6;
    registers[5] = 7;
    registers[6] = 0;
    registers[7] = 1;

    bankSelector = 0;
    mirroringRegister = 0;

    irqCounter = 0;
    irqEnable = 0;
    irqReload = 0;
    irqReloadPeriod = 0;

    UpdateBanks();
}

uint8 Nes::Mapper4::ChrRead( uint16 address ) {
    return chrBanks[address / kMMC3ChrBankSize][address & kMMC3ChrBankMask];
}

void Nes::Mapper4::ChrWrite( uint16 address, uint8 data ) {
    chrBanks[address / kMMC3ChrBankSize][address & kMMC3ChrBankMask] = data;
}

uint8 Nes::Mapper4::PrgRead( uint16 address ) {
    return address < 0x8000 ? prgRamMemory[address - 0x6000] : prgBanks[(address - 0x8000) / kMMC3PrgBankSize][address & kMMC3PrgBankMask];
}

void Nes::Mapper4::PrgWrite( uint16 address, uint8 data ) {

    if ( address < 0x8000 ) {
        prgRamMemory[address - 0x6000] = data;
    }
    else if ( address & 0x8000 ) {
        
        switch ( address & 0xE001 ) {
            case 0x8000:
            {
                bankSelector = data;
                UpdateBanks();
                break;
            }

            case 0x8001:
            {
                uint8 currentRegister = bankSelector & 0x7;
                //data = data % prgRomBankCount;
                // R0 and R1 ignore the bottom bit, as the value written still counts banks in 1KB units but odd numbered banks can't be selected.
                data = currentRegister <= 1 ? data & ~0x1 : data;
                registers[currentRegister] = data;
                UpdateBanks();
                break;
            }

            case 0xA000:
            {
                mirroringRegister = data & 0x1;
                UpdateMirroring();
                break;
            }

            case 0xA001:
            {
                UpdateBanks();
                break;
            }

            case 0xC000:
            {
                irqReloadPeriod = data;
                break;
            }

            case 0xC001:
            {
                irqReload = 1;
                irqCounter = 0;
                break;
            }

            case 0xE000:
            {
                irqEnable = 0;
                cpu->ClearIRQ( 1 );
                break;
            }

            case 0xE001:
            {
                irqEnable = 1;
                break;
            }
        }
    }
}

void Nes::Mapper4::PpuAddress12Rise() {
    if ( irqCounter == 0 && irqReload ) {
        irqCounter = irqReloadPeriod;
    }
    else {
        --irqCounter;
    }

    if ( irqCounter == 0 && irqEnable ) {
        cpu->SetIRQ( 1 );
    }

    irqReload = 0;
}

void Nes::Mapper4::UpdateMirroring() {
    mirroring = mirroringRegister ? MirrorHorizontal : MirrorVertical;
}

void Nes::Mapper4::UpdateBanks() {

    // Prg mode 1
    if ( (bankSelector & 0x40) >> 6 ) {
        uint8 bank0 = prgRomBankCount - 2;
        bank0 = bank0 >= prgRomBankCount ? 0 : bank0;
        uint8 bank1 = prgRomBankCount - 1;
        bank1 = bank1 >= prgRomBankCount ? 0 : bank1;

        prgBanks[0] = prgMemory + (bank0) * kMMC3PrgBankSize;
        prgBanks[1] = prgMemory + registers[7] * kMMC3PrgBankSize;
        prgBanks[2] = prgMemory + registers[6] * kMMC3PrgBankSize;
        prgBanks[3] = prgMemory + (bank1) * kMMC3PrgBankSize;
    }
    else {
        // Prg mode 0
        uint8 bank0 = prgRomBankCount - 2;
        bank0 = bank0 >= prgRomBankCount ? 0 : bank0;
        uint8 bank1 = prgRomBankCount - 1;
        bank1 = bank1 >= prgRomBankCount ? 0 : bank1;

        prgBanks[0] = prgMemory + registers[6] * kMMC3PrgBankSize;
        prgBanks[1] = prgMemory + registers[7] * kMMC3PrgBankSize;
        prgBanks[2] = prgMemory + (bank0) * kMMC3PrgBankSize;
        prgBanks[3] = prgMemory + (bank1) * kMMC3PrgBankSize;
    }

    // Chr mode 1
    if ( (bankSelector & 0x80) >> 7 ) {
        // 1 kb banks
        chrBanks[0] = chrMemory + registers[2] * kMMC3ChrBankSize;
        chrBanks[1] = chrMemory + registers[3] * kMMC3ChrBankSize;
        chrBanks[2] = chrMemory + registers[4] * kMMC3ChrBankSize;
        chrBanks[3] = chrMemory + registers[5] * kMMC3ChrBankSize;

        // 2 kb banks, split the addressess
        chrBanks[4] = chrMemory + (registers[0] & 0xFE) * kMMC3ChrBankSize;
        chrBanks[5] = chrMemory + (registers[0] | 0x01) * kMMC3ChrBankSize;
        chrBanks[6] = chrMemory + (registers[1] & 0xFE) * kMMC3ChrBankSize;
        chrBanks[7] = chrMemory + (registers[1] | 0x01) * kMMC3ChrBankSize;
    } else {
        // Chr mode 0

        // 2 kb banks, split the addressess
        chrBanks[0] = chrMemory + (registers[0] & 0xFE) * kMMC3ChrBankSize;
        chrBanks[1] = chrMemory + (registers[0] | 0x01) * kMMC3ChrBankSize;
        chrBanks[2] = chrMemory + (registers[1] & 0xFE) * kMMC3ChrBankSize;
        chrBanks[3] = chrMemory + (registers[1] | 0x01) * kMMC3ChrBankSize;

        // 1 kb banks
        chrBanks[4] = chrMemory + registers[2] * kMMC3ChrBankSize;
        chrBanks[5] = chrMemory + registers[3] * kMMC3ChrBankSize;
        chrBanks[6] = chrMemory + registers[4] * kMMC3ChrBankSize;
        chrBanks[7] = chrMemory + registers[5] * kMMC3ChrBankSize;
    }
}

//////////////////////////////////////////////////////////////////////////

static uint16 NameTableMirroring( uint16 addr, Nes::Mapper::Mirroring mirroring ) {
    switch ( mirroring ) {
        case Nes::Mapper::MirrorVertical:
            return addr % 0x800;
        case Nes::Mapper::MirrorHorizontal:
            return ( ( addr / 2 ) & 0x400 ) + ( addr % 0x400 );
    }
    return addr - 0x2000;
}
void Nes::MemoryController::Init( Cart* cart, Cpu* cpu, Ppu* ppu, Controller* controllers, Apu* apu ) {
    this->cart = cart;
    this->ppu = ppu;
    this->cpu = cpu;
    this->controllers = controllers;
    this->apu = apu;
}

void Nes::MemoryController::LoadMapper() {
    switch ( cart->mapperIndex ) {
        case 0: {
            mapper = new Mapper0( cpu, *cart );
            break;
        }

        case 1: {
            mapper = new Mapper1( cpu, *cart );
            break;
        }

        case 2: {
            mapper = new Mapper2( cpu, *cart );
            break;
        }

        case 3: {
            mapper = new Mapper3( cpu, *cart );
            break;
        }

        case 4: {
            mapper = new Mapper4( cpu, *cart );
            break;
        }

        default:
            PrintFormat( "Mapper %u not implemented!", cart->mapperIndex );
            mapper = nullptr;
            break;
    }
}

uint8 Nes::MemoryController::CpuRead( uint16 address ) {

    if ( address < 0x2000 ) {
        return cpu->ram[address & 0x7FF];
    }
    else if ( address < 0x4000 ) {
        return ppu->CpuRead( address );
    }
    else if ( address < 0x4014 ) {
        return apu->CpuRead( address );
    }
    else if ( address >= 0x4018 ) {
        return mapper->PrgRead( address );
    }

    switch ( address ) {
        case 0x4015: {
            return apu->ReadStatus();
            break;
        }

        case 0x4016: {
            return controllers->ReadState();

            break;
        }
                     
        case 0x4017: {
            return 0x40;
            break;
        }
    }

    //PrintFormat( "CpuRead Unhandled location %02X\n", address );
    //Assert( false && "unhandled location" );
    return 0;
}

void Nes::MemoryController::CpuWrite( uint16 address, uint8 data ) {

    if ( address < 0x2000 ) {
        cpu->ram[address & 0x7FF] = data;
    }
    else if ( address < 0x4000 ) {
        ppu->CpuWrite( address, data );
        return;
    }
    else if ( address < 0x4014 ) {
        return apu->CpuWrite( address, data );
    }
    else if ( address >= 0x4018 ) {
        mapper->PrgWrite( address, data );
        return;
    }

    switch ( address ) {
        // Sprite DMA
        case 0x4014: {
            for ( int i = 0; i < 256; i++ ) {
                uint8 v = CpuRead( data * 0x100 + i );
                CpuWrite( 0x2014, v );
            }
            return;
            break;
        }

        case 0x4015:
        case 0x4017:{
            apu->CpuWrite( address, data );
            return;
            break;
        }

        case 0x4016: {
            controllers->WriteState( data );
            return;
            break;
        }
    }

    //PrintFormat( "CpuWrite Unhandled location %02X\n", address );
    //Assert( false && "unhandled location" );
}

uint8 Nes::MemoryController::PpuRead( uint16 address ) {
    
    address &= 0X3FFF;

    if ( address <= 0x1FFF ) {
        return mapper->ChrRead( address );;
    }
    else if ( address <= 0x3EFF ) {
        return ppu->nametableRam[NameTableMirroring( address, mapper->mirroring )];
    }
    else if ( address <= 0x3FFF ) {
        // Palette mirroring is handled in the write code.
        return ppu->paletteRam[address & 0x1F] & ((ppu->mask & Nes::Ppu::MaskFlag_GreyScale ? 0x30 : 0xFF));
    }

    PrintFormat( "PpuRead Unhandled location %02X\n", address );
    //Assert( false && "unhandled location" );
    return 0;
}

void Nes::MemoryController::PpuWrite( uint16 address, uint8 data ) {

    address &= 0X3FFF;

    if ( address <= 0x1FFF ) {
        mapper->ChrWrite( address, data );
        return;
    }
    else if ( address <= 0x3EFF ) {
        ppu->nametableRam[NameTableMirroring( address, mapper->mirroring )] = data;
        return;
    }
    else if ( address <= 0x3FFF ) {

        static uint8 const palette_write_mirror[0x20] = { 
            0x10, 0x01, 0x02, 0x03, 0x14, 0x05, 0x06, 0x07,
            0x18, 0x09, 0x0A, 0x0B, 0x1C, 0x0D, 0x0E, 0x0F,
            0x00, 0x11, 0x12, 0x13, 0x04, 0x15, 0x16, 0x17,
            0x08, 0x19, 0x1A, 0x1B, 0x0C, 0x1D, 0x1E, 0x1F };

        ppu->paletteRam[palette_write_mirror[address & 0x1F]] = data;
        return;
    }

    PrintFormat( "PpuWrite Unhandled location %02X\n", address );
    //Assert( false && "unhandled location" );
}

void Nes::MemoryController::PpuAddress12Rise() {
    mapper->PpuAddress12Rise();
}

//////////////////////////////////////////////////////////////////////////
// https://github.com/AndreaOrru/LaiNES/blob/master/src/palette.inc
const uint32 nesRgb[] =
{ 0x7C7C7C, 0x0000FC, 0x0000BC, 0x4428BC, 0x940084, 0xA80020, 0xA81000, 0x881400,
  0x503000, 0x007800, 0x006800, 0x005800, 0x004058, 0x000000, 0x000000, 0x000000,
  0xBCBCBC, 0x0078F8, 0x0058F8, 0x6844FC, 0xD800CC, 0xE40058, 0xF83800, 0xE45C10,
  0xAC7C00, 0x00B800, 0x00A800, 0x00A844, 0x008888, 0x000000, 0x000000, 0x000000,
  0xF8F8F8, 0x3CBCFC, 0x6888FC, 0x9878F8, 0xF878F8, 0xF85898, 0xF87858, 0xFCA044,
  0xF8B800, 0xB8F818, 0x58D854, 0x58F898, 0x00E8D8, 0x787878, 0x000000, 0x000000,
  0xFCFCFC, 0xA4E4FC, 0xB8B8F8, 0xD8B8F8, 0xF8B8F8, 0xF8A4C0, 0xF0D0B0, 0xFCE0A8,
  0xF8D878, 0xD8F878, 0xB8F8B8, 0xB8F8D8, 0x00FCFC, 0xF8D8F8, 0x000000, 0x000000 };

void Nes::Ppu::Init( Cpu* cpu, Screen* screen, MemoryController* memoryController ) {
    this->memoryController = memoryController;
    this->screen = screen;
    this->cpu = cpu;
}

void Nes::Ppu::Tick() {
    // handle region difference

    // One cpu tick is equal to 3 ppu ticks.
    Step();
    Step();
    Step();
}

void Nes::Ppu::Reset() {
    pixelCycle = 0;
    scanline = kMaxScanlines - 1; // Start from pre render scanline
    frames = 0;
    control = mask = status = 0;
    verticalBlank = verticalBlankOccurred = 0;
    v = t = 0;
    debugCycles = false;
    spriteOverflow = spriteHit = 0;
    oamAddress = 0;

    memset( nametableRam, 0, kNameTableRamSize );
    memset( paletteRam, 0, kPaletteRamSize );
    memset( oamMemory, 0, kOAMRamSize );

#if defined(NES_TEST_PPU_CYCLE)
    if ( !debugger.pixelData ) {
        debugger.pixelData = new Ppu::PpuDebugger::PixelData[kMaxScanlines * kMaxPixels];
        memset( debugger.pixelData, 0, sizeof(Ppu::PpuDebugger::PixelData) * kMaxScanlines * kMaxPixels );
    }
#endif
}

bool Nes::Ppu::IsRendering() {
    return (mask & MaskFlag_EnableRendering) != 0;
}

uint8 Nes::Ppu::SpriteHeight() {
    return (control & ControlFlag_SpriteSize) ? 16 : 8;
}

uint8 Nes::Ppu::CpuRead(uint16 address) {
    uint8 value = 0;

    switch (address & 7) {
        case Ppu::R2000_PPUCTRL: {
        
            PrintFormat("CpuRead PPU 2000 %02X\n", address);
            //value = control;
            break;
        }

        case Ppu::R2001_PPUMASK: {

            PrintFormat("CpuRead PPU 2001 %02X\n", address);
            //value = mask;
            break;
        }

        case Ppu::R2002_PPUSTATUS: {

            value = register_ & 0x1F;
            value |= spriteOverflow << 5;
            value |= spriteHit << 6;
            value |= verticalBlank << 7;

            verticalBlank = 0;
            // reset write toggle
            w = 0;

            // https://wiki.nesdev.com/w/index.php/PPU_frame_timing
            // Special behaviour of vertical blank VBL flag when just set (scanline 241)
            if ( scanline == 241 ) {
                if ( pixelCycle < 3 ) {
                    // Reading on the same PPU clock or one later reads it as set, clears it,
                    // and suppresses the NMI for that frame.Reading two or more PPU clocks
                    // before / after it's set behaves normally/

                    // Summing up with the behaviour at pixel 0, between pixels 0 and 2 included
                    // NMI is suppressed.
                    cpu->ClearNMI();
                }
                // Reading one PPU clock before reads it as clear and never sets the flag or generates NMI for that frame.
                // NOTE: VBL is set at pixel 1 of scanline 241
                if ( pixelCycle == 0 ) {
                    // Do not return vertical blank flag.
                    value = spriteOverflow << 5;
                    value |= spriteHit << 6;
                }
            }

            break;
        }

        case Ppu::R2003_OAMADDR: {

            break;
        }

        case Ppu::R2004_OAMDATA: {
            value = oamMemory[oamAddress];
            break;
        }

        case Ppu::R2005_PPUSCROLL: {
        
            break;
        }

        case Ppu::R2006_PPUADDR: {
        
            break;
        }

        case Ppu::R2007_PPUDATA: {

            if (v < 0x3EFF) {
                value = internalReadBuffer;

                internalReadBuffer = memoryController->PpuRead(v);
            }
            else {
                value = internalReadBuffer = memoryController->PpuRead(v);
            }
        
            IncrementAddressV();
            break;
        }
    }
    // return nmi status: none, set, clear
    return value;
}

void Nes::Ppu::CpuWrite(uint16 address, uint8 data) {

    register_ = data;

    switch (address & 7) {
        case Ppu::R2000_PPUCTRL: {
            // t: ...BA.. ........ = d: ......BA
            t = (t & 0xF3FF) | ((uint16(data) & 0x03) << 10);

            // https://wiki.nesdev.com/w/index.php/PPU_registers
            // If the PPU is currently in vertical blank, and the PPUSTATUS ($2002) vblank flag is 
            // still set (1), changing the NMI flag in bit 7 of $2000 from 0 to 1 will immediately generate an NMI.
            // Detect if the nmi is just enabled during vblank and generate an NMI.
            bool previousGenerateNMI = (control & ControlFlag_GenerateNMI) == ControlFlag_GenerateNMI;
            control = data;

            bool switchingNMIOn = !previousGenerateNMI && (control & ControlFlag_GenerateNMI);
            bool properTiming = (scanline < 261 || pixelCycle > 0);
            if ( verticalBlank && switchingNMIOn && properTiming ) {
                cpu->SetNMI();
            }

            if ( scanline == 241 && pixelCycle < 3 && !(control & ControlFlag_GenerateNMI) ) {
                cpu->ClearNMI();
            }

            break;
        }

        case Ppu::R2001_PPUMASK: {
            mask = data;
            break;
        }

        case Ppu::R2002_PPUSTATUS: {
            status = data;
            break;
        }

        case Ppu::R2003_OAMADDR: {
            oamAddress = data;
            break;
        }

        case Ppu::R2004_OAMDATA: {
            oamMemory[oamAddress++] = data;
            break;
        }

        case Ppu::R2005_PPUSCROLL: {
        
            if (w == 0) {

                // t: ........ ...HGFED = d: HGFED...
                // x:               CBA = d: .....CBA
                // w:                   = 1
                t = (t & 0xFFE0) | (data >> 3);
                fineX = (data & 0x7);

                w = 1;
            }
            else {
                // t: .CBA..HG FED..... = d: HGFEDCBA
                // w:                   = 0
                t = (t & 0x8FFF) | ((uint16(data) & 0x07) << 12);
                t = (t & 0xFC1F) | ((uint16(data) & 0xF8) << 2);

                w = 0;
            }

            break;
        }

        case Ppu::R2006_PPUADDR: {

            if (w == 0) {
                // t:.FEDCBA ........ = d : ..FEDCBA
                // t : X...... ........ = 0
                // w : = 1
                t = (t & 0x80FF) | ((uint16(data) & 0x3F) << 8);

                w = 1;
            }
            else {
                // t: ........ HGFEDCBA = d: HGFEDCBA
                // v                    = t
                // w:                   = 0
                t = (t & 0xFF00) | uint16(data);
                v = t;

                w = 0;
            }
        
            break;
        }

        case Ppu::R2007_PPUDATA: {

            memoryController->PpuWrite(v, data);
        
            IncrementAddressV();
            break;
        }
    }
}

void Nes::Ppu::ReadNameTable() {
    nameTableAddress = 0x2000 | ( v & 0xFFF );
    nameTableByte = memoryController->PpuRead( nameTableAddress );

#if defined(NES_TEST_PPU_CYCLE)
    if ( debugCycles ) {
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].nameTableByte = nameTableByte;
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].nameTableAddress = nameTableAddress;
    }
#endif // NES_TEST_PPU_CYCLE
}

void Nes::Ppu::ReadAttributeTable() {
    attributeTableAddress = 0x23C0 | ( v & 0x0C00 ) | ( ( v >> 4 ) & 0x38 ) | ( ( v >> 2 ) & 0x07 );
    attributeTableByte = memoryController->PpuRead( attributeTableAddress );

#if defined(NES_TEST_PPU_CYCLE)
    if ( debugCycles ) {
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].attributeTableByte = attributeTableByte;
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].attributeTableAddress = attributeTableAddress;
    }
#endif // NES_TEST_PPU_CYCLE
}

void Nes::Ppu::ReadTileHigh() {
    
    tileHighByte = memoryController->PpuRead( tileAddress + 8 );

#if defined(NES_TEST_PPU_CYCLE)
    if ( debugCycles ) {
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].tileHighByte = tileHighByte;
    }
#endif // NES_TEST_PPU_CYCLE
}

void Nes::Ppu::ReadTileLow() {
    // DCBA98 76543210
    // -------------- -
    // 0HRRRR CCCCPTTT
    // |||||| |||||++ + -T: Fine Y offset, the row number within a tile
    // |||||| ||||+----P : Bit plane( 0: "lower"; 1: "upper" )
    // |||||| ++++---- - C : Tile column
    // || ++++----------R : Tile row
    // | +--------------H : Half of sprite table( 0: "left"; 1: "right" )
    // + -------------- - 0 : Pattern table is at $0000 - $1FFF
    uint8 fineY = ( v >> 12 ) & 0x7;
    uint8 table = (control >> 4) & 1;

    tileAddress = 0x1000 * (uint16)table + ((uint16)nameTableByte * 16 ) + fineY;
    tileLowByte = memoryController->PpuRead( tileAddress );

#if defined(NES_TEST_PPU_CYCLE)
    if ( debugCycles ) {
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].tileLowByte = tileLowByte;
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].tileAddress = tileAddress;
    }
#endif // NES_TEST_PPU_CYCLE
}

void Nes::Ppu::ClearSecondaryOAM() {
    for ( uint8 i = 0; i < kMaxPerLineSprites; ++i ) {
        secOam[i].id = 64;
        secOam[i].y = 0xFF;
        secOam[i].tile = 0xFF;
        secOam[i].attribute = 0xFF;
        secOam[i].x = 0xFF;
        secOam[i].dataL = 0;
        secOam[i].dataH = 0;
    }
}

void Nes::Ppu::EvaluateSprites() {
    int n = 0;
    for ( uint32 i = 0; i < 64; i++ ) {
        int line = (scanline == 261 ? -1 : scanline) - oamMemory[i * 4 + 0];
        // If the sprite is in the scanline, copy its properties into secondary OAM:
        if ( line >= 0 && line < SpriteHeight() ) {
            secOam[n].id = i;
            secOam[n].y = oamMemory[i * 4 + 0];
            secOam[n].tile = oamMemory[i * 4 + 1];
            secOam[n].attribute = oamMemory[i * 4 + 2];
            secOam[n].x = oamMemory[i * 4 + 3];

            if ( ++n > 8 ) {
                spriteOverflow = true;
                break;
            }
        }
    }
}

void Nes::Ppu::LoadSprites() {
    uint16 address;

    for ( uint8 i = 0; i < 8; i++ ) {
        // Copy secondary OAM into primary.
        oam[i] = secOam[i];

        // Different address modes depending on the sprite height:
        if ( SpriteHeight() == 16 )
            address = ((oam[i].tile & 1) * 0x1000) + ((oam[i].tile & ~1) * 16);
        else
            address = ((((control & ControlFlag_SpriteAddress) >> 3) & 1) * 0x1000) + (oam[i].tile * 16);

        // Line inside the sprite.
        uint16 sprY = (scanline - oam[i].y) % SpriteHeight();
        // Vertical flip.
        if ( oam[i].attribute & 0x80 )
            sprY ^= SpriteHeight() - 1;

        // Select the second tile if on 8x16.
        address += sprY + (sprY & 8);

        oam[i].dataL = memoryController->PpuRead( address + 0 );
        oam[i].dataH = memoryController->PpuRead( address + 8 );
    }
}

void Nes::Ppu::DrawPixel() {
    #define NTH_BIT(x, n) (((x) >> (n)) & 1)

    uint16 paletteAddress = 0, objectPaletteAddress = 0;
    bool objectPriority = false;
    uint16 x = pixelCycle - 2;

    // Being positive only number, if -1 x = 0xffff thus we can use this condition
    if ( x < 256 && scanline < 240 ) {
        const bool avoidLeftPixels = !(mask & MaskFlag_LeftColumnBackground) && (x < 8);

        // Background:
        // Palette index = 4 bits
        // 2 bits from background tile and 2 from attribute table
        // 3    2   1   0
        // atH atL bgH bgL
        if ( (!avoidLeftPixels) && (mask & MaskFlag_EnableBackground) ) {
            paletteAddress = (NTH_BIT( backgroundTilesHighBits, 15 - fineX ) << 1) | NTH_BIT( backgroundTilesLowBits, 15 - fineX );
            if ( paletteAddress ) {
                
                uint8 attributeTableBits = (fineX + (x& 0x07) < 8) ? previousAT << 2 : currentAT << 2;
#if defined(PPU_SHIFT_REGISTERS)
                uint8 atcurrent = ((NTH_BIT( atShiftH, 7 - fineX ) << 3) | NTH_BIT( atShiftL, 7 - fineX ) << 2);
                if ( atcurrent != attributeTableBits )
                    PrintFormat( "Wrong Attribute Table bits! \n" );
#endif
                paletteAddress |= attributeTableBits;
            }
        }

        const bool spriteAvoidLeftPixels = !(mask & MaskFlag_LeftColumnSprite) && (x < 8);
        if ( !spriteAvoidLeftPixels && (mask & MaskFlag_EnableSprite) ) {

            for ( int i = 7; i >= 0; i-- ) {
                if ( oam[i].id == 64 )
                    continue;  // Void entry.

                unsigned sprX = x - oam[i].x;
                if ( sprX >= 8 )
                    continue;            // Not in range.

                if ( oam[i].attribute & 0x40 )
                    sprX ^= 7;  // Horizontal flip.

                uint8 sprPalette = (NTH_BIT( oam[i].dataH, 7 - sprX ) << 1) |
                                    NTH_BIT( oam[i].dataL, 7 - sprX );
                if ( sprPalette == 0 )
                    continue;  // Transparent pixel.

                if ( oam[i].id == 0 && paletteAddress && x != 255 )
                    spriteHit = true;

                sprPalette |= (oam[i].attribute & 3) << 2;
                objectPaletteAddress = sprPalette + 16;
                objectPriority = oam[i].attribute & 0x20;
            }
        }

        // Evaluate priority:
        if ( objectPaletteAddress && (paletteAddress == 0 || objectPriority == 0) )
            paletteAddress = objectPaletteAddress;

        uint8 paletteIndex = memoryController->PpuRead( 0x3F00 + (IsRendering() ? paletteAddress : 0) );
        screen->WritePixel( scanline, x, nesRgb[paletteIndex] );
#if defined(NES_TEST_PPU_CYCLE)
        if ( debugCycles ) {
            debugger.pixelData[pixelCycle + scanline * kMaxPixels].events |= PpuDebugger::Draw;
            debugger.pixelData[pixelCycle + scanline * kMaxPixels].paletteAddress = paletteAddress;
            debugger.pixelData[pixelCycle + scanline * kMaxPixels].paletteIndex = paletteIndex;
        }
#endif
    }
    

    // Perform background shifts:
    backgroundTilesLowBits <<= 1;
    backgroundTilesHighBits <<= 1;

#if defined(PPU_SHIFT_REGISTERS)
    atShiftL = ( atShiftL << 1 ) | atLatchL;
    atShiftH = ( atShiftH << 1 ) | atLatchH;
#endif

#if defined(NES_TEST_PPU_CYCLE)
    if ( debugCycles ) {
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].events |= PpuDebugger::Shift;
        //PrintFormat( (mask & MaskFlag_EnableBackground) ? "bs!," : "bs ," );
    }
#endif // NES_TEST_PPU_CYCLE
}

uint32 Nes::Ppu::GetPaletteColor( uint8 index ) {
    return nesRgb[index];
}

void Nes::Ppu::ReloadShiftRegisters() {
    // After 8 shifts the high bits contain the now previous tile.
    // Copy the current tile data in the lower 8 bits.
    backgroundTilesLowBits = (backgroundTilesLowBits & 0xFF00) | tileLowByte;
    backgroundTilesHighBits = (backgroundTilesHighBits & 0xFF00) | tileHighByte;

#if defined(PPU_SHIFT_REGISTERS)
    atLatchL = (attributeTableByte & 1);
    atLatchH = (attributeTableByte >> 1) & 1;
#endif

#if defined(NES_TEST_PPU_CYCLE)
    if ( debugCycles ) {
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].events |= PpuDebugger::ReloadShift;
    }
#endif // NES_TEST_PPU_CYCLE
}

// Taken directly from
// https://wiki.nesdev.com/w/index.php/PPU_scrolling#Wrapping_around
void Nes::Ppu::IncrementHoriV() {
    if ( !IsRendering() )
        return;

    if ((v & 0x001F) == 31) {       // if coarse X == 31
        v &= ~0x001F;               // coarse X = 0
        v ^= 0x0400;                // switch horizontal nametable
    }
    else {
        v += 1;                     // increment coarse X
    }

#if defined(NES_TEST_PPU_CYCLE)
    if ( debugCycles ) {
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].events |= PpuDebugger::IncrementX;
    }
#endif // NES_TEST_PPU_CYCLE
}

void Nes::Ppu::IncrementVertV() {
    if ( !IsRendering() )
        return;

    if ((v & 0x7000) != 0x7000) {       // if fine Y < 7
        v += 0x1000;                    // increment fine Y
    }
    else {
        v &= ~0x7000;                   // fine Y = 0
        uint16 y = (v & 0x03E0) >> 5;      // let y = coarse Y
        if (y == 29) {
            y = 0;                      // coarse Y = 0
            v ^= 0x0800;                // switch vertical nametable
        }
        else if (y == 31) {
            y = 0;                      // coarse Y = 0, nametable not switched
        }
        else {
            y++;                        // increment coarse Y
        }
        v = (v & ~0x03E0) | (y << 5);   // put coarse Y back into v
    }

#if defined(NES_TEST_PPU_CYCLE)
    if ( debugCycles ) {
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].events |= PpuDebugger::IncrementY;
    }
#endif // NES_TEST_PPU_CYCLE
}

void Nes::Ppu::CopyHoriV() {
    if ( !IsRendering() )
        return;

    // v: ....F.. ...EDCBA = t: ....F.. ...EDCBA
    v = (v & ~0x041F) | (t & 0x041F);
    //v = (v & 0xFBE0) | (t & 0x041F);

#if defined(NES_TEST_PPU_CYCLE)
    if ( debugCycles ) {
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].events |= PpuDebugger::CopyX;
    }
#endif // NES_TEST_PPU_CYCLE
}

void Nes::Ppu::CopyVertV() {
    if ( !IsRendering() )
        return;

    // v: IHGF.ED CBA..... = t : IHGF.ED CBA.....
    v = (v & ~0x7BE0) | (t & 0x7BE0);
    //v = (v & 0x841F) | (t & 0x7BE0);

#if defined(NES_TEST_PPU_CYCLE)
    if ( debugCycles ) {
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].events |= PpuDebugger::CopyY;
    }
#endif // NES_TEST_PPU_CYCLE
}

void Nes::Ppu::Step() {

    
#if defined(NES_TEST_PPU_CYCLE)
    if ( debugCycles ) {
        
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].events = 0;
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].nameTableByte = nameTableByte;
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].nameTableAddress = nameTableAddress;
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].attributeTableByte = attributeTableByte;
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].attributeTableAddress = attributeTableAddress;
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].tileLowByte = tileLowByte;
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].tileHighByte = tileHighByte;
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].tileAddress = tileAddress;
        debugger.pixelData[pixelCycle + scanline * kMaxPixels].paletteAddress = paletteAddress;
    }
#endif // NES_TEST_PPU_CYCLE

    if ( scanline == 61 && pixelCycle == 70) {
        scanline = scanline;
    }

    if ( scanline == 241 && pixelCycle == 1 ) {
        verticalBlank = 1;
        currentAT = previousAT = 0;

        if ( control & ControlFlag_GenerateNMI )
            cpu->SetNMI();
    }
    else if ( scanline < 240 || scanline == 261 ) {

        switch ( pixelCycle ) {
            case 1: ClearSecondaryOAM(); if ( scanline == 261 ) { spriteOverflow = spriteHit = false; } break;
            case 257: EvaluateSprites(); break;
            case 321: LoadSprites(); break;
        }

        if ( (pixelCycle > 1) && (pixelCycle < 256) || (pixelCycle > 321 && pixelCycle < 338) ) {
            DrawPixel();
            switch ( pixelCycle % 8 ) {
                case 1: 
                    ReloadShiftRegisters();
                    previousAT = currentAT;
                    // Cache 2 bits of attribute table byte (already shifted after the read).
                    currentAT = attributeTableByte & 0x3;
                    break;
                case 2: ReadNameTable(); break;
                case 4: {

                    ReadAttributeTable();
                    // Attributes are 2 bit per quadrant.
                    // Shift vertically if coarse Y is 2 or more
                    if ( (v >> 5) & 2 )
                        attributeTableByte >>= 4;
                    // Shift horizontally if coarse X is 2 or more
                    if ( v & 2 )
                        attributeTableByte >>= 2;

                    break;
                }
                case 6: ReadTileLow(); break;
                case 0: ReadTileHigh(); IncrementHoriV(); break;
            }
        }

        switch ( pixelCycle ) {
            case 256: DrawPixel(); ReadTileHigh(); IncrementVertV(); break;
            case 257: DrawPixel(); ReloadShiftRegisters(); CopyHoriV(); break;
            
            case 1:
                if ( scanline == 261 )
                    verticalBlank = 0;
                    break;

            case 338:
                ReadNameTable();
                if ( scanline == 261 && IsRendering() && (frames & 0x01) ) {
                    ++pixelCycle;
                }
                break;
            case 340:
                ReadNameTable();
                break;
        }

        const bool copyVerticalVCycle = pixelCycle >= 280 && pixelCycle <= 304;
        if ( scanline == 261 && copyVerticalVCycle ) {
            // Reset vertical bits of V register.
            CopyVertV();
        }

        // PPU A12 pin signaling
        if ( IsRendering() ) {
            if ( pixelCycle == 260 ) {
                memoryController->PpuAddress12Rise();
            }
        }
    }

    IncrementPixelCycle();
}

void Nes::Ppu::IncrementAddressV() {

    v += (control & ControlFlag_VRAM_AddressIncrement) ? 32 : 1;
}

void Nes::Ppu::IncrementPixelCycle() {

    ++pixelCycle;

    if (pixelCycle >= kMaxPixels) {
        ++scanline;
        pixelCycle = 0;

#if defined(NES_TEST_PPU_CYCLE)
        // Change line
        //if ( debugCycles )
        //    PrintFormat( "\n" );
#endif // NES_TEST_PPU_CYCLE

        // handle region difference
        if (scanline >= kMaxScanlines) {

            scanline = 0;
            ++frames;

#if defined(NES_TEST_PPU_CYCLE)
            if ( debugFrame ) {
                debugFrame = false;
                debugCycles = true;
            }
            else {
                // Reset debug flag so it happens only for one frame
                debugCycles = false;
            }
#endif // NES_TEST_PPU_CYCLE
        }
    }
}


//////////////////////////////////////////////////////////////////////////
void Nes::Screen::Init() {
    paletteIndices = new uint8[kWidth * (kHeight + 16)];
    frameBuffer = new uint32[kWidth * (kHeight + 16)];

    memset( paletteIndices, 0, kWidth * (kHeight + 16) );
    memset( frameBuffer, 0, kWidth * (kHeight + 16) * 4 );
}

void Nes::Screen::Terminate() {
    delete[] paletteIndices;
    delete[] frameBuffer;
}

void Nes::Screen::WritePixel( uint16 scanline, uint16 pixel, uint32 color ) {
    frameBuffer[scanline * 256 + pixel] = color;
}

//////////////////////////////////////////////////////////////////////////
const uint8 apuLenghtCounterTable[] = { 10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
                                        12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30 };

// Polynomial approximation of channels outputs
float apuPulseTable[31];
float apuTNDTable[203];

static const uint32 CpuClockRate = 1789773;
static const uint32 SampleRate = 44100;

void Nes::Apu::Init( Cpu* cpu ) {
    this->cpu = cpu;

    volume = 0.1f;
    Mute( true );

#if defined (NES_EXTERNAL_APU)
    if ( !blipBuffer ) {
        blipBuffer = new Blip_Buffer();

        blipBuffer->clock_rate( CpuClockRate );
        blipBuffer->sample_rate( SampleRate );
    }

    if ( !externalApu ) {
        externalApu = new Nes_Apu();
        externalApu->volume( volume );
        externalApu->output( blipBuffer );
    }

    if ( externalApu ) {
        externalApu->reset();
        blip_eq_t trebleEq( -8.87, 8800, 44100 );

        externalApu->treble_eq( trebleEq );
    }
    blipBuffer->clear();
#endif
}

void Nes::Apu::Reset() {
#if defined (NES_EXTERNAL_APU)
    if ( externalApu ) {
        externalApu->reset();
        blipBuffer->clear();
    }
#endif
    for ( size_t i = 0; i < 31; ++i ) {
        apuPulseTable[i] = 95.52f / (8128.0f / float( i ) + 100);
    }

    for ( size_t i = 0; i < 203; ++i ) {
        apuTNDTable[i] = 163.67f / (24329.0f / float( i ) + 100);
    }

	frameCycle = 0;
}

void Nes::Apu::Tick() {
#if defined (NES_EXTERNAL_APU)
    if ( externalApu ) {
        externalApu->run_until( cpu->frameCycles - 1 );
    }
#else
	// f = set interrupt flag
	// l = clock length counters and sweep units
	// e = clock envelopes and triangle's linear counter
	// 
	// mode 0: 4 - step  effective rate(approx)
	// -------------------------------------- -
	// ---f      60 Hz
	// - l - l     120 Hz
	// e e e e     240 Hz
	// 
	// mode 1: 5 - step  effective rate(approx)
	// -------------------------------------- -
	// -----(interrupt flag never set)
	// l - l - -96 Hz
	// e e e e - 192 Hz

	// Step timers
    // The triangle channel's timer is clocked on every CPU cycle, but the pulse, noise, and DMC timers
    // are clocked only on every second CPU cycle and thus produce only even periods.
    if ( (frameCycle % 2) == 0 ) {
        pulse1.TickTimer();
        pulse2.TickTimer();
    }
	
	// Update components
	switch (frameCounter.mode) {
		case 0: {
			// NTSC
			// Timings coming from Blargg tests
			switch (frameCycle) {
				case 7459: {
					// Step 1
					// 7459  Clock linear
                    pulse1.TickEnvelope();
                    pulse2.TickEnvelope();

					break;
				}

				case 14915: {
					// Step 2
					// 14915 Clock linear & length
                    pulse1.TickEnvelope();
                    pulse2.TickEnvelope();

                    pulse1.TickLengthCounter();
                    pulse2.TickLengthCounter();

					break;
				}

				case 22373: {
					// Step 3
					// 22373 Clock linear
                    pulse1.TickEnvelope();
                    pulse2.TickEnvelope();
					
					break;
				}

				case 29830: {
					// 29830 Set frame irq
                    if ( !frameCounter.inhibitIRQ )
                        cpu->SetIRQ( 1 );
					break;
				}

				case 29831: {
					// Step 4
					// 29831 Clock linear & length and set frame irq
                    pulse1.TickEnvelope();
                    pulse2.TickEnvelope();

                    pulse1.TickLengthCounter();
                    pulse2.TickLengthCounter();

                    if ( !frameCounter.inhibitIRQ )
                        cpu->SetIRQ( 1 );

					break;
				}

				case 29832: {
					// 29832 Set frame irq
                    if ( !frameCounter.inhibitIRQ )
                        cpu->SetIRQ( 1 );

					break;
				}
			}

			break;
		}

		case 1: {
			// PAL
            // Timings coming from Blargg tests
            switch ( frameCycle ) {
                case 7459: {
					// Step 1
					// 7459  Clock linear
                    pulse1.TickEnvelope();
                    pulse2.TickEnvelope();

					break;
				}

				case 14915: {
					// Step 2
					// 14915 Clock linear & length
                    pulse1.TickEnvelope();
                    pulse2.TickEnvelope();

                    pulse1.TickLengthCounter();
                    pulse2.TickLengthCounter();

					break;
				}

				case 22373: {
					// Step 3
					// 22373 Clock linear
                    pulse1.TickEnvelope();
                    pulse2.TickEnvelope();
					
					break;
				}

				case 29829: {
					// Empty
					break;
				}

				case 37282: {
					// Step 4
					// 29831 Clock linear & length and set frame irq
                    pulse1.TickEnvelope();
                    pulse2.TickEnvelope();

                    pulse1.TickLengthCounter();
                    pulse2.TickLengthCounter();


					break;
				}
            }
			break;
		}
	}

    ++frameCycle;

	// 40 = 1789773 / 44100
    if (frameCycle % (CpuClockRate / SampleRate) == 0)  {

		static float previousSample = 0;

		float soundValue = OutputSound();

		float delta = soundValue - previousSample;
		int32 deltaInt = (int32)(delta * 32768.0f);

		deltaInt = deltaInt > 32768 ? 32768 : deltaInt;
		deltaInt = deltaInt < -32768 ? -32768 : deltaInt;

		previousSample = soundValue;
    }

	switch (frameCounter.mode) {
		case 0: {
			// Step 1
			// 37289 Clock linear
			if (frameCycle >= 37289) {
                //blipBuffer->end_frame(37289);
				//blip_end_frame(blipBuffer, 37289);
				frameCycle = 7459;
			}

			break;
		}

        case 1: {
			if (frameCycle >= 37289) {
                
                //blipBuffer->end_frame(37289);
				//blip_end_frame(blipBuffer, 37289);
				frameCycle = 7459;
			}

            break;
        }
	}
#endif // NES_EXTERNAL_APU
}

void Nes::Apu::EndFrame(uint32 count) {
#if defined(NES_EXTERNAL_APU)
    if (externalApu) {
        externalApu->end_frame( count );

        blipBuffer->end_frame( count );
    }
#else
    {
        while (count--) {
            Tick();
        }
    }
#endif
}

void Nes::Apu::WriteControl( uint8 data ) {
#if defined(NES_EXTERNAL_APU)
    if (externalApu) {
        externalApu->write_register( cpu->frameCycles, 0x4015, data);
        return;
    }
#else
    enabledChannels.data = data;

	if (!enabledChannels.pulse1) {
		pulse1.lengthCounter = 0;
	}

	if (!enabledChannels.pulse2) {
		pulse2.lengthCounter = 0;
	}
#endif
}

void Nes::Apu::WriteFrameCounter( uint8 data ) {

#if defined(NES_EXTERNAL_APU)
    if (externalApu) {
        externalApu->write_register( cpu->frameCycles, 0x4017, data);
        return;
    }
#else
    frameCounter.data = data;
	
	if (frameCounter.mode == 1) {
		// Clock immediately
		pulse1.TickEnvelope();
		pulse1.TickLengthCounter();
		pulse1.TickSweep();

		pulse2.TickEnvelope();
		pulse2.TickLengthCounter();
		pulse2.TickSweep();
	}
#endif
}

uint8 Nes::Apu::ReadStatus() {
#if defined(NES_EXTERNAL_APU)
    if ( externalApu ) {
        return externalApu->read_status( cpu->frameCycles );
    }
#endif
    uint8 status = (pulse1.lengthCounter > 0 ? 1 : 0) | (pulse1.lengthCounter > 0 ? 2 : 0);

    return status;
}

uint8 Nes::Apu::CpuRead( uint16 address ) {
    
#if defined(NES_EXTERNAL_APU)
    Assert( false );
#endif

    return 0;
}

void Nes::Apu::CpuWrite( uint16 address, uint8 data ) {

#if defined(NES_EXTERNAL_APU)
    if (externalApu) {
        externalApu->write_register( cpu->frameCycles, address, data);
        return;
    }

#else
    switch ( address ) {
        case 0x4000:
            pulse1.WriteControl( data );
            break;

        case 0x4001:
            pulse1.WriteSweep( data );
            break;

        case 0x4002:
            // Low 8 bit of timerPeriod
            pulse1.timerPeriod = (0xF0 & pulse1.timerPeriod) | data;
            break;

        case 0x4003: {
            // Hight 3 bit of timerPeriod
            pulse1.timerPeriod = (pulse1.timerPeriod & 0x0F) | ((data & 0x7) << 8);
            // 5 bit length counter load
            if ( enabledChannels.pulse1  ) {
                uint8 lengthCounterIndex = (data >> 3) & 0x1F;
                pulse1.lengthCounter = apuLenghtCounterTable[lengthCounterIndex];
            }
            pulse1.envelopeStart = true;
            pulse1.dutyCycle = 0;
            break;
        }
        case 0x4004:
            pulse2.WriteControl( data );
            break;

        case 0x4005:
            pulse2.WriteSweep( data );
            break;

        case 0x4006:
            // Low 8 bit of timerPeriod
            pulse2.timerPeriod = (0xF0 & pulse2.timerPeriod) | data;
            break;

        case 0x4007: {
            // Hight 3 bit of timerPeriod
            pulse2.timerPeriod = (pulse2.timerPeriod & 0x0F) | ((data & 0x7) << 8);
            // 5 bit length counter load
            if ( enabledChannels.pulse2 ) {
                uint8 lengthCounterIndex = (data >> 3) & 0x1F;
                pulse2.lengthCounter = apuLenghtCounterTable[lengthCounterIndex];
            }
            pulse2.envelopeStart = true;
            pulse2.dutyCycle = 0;
            break;
        }
        default:
            break;
    }

#endif
}
 static float mix( int pulse1, int pulse2, int triangle, int noise, int dmc) {

	return (0.9588f * (pulse1 + pulse2)) / (pulse1 + pulse2 + 81.28f)
		- 361.733f / (2.75167f * triangle + 1.84936f * noise + dmc + 226.38f)
		+ 1.5979f;
}

float Nes::Apu::OutputSound() {

    float pulse = apuPulseTable[pulse1.Output() + pulse2.Output()];
	float tnd = apuTNDTable[0];
    // pulse = pulseTable[pulse1 + pulse2]
    // tnd = tndTable[3* triangle + 2 * noise + dmc]
    // output = pulse + tnd
    return pulse + tnd;
	//return mix(pulse1.Output(), pulse2.Output(), 0, 0, 0);
}

void Nes::Apu::Mute( bool value ) {
    mute = value;

#if defined(NES_EXTERNAL_APU)
    if ( externalApu ) {
        externalApu->volume( mute ? 0 : volume );
    }
#endif
}

void Nes::Apu::SetVolume( float value ) {

    volume = value;

#if defined(NES_EXTERNAL_APU)
    if ( externalApu ) {
        externalApu->volume( volume );
    }
#endif
}


//////////////////////////////////////////////////////////////////////////
static const uint8 kDutyPeriod = 8;
static const uint8 kEnvelopeStartVolume = 15;
static const uint8 kDutyTable[] = { 0, 1, 0, 0, 0, 0, 0, 0,
                                    0, 1, 1, 0, 0, 0, 0, 0,
                                    0, 1, 1, 1, 1, 0, 0, 0,
                                    1, 0, 0, 1, 1, 1, 1, 1 };


static const uint8 kDutyTable2[4][8] = { {0, 1, 0, 0, 0, 0, 0, 0},
									    {0, 1, 1, 0, 0, 0, 0, 0},
									    {0, 1, 1, 1, 1, 0, 0, 0},
									    {1, 0, 0, 1, 1, 1, 1, 1} };

void Nes::Apu::Pulse::WriteControl( uint8 data ) {
    control.data = data;
    envelopeStart = true;
    envelopeCounter = control.volumeOrEnvelopeDecay;
}

void Nes::Apu::Pulse::WriteSweep( uint8 data ) {
    sweep.data = data;
    sweepStart = true;
}

void Nes::Apu::Pulse::TickTimer() {
    if ( timer == 0 ) {
        timer = timerPeriod;
        dutyCycle = (dutyCycle + 1) % kDutyPeriod;
    }
    else {
        --timer;
    }
}

void Nes::Apu::Pulse::TickLengthCounter() {
    if ( !control.envelopeLoopOrLengthCounter && lengthCounter ) {
        --lengthCounter;
    }
}

void Nes::Apu::Pulse::TickEnvelope() {
    if ( envelopeStart ) {
        envelopeStart = false;
        envelopeCounter = control.volumeOrEnvelopeDecay;
        volume = kEnvelopeStartVolume;
    }
    else if ( envelopeCounter > 0 ) {
        --envelopeCounter;
    }
    else {
        if ( volume > 0 ) {
            --volume;
        }
        else if ( control.envelopeLoopOrLengthCounter ) {
            volume = kEnvelopeStartVolume;
        }

        envelopeCounter = control.volumeOrEnvelopeDecay;
    }
}

void Nes::Apu::Pulse::TickSweep() {
    if ( sweepStart ) {
        sweepStart = false;
        sweepCycle = sweep.dividerPeriod;
    }
    else if ( sweepCycle ) {
        --sweepCycle;
    }
    else {
        if ( sweep.enabled ) {
            uint16 deltaTimer = timer >> sweep.shiftCount;
            if ( sweep.negateFlag ) {
                timer -= deltaTimer;
            }
            else {
                timer += deltaTimer;
            }
        }
        sweepCycle = sweep.dividerPeriod;
    }
}

uint8 Nes::Apu::Pulse::Output() {
    if ( lengthCounter == 0 )
        return 0;

    //if ( kDutyTable[control.dutyCycles * kDutyPeriod + dutyCycle] == 0 )
	if (kDutyTable2[control.dutyCycles][dutyCycle] == 0)
        return 0;

    if ( timer < 8 || timer > 0x7ff )
        return 0;

    if ( control.envelopeOrVolume )
        return volume;
    else
        return control.volumeOrEnvelopeDecay;

    return 0;
}

//////////////////////////////////////////////////////////////////////////
void Nes::Controller::Reset() {
    registers[0] = registers[1] = readBitIndex = 0;
    strobeToggle = 0;

    NewFrame();
}

void Nes::Controller::NewFrame() {
    state[0] = state[1] = readBitIndex = 0;
}

void Nes::Controller::SetButton( uint8 controller, Buttons button ) {
    if ( controller > 1 )
        return;

    state[controller] |= ( 1 << ( uint8 )button );
}

uint8 Nes::Controller::ReadState() {

    // When strobe is high, it keeps reading A:
    if ( strobeToggle ) {
        readBitIndex = 0;
        portdata = 0xffffff00 | state[0];
        return ((uint8)portdata & 1);
    }

    return (((portdata >> readBitIndex++) & 1) | 0x40);
}

void Nes::Controller::WriteState( uint8 strobe ) {

    strobe &= 1;
    // Read the input only when keys are transitioning from pressed to non pressed
    if ( strobeToggle || strobe ) {
        strobeToggle = strobe;
        readBitIndex = 0;
        portdata = 0xffffff00 | state[0];
        // Get state
        registers[0] = state[0];
        // Second controller is doing nothing.
        registers[1] = state[1];
    }

    
}

//////////////////////////////////////////////////////////////////////////


int dmc_read(void* data, cpu_addr_t addr) {
    Nes* nes_dmc = (Nes*)data;
    return nes_dmc->memoryController.CpuRead(addr);
}

void setIrq(void* data) {
    Nes* nes_dmc = (Nes*)data;
    nes_dmc->cpu.SetIRQ(1);
}

void Nes::Init() {

    cpu.Init( &ppu, &apu, &memoryController );
    ppu.Init( &cpu, &screen, &memoryController );
    apu.Init( &cpu );
    cart.Init();
    memoryController.Init( &cart, &cpu, &ppu, &controllers, &apu );
    screen.Init();

#if defined(NES_EXTERNAL_APU)
    if ( apu.externalApu ) {
        apu.externalApu->irq_notifier( setIrq, this );
        apu.externalApu->dmc_reader( dmc_read, this );
    }
#endif
}

void Nes::Reset() {
    cpu.Reset();
    ppu.Reset();
    apu.Reset();
    controllers.Reset();
}

void Nes::LoadRom( cstring path ) {
    
    if ( cart.LoadRom( path ) ) {
        memoryController.LoadMapper();
        Reset();
    }
}

void Nes::SaveSRAM() {
    if ( memoryController.mapper->prgRamWrite ) {

        FileHandle sramFile;
        String sramFilename( cart.filename );
        sramFilename.append( ".sram" );
        OpenFile( sramFilename.c_str(), "wb", &sramFile );
        if ( sramFile ) {
            
            fwrite( cart.prgRam.begin(), 1, cart.prgRam.size(), sramFile );

            memoryController.mapper->prgRamWrite = false;
        }
        else {
            PrintFormat( "Error saving to file %s\n", sramFilename.c_str() );
        }

        CloseFile( sramFile );
    }
}


//////////////////////////////////////////////////////////////////////////
#if defined(NES_SHOW_ASM)

enum addrmodes {
    er = 0, no, ab, ax, ay, ac, im, ix, iy, in, re, zp, zx, zy
};

#ifdef CPU_UNDOC
static char opcodes[256][4] = {
    /*  x0    x1    x2    x3    x4    x5    x6    x7    x8    x9    xA    xB    xC    xD    xE    xF           */
    "BRK","ORA","???","SLO","NOP","ORA","ASL","SLO","PHP","ORA","ASL","AAC","NOP","ORA","ASL","SLO", /*00-0F*/
    "BPL","ORA","???","SLO","NOP","ORA","ASL","SLO","CLC","ORA","NOP","SLO","NOP","ORA","ASL","SLO", /*10-1F*/
    "JSR","AND","???","RLA","BIT","AND","ROL","RLA","PLP","AND","ROL","AAC","BIT","AND","ROL","RLA", /*20-2F*/
    "BMI","AND","???","RLA","NOP","AND","ROL","RLA","SEC","AND","NOP","RLA","NOP","AND","ROL","RLA", /*30-3F*/
    "RTI","EOR","???","SRE","NOP","EOR","LSR","SRE","PHA","EOR","LSR","ASR","JMP","EOR","LSR","SRE", /*40-4F*/
    "BVC","EOR","???","SRE","NOP","EOR","LSR","SRE","CLI","EOR","NOP","SRE","NOP","EOR","LSR","SRE", /*50-5F*/
    "RTS","ADC","???","RRA","NOP","ADC","ROR","RRA","PLA","ADC","ROR","???","JMP","ADC","ROR","RRA", /*60-6F*/
    "BVS","ADC","???","RRA","NOP","ADC","ROR","RRA","SEI","ADC","NOP","RRA","NOP","ADC","ROR","RRA", /*70-7F*/
    "NOP","STA","NOP","SAX","STY","STA","STX","SAX","DEY","NOP","TXA","???","STY","STA","STX","SAX", /*80-8F*/
    "BCC","STA","???","???","STY","STA","STX","SAX","TYA","STA","TXS","XAS","SYA","STA","SXA","???", /*90-9F*/
    "LDY","LDA","LDX","LAX","LDY","LDA","LDX","LAX","TAY","LDA","TAX","ATX","LDY","LDA","LDX","LAX", /*A0-AF*/
    "BCS","LDA","???","LAX","LDY","LDA","LDX","LAX","CLV","LDA","TSX","???","LDY","LDA","LDX","LAX", /*B0-BF*/
    "CPY","CMP","NOP","DCP","CPY","CMP","DEC","DCP","INY","CMP","DEX","AXS","CPY","CMP","DEC","DCP", /*C0-CF*/
    "BNE","CMP","???","DCP","NOP","CMP","DEC","DCP","CLD","CMP","NOP","DCP","NOP","CMP","DEC","DCP", /*D0-DF*/
    "CPX","SBC","NOP","ISB","CPX","SBC","INC","ISB","INX","SBC","NOP","SBC","CPX","SBC","INC","ISB", /*E0-EF*/
    "BEQ","SBC","???","ISB","NOP","SBC","INC","ISB","SED","SBC","NOP","ISB","NOP","SBC","INC","ISB"  /*F0-FF*/
};
static uint8 addrtable[256] = {
    /* x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF         */
    im,ix,er,ix,zp,zp,zp,zp,no,im,ac,im,ab,ab,ab,ab, /*00-0f*/
    re,iy,er,iy,zx,zx,zx,zx,no,ay,im,ay,ax,ax,ax,ax, /*10-1f*/
    ab,ix,er,ix,zp,zp,zp,zp,no,im,ac,im,ab,ab,ab,ab, /*20-2f*/
    re,iy,er,iy,zx,zx,zx,zx,no,ay,im,ay,ax,ax,ax,ax, /*30-3f*/
    no,ix,er,ix,zp,zp,zp,zp,no,im,ac,im,ab,ab,ab,ab, /*40-4f*/
    re,iy,er,iy,zx,zx,zx,zx,no,ay,im,ay,ax,ax,ax,ax, /*50-5f*/
    no,ix,er,ix,zp,zp,zp,zp,no,im,ac,er,in,ab,ab,ab, /*60-6f*/
    re,iy,er,iy,zx,zx,zx,zx,no,ay,im,ay,ax,ax,ax,ax, /*70-7f*/
    im,ix,im,ix,zp,zp,zp,zp,no,im,no,er,ab,ab,ab,ab, /*80-8f*/
    re,iy,er,er,zx,zx,zy,zy,no,ay,no,ay,ax,ax,ay,er, /*90-9f*/
    im,ix,im,ix,zp,zp,zp,zp,no,im,no,im,ab,ab,ab,ab, /*a0-af*/
    re,iy,er,iy,zx,zx,zy,zy,no,ay,no,er,ax,ax,ay,ay, /*b0-bf*/
    im,ix,im,ix,zp,zp,zp,zp,no,im,no,im,ab,ab,ab,ab, /*c0-cf*/
    re,iy,iy,er,zx,zx,zx,zx,no,ay,im,ay,ax,ax,ax,ax, /*d0-df*/
    im,ix,im,ix,zp,zp,zp,zp,no,im,no,im,ab,ab,ab,ab, /*e0-ef*/
    re,iy,er,iy,zx,zx,zx,zx,no,ay,im,ay,ax,ax,ax,ax  /*f0-ff*/
};
#else
static char opcodes[256][4] = {
    "BRK","ORA","???","???","???","ORA","ASL","???","PHP","ORA","ASL","???","???","ORA","ASL","???", /*00-0F*/
    "BPL","ORA","???","???","???","ORA","ASL","???","CLC","ORA","???","???","???","ORA","ASL","???", /*10-1F*/
    "JSR","AND","???","???","BIT","AND","ROL","???","PLP","AND","ROL","???","BIT","AND","ROL","???", /*20-2F*/
    "BMI","AND","???","???","???","AND","ROL","???","SEC","AND","???","???","???","AND","ROL","???", /*30-3F*/
    "RTI","EOR","???","???","???","EOR","LSR","???","PHA","EOR","LSR","???","JMP","EOR","LSR","???", /*40-4F*/
    "BVC","EOR","???","???","???","EOR","LSR","???","CLI","EOR","???","???","???","EOR","LSR","???", /*50-5F*/
    "RTS","ADC","???","???","???","ADC","ROR","???","PLA","ADC","ROR","???","JMP","ADC","ROR","???", /*60-6F*/
    "BVS","ADC","???","???","???","ADC","ROR","???","SEI","ADC","???","???","???","ADC","ROR","???", /*70-7F*/
    "???","STA","???","???","STY","STA","STX","???","DEY","???","TXA","???","STY","STA","STX","???", /*80-8F*/
    "BCC","STA","???","???","STY","STA","STX","???","TYA","STA","TXS","???","???","STA","???","???", /*90-9F*/
    "LDY","LDA","LDX","???","LDY","LDA","LDX","???","TAY","LDA","TAX","???","LDY","LDA","LDX","???", /*A0-AF*/
    "BCS","LDA","???","???","LDY","LDA","LDX","???","CLV","LDA","TSX","???","LDY","LDA","LDX","???", /*B0-BF*/
    "CPY","CMP","???","???","CPY","CMP","DEC","???","INY","CMP","DEX","???","CPY","CMP","DEC","???", /*C0-CF*/
    "BNE","CMP","???","???","???","CMP","DEC","???","CLD","CMP","???","???","???","CMP","DEC","???", /*D0-DF*/
    "CPX","SBC","???","???","CPX","SBC","INC","???","INX","SBC","NOP","???","CPX","SBC","INC","???", /*E0-EF*/
    "BEQ","SBC","???","???","???","SBC","INC","???","SED","SBC","???","???","???","SBC","INC","???"  /*F0-FF*/
};
static uint8 addrtable[256] = {
    /* x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF         */
    im,ix,er,er,er,zp,zp,er,no,im,ac,er,er,ab,ab,er, /*00-0f*/
    re,iy,er,er,er,zx,zx,er,no,ay,er,er,er,ax,ax,er, /*10-1f*/
    ab,ix,er,er,zp,zp,zp,er,no,im,ac,er,ab,ab,ab,er, /*20-2f*/
    re,iy,er,er,er,zx,zx,er,no,ay,er,er,er,ax,ax,er, /*30-3f*/
    no,ix,er,er,er,zp,zp,er,no,im,ac,er,ab,ab,ab,er, /*40-4f*/
    re,iy,er,er,er,zx,zx,er,no,ay,er,er,er,ax,ax,er, /*50-5f*/
    no,ix,er,er,er,zp,zp,er,no,im,ac,er,in,ab,ab,er, /*60-6f*/
    re,iy,er,er,er,zx,zx,er,no,ay,er,er,er,ax,ax,er, /*70-7f*/
    er,ix,er,er,zp,zp,zp,er,no,er,no,er,ab,ab,ab,er, /*80-8f*/
    re,iy,er,er,zx,zx,zy,er,no,ay,no,er,er,ax,er,er, /*90-9f*/
    im,ix,im,er,zp,zp,zp,er,no,im,no,er,ab,ab,ab,er, /*a0-af*/
    re,iy,er,er,zx,zx,zy,er,no,ay,no,er,ax,ax,ay,er, /*b0-bf*/
    im,ix,er,er,zp,zp,zp,er,no,im,no,er,ab,ab,ab,er, /*c0-cf*/
    re,iy,er,er,er,zx,zx,er,no,ay,er,er,er,ax,ax,er, /*d0-df*/
    im,ix,er,er,zp,zp,zp,er,no,im,no,er,ab,ab,ab,er, /*e0-ef*/
    re,iy,er,er,zx,zx,zx,er,no,ay,er,er,ax,ax,ax,er  /*f0-ff*/
};
#endif

static uint8 oplength[256];

uint16 Nes::Cpu::CpuDisassemble( char *buffer, uint16 opcodepos ) {
    uint8 opcode, size;
    uint16 addr;

    strcpy( buffer, "" );
    opcode = memoryController->CpuRead( opcodepos );
    switch ( addrtable[opcode] ) {
        case er:size = 1; sprintf( buffer, "%02X       .db $%02x", opcode, opcode ); break;
        case no:size = 1; sprintf( buffer, "%02X       %s", opcode, opcodes[opcode] ); break;
        case ac:size = 1; sprintf( buffer, "%02X       %s a", opcode, opcodes[opcode] ); break;
        case ab:
            size = 3;
            addr = memoryController->CpuRead( opcodepos + 1 ) | ( memoryController->CpuRead( opcodepos + 2 ) << 8 );
            sprintf( buffer, "%02X %02X %02X %s $%04X", opcode, addr & 0xFF, ( addr >> 8 ) & 0xFF, opcodes[opcode], addr );
            break;
        case ax:
            size = 3;
            addr = memoryController->CpuRead( opcodepos + 1 ) | ( memoryController->CpuRead( opcodepos + 2 ) << 8 );
            sprintf( buffer, "%02X %02X %02X %s $%04X,x", opcode, addr & 0xFF, ( addr >> 8 ) & 0xFF, opcodes[opcode], addr );
            break;
        case ay:
            size = 3;
            addr = memoryController->CpuRead( opcodepos + 1 ) | ( memoryController->CpuRead( opcodepos + 2 ) << 8 );
            sprintf( buffer, "%02X %02X %02X %s $%04X,y", opcode, addr & 0xFF, ( addr >> 8 ) & 0xFF, opcodes[opcode], addr );
            break;
        case in:size = 3; sprintf( buffer, "%02X %02X %02X %s ($%04X)", opcode, memoryController->CpuRead( opcodepos + 1 ), memoryController->CpuRead( opcodepos + 2 ), opcodes[opcode], memoryController->CpuRead( opcodepos + 1 ) | ( memoryController->CpuRead( opcodepos + 2 ) << 8 ) ); break;
        case im:size = 2; sprintf( buffer, "%02X %02X    %s #$%02X", opcode, memoryController->CpuRead( opcodepos + 1 ), opcodes[opcode], memoryController->CpuRead( opcodepos + 1 ) ); break;
        case ix:size = 2; sprintf( buffer, "%02X %02X    %s ($%02X,x)", opcode, memoryController->CpuRead( opcodepos + 1 ), opcodes[opcode], memoryController->CpuRead( opcodepos + 1 ) ); break;
        case iy:size = 2; sprintf( buffer, "%02X %02X    %s ($%02X),y", opcode, memoryController->CpuRead( opcodepos + 1 ), opcodes[opcode], memoryController->CpuRead( opcodepos + 1 ) ); break;
        case re:size = 2; sprintf( buffer, "%02X %02X    %s $%04X", opcode, memoryController->CpuRead( opcodepos + 1 ), opcodes[opcode], opcodepos + size + ( (int8)memoryController->CpuRead( opcodepos + 1 ) ) ); break;
        case zp:size = 2; sprintf( buffer, "%02X %02X    %s $%02X", opcode, memoryController->CpuRead( opcodepos + 1 ), opcodes[opcode], memoryController->CpuRead( opcodepos + 1 ) ); break;
        case zx:size = 2; sprintf( buffer, "%02X %02X    %s $%02X,x", opcode, memoryController->CpuRead( opcodepos + 1 ), opcodes[opcode], memoryController->CpuRead( opcodepos + 1 ) ); break;
        case zy:size = 2; sprintf( buffer, "%02X %02X    %s $%02X,y", opcode, memoryController->CpuRead( opcodepos + 1 ), opcodes[opcode], memoryController->CpuRead( opcodepos + 1 ) ); break;
        default:size = 1; sprintf( buffer, "disassembler bug" ); break;
    }

    static char buf[512], buf2[512];
    sprintf( buf, "%04X %s", opcodepos, buffer );
    int l = (int)strlen( buf );
    for ( int i = 0; i < 48 - l; ++i )
        strcat( buf, " " );

    // A:00 X:00 Y:00 P:24 SP:FD CYC:  0 SL:241
    sprintf( buf2, "A:%02X X:%02X Y:%02X P:%02X SP:%02X PPU:%03u CpuCycles:%llu", A, X, Y, P, S, ppu->pixelCycle, cycles );
    strcat( buf, buf2 );

    if ( testAsm )
        PrintFormat( "%s\n", buf );

    strcpy( asmBuffer.begin(), buf );

    return( opcodepos + size );
}

void Nes::Cpu::CpuDisassembleInit() {

    asmBuffer[0] = 0;

    int i;

    for ( i = 0; i < 256; i++ ) {
        switch ( addrtable[i] ) {
            case er:
            case no:
            case ac:
                oplength[i] = 1;
                break;
            case im:
            case ix:
            case iy:
            case re:
            case zp:
            case zx:
            case zy:
                oplength[i] = 2;
                break;
            case ab:
            case ax:
            case ay:
            case in:
                oplength[i] = 3;
                break;
        }
    }
}
#endif // (NES_SHOW_ASM)


} // namespace hydra
