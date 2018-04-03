#pragma once

#include <string>
#include <memory>

class RTPlot
{
public:
	RTPlot(int argc, char* argv[], bool start_stdin_parser = false);
	RTPlot(bool start_stdin_parser = false);
	~RTPlot();

	void setGridSize(size_t rows, size_t cols);
	void newPoint(size_t plot, int curve, float x, float y);
	void removeFirstPoint(size_t plot, int curve);
	void quit();
	void setXLabel(size_t plot, const std::string& name);
	void setYLabel(size_t plot, const std::string& name);
	void setCurveName(size_t plot, int curve, const std::string& name);
	void setPlotName(size_t plot, const std::string& name);
	void refresh();
	void autoRefresh(bool enable, uint period_ms = 0);
	void setXRange(size_t plot, float min, float max);
	void setYRange(size_t plot, float min, float max);
	void autoXRange(size_t plot);
	void autoYRange(size_t plot);
	void setMaxPoints(size_t plot, size_t count);

	void run();
	void check();

private:
	void create(bool start_stdin_parser);
	void checkPlot(size_t plot);
	void updateLayout();

	struct rtplot_members;
	std::unique_ptr<rtplot_members> impl_;
};
