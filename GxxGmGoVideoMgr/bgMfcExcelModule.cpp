#include "stdafx.h"
#include "bgMfcExcelModule.h"

// 参考页面：https://blog.csdn.net/cai_niaocainiao/article/details/81806928
// 参考页面：https://www.cnblogs.com/tianya2543/p/4165997.html
// 参考页面：https://www.cnblogs.com/lvdongjie/p/4614760.html

bgMfcExcelModule::bgMfcExcelModule()
: rows_(0)
, cols_(0)
, current_row_(0)
{
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		// 初始化失败
	}
}

bgMfcExcelModule::~bgMfcExcelModule()
{
	// 
	CoUninitialize();
}

int bgMfcExcelModule::WriteInitialize(const wchar_t *xls_path, const wchar_t *sheet_name)
{
	int errCode = 0;

	xls_path_ = xls_path;
	std::wstring xls_tmp_path = xls_path_ + L".tmp";
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	BOOL bret = app1.CreateDispatch(_T("Excel.Application"), NULL);
	if (!bret)
	{
		return -1;
	}

	books.AttachDispatch(app1.get_Workbooks());

	// 在写入的逻辑里面，我们不考虑打开已有文件
	// 我们的所有做法都是创建一个临时文件，写入后覆盖掉原有文件
	//lpDisp = books.Open(xls_tmp_path.c_str(), covOptional, covOptional,
	//	covOptional, covOptional, covOptional, covOptional, covOptional,
	//	covOptional, covOptional, covOptional, covOptional, covOptional,
	//	covOptional, covOptional);
	//book.AttachDispatch(lpDisp); 

	book = books.Add(covOptional);

	//加载Sheet页面 
	sheets.AttachDispatch(book.get_Sheets(),TRUE);

	// 添加新的Sheet页面
	sheets.Add(vtMissing, vtMissing, _variant_t((long)1), vtMissing);

	sheet.AttachDispatch(sheets.get_Item(_variant_t((long)1)), TRUE);

	////得到当前活跃sheet 
	////如果有单元格正处于编辑状态中，此操作不能返回，会一直等待 
	//lpDisp = book.get_ActiveSheet();
	//sheet.AttachDispatch(lpDisp);

	// 设置sheet名称？
	sheet.put_Name(sheet_name);
	current_row_ = 0;

	// 加载所有单元格
	range.AttachDispatch(sheet.get_Cells(), TRUE);

	return errCode;
}

int bgMfcExcelModule::WriteHeader(std::vector<std::wstring> headers)
{
	int errCode = 0;

	current_row_ = 0;
	errCode = WriteLine(headers);

	return errCode;
}

int bgMfcExcelModule::WriteLine(std::vector<std::wstring> rows)
{
	int errCode = 0;

	++current_row_;
	COleVariant vResult;
	for (long col_index = 1; col_index <= rows.size(); ++col_index)
	{
		// 以字符串形式写入
		std::wstring cell_value = L"\'" + rows[col_index - 1];
		range.put_Item(COleVariant(current_row_), COleVariant(col_index), COleVariant(cell_value.c_str()));

		// 设置当前单元格格式为文本型，并且调整单元格自适应
	}

	return errCode;
}

void bgMfcExcelModule::WriteFinish()
{
	// 保存文件，成功后改名
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	VARIANT path;
	path.vt = VT_BSTR;
	path.bstrVal = _bstr_t(xls_path_.c_str()); 

	//book.SaveAs(path, covOptional, covOptional,
	//	covOptional, covOptional, covOptional, 0,
	//	covOptional, covOptional, covOptional, covOptional,
	//	covOptional, covOptional);

	book._SaveAs(path, covOptional, covOptional,
		covOptional, covOptional, covOptional, 0,
		covOptional, covOptional, covOptional, covOptional,
		covOptional);

	
	book.put_Saved(TRUE);
	app1.put_UserControl(FALSE);
	books.Close();
	app1.Quit();//ExcelApp,是_Applacation对象，以上几个对象同理。

	range.ReleaseDispatch();
	sheet.ReleaseDispatch();
	sheets.ReleaseDispatch();
	book.ReleaseDispatch();
	books.ReleaseDispatch();
	app1.ReleaseDispatch();
}

int bgMfcExcelModule::ReadInitialize(const wchar_t *xls_path, const wchar_t *sheet_name)
{
	int errCode = 0;

	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	BOOL bret = app1.CreateDispatch(_T("Excel.Application"), NULL);
	if (!bret)
	{
		return -1;
	}

	books.AttachDispatch(app1.get_Workbooks());
	lpDisp = books.Open(xls_path, covOptional, covOptional,
		covOptional, covOptional, covOptional, covOptional, covOptional, 
		covOptional, covOptional, covOptional, covOptional, covOptional,
		covOptional, covOptional);

	//得到Workbook    
	book.AttachDispatch(lpDisp);
	//得到Worksheets   
	sheets.AttachDispatch(book.get_Worksheets());
	//sheet = sheets.get_Item(COleVariant((short)1));
	//得到当前活跃sheet 
	//如果有单元格正处于编辑状态中，此操作不能返回，会一直等待 
	lpDisp = book.get_ActiveSheet();
	sheet.AttachDispatch(lpDisp);

	// 得到已使用的行列数
	CRange usedRange;
	usedRange.AttachDispatch(sheet.get_UsedRange());

	//已经使用的行数
	range.AttachDispatch(usedRange.get_Rows());
	rows_ = range.get_Count();                   

	//已经使用的列数
	range.AttachDispatch(usedRange.get_Columns());
	cols_ = range.get_Count(); 

	current_row_ = 0;

	return errCode;
}

int bgMfcExcelModule::ReadHeader(std::vector<std::wstring> &headers)
{
	int errCode = 0;

	//读取第一个单元格的值 
	COleVariant vResult;
	current_row_ = 0;
	errCode = ReadNextLine(headers);
// 	for (long col_index = 1; col_index <= cols_; ++col_index)
// 	{
// 		range.AttachDispatch(sheet.get_Cells());
// 		range.AttachDispatch(range.get_Item(COleVariant(current_row_), COleVariant(col_index)).pdispVal);   //第一变量是行，第二个变量是列，即第二行第一列
// 		vResult = range.get_Value2();
// 
// 		CString str;
// 		if (vResult.vt == VT_BSTR) //字符串  
// 		{
// 			str = vResult.bstrVal;
// 		}
// 		else if (vResult.vt == VT_R8) //8字节的数字  
// 		{
// 			str.Format(_T("%f"), vResult.dblVal);
// 		}
// 		else if (vResult.vt == VT_NULL)
// 		{
// 			str = L"";
// 		}
// 
// 		headers.push_back(str.GetString());
// 	}

	return errCode;
}

int bgMfcExcelModule::ReadNextLine(std::vector<std::wstring> &rows)
{
	int errCode = 0;

	++current_row_;
	if (current_row_ > rows_)
	{
		// 已经读取完毕
		return -1;
	}

	COleVariant vResult;
	for (long col_index = 1; col_index <= cols_; ++col_index)
	{
		range.AttachDispatch(sheet.get_Cells());
		range.AttachDispatch(range.get_Item(COleVariant(current_row_), COleVariant(col_index)).pdispVal);   //第一变量是行，第二个变量是列，即第二行第一列
		vResult = range.get_Value2();

		CString str;
		if (vResult.vt == VT_BSTR) //字符串  
		{
			str = vResult.bstrVal;
		}
		else if (vResult.vt == VT_R8) //8字节的数字  
		{
			str.Format(_T("%f"), vResult.dblVal);
		}
		else if (vResult.vt == VT_NULL)
		{
			str = L"";
		}

		rows.push_back(str.GetString());
	}

	return errCode;
}

void bgMfcExcelModule::ReadFinish()
{
	book.put_Saved(TRUE);
	app1.put_UserControl(FALSE);
	books.Close();
	app1.Quit();//ExcelApp,是_Applacation对象，以上几个对象同理。

	range.ReleaseDispatch();
	sheet.ReleaseDispatch();
	sheets.ReleaseDispatch();
	book.ReleaseDispatch();
	books.ReleaseDispatch();
	app1.ReleaseDispatch();
}