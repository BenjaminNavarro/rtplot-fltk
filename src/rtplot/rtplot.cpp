#include <rtplot/rtplot.h>
#include <rtplot/Fl_Plot.H>

#include <FL/Fl.H>
#include "rtplot_window.h"
#include "rtplot_layout.h"

#include <X11/Xlib.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <cassert>

#include <rtplot/inputparserthread.h>

using namespace std;

namespace {
bool thread_init_done = false;
mutex auto_refresh_mtx;
}

struct RTPlot::rtplot_members {
	rtplot_members() :
		auto_refresh_period_(0),
		grid_rows(1),
		grid_cols(1)
	{
	}

	~rtplot_members() {

	}

	std::unique_ptr<RTPlotWindow> window_;
	std::unique_ptr<RTPlotLayout> layout_;
	std::unique_ptr<InputParserThread> parser_;
	std::vector<std::shared_ptr<Fl_Plot>> plots_;

	std::thread auto_refresh_thread_;
	std::mutex auto_refresh_period_lock_;
	size_t auto_refresh_period_;
	size_t grid_rows;
	size_t grid_cols;
};

constexpr int _plot_width = 655;
constexpr int _plot_height = 450;

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

RTPlot::~RTPlot() {
	quit();
}

void RTPlot::create(bool start_stdin_parser) {
	if(not thread_init_done) {
		Fl::lock();
		XInitThreads();
		thread_init_done = true;
	}

	impl_ = std::make_unique<RTPlot::rtplot_members>();

	impl_->window_ = std::make_unique<RTPlotWindow>(_plot_width, _plot_height, "RTPlot");

	if(start_stdin_parser) {
		impl_->parser_ = std::make_unique<InputParserThread>(this);
		impl_->parser_->run();
	}

	impl_->layout_ = std::make_unique<RTPlotLayout>(0, 0, _plot_width, _plot_height, "RTPlot layout");

	impl_->window_->end();

	Fl::scheme("plastic"); // Better looking GUI

	impl_->window_->resizable(*impl_->layout_);

	// This makes sure you can use Xdbe on servers where double buffering does not exist for every visual
	Fl::visual(FL_DOUBLE|FL_INDEX);
}

void RTPlot::checkPlot(size_t plot) {
	assert(plot < impl_->grid_rows*impl_->grid_cols);
	if(not static_cast<bool>(impl_->plots_[plot])) {
		impl_->plots_[plot] = std::make_shared<Fl_Plot>();
		updateLayout();
	}
}

void RTPlot::updateLayout() {
	impl_->layout_->setSize(impl_->plots_, impl_->grid_rows, impl_->grid_cols);
	refresh();
}

void RTPlot::setGridSize(size_t rows, size_t cols) {
	assert((rows > 1) and (cols > 1));
	impl_->grid_rows = rows;
	impl_->grid_cols = cols;
	impl_->plots_.resize(rows*cols);
	impl_->window_->size_range(cols*_plot_width, rows*_plot_height, 0, 0, 0, 0, 1);
	updateLayout();
}

void RTPlot::quit() {
	if(impl_->parser_)
		impl_->parser_->stop();
	autoRefresh(false);
	impl_->window_->hide();
	refresh();
}

void RTPlot::newPoint(size_t plot, int curve, float x, float y) {
	checkPlot(plot);
	impl_->plots_[plot]->addPoint(curve, x, y);
}

void RTPlot::removeFirstPoint(size_t plot, int curve) {
	checkPlot(plot);
	impl_->plots_[plot]->removeFirstPoint(curve);
}

void RTPlot::setXLabel(size_t plot, const std::string& name) {
	checkPlot(plot);
	impl_->plots_[plot]->setXLabel(name);
	refresh();
}

void RTPlot::setYLabel(size_t plot, const std::string& name) {
	checkPlot(plot);
	impl_->plots_[plot]->setYLabel(name);
	refresh();
}

void RTPlot::setCurveName(size_t plot, int curve, const std::string& name) {
	checkPlot(plot);
	impl_->plots_[plot]->setCurveLabel(curve, name);
	refresh();
}

void RTPlot::setPlotName(size_t plot, const std::string& name) {
	checkPlot(plot);
	impl_->plots_[plot]->setPlotName(name);
	refresh();
}

void RTPlot::refresh() {
	impl_->window_->redraw();
	Fl::check();
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

void RTPlot::setXRange(size_t plot, float min, float max) {
	checkPlot(plot);
	impl_->plots_[plot]->setXRange(min, max);
	refresh();
}

void RTPlot::setYRange(size_t plot, float min, float max) {
	checkPlot(plot);
	impl_->plots_[plot]->setYRange(min, max);
	refresh();
}

void RTPlot::autoXRange(size_t plot) {
	checkPlot(plot);
	impl_->plots_[plot]->setAutoXRange();
	refresh();
}

void RTPlot::autoYRange(size_t plot) {
	checkPlot(plot);
	impl_->plots_[plot]->setAutoYRange();
	refresh();
}

void RTPlot::setMaxPoints(size_t plot, size_t count) {
	checkPlot(plot);
	impl_->plots_[plot]->setMaxPoints(count);
	refresh();
}

void RTPlot::run() {
	Fl::run();
}

void RTPlot::check() {
	Fl::check();
}
