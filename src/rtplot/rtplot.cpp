#include "rtplot.h"

#include <iostream>
#include <chrono>

using namespace std;

const int _plot_x_offset = 70;
const int _plot_y_offset = 15;
const int _plot_x_size = 555;
const int _plot_y_size = 380;

RTPlot::RTPlot() :
	auto_refresh_period_(0),
	auto_x_range(true),
	auto_y_range(true)
{
	create();
	window_->size_range(655, 450, 0, 0, 0, 0, 1);
}

RTPlot::RTPlot(int argc, char* argv[]) :
	auto_refresh_period_(0),
	auto_x_range(true),
	auto_y_range(true)
{
	create();
	window_->show(argc, argv);
}

RTPlot::~RTPlot()
{
}

void RTPlot::create() {

	window_ = new Fl_Window(655, 450, "RTPlot");

	parser_ = new InputParserThread(this);
	parser_->run();

	plot_ = new Fl_Plot(_plot_x_offset, _plot_y_offset, _plot_x_size, _plot_y_size, "plot");

	window_->end();
	Fl::scheme("plastic"); // Better looking GUI
	// window_->size_range(window_->w(), window_->h());
	window_->size_range(655, 450, 0, 0, 0, 0, 1);
}

void RTPlot::quit() {
	parser_->stop();
	autoRefresh(false);
	window_->hide();
	refresh();
}

void RTPlot::newPoint(int curve, float x, float y) {
	// cout << "newPoint start\n";
	plot_->addPoint(curve, x, y);
	// cout << "newPoint end\n";
}

void RTPlot::removeFirstPoint(int curve) {
	// cout << "removeFirstPoint start\n";
	plot_->removeFirstPoint(curve);
	// cout << "removeFirstPoint end\n";
}

void RTPlot::setXLabel(const std::string& name) {
	plot_->setXLabel(name);
	refresh();
}

void RTPlot::setYLabel(const std::string& name) {
	plot_->setYLabel(name);
	refresh();
}

void RTPlot::setCurveName(int curve, const std::string& name) {
	plot_->setCurveLabel(curve, name);
	refresh();
}

void RTPlot::refresh() {
	// cout << "refresh start\n";
	plot_->refresh();
	window_->redraw();
	// cout << "refresh end\n";
}

void RTPlot::autoRefresh(bool enable, uint period_ms) {
	if(enable) {
		bool create_thread = (auto_refresh_period_ == 0);
		auto_refresh_period_ = period_ms;

		if(create_thread) {
			auto_refresh_thread_ = thread([this]() {
			                                  while(auto_refresh_period_) {
			                                      this->refresh();
			                                      this_thread::sleep_for(std::chrono::milliseconds(auto_refresh_period_));
											  }
										  });
		}
	}
	else {
		if(auto_refresh_period_) {
			auto_refresh_period_ = 0;
			auto_refresh_thread_.join();
		}
	}
}

void RTPlot::setXRange(float min, float max) {
	plot_->setXRange(min, max);
	refresh();
}

void RTPlot::setYRange(float min, float max) {
	plot_->setYRange(min, max);
	refresh();
}

void RTPlot::autoXRange() {
	plot_->setAutoXRange();
	refresh();
}

void RTPlot::autoYRange() {
	plot_->setAutoYRange();
	refresh();
}
