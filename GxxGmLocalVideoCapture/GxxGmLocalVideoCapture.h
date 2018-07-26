#ifndef _GxxGmLocalVideoCapture_H_
#define _GxxGmLocalVideoCapture_H_

class GxxGmLocalVideoCapture
{
public:
	GxxGmLocalVideoCapture();
	~GxxGmLocalVideoCapture();

public:
	int EnumVideoDevices();

public:
	int StartCapture(int video_index);
	int StopCapture();
};

#endif//_GxxGmLocalVideoCapture_H_
