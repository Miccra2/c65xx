#ifndef __E65_H__
#define __E65_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

enum {
    // load Accumulator
    IMM_LDA = 0xA9,     // takes 2 cycles
    ZPG_LDA = 0xA5,     // takes 3 cycles
    ZPX_LDA = 0xB5,     // takes 4 cycles
    ABS_LDA = 0xAD,     // takes 4 cycles
    ABX_LDA = 0xBD,     // takes 4 or 5 cycles
    ABY_LDA = 0xB9,     // takes 4 or 5 cycles
    INX_LDA = 0xA1,     // takes 6 cycles
    INY_LDA = 0xB1,     // takes 5 or 6 cycles
};

// CPU flags
#define FC 0x01     // Carry
#define FZ 0x02     // Zero
#define FI 0x04     // Interrupt Disable
#define FD 0x08     // Decimal Mode Enable
#define FB 0x10     // Break
#define FU 0x20     // Unused
#define FV 0x40     // Overflow
#define FN 0x80     // Negative

// 6501/6502/6510 CPU registers for c64 emulation
typedef struct {
    uint8_t  ac;        // Accumulator
    uint8_t  x;         // X index
    uint8_t  y;         // Y index
    uint16_t pc;        // Program Counter
    uint8_t  sp;        // Stack Pointer
    uint8_t  p;         // Processor Status
} CPU_65XX;

#ifndef C64_EMULATION
#define MEMSZ 0x10000   // 64 KiB Memory
#else
#define MEMSZ 0x100000  // 1 MiB Memory for memory banking in c64 emulation
#endif // C64_EMULATION

#define CYCLES(x, y) ((addr>>8) == (cpu->pc>>8) ? x : y)

// memory operations for handeling memory banking of c64 emulation
uint8_t mem_read_byte(uint8_t *mem, uint16_t addr);
uint16_t mem_read_word(uint8_t *mem, uint16_t addr);
void mem_write_byte(uint8_t *mem, uint16_t addr, uint8_t data);
void mem_write_word(uint8_t *mem, uint16_t addr, uint16_t data);

// 65XX CPU addressing
uint16_t addr_imm(CPU_65XX *cpu);
uint16_t addr_zpg(CPU_65XX *cpu, uint8_t *mem);
uint16_t addr_zpx(CPU_65XX *cpu, uint8_t *mem);
uint16_t addr_zpy(CPU_65XX *cpu, uint8_t *mem);
uint16_t addr_abs(CPU_65XX *cpu, uint8_t *mem);
uint16_t addr_abx(CPU_65XX *cpu, uint8_t *mem);
uint16_t addr_aby(CPU_65XX *cpu, uint8_t *mem);
uint16_t addr_inx(CPU_65XX *cpu, uint8_t *mem);
uint16_t addr_iny(CPU_65XX *cpu, uint8_t *mem);

// 65XX CPU operations
uint8_t fetch_byte(CPU_65XX *cpu, uint8_t *mem, uint16_t addr);
uint16_t fetch_word(CPU_65XX *cpu, uint8_t *mem, uint16_t addr);
void set(CPU_65XX *cpu, size_t reg, uint8_t flags);
void reset(CPU_65XX *cpu, uint8_t *mem, uint16_t reset_vec);
ssize_t execute(CPU_65XX *cpu, uint8_t *mem, uint8_t ins);
void run(CPU_65XX *cpu, uint8_t *mem, ssize_t *cycles);
void run_endless(CPU_65XX *cpu, uint8_t *mem);

#endif // __E65_H__
