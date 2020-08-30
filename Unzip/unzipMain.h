#pragma once
#include <cstdio>
typedef void* unzFile;
typedef struct tm_unz_s {
	unsigned int tm_sec;            /* seconds after the minute - [0,59] */
	unsigned int tm_min;            /* minutes after the hour - [0,59] */
	unsigned int tm_hour;           /* hours since midnight - [0,23] */
	unsigned int tm_mday;           /* day of the month - [1,31] */
	unsigned int tm_mon;            /* months since January - [0,11] */
	unsigned int tm_year;           /* years - [1980..2044] */
} tm_unz;
typedef struct unz_global_info_s {
	unsigned long number_entry;         /* total number of entries in the central dir on this disk */
	unsigned long size_comment;         /* size of the global comment of the zipfile */
} unz_global_info;
typedef struct unz_file_info_s {
	unsigned long version;              /* version made by                 2 unsigned chars */
	unsigned long version_needed;       /* version needed to extract       2 unsigned chars */
	unsigned long flag;                 /* general purpose bit flag        2 unsigned chars */
	unsigned long compression_method;   /* compression method              2 unsigned chars */
	unsigned long dosDate;              /* last mod file date in Dos fmt   4 unsigned chars */
	unsigned long crc;                  /* crc-32                          4 unsigned chars */
	unsigned long compressed_size;      /* compressed size                 4 unsigned chars */
	unsigned long uncompressed_size;    /* uncompressed size               4 unsigned chars */
	unsigned long size_filename;        /* filename length                 2 unsigned chars */
	unsigned long size_file_extra;      /* extra field length              2 unsigned chars */
	unsigned long size_file_comment;    /* file comment length             2 unsigned chars */
	unsigned long disk_num_start;       /* disk number start               2 unsigned chars */
	unsigned long internal_fa;          /* internal file attributes        2 unsigned chars */
	unsigned long external_fa;          /* external file attributes        4 unsigned chars */
	tm_unz tmu_date;
} unz_file_info;
typedef struct unz_file_info_internal_s {
	unsigned long offset_curfile; /* relative offset of static header 4 unsigned chars */
} unz_file_info_internal;
typedef void* (*alloc_func)(void* opaque, unsigned int items, unsigned int size);
typedef void (*free_func)(void* opaque, void* address);
struct internal_state;
typedef struct z_stream_s {
	unsigned char* next_in;  /* next input unsigned char */
	unsigned int avail_in;      /* number of unsigned chars available at next_in */
	unsigned long total_in;     /* total nb of input unsigned chars read so */
	unsigned char* next_out; /* next output unsigned char should be put there */
	unsigned int avail_out;     /* remaining free space at next_out */
	unsigned long total_out;    /* total nb of unsigned chars output so */
	char* msg;      /* last error message, NULL if no error */
	struct internal_state* state; /* not visible by applications */
	alloc_func zalloc;  /* used to allocate the internal state */
	free_func zfree;    /* used to free the internal state */
	unsigned char* opaque;  /* private data object passed to zalloc and zfree */
	int data_type;      /* best guess about the data type: ascii or binary */
	unsigned long adler;        /* adler32 value of the uncompressed data */
	unsigned long reserved;     /* reserved for future use */
} z_stream;
typedef z_stream* z_streamp;
typedef struct {
	char* read_buffer;         /* internal buffer for compressed data */
	z_stream stream;            /* zLib stream structure for inflate */
	unsigned long pos_in_zipfile;       /* position in unsigned char on the zipfile, for fseek*/
	unsigned long stream_initialised;   /* flag set if stream structure is initialised*/
	unsigned long offset_local_extrafield; /* offset of the static extra field */
	unsigned int size_local_extrafield; /* size of the static extra field */
	unsigned long pos_local_extrafield;   /* position in the static extra field in read*/
	unsigned long crc32;                /* crc32 of all data uncompressed */
	unsigned long crc32_wait;           /* crc32 we must obtain after decompress all */
	unsigned long rest_read_compressed; /* number of unsigned char to be decompressed */
	unsigned long rest_read_uncompressed; /*number of unsigned char to be obtained after decomp*/
	FILE* file;                 /* io structore of the zipfile */
	unsigned long compression_method;   /* compression method (0==store) */
	unsigned long byte_before_the_zipfile; /* unsigned char before the zipfile, (>0 for sfx)*/
} file_in_zip_read_info_s;
typedef struct {
	FILE* file;                 /* io structore of the zipfile */
	unz_global_info gi;       /* public global information */
	unsigned long byte_before_the_zipfile; /* unsigned char before the zipfile, (>0 for sfx)*/
	unsigned long num_file;             /* number of the current file in the zipfile*/
	unsigned long pos_in_central_dir;   /* pos of the current file in the central dir*/
	unsigned long current_file_ok;      /* flag about the usability of the current file*/
	unsigned long central_pos;          /* position of the beginning of the central dir*/
	unsigned long size_central_dir;     /* size of the central directory  */
	unsigned long offset_central_dir;   /* offset of start of central directory with respect to the starting disk number */
	unz_file_info cur_file_info; /* public info about the current file in zip*/
	unz_file_info_internal cur_file_info_internal; /* private info about it*/
	file_in_zip_read_info_s* pfile_in_zip_read; /* structure about the current
												   file if we are decompressing it */
} unz_s;
#define UNZ_OK                                  ( 0 )
#define UNZ_END_OF_LIST_OF_FILE ( -100 )
#define UNZ_ERRNO               ( Z_ERRNO )
#define UNZ_EOF                 ( 0 )
#define UNZ_PARAMERROR                  ( -102 )
#define UNZ_BADZIPFILE                  ( -103 )
#define UNZ_INTERNALERROR               ( -104 )
#define UNZ_CRCERROR                    ( -105 )
#define UNZ_CASESENSITIVE       1
#define UNZ_NOTCASESENSITIVE    2
#define UNZ_OSDEFAULTCASE       0
extern int unzStringFileNameCompare(const char* fileName1, const char* fileName2, int iCaseSensitivity);
extern unzFile unzOpen(const char* path);
extern unzFile unzReOpen(const char* path, unzFile file);
extern int unzClose(unzFile file);
extern int unzGetGlobalInfo(unzFile file, unz_global_info* pglobal_info);
extern int unzGetGlobalComment(unzFile file, char* szComment, unsigned long uSizeBuf);
extern int unzGoToFirstFile(unzFile file);
extern int unzGoToNextFile(unzFile file);
extern int unzLocateFile(unzFile file, const char* szFileName, int iCaseSensitivity);
extern int unzGetCurrentFileInfo(unzFile file, unz_file_info* pfile_info, char* szFileName, unsigned long fileNameBufferSize, void* extraField, unsigned long extraFieldBufferSize, char* szComment, unsigned long commentBufferSize);
extern int unzOpenCurrentFile(unzFile file);
extern int unzCloseCurrentFile(unzFile file);
extern int unzReadCurrentFile(unzFile file, void* buf, unsigned len);
extern long unztell(unzFile file);
extern int unzeof(unzFile file);
extern int unzGetLocalExtrafield(unzFile file, void* buf, unsigned len);
