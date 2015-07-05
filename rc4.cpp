
#include "stdafx.h"
#include "rc4.h"

void rc4_setup(struct rc4_state *s, unsigned char *key, int length)
{
	int i, j, k, *m, a;

	s->x = 0;
	s->y = 0;
	m = s->m;

	for (i = 0; i < 256; i++)
	{
		m[i] = i;
	}

	j = k = 0;

	for (i = 0; i < 256; i++)
	{
		a = m[i];
		j = (unsigned char)(j + a + key[k]);
		m[i] = m[j]; m[j] = a;
		if (++k >= length) k = 0;
	}
}

void rc4_crypt(struct rc4_state *s, unsigned char *data, int length)
{
	int i, x, y, *m, a, b;

	x = s->x;
	y = s->y;
	m = s->m;

	for (i = 0; i < length; i++)
	{
		x = (unsigned char)(x + 1); a = m[x];
		y = (unsigned char)(y + a);
		m[x] = b = m[y];
		m[y] = a;
		data[i] ^= m[(unsigned char)(a + b)];
	}

	s->x = x;
	s->y = y;
}
// 把字节码转为十六进制码，一个字节两个十六进制，内部为字符串分配空间
char* ByteToHex(char* vByte, int vLen)
{
	if (!vByte)
		return NULL;

	char* tmp = new char[vLen * 2 + 1]; // 一个字节两个十六进制码，最后要多一个'\0'

	int tmp2;
	for (int i = 0; i<vLen; i++)
	{
		tmp2 = (int)(vByte[i]) / 16;
		tmp[i * 2] = (char)(tmp2 + ((tmp2>9) ? 'A' - 10 : '0'));
		tmp2 = (int)(vByte[i]);
		tmp[i * 2 + 1] = (char)(tmp2 + ((tmp2>9) ? 'A' - 10 : '0'));
	}

	tmp[vLen * 2] = '\0';
	return tmp;
}
