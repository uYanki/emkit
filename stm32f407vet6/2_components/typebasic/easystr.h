#ifndef __EASYSTR_H__
#define __EASYSTR_H__

#include "typebasic.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define _islower(c)  ('a' <= (c) && (c) <= 'z')
#define _isupper(c)  ('A' <= (c) && (c) <= 'Z')
#define _isalpha(c)  (_islower(c) || _isupper(c))
#define _isspace(c)  ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r' || (c) == '\f' || (c) == '\v')  ///< is blank character

#define _isbdigit(c) ((c) == '0' || (c) == '1')                                                 ///< is binary number
#define _isdigit(c)  ('0' <= (c) && (c) <= '9')                                                 ///< is decimal number
#define _isxdigit(c) (_isdigit(c) || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))  ///< is hexadecimal number

inline s32 _str2dec(__IN RO u8* str);                  ///< ascii to decimal integer
inline u32 _str2hex(__IN RO u8* str);                  ///< ascii to hexadecimal integer
inline f32 _str2flt(__IN RO u8* str);                  ///< ascii to real
inline u8* _int2str(__OUT u8* str, s32 num, u8 base);  ///< integers from 2 to 36 bases convert to ascii

inline u32 _strlen(RO u8* str);                                ///< calculate the length of a string
inline s32 _strcmp(RO u8* str1, RO u8* str2);                  ///< compare the size of two strings
inline u8* _strcpy(u8* dest, RO u8* src);                      ///< copy the source string to the destination string
inline u8* _strncpy(u8* dest, RO u8* src, u32 len);            ///< copy the first n characters of the source string to the destination string
inline u8* _strcat(u8* dest, RO u8* src);                      ///< append the source string to the end of the destination string
inline u8* _strstr(RO u8* str, RO u8* substr);                 ///< find the position of a substring in a string
inline u8* _substr(u8* dest, RO u8* src, u32 start, u32 len);  ///< take n characters from a string starting at a certain position

inline u8* _strlwr(RO u8* str);  ///< lower
inline u8* _strupr(RO u8* str);  ///< upper

s32  _atoi(const char* s);
s32  _atol(const char* s);
void _itoa(s32 num, char* str, u8 base);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
