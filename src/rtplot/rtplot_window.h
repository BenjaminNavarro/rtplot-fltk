#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>

class RTPlotWindow : public Fl_Double_Window {
public:
	RTPlotWindow(int width, int height, const char* name) :
		Fl_Double_Window(width, height, name)
	{
	}
	~RTPlotWindow() = default;

protected:
	void draw() {
		color(FL_WHITE);
		Fl_Double_Window::draw();
	}
};
