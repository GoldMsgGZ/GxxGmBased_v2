#ifndef _bgMfcExcelModule_H_
#define _bgMfcExcelModule_H_

#include <vector>
#include <string>

#include "CWorkbook.h"		// 管理单个工作表
#include "CWorkbooks.h"		// 统管所有的工作簿
#include "CApplication.h"	// Excel应用程序类,管理我们打开的这整个Excel应用
#include "CRange.h"			// 区域类，对EXcel的大部分操作都要和这个打招呼
#include "CWorksheet.h"		// 工作薄中的单个工作表
#include "CWorksheets.h"	// 统管当前工作簿中的所有工作表

class bgMfcExcelModule
{
public:
	bgMfcExcelModule();
	~bgMfcExcelModule();

public:
	int WriteInitialize(const wchar_t *xls_path, const wchar_t *sheet_name);
	int WriteHeader(std::vector<std::wstring> headers);
	int WriteLine(std::vector<std::wstring> rows);
	void WriteFinish();

public:
	int ReadInitialize(const wchar_t *xls_path, const wchar_t *sheet_name);
	int ReadHeader(std::vector<std::wstring> &headers);
	int ReadNextLine(std::vector<std::wstring> &rows);
	void ReadFinish();

private:
	std::wstring xls_path_;
	std::wstring sheet_name_;

private:
	CApplication app1;
	CWorkbooks books;
	CWorkbook book;
	CWorksheets sheets;
	CWorksheet sheet;
	CRange range;
	CRange iCell;
	LPDISPATCH lpDisp;

	long rows_;
	long cols_;
	long current_row_;
};

#endif//_bgMfcExcelModule_H_
