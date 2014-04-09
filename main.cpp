#include <Windows.h>
#include <ShObjIdl.h>
#include <ShlObj.h>
#include <string>
#include <iostream>
#include <fstream>

int main(int argc, char **argv)
{
	HRESULT hr;

	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if(!SUCCEEDED(hr)) return 1;
	
	IFileSaveDialog *pfd = NULL;
	hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void **>(&pfd));
	if(!SUCCEEDED(hr)) return 1;

	IShellItem *psi;
	hr = SHGetKnownFolderItem(FOLDERID_Libraries, KF_FLAG_DEFAULT, NULL, IID_PPV_ARGS(&psi));
	if(!SUCCEEDED(hr)) return 1;

	hr = pfd->SetDefaultFolder(psi);
	if(!SUCCEEDED(hr)) return 1;

	hr = pfd->Show(NULL);
	if(!SUCCEEDED(hr)) return 1;

	IShellItem *pItem;
	hr = pfd->GetResult(&pItem);
	if(!SUCCEEDED(hr)) return 1;

	PWSTR pszPath;
	hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
	if(!SUCCEEDED(hr)) return 1;

	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	std::ofstream out;
	out.open(pszPath, std::ios::out | std::ios::trunc | std::ios::binary);

	DWORD bytesRead;
	do
	{
		char buf[1024];
		ReadFile(hIn, buf, 1023, &bytesRead, NULL);
		buf[bytesRead] = 0;
		out.write(buf, bytesRead);
	} while(bytesRead != 0);

	out.close();

	CoTaskMemFree(pszPath);
	psi->Release();
	pItem->Release();
	pfd->Release();
	CoUninitialize();

	return 0;
}