/**
 * Resource management, RGR, Variant 06.
 * Dynamic-Link Library.
 * @author      Zhigalov Peter
 * @version     1.2
 */
#include <windows.h>

/**
 * @brief Function for determine maximum height of the window
 * @return maximum height of the window
 */
extern "C" __declspec(dllexport) int get_max_window_height()
{
    return GetSystemMetrics(SM_CYFULLSCREEN);
}

/**
 * @brief Function for check SSE support
 * @return 0 - not support, 1 - support, -1 - can`t CPUID
 */
extern "C"
__declspec(dllexport)
int has_sse()
{
    int sse_flag;
    __asm
    {
        ; Check support CPUID instruction
        pushfd              ; push EFLAGS to stack
        pop eax             ; pop to EAX
        mov ebx, eax        ; save to EBX
        xor eax, 200000h    ; change bit 21
        push eax            ; push changes to stack
        popfd               ; pop new EFLAGS
        pushfd              ; push EFLAGS to stack
        pop eax             ; pop to EAX
        xor eax, ebx        ; check bit 21
        je no_cpuid         ; if 0 CPUID not supported
        ; Check support SSE
        mov eax, 1          ; EAX=1: Processor Info and Feature Bits
        cpuid               ; call CPUID
        and edx, 2000000h   ; mask
        shr edx, 25         ; shift to 25 bit
        mov sse_flag, edx   ; save result
        jmp end
        ; No CPUID support
        no_cpuid:
        mov sse_flag, -1    ; return -1
        ; End
        end:
    }
    return sse_flag;
}
