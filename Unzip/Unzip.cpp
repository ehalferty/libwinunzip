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

CUnzip::CUnzip(std::string filePath) {
    failed = false;
    uf = unzOpen(filePath.c_str());
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
    return;
}

CUnzip::CUnzip(std::shared_ptr<std::vector<uint8_t>> fileData) {
    // TODO: Write to a temp file and re-open. Or maybe make parallel versions of (many) function sin unzipMain.cpp so they'll accept a pseudo-file struct?
    return;
}

std::shared_ptr<std::vector<std::string>> CUnzip::GetFiles() {
    auto results = std::make_shared<std::vector<std::string>>(new std::vector<std::string>());
    char filename_inzip[NAME_MAX];
    unz_file_info file_info;
    for (int i = 0; i < gi.number_entry; i++) {
        int err = unzGetCurrentFileInfo(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
        if (err != UNZ_OK) {
            break;
        }
        results->emplace_back(new std::string(filename_inzip));
        if ((i + 1) < gi.number_entry) {
            err = unzGoToNextFile(uf);
            if (err != UNZ_OK) {
                break;
            }
        }
    }
    return results;
}

bool CUnzip::FileExists(std::string filePath) {
    return false;
}

std::shared_ptr<std::vector<uint8_t>> CUnzip::GetFile(std::string) {
    return std::make_shared<std::vector<uint8_t>>(std::vector<uint8_t>(0));
}

void CUnzip::Close() {
}
