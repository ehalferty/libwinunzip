// Unzip.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "Unzip.h"


// This is an example of an exported variable
UNZIP_API int nUnzip=0;

// This is an example of an exported function.
UNZIP_API int fnUnzip(void)
{
    return 0;
}

CUnzip::CUnzip(std::string_view filePath) {
    failed = false;
    uf = unzOpen(filePath.data());
    if (uf == NULL) {
        failed = true;
    } else {
        int err = unzGetGlobalInfo(uf, &gi);
        if (err != UNZ_OK) {
            failed = true;
        } else {
            unzGoToFirstFile(uf);
        }
    }
}

CUnzip::CUnzip(std::vector<uint8_t> const &fileData) {
    // TODO: Write to a temp file and re-open. Or maybe make parallel versions of (many) function sin unzipMain.cpp so they'll accept a pseudo-file struct?
}

std::vector<std::string> CUnzip::GetFiles() {
    auto results = std::vector<std::string>();
    char filename_inzip[NAME_MAX];
    unz_file_info file_info;
    for (unsigned int i = 0; i < gi.number_entry; i++) {
        int err = unzGetCurrentFileInfo(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
        if (err != UNZ_OK) {
            break;
        }
        results.emplace_back(filename_inzip);
        if ((i + 1) < gi.number_entry) {
            err = unzGoToNextFile(uf);
            if (err != UNZ_OK) {
                break;
            }
        }
    }
    return results;
}

bool CUnzip::FileExists(std::string_view filePath) {
    return (unzLocateFile(uf, filePath.data(), isCaseSensitive) == UNZ_OK);
}

std::vector<uint8_t> CUnzip::GetFile(std::string_view filePath) {
    if (!FileExists(filePath)) {
        failed = true;
        return std::vector<uint8_t>();
    }
    if (unzOpenCurrentFile(uf) != UNZ_OK) {
        failed = true;
        return std::vector<uint8_t>();
    }
    unz_file_info file_info;
    char filename_inzip[NAME_MAX];
    if (unzGetCurrentFileInfo(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0) != UNZ_OK) {
        failed = true;
        return std::vector<uint8_t>();
    }
    auto buffer = static_cast<uint8_t*>(HeapAlloc(GetProcessHeap(), 0, sizeof(uint8_t) * file_info.uncompressed_size));
    int bytesRead = unzReadCurrentFile(uf, buffer, file_info.uncompressed_size);
    return std::vector(&buffer[0], &buffer[file_info.uncompressed_size]);
}

CUnzip::~CUnzip() {
    // TODO: Close file handle, free zlib memory or whatever lolz
}
