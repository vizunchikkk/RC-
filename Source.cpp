#include <iostream>
#include <algorithm>
#include <iomanip>

using namespace std;
/*Эта программа была написана по шаблону самого автора программы,
поэтому следующие константы как и в примере,
для того чтобы проверить работу алгоритма*/
const int b = 32; //длина половины 1 подблока (A-левого и B-правого)
const int l = 16;//длина секретного ключа в байтах (те 128 бит)
const int R = 12;//число раундов
const unsigned long P = 0xb7e15163;//у автора названы магиическими константами, они формируются используя здесь число фи(золотое сечение)
// из него вычитается 1, затем домножаются на 2^b, после округление до ближайшего нечетного целого
const unsigned long Q = 0x9e3779b9;//здесь используется число е(константа Эйлера)

//циклический сдвиг влево
unsigned long leftShift(unsigned long a, unsigned long c)
{
	return (a << c) | (a >> (b - c));
}
//вправо
unsigned long rightShift(unsigned long a, unsigned long c)
{
	return (a >> c) | (a << (b - c));
}

void Code(unsigned long A, unsigned long B, unsigned long* C1, unsigned long* C2, unsigned long* S)
{
	A += S[0];//складываем с 0 подключом
	B += S[1];//складываем с 1 подключом
	//выполняем алгоритм шифрования
	for (int i = 1; i <= R; i++)
	{
		A = leftShift((A ^ B), B) + S[i * 2];
		B = leftShift((B ^ A), A) + S[i * 2 + 1];
	}
	*C1 = A, * C2 = B;//в С1 и C2 переносим результат
}

void Decode(unsigned long A, unsigned long B, unsigned long* D1, unsigned long* D2, unsigned long* S)
{
	//выполняем алгоритм дешифрования
	for (int i = R; i >= 1; i--)
	{
		B = rightShift((B - S[i * 2 + 1]), A) ^ A;
		A = rightShift((A - S[i * 2]), B) ^ B;
	}
	*D1 = A - S[0], * D2 = B - S[1];//в D1 и D2 переносим результат
}

void creatingKey(unsigned char* key, unsigned long* S)
{
	int u = b / 8;//длина подблока в байтах
	int c = l / u;//число слов (длина ключа/длина подблока и все это в байтах)
	unsigned long A, B, * L;
	//L-массив слов
	L = (unsigned long*)calloc(c, sizeof(unsigned long));//почему то если выделить память как то по другому то ошибки
	for (int i = l; i <= (c * u - 1); i++)
	{
		key[i] = 0;//дополняем массив ключа нулевыми битами, если не хватило
	}
	//копирование ключа в массив слов
	L[c - l] = 0;
	for (int i = b - l; i >= 0; i--)
	{
		L[i / u] = leftShift(L[i / u], 8) + key[i];
	}
	S[0] = P;//присваиваем магическую константу
	int t = 2 * (R + 1); //число формируемых подключй
	for (int i = 1; i < t; i++)
	{
		S[i] = S[i - 1] + Q;//строим подключи с помощью магической константы
	}
	int m = max(t, c);//выбираем максимум из 2 значений (не умножаю на 3 тк результат одиаковый)
	//смешавание секретного ключа пользователя за три прохода,обработка массивов слов и подключей
	//больший массив будет обработан три раза, а другой может быть обработан больше раз
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
	//переменные для хранения ответов при кодировании и декодировании
	unsigned long C1, C2, D1, D2;
	
	//сам ключ
	//А-левая часть подблока, В-правая
	unsigned char key[l] = {
		0x52, 0x69, 0xf1, 0x49,
		0xd4, 0x1b, 0xa0, 0x15,
		0x24, 0x97, 0x57, 0x4d,
		0x7f, 0x15, 0x31, 0x25 };
	unsigned long A = 0xb278c165ul, B = 0xcc97d184ul;
	cout << "Начальная строка:" << endl;
	cout << hex << A << "  " << B << endl;
	unsigned long S[2 * (R + 1)];//переменная для храния формируемых подключей

	creatingKey(key, S);//здесь формируем подключи
	Code(A, B, &C1, &C2, S);
	cout << "Зашифрованное:" << endl;
	cout << hex << C1 << "  " << C2 << endl;
	Decode(C1, C2, &D1, &D2, S);
	cout << "Расшифрованное:" << endl;
	cout << hex << D1 << "  " << D2 << endl;

	return 0;
}