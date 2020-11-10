#pragma once
#include <base64.h>
#include <openssl/evp.h>
#include <openssl/aes.h>

typedef enum {
	ECMTYPE_CBC = 0,
	ECMTYPE_ECB = 1,
	ECMTYPE_CFB = 2,
	ECMTYPE_OFB = 3,
	ECMTYPE_CTR = 4,
	ECMTYPE_CCM = 5,
	ECMTYPE_GCM = 6,
	ECMTYPE_XTS = 7,
	ECMTYPE_WRAP = 8,
}EVPCIPHERMODE_TYPE;
typedef enum {
	SKBTYPE_128 = 0,
	SKBTYPE_192 = 1,
	SKBTYPE_256 = 2,
	SKBTYPE_MAX
}SECRETKEYBITS_TYPE;
typedef enum {
	PMTYPE_NULL = 0,
	PMTYPE_ZERO = 1,
	PMTYPE_PKCS5 = 2,
	PMTYPE_PKCS7 = 3,
	PMTYPE_ISO7816 = 4,
	PMTYPE_ISO10126 = 5,
	PMTYPE_ANSIX923 = 6,
}PADDINGMODE_TYPE;

__inline static 
size_t CALC_DATA_BLOCK_SIZE(const char * data)
{
	return ((strlen(data) * sizeof(char)) + (((strlen(data) * sizeof(char)) % AES_BLOCK_SIZE) ? (AES_BLOCK_SIZE - (strlen(data) * sizeof(char) % AES_BLOCK_SIZE)) : 0));
}
__inline static 
size_t CALC_DATA_BLOCK_SIZE(const wchar_t * data)
{
	return ((wcslen(data) * sizeof(wchar_t)) + (((wcslen(data) * sizeof(wchar_t)) % AES_BLOCK_SIZE) ? (AES_BLOCK_SIZE - (wcslen(data) * sizeof(wchar_t) % AES_BLOCK_SIZE)) : 0));
}

const struct AES_EVPCipher
{
	EVPCIPHERMODE_TYPE evpciphermode_type;
	struct AES_EVPMethod
	{
		SECRETKEYBITS_TYPE secretkeybits_type;
		EVP_CIPHER * evp_cipher;
	} evp_methods[SKBTYPE_MAX];
} evp_ciphers[] = {
	{
		ECMTYPE_CBC,
		{
			{ SKBTYPE_128, (EVP_CIPHER *)EVP_aes_128_cbc() },
			{ SKBTYPE_192, (EVP_CIPHER *)EVP_aes_192_cbc() },
			{ SKBTYPE_256, (EVP_CIPHER *)EVP_aes_256_cbc() }
		}
	},
	{
		ECMTYPE_ECB,
		{
			{ SKBTYPE_128, (EVP_CIPHER *)EVP_aes_128_ecb() },
			{ SKBTYPE_192, (EVP_CIPHER *)EVP_aes_192_ecb() },
			{ SKBTYPE_256, (EVP_CIPHER *)EVP_aes_256_ecb() }
		}
	},
	{
		ECMTYPE_CFB,
		{
			{ SKBTYPE_128, (EVP_CIPHER *)EVP_aes_128_cfb() },
			{ SKBTYPE_192, (EVP_CIPHER *)EVP_aes_192_cfb() },
			{ SKBTYPE_256, (EVP_CIPHER *)EVP_aes_256_cfb() }
		}
	},
	{
		ECMTYPE_OFB,
		{
			{ SKBTYPE_128, (EVP_CIPHER *)EVP_aes_128_ofb() },
			{ SKBTYPE_192, (EVP_CIPHER *)EVP_aes_192_ofb() },
			{ SKBTYPE_256, (EVP_CIPHER *)EVP_aes_256_ofb() }
		}
	},
	{
		ECMTYPE_CTR,
		{
			{ SKBTYPE_128, (EVP_CIPHER *)EVP_aes_128_ctr() },
			{ SKBTYPE_192, (EVP_CIPHER *)EVP_aes_192_ctr() },
			{ SKBTYPE_256, (EVP_CIPHER *)EVP_aes_256_ctr() }
		}
	},
	{
		ECMTYPE_CCM,
		{
			{ SKBTYPE_128, (EVP_CIPHER *)EVP_aes_128_ccm() },
			{ SKBTYPE_192, (EVP_CIPHER *)EVP_aes_192_ccm() },
			{ SKBTYPE_256, (EVP_CIPHER *)EVP_aes_256_ccm() }
		}
	},
	{
		ECMTYPE_GCM,
		{
			{ SKBTYPE_128, (EVP_CIPHER *)EVP_aes_128_gcm() },
			{ SKBTYPE_192, (EVP_CIPHER *)EVP_aes_192_gcm() },
			{ SKBTYPE_256, (EVP_CIPHER *)EVP_aes_256_gcm() }
		}
	},
	{
		ECMTYPE_XTS,
		{
			{ SKBTYPE_128, (EVP_CIPHER *)EVP_aes_128_xts() },
			{ SKBTYPE_192, (EVP_CIPHER *)NULL },
			{ SKBTYPE_256, (EVP_CIPHER *)EVP_aes_256_xts() }
		}
	},
	{
		ECMTYPE_WRAP,
		{
			{ SKBTYPE_128, (EVP_CIPHER *)EVP_aes_128_wrap() },
			{ SKBTYPE_192, (EVP_CIPHER *)EVP_aes_192_wrap() },
			{ SKBTYPE_256, (EVP_CIPHER *)EVP_aes_256_wrap() }
		}
	},
};
//功能：AES加密
//参数:
//	text	--	要加密的字符串
//	ecmtype	--	加解密模式选择
//	mbtype	--	加解密位数选择
//	pbtype	--	填充模式选择
//	userkey --	用户加密密钥
//	iv		--	iv初始化向量
//返回:
//	加密后的字符串，错误返回空字符串
__inline static 
std::string AES_Encrypt(const std::string& text, EVPCIPHERMODE_TYPE ecmtype, SECRETKEYBITS_TYPE skbtype,
	PADDINGMODE_TYPE pmtype, const char userkey[EVP_MAX_KEY_LENGTH], const char iv[EVP_MAX_IV_LENGTH])
{
	int nret = 0;
	int tlen = 0;
	int mlen = 0;
	int flen = 0;
	EVP_CIPHER_CTX ctx = { 0 };
	const char * pData = text.c_str();
	size_t nSize = strlen(text.c_str()) * sizeof(char);
	size_t nDataBlockSize = CALC_DATA_BLOCK_SIZE(text.c_str());
	unsigned char *p_data = (unsigned char *)malloc(nDataBlockSize);
	unsigned char *p_encrypt = (unsigned char *)malloc(nDataBlockSize * sizeof(wchar_t));
	const EVP_CIPHER * p_evp_cipher = evp_ciphers[ecmtype].evp_methods[skbtype].evp_cipher;

	if (!p_data || !p_encrypt || !p_evp_cipher)
	{
#if defined(_DEBUG) || defined(DEBUG)
		printf("data=(0x%x),encrypt=(0x%x),evp_cipher=(0x%x)\n", p_data, p_encrypt, p_evp_cipher);
#endif
		goto __LEAVE_CLEAN__;
	}

	memset((void*)p_data, '\0', nDataBlockSize);
	memcpy((void*)p_data, (const void *)pData, nSize);

	//初始化ctx
	EVP_CIPHER_CTX_init(&ctx);

	//指定加密算法及key和iv(此处IV没有用)
	nret = EVP_EncryptInit_ex(&ctx, p_evp_cipher, NULL, (const unsigned char *)userkey, (const unsigned char *)iv);
	if (nret != 1)
	{
#if defined(_DEBUG) || defined(DEBUG)
		printf("EVP_EncryptInit_ex failed\n");
#endif
		goto __LEAVE_CLEAN__;
	}

	//禁用padding功能
	EVP_CIPHER_CTX_set_padding(&ctx, ((pmtype == PMTYPE_NULL) ? 0 : 1));
	//进行加密操作
	nret = EVP_EncryptUpdate(&ctx, p_encrypt, &mlen, p_data, nDataBlockSize);
	if (nret != 1)
	{
#if defined(_DEBUG) || defined(DEBUG)
		printf("EVP_EncryptUpdate failed\n");
#endif
		goto __LEAVE_CLEAN__;
	}
	//结束加密操作
	nret = EVP_EncryptFinal_ex(&ctx, p_encrypt + mlen, &flen);
	if (nret != 1)
	{
#if defined(_DEBUG) || defined(DEBUG)
		printf("EVP_EncryptFinal_ex failed\n");
#endif
		goto __LEAVE_CLEAN__;
	}

	tlen = mlen + flen;

	return std::move(std::string((const char *)p_encrypt, (size_t)mlen));

__LEAVE_CLEAN__:
	if (p_data)
	{
		free(p_data);
	}
	if (p_encrypt)
	{
		free(p_encrypt);
	}
	return std::string("");
}

//功能：AES解密
//参数:
//	text	--	要加密的字符串
//	ecmtype	--	加解密模式选择
//	mbtype	--	加解密位数选择
//	pbtype	--	填充模式选择
//	userkey --	用户加密密钥
//	iv		--	iv初始化向量
//返回:
//	解密后的字符串，错误返回空字符串
__inline static
std::string AES_Decrypt(const std::string& text, EVPCIPHERMODE_TYPE ecmtype, SECRETKEYBITS_TYPE skbtype,
	PADDINGMODE_TYPE pmtype, const char userkey[EVP_MAX_KEY_LENGTH], const char iv[EVP_MAX_IV_LENGTH])
{
	int nret = 0;
	int tlen = 0;
	int mlen = 0;
	int flen = 0;
	EVP_CIPHER_CTX ctx = { 0 };
	EVP_CIPHER * evp_cipher = NULL;
	const char * pData = text.c_str();
	size_t nSize = text.size(), nDataBlockSize = text.size();
	unsigned char *p_plain = (unsigned char *)malloc(nDataBlockSize);
	unsigned char *p_encrypt = (unsigned char *)malloc(nDataBlockSize);
	const EVP_CIPHER * p_evp_cipher = evp_ciphers[ecmtype].evp_methods[skbtype].evp_cipher;
	
	if (!p_encrypt || !p_plain || !p_evp_cipher)
	{
#if defined(_DEBUG) || defined(DEBUG)
		printf("encrypt=(0x%x),plain=(0x%x),evp_cipher=(0x%x)\n", p_encrypt, p_plain, p_evp_cipher);
#endif
		goto __LEAVE_CLEAN__;
	}

	memcpy((void*)p_encrypt, (const void *)pData, nSize);

	//初始化ctx
	EVP_CIPHER_CTX_init(&ctx);

	//指定解密算法及key和iv(此处IV没有用)
	nret = EVP_DecryptInit_ex(&ctx, p_evp_cipher, NULL, (const unsigned char *)userkey, (const unsigned char *)iv);
	if (nret != 1)
	{
#if defined(_DEBUG) || defined(DEBUG)
		printf("EVP_DecryptInit_ex failed\n");
#endif
		goto __LEAVE_CLEAN__;
	}

	EVP_CIPHER_CTX_set_padding(&ctx, ((pmtype == PMTYPE_NULL) ? 0 : 1));

	//进行解密操作
	nret = EVP_DecryptUpdate(&ctx, p_plain, &mlen, p_encrypt, nDataBlockSize);
	if (nret != 1)
	{
#if defined(_DEBUG) || defined(DEBUG)
		printf("EVP_DecryptUpdate failed\n");
#endif
		goto __LEAVE_CLEAN__;
	}

	//结束解密操作
	nret = EVP_DecryptFinal_ex(&ctx, p_plain + mlen, &flen);
	if (nret != 1)
	{
#if defined(_DEBUG) || defined(DEBUG)
		printf("EVP_DecryptFinal_ex failed\n");
#endif
		goto __LEAVE_CLEAN__;
	}

	//释放ctx
	EVP_CIPHER_CTX_cleanup(&ctx);

	return std::move(std::string((const char *)p_plain, (size_t)mlen));

__LEAVE_CLEAN__:
	if (p_encrypt)
	{
		free(p_encrypt);
	}
	if (p_plain)
	{
		free(p_plain);
	}
	return std::string("");
}
