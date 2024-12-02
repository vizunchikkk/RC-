#include <iostream>
#include <algorithm>
#include <iomanip>

using namespace std;
/*��� ��������� ���� �������� �� ������� ������ ������ ���������,
������� ��������� ��������� ��� � � �������,
��� ���� ����� ��������� ������ ���������*/
const int b = 32; //����� �������� 1 �������� (A-������ � B-�������)
const int l = 16;//����� ���������� ����� � ������ (�� 128 ���)
const int R = 12;//����� �������
const unsigned long P = 0xb7e15163;//� ������ ������� ������������ �����������, ��� ����������� ��������� ����� ����� ��(������� �������)
// �� ���� ���������� 1, ����� ����������� �� 2^b, ����� ���������� �� ���������� ��������� ������
const unsigned long Q = 0x9e3779b9;//����� ������������ ����� �(��������� ������)

//����������� ����� �����
unsigned long leftShift(unsigned long a, unsigned long c)
{
	return (a << c) | (a >> (b - c));
}
//������
unsigned long rightShift(unsigned long a, unsigned long c)
{
	return (a >> c) | (a << (b - c));
}

void Code(unsigned long A, unsigned long B, unsigned long* C1, unsigned long* C2, unsigned long* S)
{
	A += S[0];//���������� � 0 ���������
	B += S[1];//���������� � 1 ���������
	//��������� �������� ����������
	for (int i = 1; i <= R; i++)
	{
		A = leftShift((A ^ B), B) + S[i * 2];
		B = leftShift((B ^ A), A) + S[i * 2 + 1];
	}
	*C1 = A, * C2 = B;//� �1 � C2 ��������� ���������
}

void Decode(unsigned long A, unsigned long B, unsigned long* D1, unsigned long* D2, unsigned long* S)
{
	//��������� �������� ������������
	for (int i = R; i >= 1; i--)
	{
		B = rightShift((B - S[i * 2 + 1]), A) ^ A;
		A = rightShift((A - S[i * 2]), B) ^ B;
	}
	*D1 = A - S[0], * D2 = B - S[1];//� D1 � D2 ��������� ���������
}

void creatingKey(unsigned char* key, unsigned long* S)
{
	int u = b / 8;//����� �������� � ������
	int c = l / u;//����� ���� (����� �����/����� �������� � ��� ��� � ������)
	unsigned long A, B, * L;
	//L-������ ����
	L = (unsigned long*)calloc(c, sizeof(unsigned long));//������ �� ���� �������� ������ ��� �� �� ������� �� ������
	for (int i = l; i <= (c * u - 1); i++)
	{
		key[i] = 0;//��������� ������ ����� �������� ������, ���� �� �������
	}
	//����������� ����� � ������ ����
	L[c - l] = 0;
	for (int i = b - l; i >= 0; i--)
	{
		L[i / u] = leftShift(L[i / u], 8) + key[i];
	}
	S[0] = P;//����������� ���������� ���������
	int t = 2 * (R + 1); //����� ����������� ��������
	for (int i = 1; i < t; i++)
	{
		S[i] = S[i - 1] + Q;//������ �������� � ������� ���������� ���������
	}
	int m = max(t, c);//�������� �������� �� 2 �������� (�� ������� �� 3 �� ��������� ���������)
	//���������� ���������� ����� ������������ �� ��� �������,��������� �������� ���� � ���������
	//������� ������ ����� ��������� ��� ����, � ������ ����� ���� ��������� ������ ���
	for (int i = 0, j = 0, A = B = 0, z = 1; z <= 3 * m; z++)
	{
		A = S[i] = leftShift(S[i] + A + B, 3);
		i = (i + 1) % m;
		B = L[j] = leftShift((L[j] + A + B), (A + B));
		j = (j + 1) % c;
	}
}

int main(void)
{
	setlocale(LC_ALL, "Russian");
	//���������� ��� �������� ������� ��� ����������� � �������������
	unsigned long C1, C2, D1, D2;
	
	//��� ����
	//�-����� ����� ��������, �-������
	unsigned char key[l] = {
		0x52, 0x69, 0xf1, 0x49,
		0xd4, 0x1b, 0xa0, 0x15,
		0x24, 0x97, 0x57, 0x4d,
		0x7f, 0x15, 0x31, 0x25 };
	unsigned long A = 0xb278c165ul, B = 0xcc97d184ul;
	cout << "��������� ������:" << endl;
	cout << hex << A << "  " << B << endl;
	unsigned long S[2 * (R + 1)];//���������� ��� ������ ����������� ���������

	creatingKey(key, S);//����� ��������� ��������
	Code(A, B, &C1, &C2, S);
	cout << "�������������:" << endl;
	cout << hex << C1 << "  " << C2 << endl;
	Decode(C1, C2, &D1, &D2, S);
	cout << "��������������:" << endl;
	cout << hex << D1 << "  " << D2 << endl;

	return 0;
}