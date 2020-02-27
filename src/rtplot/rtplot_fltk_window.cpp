#include "rtplot_fltk_window.h"

#include <FL/fl_draw.H>

using namespace rtp;

RTPlotFLTKWindow::RTPlotFLTKWindow(int width, int height, const char* name)
    : Fl_Double_Window(width, height, name), RTPlotWindow() {
}

RTPlotFLTKWindow::~RTPlotFLTKWindow() = default;

void RTPlotFLTKWindow::show() {
    Fl_Double_Window::show();
}

void RTPlotFLTKWindow::hide() {
    Fl_Double_Window::hide();
}

void RTPlotFLTKWindow::setMinimumSize(size_t width, size_t height) {
    Fl_Double_Window::size_range(width, height, 0, 0, 0, 0, 1);
}

void RTPlotFLTKWindow::redraw() {
    Fl_Double_Window::redraw();
}

void RTPlotFLTKWindow::draw() {
    color(FL_WHITE);
    Fl_Double_Window::draw();
}
