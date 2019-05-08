#include <windows.h>

//������� ���������� ������ ���������� ������� �������������� ����
extern "C" __declspec(dllexport) int win_height()
{
    return GetSystemMetrics(SM_CYFULLSCREEN);
}

//������� ���������� ��������� SSE
extern "C" __declspec(dllexport) int support_sse()
{
    int sse;
    __asm
    {
		//���� ��������� � ������� EAX �������
		//�� ����� ������� ������� cpuid
		//� �������� EDX 25-�� ��� ����� ����������
		//���� �� ��������� SSE ��� ���
		//���� 25-�� ��� ����� 1, ������ ����
		//���� 25-�� ��� ����� 0, ������ ���
        MOV EAX, 1h
        cpuid
		//�������� ��� ���� ����� 25
        AND EDX, 02000000h
		//�������� �� 25 ����� ������
		//25 ��� ������ ���� ������� �����
        SHR EDX, 25
		//���������� ��������, ������ �����
		//���� �������, ���� ����
		MOV sse, EDX
    }
    return sse;
}
