#ifndef RTPLOT_H
#define RTPLOT_H

#include "inputparserthread.h"

#include <string>
#include <thread>
#include <mutex>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>

#include "Fl_plot.H"

class RTPlot
{
public:
	RTPlot(int argc, char* argv[]);
	~RTPlot();

	void newPoint(int curve, float x, float y);
	void removeFirstPoint(int curve);
	void quit();
	void setXLabel(const std::string& name);
	void setYLabel(const std::string& name);
	void setCurveName(int curve, const std::string& name);
	void refresh();
	void autoRefresh(bool enable, uint period_ms = 0);
	void setXRange(float min, float max);
	void setYRange(float min, float max);
	void autoXRange();
	void autoYRange();

private:
	Fl_Window* window_;

	InputParserThread* parser_;
	std::thread auto_refresh_thread_;
	std::mutex auto_refresh_period_lock_;
	unsigned int auto_refresh_period_;
	Fl_Plot* plot_;
	bool auto_x_range, auto_y_range;
	uint color_index;
};

#endif // RTPLOT_H
