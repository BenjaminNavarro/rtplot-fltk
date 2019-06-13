#include <rtplot/rtplot_fltk.h>
#include <rtplot/internal/rtplot_pimpl.h>
#include <rtplot/Fl_Plot.H>

#include <FL/Fl.H>
#include "rtplot_fltk_window.h"
#include "rtplot_fltk_layout.h"

#include <X11/Xlib.h>

using namespace rtp;

namespace {
bool thread_init_done = false;
}


RTPlotFLTK::RTPlotFLTK() :
	RTPlot()
{
	init();
}

RTPlotFLTK::~RTPlotFLTK() = default;

void RTPlotFLTK::create() {
	if(not thread_init_done) {
		Fl::lock();
		XInitThreads();
		thread_init_done = true;
	}

	impl_->window_ = std::make_unique<RTPlotFLTKWindow>(getPlotWidth(), getPlotHeight(), "RTPlot");
	auto window = dynamic_cast<RTPlotFLTKWindow*>(impl_->window_.get());

	impl_->layout_ = std::make_unique<RTPlotFLTKLayout>(0, 0, getPlotWidth(), getPlotHeight(), "RTPlot layout");
	auto layout = dynamic_cast<RTPlotFLTKLayout*>(impl_->layout_.get());

	window->end();

	Fl::scheme("gtk+"); // Better looking GUI (changed from "plastic" because of background color issues)

	window->resizable(layout);

	// This makes sure you can use Xdbe on servers where double buffering does not exist for every visual
	Fl::visual(FL_DOUBLE|FL_INDEX);
}

void RTPlotFLTK::refresh() {
	impl_->window_->redraw();
	Fl::check();
}

void RTPlotFLTK::run() {
	Fl::run();
}

bool RTPlotFLTK::check() {
	return Fl::check();
}

std::shared_ptr<RTPlotCore> RTPlotFLTK::makePlot() {
	return std::make_shared<Fl_Plot>();
}
