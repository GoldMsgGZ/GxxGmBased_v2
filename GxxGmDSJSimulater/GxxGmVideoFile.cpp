#include "GxxGmVideoFile.h"

#define MIN_LEN_TO_PRINT 1024

GxxGmVideoFile::GxxGmVideoFile()
{
	memset(m_videoInfo, 0, 1024);
	m_struGSAVFrameVector.clear();
}

GxxGmVideoFile::~GxxGmVideoFile()
{
	std::vector<StruGSAVFrame>::iterator iter = m_struGSAVFrameVector.begin();
	while (iter != m_struGSAVFrameVector.end())
	{
		if (iter->pMediaData)
			delete [] iter->pMediaData;
		
		++iter;
	}

	m_struGSAVFrameVector.clear();
}

void GxxGmVideoFile::ReadVideoFile(std::string &file_path, int first_frame, int first_number)
{
	m_FilePath = file_path;

	GS_FILEHANDLE fileHandle = NULL;
	int errCode = GMF_OpenFile(&fileHandle, file_path.c_str(), eOPEN_READ);
	if (errCode != 0)
	{
		return ;
	}

	m_videoFrameRate = 0;
	StruGSAVFrame struFram;
	unsigned char *frameData = NULL;
	int ret = 0;
	int actualFirstFrame = 0;
	int actualFrameNumber = 0;
	int videoFrameRate = 0;

	for (int i = 0, j = 0; i < first_number; )
	{
		errCode = GMF_ReadFrame(fileHandle, &struFram);
		if (errCode != 0)
			break;

		// 是视频帧
		if (struFram.eMediaType == EnumGSMediaType::GS_MEDIA_TYPE_VIDEO)
		{
			if (j > first_frame)
			{
				if (j == first_frame)
				{
					// 找到非关键帧，定位到前面的关键帧，若视频没有帧索引，则定位到后面的关键帧，读取frame_number帧
					if (struFram.bKeyFrame == 0)
					{
						int index_list_number = 0;
						StruIFrameIndexV2 *frame_index = NULL;

						errCode = GMF_GetIFrameIndexList(fileHandle, &index_list_number, &frame_index);
						if (frame_index != NULL)
							GMF_ReleaseIFrameIndexList(frame_index);

						if (index_list_number == 0)
						{
							if (actualFirstFrame < first_frame)
								actualFirstFrame = first_frame;

							actualFirstFrame++;
							continue;
						}

						errCode = GMF_SeekPreviousIFrame(fileHandle);
						if(ret != 0)
						{
							if(actualFirstFrame < first_frame)
								actualFirstFrame = first_frame;

							actualFirstFrame++;
						}
						else 
							j++;

						continue;
					}
					else
					{
						if(actualFirstFrame <= first_frame)
							actualFirstFrame = first_frame;
					}
				}
				i++;
				actualFrameNumber++;
			}
			else
			{
				if(struFram.bKeyFrame == 1)
					actualFirstFrame = j;
			}

			j++;
		}
		videoFrameRate++;
		
		if(struFram.iTimestamp == 1 && m_videoFrameRate == 0 && struFram.eMediaType == GS_MEDIA_TYPE_VIDEO)
			m_videoFrameRate = videoFrameRate;

		if(j >= first_frame && i != 0)
		{
			frameData = new unsigned char[struFram.iMediaDataLen];
			memcpy((char*)frameData, (char*)struFram.pMediaData, struFram.iMediaDataLen);
			struFram.pMediaData = frameData;

			//PrameFrameData(struFram);

			m_struGSAVFrameVector.push_back(struFram);
		}
	}

	GMF_Close(fileHandle);
}

int GxxGmVideoFile::GetVideoFrameRate()
{
	return m_videoFrameRate;
}

char* GxxGmVideoFile::GetVideoInfo()
{
	return m_videoInfo;
}

std::string GxxGmVideoFile::GetFilePath()
{
	return m_FilePath;
}

bool GxxGmVideoFile::IsFileEmpty()
{
	std::vector<StruGSAVFrame>::iterator pos = m_struGSAVFrameVector.begin();
	if (pos == m_struGSAVFrameVector.end())
		return true;
	
	return false;
}

void GxxGmVideoFile::PrameFrameData(const StruGSAVFrame & stFrame)
{

}

std::vector<StruGSAVFrame>::iterator GxxGmVideoFile::LastFrame()
{
	return m_struGSAVFrameVector.end();
}

std::vector<StruGSAVFrame>::iterator GxxGmVideoFile::FirstFrame()
{
	return m_struGSAVFrameVector.begin();
}