#include <rtplot/Fl_Plot.H>

#include <FL/fl_draw.H>

#include <iostream>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <mutex>
#include <functional>

using namespace rtp;

Fl_Plot::Fl_Plot(int x, int y, int w, int h, const char *label) : Fl_Widget(x,y,w,h,label) {
	palette_ = {
		FL_BLACK,
		FL_RED,
		FL_GREEN,
		FL_YELLOW,
		FL_BLUE,
		FL_MAGENTA,
		FL_CYAN,
		FL_DARK_RED,
		FL_DARK_GREEN,
		FL_DARK_YELLOW,
		FL_DARK_BLUE,
		FL_DARK_MAGENTA,
		FL_DARK_CYAN
	};

	saved_color_ = FL_BLACK;

	display_labels_btn_ = new Fl_Button(0, 0, 0, 0, getToggleButtonText().c_str());
	display_labels_btn_->callback(&Fl_Plot::display_labels_btn_clb, this);

	if(label) {
		setPlotName(std::string(label));
	}
}

Fl_Plot::~Fl_Plot() = default;

void Fl_Plot::refresh() {
	redraw();
}

void Fl_Plot::display_labels_btn_clb(Fl_Widget *w, void *obj) {
	static_cast<Fl_Plot*>(obj)->toggleLabelsProcess();
}

void Fl_Plot::draw() {
	drawPlot();
	auto btn_pos = getToggleButtonPosition();
	auto btn_size = getToggleButtonSize();
	display_labels_btn_->resize(btn_pos.first, btn_pos.second, btn_size.first, btn_size.second);
}

int Fl_Plot::handle(int event) {
	MouseEvent event_type;
	int ret_code = 1;
	switch(event) {
	case FL_ENTER:
		event_type = MouseEvent::EnterWidget;
		break;
	case FL_LEAVE:
		event_type = MouseEvent::LeaveWidget;
		break;
	case FL_MOVE:
		event_type = MouseEvent::MoveInsideWidget;
		break;
	default:
		event_type = MouseEvent::Unknown;
		ret_code = 0;
		break;
	}

	handleWidgetEvent(event_type, std::make_pair(Fl::event_x(), Fl::event_y()));

	return ret_code;
}

size_t Fl_Plot::getWidth() {
	return w();
}

size_t Fl_Plot::getHeight() {
	return h();
}

int Fl_Plot::getXPosition() {
	return x();
}

int Fl_Plot::getYPosition() {
	return y();
}

void Fl_Plot::setSize(const Pairf& size) {
	resize(x(), y(), size.first, size.second);
}

void Fl_Plot::setPosition(const PointXY& position) {
	resize(position.first, position.second, w(), h());
}

void Fl_Plot::pushClip(const PointXY& start, const Pairf& size) {
	fl_push_clip(start.first, start.second, size.first, size.second);
}

void Fl_Plot::popClip() {
	fl_pop_clip();
}

void Fl_Plot::startLine() {
	fl_begin_line();
}

void Fl_Plot::drawLine(const PointXY& start, const PointXY& end) {
	fl_line(start.first, start.second, end.first, end.second);
}

void Fl_Plot::endLine() {
	fl_end_line();
}

void Fl_Plot::setLineStyle(RTPlotCore::LineStyle style) {
	switch (style) {
	case LineStyle::Solid:
		fl_line_style(FL_SOLID);
		break;
	case LineStyle::Dotted:
		fl_line_style(FL_DOT);
		break;
	}
}

void Fl_Plot::drawText(const std::string& text, const PointXY& position, int angle) {
	if(angle == 0) {
		fl_draw(text.c_str(), position.first, position.second);
	}
	else {
		fl_draw("", 0, 0); // Needed to draw rotated text, don't know why. Maybe a bug....
		fl_draw(angle, text.c_str(), position.first, position.second);
	}
}

RTPlotCore::Pairf Fl_Plot::measureText(const std::string& text) {
	int textw=0, texth=0;
	fl_measure(text.c_str(), textw, texth);
	return Pairf{textw, texth};
}

void Fl_Plot::setColor(size_t palette_idx) {
	fl_color(palette_[palette_idx]);
}

void Fl_Plot::setColor(RTPlotCore::Colors color) {
	switch(color) {
	case Colors::Black:
		fl_color(FL_BLACK);
		break;
	case Colors::White:
		fl_color(FL_WHITE);
		break;
	case Colors::Gray:
		fl_color(200,200,200);
		break;
	}
}

void Fl_Plot::saveColor() {
	saved_color_ = fl_color();
}

void Fl_Plot::restoreColor() {
	fl_color(saved_color_);
}

void Fl_Plot::toggleLabelsProcess() {
	toggleLabels();
}
