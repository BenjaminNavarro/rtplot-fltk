#include "rtplot.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>

#include "Fl_plot.H"

#include <X11/Xlib.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

#include "inputparserthread.h"

using namespace std;

namespace {
bool thread_init_done = false;
mutex auto_refresh_mtx;
}

struct RTPlot::rtplot_members {
	rtplot_members() :
		window_(nullptr),
		parser_(nullptr),
		auto_refresh_period_(0),
		plot_(nullptr),
		auto_x_range(true),
		auto_y_range(true)
	{
	}

	Fl_Window* window_;

	InputParserThread* parser_;
	std::thread auto_refresh_thread_;
	std::mutex auto_refresh_period_lock_;
	unsigned int auto_refresh_period_;
	Fl_Plot* plot_;
	bool auto_x_range, auto_y_range;
	uint color_index;
};

const int _plot_x_offset = 70;
const int _plot_y_offset = 15;
const int _plot_x_size = 555;
const int _plot_y_size = 380;

RTPlot::RTPlot(bool start_stdin_parser) : impl_(nullptr)
{
	create(start_stdin_parser);
	impl_->window_->show();
}

RTPlot::RTPlot(int argc, char* argv[], bool start_stdin_parser) : impl_(nullptr)
{
	create(start_stdin_parser);
	impl_->window_->show(argc, argv);
}

RTPlot::~RTPlot()
{
	if(impl_ != nullptr) {
		delete impl_;
		impl_ = nullptr;
	}
}

void RTPlot::create(bool start_stdin_parser) {
	if(not thread_init_done) {
		Fl::lock();
		XInitThreads();
		thread_init_done = true;
	}

	impl_ = new rtplot_members;

	impl_->window_ = new Fl_Window(655, 450, "RTPlot");

	if(start_stdin_parser) {
		impl_->parser_ = new InputParserThread(this);
		impl_->parser_->run();
	}

	impl_->plot_ = new Fl_Plot(_plot_x_offset, _plot_y_offset, _plot_x_size, _plot_y_size, "plot");

	impl_->window_->end();

	Fl::scheme("plastic"); // Better looking GUI

	impl_->window_->size_range(655, 450, 0, 0, 0, 0, 1);
}

void RTPlot::quit() {
	if(impl_->parser_ != nullptr)
		impl_->parser_->stop();
	autoRefresh(false);
	impl_->window_->hide();
	refresh();
}

void RTPlot::newPoint(int curve, float x, float y) {
	impl_->plot_->addPoint(curve, x, y);
}

void RTPlot::removeFirstPoint(int curve) {
	impl_->plot_->removeFirstPoint(curve);
}

void RTPlot::setXLabel(const std::string& name) {
	impl_->plot_->setXLabel(name);
	refresh();
}

void RTPlot::setYLabel(const std::string& name) {
	impl_->plot_->setYLabel(name);
	refresh();
}

void RTPlot::setCurveName(int curve, const std::string& name) {
	impl_->plot_->setCurveLabel(curve, name);
	refresh();
}

void RTPlot::refresh() {
	impl_->plot_->refresh();
	impl_->window_->redraw();
}

void RTPlot::autoRefresh(bool enable, uint period_ms) {
	if(enable) {
		bool create_thread = (impl_->auto_refresh_period_ == 0);
		impl_->auto_refresh_period_ = period_ms;

		if(create_thread) {
			impl_->auto_refresh_thread_ = thread(
			    [this]() {
			        while(impl_->auto_refresh_period_) {
			            /* Using Fl::lock() blocks everything but a standard mutex seems to do the job */
			            // Fl::lock();
			            auto_refresh_mtx.lock();
			            this->refresh();
			            auto_refresh_mtx.unlock();
			            // Fl::unlock();
			            this_thread::sleep_for(std::chrono::milliseconds(impl_->auto_refresh_period_));
					}
				});
		}
	}
	else {
		if(impl_->auto_refresh_period_) {
			impl_->auto_refresh_period_ = 0;
			impl_->auto_refresh_thread_.join();
		}
	}
}

void RTPlot::setXRange(float min, float max) {
	impl_->plot_->setXRange(min, max);
	refresh();
}

void RTPlot::setYRange(float min, float max) {
	impl_->plot_->setYRange(min, max);
	refresh();
}

void RTPlot::autoXRange() {
	impl_->plot_->setAutoXRange();
	refresh();
}

void RTPlot::autoYRange() {
	impl_->plot_->setAutoYRange();
	refresh();
}
