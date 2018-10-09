#include "stdafx.h"
#include "sundries.h"


const uint8_t c_u8CRCHigh[] =
{
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40
};

const uint8_t c_u8CRCLow[] =
{
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
	0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
	0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
	0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
	0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
	0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
	0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
	0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
	0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
	0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
	0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
	0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
	0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
	0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
	0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
	0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
	0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
	0x41, 0x81, 0x80, 0x40
};

/*
* 函数名      : CRC16
* 功能        : 计算一段缓存的CRC16值
* 参数        : pBuf [in] (uint8_t *)缓存指针
*             : u32Length [in] (uint32_t)缓存的大小
* 返回值      : uint16_t 类型, 表示CRC16值
* 作者        : 许龙杰
*/
uint16_t CRC16(const uint8_t *pFrame, uint16_t u16Len)
{
	uint8_t u8CRCHigh = 0xFF;
	uint8_t u8CRCLow = 0xFF;
	int32_t u32Index;
	if (pFrame == NULL)
	{
		return ~0;
	}

	while (u16Len--)
	{
		u32Index = u8CRCLow ^ (*(pFrame++));
		u8CRCLow = (uint8_t)(u8CRCHigh ^ c_u8CRCHigh[u32Index]);
		u8CRCHigh = c_u8CRCLow[u32Index];
	}
	return (uint16_t)(((uint16_t)u8CRCHigh) << 8 | u8CRCLow);
}

#define	 MX ((((z >> 5) ^ (y << 2)) + ((y >> 3) ^ (z << 4))) ^ ((sum ^ y) + (k[(p & 3) ^ e] ^ z)))

/*
* 函数名      : btea
* 功能        : XXTEA加密/解密
* 参数        : v [in/out] (int32_t * 类型): 需要加密/解密的数据指针
*             : n [in] (int32_t 类型): 需要加密/解密的数据长度，正值表示加密，负值表示解密
*             : k [in] (int32_t *): 128位的Key值
* 返回        : 正确返回0, 错误返回错误码
* 作者        : 许龙杰
*/
int32_t btea(int32_t *v, int32_t n, const int32_t *k)
{
	uint32_t z = 0, y = v[0], sum = 0, e, DELTA = 0x9E3779B9;
	int32_t p, q;
	if (n > 1)
	{ /* Coding Part */
		z = v[n - 1];
		q = 6 + 52 / n;
		while (q-- > 0)
		{
			sum += DELTA;
			e = (sum >> 2) & 3;
			for (p = 0; p < n - 1; p++)
			{
				y = v[p + 1];
				v[p] += MX;
				z = v[p];
			}
			y = v[0];
			v[n - 1] += MX;
			z = v[n - 1];
		}
		return 0;
	}
	else if (n < -1)
	{ /* Decoding Part */
		n = -n;
		z = v[n - 1];
		q = 6 + 52 / n;
		sum = q * DELTA;
		while (sum != 0)
		{
			e = (sum >> 2) & 3;
			for (p = n - 1; p > 0; p--)
			{
				z = v[p - 1];
				v[p] -= MX;
				y = v[p];
			}
			z = v[n - 1];
			v[0] -= MX;
			y = v[0];
			sum -= DELTA;
		}
		return 0;
	}
	return 1;
}

int32_t CheckSum(int32_t *pData, uint32_t u32Cnt)
{
	uint32_t i;
	int32_t s32CheckSum = 0;
	for (i = 0; i < u32Cnt; i++)
	{
		s32CheckSum += pData[i];
	}
	s32CheckSum = ~s32CheckSum;
	s32CheckSum -= 1;
	return s32CheckSum;
}


/*
* 函数名      : Base64Encode
* 功能        : Base64编码
* 参数        : pSrc[in] (const void * 类型): 要编码的Buf指针
*             : u32Length[in] (uint32_t 类型): Buf的长度
*             : pDest[out] (void * 类型): 输出Buf指针
* 返回值      : 返回编码长度
* 作者        : 许龙杰
*/
int32_t Base64Encode(const char *pSrc, uint32_t u32Length, char *pDest)
{
	base64_encodestate stBase64State;
	base64_init_encodestate(&stBase64State);

	int s32EncodeLength = base64_encode_block((const char *)pSrc, u32Length,
		pDest, &stBase64State);

	s32EncodeLength += base64_encode_blockend(pDest + s32EncodeLength,
		&stBase64State);
	pDest[s32EncodeLength] = 0;
	return s32EncodeLength;
}

/*
* 函数名      : Base64Decode
* 功能        : Base64解码
* 参数        : pSrc[in] (const void * 类型): 要解码的Buf指针
*             : u32Length[in] (uint32_t 类型): Buf的长度
*             : pDest[out] (void * 类型): 输出Buf指针
* 返回值      : 返回解码长度
* 作者        : 许龙杰
*/
int32_t Base64Decode(const char *pSrc, uint32_t u32Length, char *pDest)
{
	base64_decodestate stBase64State;
	base64_init_decodestate(&stBase64State);

	int s32OrgLength = base64_decode_block(
		pSrc,
		u32Length,
		pDest,
		&stBase64State);

	return s32OrgLength;
}

const int32_t c_s32Key[4] = {0x2B7A2131, 0x78D06BD9,0x133F3173, 0x3CF10149 };

int32_t GetDecodePassword(const char *pBase64, char **p2Password)
{
	uint32_t u32Len = 0;
	int32_t s32DecodeLen = 0;
	char *pTmp = NULL;
	if (pBase64 == NULL || p2Password == NULL)
	{
		return -1;
	}

	u32Len = strlen(pBase64);
	if (u32Len == 0)
	{
		return -1;
	}
	pTmp = (char *)calloc(1, u32Len);
	if (pTmp == NULL)
	{
		return -1;
	}

	s32DecodeLen = Base64Decode(pBase64, u32Len, pTmp);
	if (s32DecodeLen <= 0)
	{
		free(pTmp);
		return -1;
	}

	if ((s32DecodeLen & 0x03) != 0x00)
	{
		free(pTmp);
		return -1;
	}

	btea((int32_t *)pTmp, 0 - (s32DecodeLen >> 2), c_s32Key);

	{
		uint16_t u16CRC = CRC16((uint8_t *)pTmp, s32DecodeLen - 2);
		uint16_t u16CRCOld = (uint8_t)(pTmp[s32DecodeLen - 2]);
		u16CRCOld = (u16CRCOld << 8) | ((uint8_t)(pTmp[s32DecodeLen - 1]));
		if (u16CRCOld != u16CRC)
		{
			free(pTmp);
			return -1;
		}
	}

	*p2Password = pTmp;

	return 0;
}

int32_t GetEncodePassword(const char *pPassword, char **p2Base64)
{
	int32_t s32PWLen = 0;
	char *pTmp = NULL;
	char *pBase64 = NULL;
	if (pPassword == NULL || p2Base64 == NULL)
	{
		return -1;
	}

	s32PWLen = strlen(pPassword);
	s32PWLen = (s32PWLen + 1 + 2 + 3) & (~0x03);

	pTmp = (char *)calloc(1, s32PWLen);
	if (pTmp == NULL)
	{
		return -1;
	}

	strcpy_s(pTmp, s32PWLen, pPassword);

	{
		uint16_t u16CRC = CRC16((uint8_t *)pTmp, s32PWLen - 2);
		pTmp[s32PWLen - 2] = (int8_t)(u16CRC >> 8);
		pTmp[s32PWLen - 1] = (int8_t)u16CRC;
	}

	{
		btea((int32_t *)pTmp, s32PWLen >> 2, c_s32Key);
	}

	pBase64 = (char *)calloc(1, s32PWLen * 2 + 4);
	if (pBase64 == NULL)
	{
		free(pTmp);
		return -1;
	}

	{
		int32_t s32Len = Base64Encode(pTmp, s32PWLen, pBase64);
		free(pTmp);
		
		if (s32Len <= 0)
		{
			free(pBase64);
			return -1;
		}
	}

	*p2Base64 = pBase64;

	return 0;
}


int base64_decode_value(char value_in)
{
	static const char decoding[] = { 62,-1,-1,-1,63,52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-2,-1,-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51 };
	static const char decoding_size = sizeof(decoding);
	value_in -= 43;
	if (value_in < 0 || value_in >= decoding_size) return -1;
	return decoding[(int)value_in];
}

void base64_init_decodestate(base64_decodestate* state_in)
{
	state_in->step = step_a;
	state_in->plainchar = 0;
}

int base64_decode_block(const char* code_in, const int length_in, char* plaintext_out, base64_decodestate* state_in)
{
	const char* codechar = code_in;
	char* plainchar = plaintext_out;
	char fragment;

	*plainchar = state_in->plainchar;

	switch (state_in->step)
	{
		while (1)
		{
	case step_a:
		do {
			if (codechar == code_in + length_in)
			{
				state_in->step = step_a;
				state_in->plainchar = *plainchar;
				return plainchar - plaintext_out;
			}
			fragment = (char)base64_decode_value(*codechar++);
		} while (fragment < 0);
		*plainchar = (fragment & 0x03f) << 2;
	case step_b:
		do {
			if (codechar == code_in + length_in)
			{
				state_in->step = step_b;
				state_in->plainchar = *plainchar;
				return plainchar - plaintext_out;
			}
			fragment = (char)base64_decode_value(*codechar++);
		} while (fragment < 0);
		*plainchar++ |= (fragment & 0x030) >> 4;
		*plainchar = (fragment & 0x00f) << 4;
	case step_c:
		do {
			if (codechar == code_in + length_in)
			{
				state_in->step = step_c;
				state_in->plainchar = *plainchar;
				return plainchar - plaintext_out;
			}
			fragment = (char)base64_decode_value(*codechar++);
		} while (fragment < 0);
		*plainchar++ |= (fragment & 0x03c) >> 2;
		*plainchar = (fragment & 0x003) << 6;
	case step_d:
		do {
			if (codechar == code_in + length_in)
			{
				state_in->step = step_d;
				state_in->plainchar = *plainchar;
				return plainchar - plaintext_out;
			}
			fragment = (char)base64_decode_value(*codechar++);
		} while (fragment < 0);
		*plainchar++ |= (fragment & 0x03f);
		}
	}
	/* control should not reach here */
	return plainchar - plaintext_out;
}


const int CHARS_PER_LINE = 72;

void base64_init_encodestate(base64_encodestate* state_in)
{
	state_in->step = step_A;
	state_in->result = 0;
	state_in->stepcount = 0;
}

char base64_encode_value(char value_in)
{
	static const char* encoding = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	if (value_in > 63) return '=';
	return encoding[(int)value_in];
}

int base64_encode_block(const char* plaintext_in, int length_in, char* code_out, base64_encodestate* state_in)
{
	const char* plainchar = plaintext_in;
	const char* const plaintextend = plaintext_in + length_in;
	char* codechar = code_out;
	char result;
	char fragment;

	result = state_in->result;

	switch (state_in->step)
	{
		while (1)
		{
	case step_A:
		if (plainchar == plaintextend)
		{
			state_in->result = result;
			state_in->step = step_A;
			return codechar - code_out;
		}
		fragment = *plainchar++;
		result = (fragment & 0x0fc) >> 2;
		*codechar++ = base64_encode_value(result);
		result = (fragment & 0x003) << 4;
	case step_B:
		if (plainchar == plaintextend)
		{
			state_in->result = result;
			state_in->step = step_B;
			return codechar - code_out;
		}
		fragment = *plainchar++;
		result |= (fragment & 0x0f0) >> 4;
		*codechar++ = base64_encode_value(result);
		result = (fragment & 0x00f) << 2;
	case step_C:
		if (plainchar == plaintextend)
		{
			state_in->result = result;
			state_in->step = step_C;
			return codechar - code_out;
		}
		fragment = *plainchar++;
		result |= (fragment & 0x0c0) >> 6;
		*codechar++ = base64_encode_value(result);
		result = (fragment & 0x03f) >> 0;
		*codechar++ = base64_encode_value(result);

		++(state_in->stepcount);
		if (state_in->stepcount == CHARS_PER_LINE / 4)
		{
			//*codechar++ = '\n';
			state_in->stepcount = 0;
		}
		}
	}
	/* control should not reach here */
	return codechar - code_out;
}

int base64_encode_blockend(char* code_out, base64_encodestate* state_in)
{
	char* codechar = code_out;

	switch (state_in->step)
	{
	case step_B:
		*codechar++ = base64_encode_value(state_in->result);
		*codechar++ = '=';
		*codechar++ = '=';
		break;
	case step_C:
		*codechar++ = base64_encode_value(state_in->result);
		*codechar++ = '=';
		break;
	case step_A:
		break;
	}
	//*codechar++ = '\n';

	return codechar - code_out;
}

