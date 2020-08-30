// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the UNZIP_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// UNZIP_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#include <memory>
#include <string>
#include <vector>
#include "unzipMain.h"
#ifdef UNZIP_EXPORTS
#define UNZIP_API __declspec(dllexport)
#else
#define UNZIP_API __declspec(dllimport)
#endif

#define NAME_MAX 255

// This class is exported from the dll
class UNZIP_API CUnzip {
public:
	bool failed;
	unzFile uf;
	unz_global_info gi;
	CUnzip(std::string_view);
	CUnzip(std::vector<uint8_t> const &);
	~CUnzip();
	std::vector<std::string> GetFiles();
	bool FileExists(std::string_view);
	std::vector<uint8_t> GetFile(std::string_view);
};

extern UNZIP_API int nUnzip;

UNZIP_API int fnUnzip(void);
