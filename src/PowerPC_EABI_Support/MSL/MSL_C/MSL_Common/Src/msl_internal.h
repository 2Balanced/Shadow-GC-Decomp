#ifndef MSL_INTERNAL_H_
#define MSL_INTERNAL_H_

/* Declarations shared by the MSL C library's own sources (this directory). The game code never
 * includes this file: it uses the stand-in headers in ../Include, which keep the stream type
 * opaque. */

typedef unsigned long size_t;
typedef unsigned short wchar_t;
typedef unsigned long fpos_t;

#ifndef NULL
#define NULL 0
#endif

#define EOF (-1)

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

/* Buffering modes (setvbuf). */
#define _IONBF 0
#define _IOLBF 1
#define _IOFBF 2

/* errno values the library sets. */
#define ERANGE 34
#define EFPOS 40

extern int errno;

/* Critical regions: placeholders on the GameCube, which runs the library single-threaded. */
enum critical_regions {
    atexit_funcs_access = 0,
    malloc_pool_access = 1,
    stdin_access = 2,
    stdout_access = 3,
    stderr_access = 4,
    files_access = 5,
    console_status_access = 6,
    signal_funcs_access = 7,
    thread_access = 8,
    num_critical_regions = 9
};

void __init_critical_regions(void);
void __kill_critical_regions(void);
void __begin_critical_region(int region);
void __end_critical_region(int region);

/* Streams. */

enum __file_kinds { __closed_file, __disk_file, __console_file, __unavailable_file };
enum __open_modes { __must_exist, __create_if_necessary, __create_or_truncate };
enum __file_orientation { __unoriented, __char_oriented, __wide_oriented };
enum __io_modes { __read = 1, __write = 2, __read_write = 3, __append = 4 };
enum __io_states { __neutral, __writing, __reading, __rereading };
enum __io_results { __no_io_error, __io_error, __io_EOF };

typedef unsigned long __file_handle;

typedef struct {
    unsigned int open_mode : 2;
    unsigned int io_mode : 3;
    unsigned int buffer_mode : 2;
    unsigned int file_kind : 3;
    unsigned int file_orientation : 2;
    unsigned int binary_io : 1;
} __file_modes;

typedef struct {
    unsigned int io_state : 3;
    unsigned int free_buffer : 1;
    unsigned char eof;
    unsigned char error;
} __file_state;

typedef void *__ref_con;
typedef int (*__pos_proc)(__file_handle file, fpos_t *position, int mode, __ref_con ref_con);
typedef int (*__io_proc)(__file_handle file, unsigned char *buff, size_t *count,
                         __ref_con ref_con);
typedef int (*__close_proc)(__file_handle file);

typedef struct _FILE {
    __file_handle handle;                   /* 0x00 */
    __file_modes mode;                      /* 0x04 */
    __file_state state;                     /* 0x08 */
    unsigned char is_dynamically_allocated; /* 0x0C */
    unsigned char char_buffer;              /* 0x0D */
    unsigned char char_buffer_overflow;     /* 0x0E */
    unsigned char ungetc_buffer[2];         /* 0x0F */
    wchar_t ungetwc_buffer[2];              /* 0x12 */
    unsigned long position;                 /* 0x18 */
    unsigned char *buffer;                  /* 0x1C */
    unsigned long buffer_size;              /* 0x20 */
    unsigned char *buffer_ptr;              /* 0x24 */
    unsigned long buffer_len;               /* 0x28 */
    unsigned long buffer_alignment;         /* 0x2C */
    unsigned long saved_buffer_len;         /* 0x30 */
    unsigned long buffer_pos;               /* 0x34 */
    __pos_proc position_proc;               /* 0x38 */
    __io_proc read_proc;                    /* 0x3C */
    __io_proc write_proc;                   /* 0x40 */
    __close_proc close_proc;                /* 0x44 */
    __ref_con ref_con;                      /* 0x48 */
    struct _FILE *next_file_struct;         /* 0x4C */
} FILE;

extern FILE __files[];

#define stdin (&__files[0])
#define stdout (&__files[1])
#define stderr (&__files[2])

/* A failed transfer: the stream stays in error until it is cleared, and its buffer is empty. */
#define set_error(file)          \
    do {                         \
        (file)->state.error = 1; \
        (file)->buffer_len = 0;  \
    } while (0)

void __prep_buffer(FILE *file);
int __flush_buffer(FILE *file, size_t *bytes_flushed);
int __flush_all(void);
void __close_all(void);
void __stdio_atexit(void);
int __put_char(int c, FILE *file);
size_t __fwrite(const void *ptr, size_t memb_size, size_t num_memb, FILE *file);
int _fseek(FILE *file, fpos_t offset, int whence);

int fwide(FILE *file, int mode);
int fflush(FILE *file);
int fclose(FILE *file);
int fseek(FILE *file, long offset, int whence);
long ftell(FILE *file);
size_t fwrite(const void *ptr, size_t memb_size, size_t num_memb, FILE *file);

void *memcpy(void *dst, const void *src, size_t n);
void *memset(void *dst, int c, size_t n);
void *memchr(const void *src, int c, size_t n);
void *__memrchr(const void *src, int c, size_t n);
size_t strlen(const char *s);

/* Game and SDK sources still call these library functions by their placeholder symbols, so the
 * library defines them under those names. */
#define calloc fn_803A2C40
#define realloc fn_803A2CC8
#define free fn_803A2D54
#define malloc fn_803A2DC4
#define labs fn_803A7B20
#define bsearch fn_803A7B40
#define puts fn_803A7D24
#define tolower fn_803A8024
#define memcmp fn_803A8DA8
#define vsnprintf fn_803A9350
#define printf fn_803A953C
#define strncpy fn_803ACABC

/* Variable arguments (PowerPC EABI). */
typedef struct __va_list_struct {
    char gpr;
    char fpr;
    char reserved[2];
    char *input_arg_area;
    char *reg_save_area;
} __va_list[1];
typedef __va_list va_list;

void *__va_arg(void *list, int type);

#define va_start(ap, last) ((void)last, __builtin_va_info(&ap))
#define va_arg(ap, type) (*((type *)__va_arg(ap, _var_arg_typeof(type))))
#define va_end(ap) ((void)0)

/* Character classes (ctype.c). */
extern unsigned char __ctype_map[256];
extern unsigned char __lower_map[256];
extern unsigned char __upper_map[256];

#define __control_char 0x01
#define __motion_char 0x02
#define __space_char 0x04
#define __punctuation 0x08
#define __digit 0x10
#define __hex_digit 0x20
#define __lower_case 0x40
#define __upper_case 0x80

#define isdigit(c) (__ctype_map[(unsigned char)(c)] & __digit)
#define isupper(c) (__ctype_map[(unsigned char)(c)] & __upper_case)

char *strcpy(char *dst, const char *src);
char *strchr(const char *s, int c);
size_t wcstombs(char *s, const wchar_t *pwcs, size_t n);

void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);

#endif
