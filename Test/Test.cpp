// Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include "../Unzip/Unzip.h"

#include "Shlwapi.h"

#pragma comment( lib, "Shlwapi" )

//#define BUFFLEN 65536

int main() {
    //auto buff = reinterpret_cast<TCHAR *>(HeapAlloc(GetProcessHeap(), 0, sizeof(TCHAR) * BUFFLEN));
    //DWORD success2 = GetCurrentDirectory(BUFFLEN, buff);
    std::string path = "..\\vader.zip";
    bool exists = PathFileExistsA(path.data());
    auto zip = new CUnzip(path);
    auto fileNames = zip->GetFiles();
    //bool success = SetDllDirectory(TEXT("..\\Debug\\"));
    //HINSTANCE dll = LoadLibrary(TEXT("Unzip.dll"));
    //DWORD x = GetLastError();
    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
