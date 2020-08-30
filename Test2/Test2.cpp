// This test project is set up to include unzip as a DLL (lib file (make sure it's x64/debug if this is x64!) output from unzip included in this project)

#include <iostream>
#include <windows.h>
#include <gdiplus.h>
#include "../Unzip/Unzip.h"
#include "imagehlp.h"

//#include "Shlwapi.h"

//#pragma comment( lib, "Shlwapi" )
#pragma comment (lib, "Gdiplus")
#pragma comment (lib, "Imagehlp")

//#define BUFFLEN 65536

#define MAIN_WINDOW_CLASS_NAME L"TEST_WINDOW"

void ListDLLFunctions(std::string sADllName, std::vector<std::string>& slListOfDllFunctions) {
    DWORD* dNameRVAs(0);
    _IMAGE_EXPORT_DIRECTORY* ImageExportDirectory;
    unsigned long cDirSize;
    _LOADED_IMAGE LoadedImage;
    std::string sName;
    slListOfDllFunctions.clear();
    if (MapAndLoad(sADllName.c_str(), NULL, &LoadedImage, TRUE, TRUE)) {
        ImageExportDirectory = (_IMAGE_EXPORT_DIRECTORY*)
            ImageDirectoryEntryToData(LoadedImage.MappedAddress,
                false, IMAGE_DIRECTORY_ENTRY_EXPORT, &cDirSize);
        if (ImageExportDirectory != NULL) {
            dNameRVAs = (DWORD*)ImageRvaToVa(LoadedImage.FileHeader,
                LoadedImage.MappedAddress,
                ImageExportDirectory->AddressOfNames, NULL);
            for (size_t i = 0; i < ImageExportDirectory->NumberOfNames; i++) {
                sName = (char*)ImageRvaToVa(LoadedImage.FileHeader,
                    LoadedImage.MappedAddress,
                    dNameRVAs[i], NULL);
                slListOfDllFunctions.push_back(sName);
            }
        }
        UnMapAndLoad(&LoadedImage);
    }
}
HBITMAP bitmap;
int bitmapW;
int bitmapH;
HDC drawnBitmap = nullptr;
Gdiplus::Bitmap* pBitmap = NULL;
IStream* pStream = NULL;

void DrawHBITMAPtoHDC(HBITMAP hBitmap, HDC hdc) {
    BITMAP bm;
    HDC MemDCExercising = CreateCompatibleDC(hdc);
    HBITMAP oldbitmap = (HBITMAP)SelectObject(MemDCExercising, hBitmap);
    GetObject(hBitmap, sizeof(bm), &bm);
    BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, MemDCExercising, 0, 0, SRCCOPY);
    SelectObject(MemDCExercising, oldbitmap);
    DeleteDC(MemDCExercising);
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    RECT rt;
    PAINTSTRUCT ps;
    HDC hdc;
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
    {
        GetWindowRect(hwnd, &rt);
        hdc = BeginPaint(hwnd, &ps);
        //if (drawnBitmap == nullptr) {
        //    DrawHBITMAPtoHDC(bitmap, drawnBitmap);
        //}
        HBITMAP hBMP = (HBITMAP)LoadImage(NULL, L"C:\\Users\\edwar\\OneDrive\\Desktop\\vader.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        //DrawHBITMAPtoHDC(hBMP, hdc);
        DrawHBITMAPtoHDC(bitmap, hdc);
        //StretchBlt(hdc, 0, 0, rt.right, rt.bottom, drawnBitmap, 0, 0, bitmapW, bitmapH, SRCCOPY);
        //FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
    }
    return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main() {
    bool success = SetDllDirectory(TEXT("..\\Debug\\"));
    auto results = std::vector<std::string>();
    ListDLLFunctions("Unzip.dll", results);

    for (auto& functionName : results) {
        std::cout << "FUNCTION: \"" << functionName << "\"" << std::endl;
    }


    //HINSTANCE dll = LoadLibrary(TEXT("Unzip.dll"));
    std::cout << "DONE" << std::endl;
    //auto buff = static_cast<TCHAR *>(HeapAlloc(GetProcessHeap(), 0, sizeof(TCHAR) * BUFFLEN));
    //DWORD success2 = GetCurrentDirectory(BUFFLEN, buff);
    std::string path = "..\\vader.zip";
    //bool exists = PathFileExistsA(path.data());
    auto zip = new CUnzip(path);
    auto firstFileName = zip->GetFiles().front();
    std::cout << "First file name: " << firstFileName << std::endl;
    auto fileExists = zip->FileExists(firstFileName);
    std::cout << "File exists: " << fileExists << std::endl;
    auto fileData = zip->GetFile(firstFileName);

    HANDLE f = CreateFile(L"vader_out.bmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD actualBytesWritten;
    WriteFile(f, fileData.data(), fileData.size(), &actualBytesWritten, NULL);
    /*bitmapW = (int)(fileData[18] | fileData[19] << 8);
    bitmapH = (int)(fileData[22] | fileData[23] << 8);
    auto nPlanes = (int)(fileData[26] | fileData[27] << 8);
    auto nBpp = (int)(fileData[28] | fileData[29] << 8);*/
    //bitmap = CreateBitmap(bitmapW, bitmapH, nPlanes, nBpp, fileData.data());
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    // Initialize GDI+.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    HRESULT hResult = ::CreateStreamOnHGlobal(NULL, TRUE, &pStream);
    if (hResult == S_OK && pStream) {
        hResult = pStream->Write(&fileData.data()[0], ULONG(fileData.size()), NULL);
        if (hResult == S_OK)
            pBitmap = Gdiplus::Bitmap::FromStream(pStream);
        pStream->Release();
    }
    pBitmap->GetHBITMAP(0, &bitmap);

    HINSTANCE inst = GetModuleHandle(NULL);
    WNDCLASSEX mainWindowClass = { 0 };
    mainWindowClass.cbSize = sizeof(WNDCLASSEX);
    mainWindowClass.style = 0;
    mainWindowClass.lpfnWndProc = MainWndProc;
    mainWindowClass.hInstance = inst;
    mainWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    mainWindowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    mainWindowClass.lpszMenuName = NULL;
    mainWindowClass.lpszClassName = MAIN_WINDOW_CLASS_NAME;
    RegisterClassEx(&mainWindowClass);
    HWND hwnd = CreateWindowEx(0, MAIN_WINDOW_CLASS_NAME, L"Hello, zipfile contents", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, inst, NULL);
    ShowWindow(hwnd, SW_SHOW);
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    //bool success = SetDllDirectory(TEXT("..\\Debug\\"));
    //HINSTANCE dll = LoadLibrary(TEXT("Unzip.dll"));
    //DWORD x = GetLastError();
    //std::cout << "Hello World!\n";
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
