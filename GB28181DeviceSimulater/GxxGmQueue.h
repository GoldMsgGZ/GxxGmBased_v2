#ifndef _GxxGmQueue_H_
#define _GxxGmQueue_H_

#include <queue>

#include "Poco/Mutex.h"
#include "Poco/Event.h"

template <class T>
class GxxGmQueue
{
public:
	GxxGmQueue() {};
	~GxxGmQueue() {};

public:
	// 入队，进队尾
	int push(T element)
	{
		int errCode = 0;

		// 上锁
		rw_lock_.lock();
		elements_.push(element);
		rw_lock_.unlock();

		empty_event_.set();

		return errCode;
	}

	/**
	 * 访问第一个元素
	 * 参数：
	 *	@bolck 是否阻塞访问
	 */
	int front(T **element, bool block = false)
	{
		while (elements_.empty())
		{
			// 阻塞读取，如果队列为空，则一直等下去
			if (block)
			{
				try
				{
					empty_event_.wait(1000);
				}
				catch (Poco::Exception e)
				{
					char errmsg[4096] = {0};
					std::string estr = e.displayText();
					int errCode = e.code();
					sprintf_s(errmsg, 4096, "[阻塞队列] 出现异常。异常信息：%s，异常代码：%d\n", estr.c_str(), errCode);
					OutputDebugStringA(errmsg);
				}
				
			}
			else
				return -1;
		}

		rw_lock_.lock();
		*element = av_packet_clone(&elements_.front());
		rw_lock_.unlock();

		return 0;
	}

	// 第一个元素出队
	int pop()
	{
		int errCode = 0;

		rw_lock_.lock();
		elements_.pop();
		rw_lock_.unlock();

		return errCode;
	}

public:
	std::queue<T> elements_;
	Poco::Mutex rw_lock_;
	Poco::Event empty_event_;
};
#endif
