#ifndef _GxxGmExcelHandler_H_
#define _GxxGmExcelHandler_H_

// 我们这里采用COM组件来进行处理

class GxxGmExcelHandler
{
public:
	GxxGmExcelHandler();
	~GxxGmExcelHandler();

public:
	int Initialize();

private:
	HRESULT AutoWrap(int autoType, VARIANT *pvResult, IDispatch *pDisp,	LPOLESTR ptName, int cArgs...);
};

#endif//_GxxGmExcelHandler_H_
