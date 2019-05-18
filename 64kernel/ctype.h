#ifndef SINOS_64KERNEL_CTYPE_H
#define SINOS_64KERNEL_CTYPE_H

inline bool isblank(char c) {return (c == ' ' || c == '\t') ? true : false;}
inline bool isdigit(char c) {return ('0' <= c && c <= '9') ? true : false;}
inline bool isalpha(char c) {return ('A' <= c && c <= 'Z' ||
				     'a' <= c && c <= 'z') ? true : false;}
inline char tolower(char c) {return (isalpha(c) && c <= 'Z') ? c - 'A' + 'a' : c;}
inline char toupper(char c) {return (isalpha(c) && c >= 'a') ? c - 'a' + 'A' : c;}

#endif // SINOS_64KERNEL_CTYPE_H
