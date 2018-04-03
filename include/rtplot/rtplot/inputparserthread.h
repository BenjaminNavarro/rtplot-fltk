#pragma once

class RTPlot;

#include <thread>

class InputParserThread
{
public:
	InputParserThread(RTPlot* mw);
	~InputParserThread();

	void run();

	void stop();

	void join();

private:
	int peek_stdin(unsigned int secs);
	bool stop_;
	RTPlot* mw_;
	std::thread th_;

	void process();
};
