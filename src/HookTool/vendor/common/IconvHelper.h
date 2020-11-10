/**
 * @file signal_handler.h
 * @brief signal_handler defines
 */

#pragma once

#include <iconv.h>
#include <string.h>
 //////////////////////////////////////////////////////////////
 // 目的编码:
 //      TRANSLIT=遇到无法转换的字符就找相近字符替换
 //      IGNORE=遇到无法转换字符跳过
__inline static
int code_convert(const char* from_charset, const char* to_charset, const char* inbuf, size_t* inlen, char* outbuf, size_t* outlen)
{
    iconv_t cd;
    int rc;
    char** pin = (char**)&inbuf;
    char** pout = (char**)&outbuf;
    cd = iconv_open(to_charset, from_charset);
    if (cd == 0)
    {
        printf("iconv_open err=%d(%s)\n", errno, strerror(errno));
        return -1;
    }
    memset(outbuf, 0, *outlen);
    if (iconv(cd, pin, inlen, pout, outlen) == -1)
    {
        printf("iconv err=%d(%s)\n", errno, strerror(errno));
        return -1;
    }
    iconv_close(cd);
    return 0;
}
__inline static
int u2g(const char* inbuf, size_t* inlen, char* outbuf, size_t * outlen)
{
    return code_convert("UTF-8//IGNORE", "GB2312//IGNORE", inbuf, inlen, outbuf, outlen);
}
__inline static
int g2u(const char* inbuf, size_t* inlen, char* outbuf, size_t* outlen)
{
    return code_convert("GB2312//IGNORE", "UTF-8//IGNORE", inbuf, inlen, outbuf, outlen);
}
__inline static
int g2w(const char* inbuf, size_t* inlen, char* outbuf, size_t* outlen)
{
    return code_convert("GB2312//IGNORE", "UNICODE//IGNORE", inbuf, inlen, outbuf, outlen);
}
__inline static
int u2w(const char* inbuf, size_t* inlen, char* outbuf, size_t* outlen)
{
    return code_convert("UTF-8//IGNORE", "UNICODE//IGNORE", inbuf, inlen, outbuf, outlen);
}
__inline static
int w2u(const char* inbuf, size_t* inlen, char* outbuf, size_t* outlen)
{
    return code_convert("UNICODE//IGNORE", "UTF-8//IGNORE", inbuf, inlen, outbuf, outlen);
}