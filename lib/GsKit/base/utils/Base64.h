/**
 * Base64 Functions we use in CG to decode and enconde base64 data. We use that for savegame states at the part where we have
 * binary data in a xml
 * Code taken from http://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64
 */

#include <string>
#include <vector>

#ifndef BYTE
    #define BYTE unsigned char
#endif

#ifndef TCHAR
    #define TCHAR char
#endif

// Prototypes
std::basic_string<TCHAR> base64Encode(const std::vector<BYTE> &inputBuffer);
std::basic_string<TCHAR> base64Encode(BYTE *inputBuffer, const size_t size);
std::vector<BYTE> base64Decode(const std::basic_string<TCHAR>& input);
void base64Decode(BYTE *outBuffer, const std::basic_string<TCHAR> &inBuffer);

/* Define these if they aren't already in your environment
 * #define TEXT(x) Lx    //Unicode
 * #define TCHAR wchar_t //Unicode
 * #define TCHAR char    //Not unicode
 * #define TEXT(x) x     //Not unicode
 * #define DWORD long
 * #define BYTE unsigned char
 * They are defined by default in Windows.h
 */
