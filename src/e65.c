#include "e65.h"

#ifdef C64_EMULATION

uint8_t mem_read_byte(uint8_t *mem, uint16_t addr) {
    return mem[(size_t)(addr) + (size_t)(mem[0])];
}

uint16_t mem_read_word(uint8_t *mem, uint16_t addr) {
    return mem[(size_t)(addr) + (size_t)(mem[0])] | mem[(size_t)(++addr) + (size_t)(mem[0])];
}

void mem_write_byte(uint8_t *mem, uint16_t addr, uint8_t data) {
    mem[(size_t)(addr) + (size_t)(mem[0])] = data;
}

void mem_write_word(uint8_t *mem, uint16_t addr, uint16_t data) {
    mem[(size_t)(addr)   + (size_t)(mem[0])] = (uint8_t)(data);
    mem[(size_t)(++addr) + (size_t)(mem[0])] = (uint8_t)(data >> 8);
}

#else // C64_EMULATION

uint8_t mem_read_byte(uint8_t *mem, uint16_t addr) {
    return mem[addr];
}

uint16_t mem_read_word(uint8_t *mem, uint16_t addr) {
    return mem[addr] | (mem[addr+1] << 8);
}

void mem_write_byte(uint8_t *mem, uint16_t addr, uint8_t data) {
    mem[addr] = data;
}

void mem_write_word(uint8_t *mem, uint16_t addr, uint16_t data) {
    mem[addr]   = (uint8_t)(data);
    mem[++addr] = (uint8_t)(data >> 8);
}

#endif // C64_EMULATION

uint16_t addr_imm(CPU_65XX *cpu) {
    return cpu->pc++;
}

uint16_t addr_zpg(CPU_65XX *cpu, uint8_t *mem) {
    return (uint16_t)(fetch_byte(cpu, mem));
}

uint16_t addr_zpx(CPU_65XX *cpu, uint8_t *mem) {
    return (uint16_t)((uint8_t)(addr_zpg(cpu, mem)) + cpu->x);
}

uint16_t addr_zpy(CPU_65XX *cpu, uint8_t *mem) {
    return (uint16_t)((uint8_t)(addr_zpg(cpu, mem)) + cpu->y);
}

uint16_t addr_abs(CPU_65XX *cpu, uint8_t *mem) {
    return fetch_word(cpu, mem);
}

uint16_t addr_abx(CPU_65XX *cpu, uint8_t *mem) {
    return addr_abs(cpu, mem) + (uint16_t)(cpu->x);
}

uint16_t addr_aby(CPU_65XX *cpu, uint8_t *mem) {
    return addr_abs(cpu, mem) + (uint16_t)(cpu->y);
}

uint16_t addr_inx(CPU_65XX *cpu, uint8_t *mem) {
    return mem_read_word(mem, addr_zpx(cpu, mem));
}

uint16_t addr_iny(CPU_65XX *cpu, uint8_t *mem) {
    return mem_read_word(mem, addr_zpg(cpu, mem) + (uint16_t)(cpu->y));
}

uint8_t fetch_byte(CPU_65XX *cpu, uint8_t *mem) {
    return mem_read_byte(mem, cpu->pc++);
}

uint16_t fetch_word(CPU_65XX *cpu, uint8_t *mem) {
    uint16_t addr = mem_read_word(mem, cpu->pc);
    cpu->pc += 2;
    return addr;
}

// set cpu flags
void set(CPU_65XX *cpu, size_t reg, uint8_t flags) {
    if ((flags&FZ) && !reg) {
        cpu->p |= FZ;
    }

    if ((flags&FN) && (reg&0x80)) {
        cpu->p |= FN;
    }
}

void reset(CPU_65XX *cpu, uint8_t *mem, uint16_t reset_vec) {
    cpu->ac = 0;
    cpu->x  = 0;
    cpu->y  = 0;
    cpu->pc = reset_vec;
    cpu->sp = 0xFF;
    cpu->p  = 0;

    for (size_t i = 0; i < MEMSZ; i++) {
        mem[i] = 0;
    }
}

/**
 * Execute instruction ins if instruction is illegal return only one cycle
 */
ssize_t execute(CPU_65XX *cpu, uint8_t *mem, uint8_t ins) {
    switch (ins) {
    case IMM_LDA: {
        cpu->ac = fetch_byte(cpu, mem);
        set(cpu, (size_t)(cpu->ac), FN&FZ);     // set negative and zero flags
        return 2;
    } break;
    case ZPG_LDA: {
        cpu->ac = mem_read_byte(mem, addr_zpg(cpu, mem));
        set(cpu, (size_t)(cpu->ac), FN&FZ);     // set negative and zero flags
        return 3;
    } break;
    case ZPX_LDA: {
        cpu->ac = mem_read_byte(mem, addr_zpx(cpu, mem));
        set(cpu, (size_t)(cpu->ac), FN&FZ);     // set negative and zero flags
        return 4;
    } break;
    case ABS_LDA: {
        cpu->ac = mem_read_byte(mem, addr_abs(cpu, mem));
        set(cpu, (size_t)(cpu->ac), FN&FZ);     // set negative and zero flags
        return 4;
    } break;
    case ABX_LDA: {
        uint16_t addr = cpu->pc;
        cpu->ac = mem_read_byte(mem, addr_abx(cpu, mem));
        set(cpu, (size_t)(cpu->ac), FN&FZ);     // set negative and zero flags
        return CYCLES (4, 5);
    } break;
    case ABY_LDA: {
        uint16_t addr = cpu->pc;
        cpu->ac = mem_read_byte(mem, addr_aby(cpu, mem));
        set(cpu, (size_t)(cpu->ac), FN&FZ);     // set negative and zero flags
        return CYCLES (4, 5);
    } break;
    case INX_LDA: {
        cpu->ac = mem_read_byte(mem, addr_inx(cpu, mem));
        set(cpu, (size_t)(cpu->ac), FN&FZ);     // set negative and zero flags
        return 5;
    } break;
    case INY_LDA: {
        uint16_t addr = cpu->pc;
        cpu->ac = mem_read_byte(mem, addr_iny(cpu, mem));
        set(cpu, (size_t)(cpu->ac), FN&FZ);     // set negative and zero flags
        return CYCLES (5, 6);
    } break;
    default: {
        return 1;
    } break;
    }
}

void run(CPU_65XX *cpu, uint8_t *mem, ssize_t *cycles) {
    bool run = true;
    while (run && *cycles > 0) {
        uint8_t ins = fetch_byte(cpu, mem);
        *cycles -= execute(cpu, mem, ins);
    }
}

void run_endless(CPU_65XX *cpu, uint8_t *mem) {
    bool run = true;
    while (run) {
        uint8_t ins = fetch_byte(cpu, mem);
        execute(cpu, mem, ins);
    }
}
