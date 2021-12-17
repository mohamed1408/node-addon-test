#include <iostream>
#include <Windows.h>
#include <vector>
#include <string>
#include <Winspool.h>

#pragma comment(lib, "winspool.lib")

bool getListPrinters(std::vector<std::string>&);

struct PrinterHandle
{
	PrinterHandle(LPWSTR iPrinterName)
	{
		_ok = OpenPrinterW(iPrinterName, &_printer, NULL);
	}
	~PrinterHandle()
	{
		if (_ok)
		{
			ClosePrinter(_printer);
		}
	}
	operator HANDLE() { return _printer; }
	operator bool() { return (!!_ok); }
	HANDLE& operator *() { return _printer; }
	HANDLE* operator ->() { return &_printer; }
	const HANDLE& operator ->() const { return _printer; }
	HANDLE _printer = INVALID_HANDLE_VALUE;
	BOOL _ok;
};


int main() {
	HANDLE h = INVALID_HANDLE_VALUE;
	DWORD bytes_w = 0L;
	DOC_INFO_1W doc{};
	char docname[] = "";
	char doctype[] = "text";
	char docout[] = "";
	std::vector<std::string> printerList;
	bool getP = getListPrinters(printerList);
	if (!getP) {
		std::cout << "Windows Error" << GetLastError() << std::endl; //something wrong with program or the OS
		system("pause");
		return 0;
	}
	std::cout << "Printer list: " << std::endl;
	int i = 0;
	for (auto p : printerList) {
		std::cout << i << "\t" << p << std::endl;
		i++;
	}

	int userInp;
	do {
		std::cout << "Enter printer number: ";
		std::cin >> userInp;
	} while (userInp >= printerList.size());

	// bool openPrinter = OpenPrinterW((LPSTR)printerList[userInp].c_str(), &h, NULL); //Open The Printer
	PrinterHandle printerHandle((LPWSTR)(*printerList[userInp].c_str()));
	if (!printerHandle) {
		std::cout << "Error opening printer!" << std::endl;
		system("pause");
		return 0;
	}
	char printThisText[] = "Hello there, my name is Cipher!";

	doc.pDocName = (LPWSTR)(*docname);
	doc.pDatatype = (LPWSTR)(*doctype);
	doc.pOutputFile = NULL;
	StartDocPrinterW(*printerHandle, 1, (LPBYTE)&doc);
	StartPagePrinter(*printerHandle);
	WritePrinter(*printerHandle, (LPVOID)printThisText, (DWORD)strlen(printThisText), (LPDWORD)&bytes_w);
	EndPagePrinter(*printerHandle);
	EndDocPrinter(*printerHandle);
	ClosePrinter(*printerHandle);
	system("pause");
	return 0;
}


bool getListPrinters(std::vector<std::string>& printerList) {
	PRINTER_INFO_5* printer = nullptr;
	DWORD sizeBuff = 0;
	DWORD entries = 0; // total printer installed on computer
	EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 5, NULL, NULL, &sizeBuff, &entries);
	std::unique_ptr<BYTE[]>ptrBuffer(new BYTE[sizeBuff]);
	EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 5, ptrBuffer.get(), sizeBuff, &sizeBuff, &entries);
	if (!entries) {
		return false;
	}
	else {
		printer = (PRINTER_INFO_5*)ptrBuffer.get();
		for (UINT i = 0; i < entries; i++) {
			printerList.push_back(printer->pPrinterName);
			printer++; // increment to next printer
		}
	}
	return true;
}