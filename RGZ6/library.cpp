#include <windows.h>

//функция определяет высоту клиентской области полноэкранного окна
extern "C" __declspec(dllexport) int win_height()
{
    return GetSystemMetrics(SM_CYFULLSCREEN);
}

//функция определяет поддержка SSE
extern "C" __declspec(dllexport) int support_sse()
{
    int sse;
    __asm
    {
		//если поместить в регистр EAX единицу
		//то после запуска команды cpuid
		//в регистре EDX 25-ый бит будет обозначать
		//есть ли поддержка SSE или нет
		//если 25-ый бит равен 1, значит есть
		//если 25-ый бит равен 0, значит нет
        MOV EAX, 1h
        cpuid
		//обнуляем все биты кроме 25
        AND EDX, 02000000h
		//сдвигаем на 25 битов вправо
		//25 бит теперь стал нулевым битом
        SHR EDX, 25
		//запоминаем значение, котрое равно
		//либо единице, либо нулю
		MOV sse, EDX
    }
    return sse;
}
