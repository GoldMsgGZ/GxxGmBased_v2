#ifndef _GxxGmVideoFile_H_
#define _GxxGmVideoFile_H_

#include <string>
#include <vector>
#include "GSAVFileDef.h"
#include "GMFLib.h"

class GxxGmVideoFile
{
public:
	GxxGmVideoFile();
	~GxxGmVideoFile();

public:
	void ReadVideoFile(std::string &file_path, int first_frame, int first_number);
	int GetVideoFrameRate();
	char* GetVideoInfo();
	std::string GetFilePath();
	bool IsFileEmpty();
	void PrameFrameData(const StruGSAVFrame & stFrame);

public:
	std::vector<StruGSAVFrame>::iterator LastFrame();
	std::vector<StruGSAVFrame>::iterator FirstFrame();

private:
	std::vector<StruGSAVFrame> m_struGSAVFrameVector;
	std::string m_FilePath;
	int m_videoFrameRate;
	char m_videoInfo[1024];
};

#endif//_GxxGmVideoFile_H_
