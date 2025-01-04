#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 1024 * 1024

typedef struct {
    // registers
    uint32_t eax, ebx, ecx, edx;
    // eip: Extended Instruction Pointer
    uint32_t eip;
    // simulate memory
    uint8_t *memory;
} CPU;

CPU cpu;

void init_cpu() {
    cpu.memory = malloc(MEMORY_SIZE);
    memset(cpu.memory, 0, MEMORY_SIZE);
    cpu.eip = 0;
}

void deinit_cpu() {
    free(cpu.memory);
}

void load_binary() {
    const uint8_t program[] = {
        // mov eax, imm32 (move an immediate 32bit value into eax)
        0xb8, 0x04, 0x00, 0x00, 0x00,   // mov eax, imm32; (4)      ; syscall number (4 for write)
        0xbb, 0x01, 0x00, 0x00, 0x00,   // mov ebx, imm32; (1)      ; file descriptor (1 for stdout)
        0xb9, 0x00, 0x10, 0x00, 0x00,   // mov ecx, imm32; (0x1000) ; address of the msg (buffer address)
        0xba, 0x0b, 0x00, 0x00, 0x00,   // mov edx, imm32; (11)     ; length of the msg (buffer size)
        0xcd, 0x80,                     // int 0x80 (syscall)       ; trigger syscall
        0xf4                            // hlt (halt)               ; stop execution
    };
    memcpy(cpu.memory, program, sizeof(program));
    strcpy((char *)(cpu.memory + 0x1000), "hello world");
}

void execute() {
    while (1) {
        uint8_t opcode = cpu.memory[cpu.eip++];
        switch (opcode) {
            case 0xb8:
                // accessing 4 bytes from memory address as 32bit value
                cpu.eax = *(uint32_t *)&cpu.memory[cpu.eip];
                cpu.eip += 4;
                break;
            case 0xbb:
                cpu.ebx = *(uint32_t *)&cpu.memory[cpu.eip];
                cpu.eip += 4;
                break;
            case 0xb9:
                cpu.ecx = *(uint32_t *)&cpu.memory[cpu.eip];
                cpu.eip += 4;
                break;
            case 0xba:
                cpu.edx = *(uint32_t *)&cpu.memory[cpu.eip];
                cpu.eip += 4;
                break;
            case 0xcd:
                if (cpu.memory[cpu.eip++] == 0x80) {
                    if (cpu.eax == 4) { // write syscall
                        // ecx: address, edx: length
                        fwrite(&cpu.memory[cpu.ecx], 1, cpu.edx, stdout);
                    }
                }
                break;
            case 0xf4:
                return;
            default:
                printf("unknown instruction: 0x%02x\n", opcode);
                return;
        }
    }
}

int main() {
    init_cpu();
    load_binary();
    execute();
    deinit_cpu();
    return 0;
}