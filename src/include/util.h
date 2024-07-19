
#ifndef UTIL_H_
#define UTIL_H_ 1

/* Print a format message, and exit */
void die(const char* fmt, ...);

/* Return the size of a file using `fseek' */
size_t get_file_size(FILE* fp);

#endif /* UTIL_H_ */
