#ifndef _GxxGmExcelHandler_H_
#define _GxxGmExcelHandler_H_

#include <string>
#include <vector>

#pragma region Import the type libraries

#import "libid:2DF8D04C-5BFA-101B-BDE5-00AA0044DE52" \
	rename("RGB", "MSORGB") \
	rename("DocumentProperties", "MSODocumentProperties")
// 或者
//#import "C:\\Program Files\\Common Files\\Microsoft Shared\\OFFICE12\\MSO.DLL" \
//    rename("RGB", "MSORGB") \
//    rename("DocumentProperties", "MSODocumentProperties")

using namespace Office;

#import "libid:0002E157-0000-0000-C000-000000000046"
// 或者
//#import "C:\\Program Files\\Common Files\\Microsoft Shared\\VBA\\VBA6\\VBE6EXT.OLB"

using namespace VBIDE;

#import "libid:00020813-0000-0000-C000-000000000046" \
	rename("DialogBox", "ExcelDialogBox") \
	rename("RGB", "ExcelRGB") \
	rename("CopyFile", "ExcelCopyFile") \
	rename("ReplaceText", "ExcelReplaceText") \
	no_auto_exclude
// 或者
//#import "C:\\Program Files\\Microsoft Office\\Office12\\EXCEL.EXE" \
//    rename("DialogBox", "ExcelDialogBox") \
//    rename("RGB", "ExcelRGB") \
//    rename("CopyFile", "ExcelCopyFile") \
//    rename("ReplaceText", "ExcelReplaceText") \
//    no_auto_exclude

#pragma endregion

class GxxGmExcelHandler
{
public:
	GxxGmExcelHandler();
	~GxxGmExcelHandler();



public:
	//////////////////////////////////////////////////////////////////////////
	//
	// 创建EXCEL
	//
	//////////////////////////////////////////////////////////////////////////

public:
	//////////////////////////////////////////////////////////////////////////
	//
	// 写入EXCEL
	//
	//////////////////////////////////////////////////////////////////////////
	int InitializeWrite(const char * xls_path);
	int WriteHeader(const char *sheet_name, std::vector<std::string> headers);
	int WriteLine(std::vector<std::string> item);
	void FinishWrite();

public:
	//////////////////////////////////////////////////////////////////////////
	//
	// 读取EXCEL
	//
	//////////////////////////////////////////////////////////////////////////
	int InitializeRead(const char * xls_path);
	int ReadLine(std::vector<std::string> &item);
	int FinishRead();

private:
	int InitializeExcel();
	void CleanupExcel();

	HRESULT AutoWrap(int autoType, VARIANT *pvResult, IDispatch *pDisp,	LPOLESTR ptName, int cArgs...);
	HRESULT SafeArrayPutName(SAFEARRAY* psa, long index, int args...);

private:
	Excel::_ApplicationPtr spXlApp;
	//IDispatch *pXlApp;
	std::string file_path;
};

#endif//_GxxGmExcelHandler_H_
