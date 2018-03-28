#include "Fl_plot.H"

#include <FL/fl_draw.H>

#include <iostream>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <mutex>

using namespace std;

typedef Fl_Plot::PointXY PointXY;
typedef Fl_Plot::Paird Paird;

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

	xrange_ = make_pair(0.0, 10.0);
	yrange_ = make_pair(0.0, 10.0);

	xbounds_ = make_pair(0, 0);
	ybounds_ = make_pair(0, 0);

	plot_offset_ = make_pair(x, y);
	plot_size_ = make_pair(w, h);
	plot_margins_ = make_pair(parent()->w()-w-x, parent()->h()-h-y);

	label_area_width_ = 200;
	display_labels_ = false;
	toggle_labels_ = false;

	subdivisions_ = 6;

	xlabel_ = "x axis";

	auto_xrange_ = false;
	auto_yrange_ = false;

	display_labels_btn_text_ = "+";
	display_labels_btn_ = new Fl_Button(x+w+5, y, 20, 20, display_labels_btn_text_.c_str());
	display_labels_btn_->callback(&Fl_Plot::display_labels_btn_clb, this);
}


void Fl_Plot::refresh() {
	redraw();
	Fl::check();
}

void Fl_Plot::addPoint(int curve, float x, float y) {
	curves_[curve].push_back(make_pair(x,y));
	ordered_lists_[curve].first.insert(x);
	ordered_lists_[curve].second.insert(y);

	if(auto_xrange_) {
		float min_val = std::numeric_limits<float>::infinity();
		float max_val = -std::numeric_limits<float>::infinity();
		for(auto& c_list : ordered_lists_) {
			auto& list = c_list.second.first; // xvalues
			float c_min = *list.begin();
			float c_max = *(--list.end());
			min_val = min(min_val, c_min);
			max_val = max(max_val, c_max);
		}
		xrange_auto_.first = min_val;
		xrange_auto_.second = max_val;
	}
	if(auto_yrange_) {
		float min_val = std::numeric_limits<float>::infinity();
		float max_val = -std::numeric_limits<float>::infinity();
		for(auto& c_list : ordered_lists_) {
			auto& list = c_list.second.second; // yvalues
			float c_min = *list.begin();
			float c_max = *(--list.end());
			min_val = min(min_val, c_min);
			max_val = max(max_val, c_max);
		}
		yrange_auto_.first = min_val;
		yrange_auto_.second = max_val;
	}

}

void Fl_Plot::removeFirstPoint(int curve) {
	try {
		auto& c = curves_.at(curve); // check for existance
		if(c.begin() == c.end()) // check if non-empty
			return;
	}
	catch (const std::out_of_range& oor) {
		std::cerr << "Curve " << curve << " doesn't exist, can't remove a point from it\n";
		return;
	}

	PointXY removed_point = curves_[curve].front();

	curves_lock_.lock();
	curves_[curve].pop_front();
	curves_lock_.unlock();

	auto& ordered_list = ordered_lists_[curve];

	auto& xlist = ordered_list.first;
	auto it = xlist.find(removed_point.first);
	if(it != xlist.end()) { // Shouldn't be necessary
		xlist.erase(it);
	}

	auto& ylist = ordered_list.second;
	it = ylist.find(removed_point.second);
	if(it != ylist.end()) { // Shouldn't be necessary
		ylist.erase(it);
	}

}

void Fl_Plot::displayLabels() {
	if(not display_labels_) {
		display_labels_ = true;
		display_labels_btn_text_ = "-";

		int max_text_width = 0;
		for(auto& lbl : labels_) {
			int texth=0, textw=0;
			fl_measure(lbl.second.c_str(), textw, texth);
			max_text_width = max(max_text_width, textw);
		}
		label_area_width_ = max_text_width;
		if(label_area_width_)
			label_area_width_ += 40;

		auto w = static_cast<Fl_Window*>(parent());
		int new_width = plot_offset_.first+plot_size_.first+plot_margins_.first+label_area_width_;
		int new_height = plot_offset_.second+plot_size_.second+plot_margins_.second;
		w->size_range(
			new_width > Fl::w() ? Fl::w() : new_width,
			new_height > Fl::h() ? Fl::h() : new_height,
			0, 0, 0, 0, 1);


		refresh();
	}
}
void Fl_Plot::hideLabels() {
	if(display_labels_) {
		display_labels_ = false;
		display_labels_btn_text_ = "+";
		auto w = static_cast<Fl_Window*>(parent());
		w->size_range(
			plot_offset_.first+plot_size_.first+plot_margins_.first,
			plot_offset_.second+plot_size_.second+plot_margins_.second,
			0, 0, 0, 0, 1);


		refresh();
	}
}

void Fl_Plot::toggleLabels() {
	toggle_labels_ = true;
}

void Fl_Plot::setSubdivisions(int sub) {
	assert(sub > 0);
	subdivisions_ = sub;
}

void Fl_Plot::setXRange(float min, float max) {
	xrange_ = make_pair(min, max);
	auto_xrange_ = false;
}

void Fl_Plot::setYRange(float min, float max) {
	yrange_ = make_pair(min, max);
	auto_yrange_ = false;
}

void Fl_Plot::setXLabel(const std::string& label) {
	xlabel_ = label;
}

void Fl_Plot::setYLabel(const std::string& label) {
	ylabel_ = label;
}

void Fl_Plot::setCurveLabel(int curve, const std::string& label) {
	labels_[curve] = label;
}

void Fl_Plot::setAutoXRange() {
	auto_xrange_ = true;
}

void Fl_Plot::setAutoYRange() {
	auto_yrange_ = true;
}

void Fl_Plot::display_labels_btn_clb(Fl_Widget *w, void *obj) {
	static_cast<Fl_Plot*>(obj)->toggleLabels();
}

void Fl_Plot::draw() {
	Fl_Color init_color = fl_color();

	int label_width = display_labels_ ? label_area_width_ : 0;
	auto w = static_cast<Fl_Window*>(parent());
	plot_size_ = make_pair(
		w->w()-plot_offset_.first-plot_margins_.first-label_width,
		w->h()-plot_offset_.second-plot_margins_.second);
	this->resize(plot_offset_.first, plot_offset_.second, plot_size_.first, plot_size_.second);
	display_labels_btn_->resize(plot_offset_.first+plot_size_.first+5, plot_offset_.second, 20, 20);

	drawBackground();
	drawAxes();
	if(toggle_labels_) {
		toggle_labels_ = false;
		if(display_labels_) {
			hideLabels();
		}
		else {
			displayLabels();
		}
	}
	if(display_labels_)
		drawLabels();

	// Avoid drawing outside of the plot area
	fl_push_clip(plot_offset_.first, plot_offset_.second, plot_size_.first, plot_size_.second);
	int idx = 0;
	curves_lock_.lock();
	initScaleToPlot();
	for(auto &c : curves_) {
		++idx;
		if(c.second.size() > 1) {
			PointXY prev, curr;
			auto it = c.second.begin();

			scaleToPlot(*it, prev);

			fl_color(palette_[idx%palette_.size()]);
			fl_begin_line();
			for(++it; it != c.second.end(); ++it) {
				scaleToPlot(*it, curr);
				fl_line(prev.first, prev.second, curr.first, curr.second);
				prev = curr;
			}
			fl_end_line();
		}
	}
	curves_lock_.unlock();
	fl_pop_clip();

	if(display_cursor_coordinates_) {
		PointXY p = scaleToGraph(last_cursor_position_);
		fl_color(FL_BLACK);
		fl_draw((to_string(p.first) + ", " + to_string(p.second)).c_str(), 10, parent()->h()-10);
	}

	fl_color(init_color);
}

int Fl_Plot::handle(int event) {
	int ret_code = 0;
	switch(event) {
	case FL_ENTER:
		display_cursor_coordinates_ = true;
		ret_code = 1;
		break;
	case FL_LEAVE:
		display_cursor_coordinates_ = false;
		ret_code = 1;
		break;
	case FL_MOVE:
	{
		last_cursor_position_ = std::make_pair(Fl::event_x(), Fl::event_y());
		ret_code = 1;
	}
	break;
	default:
		break;
	}
	return ret_code;
}

void Fl_Plot::drawAxes() {
	Paird xrange, yrange;
	xrange = auto_xrange_ ? xrange_auto_ : xrange_;
	yrange = auto_yrange_ ? yrange_auto_ : yrange_;
	fl_color(FL_BLACK);
	fl_draw("", 0, 0); // Needed to draw rotated text, don't know why. Maybe a bug....
	int textw=0, texth=0;
	fl_measure(ylabel_.c_str(), textw, texth);
	fl_draw(90, ylabel_.c_str(), 10+texth/2, (plot_offset_.second+plot_size_.second+textw)/2);

	textw = texth = 0;
	fl_measure(xlabel_.c_str(), textw, texth);
	fl_draw(xlabel_.c_str(), plot_offset_.first+(plot_size_.first-textw)/2, plot_offset_.second+plot_size_.second+40);

	fl_begin_line();
	// Y axis line
	fl_line(plot_offset_.first, plot_offset_.second, plot_offset_.first, plot_offset_.second + plot_size_.second);
	// X axis line
	fl_line(plot_offset_.first, plot_offset_.second + plot_size_.second, plot_offset_.first + plot_size_.first, plot_offset_.second + plot_size_.second);

	// Draw axes ticks
	int nticks = 4*subdivisions_;
	float xtick = plot_size_.first/float(nticks);
	float ytick = plot_size_.second/float(nticks);
	float xtick_range = float(xrange.second - xrange.first)/float(subdivisions_ * 4);
	float ytick_range = float(yrange.second - yrange.first)/float(subdivisions_ * 4);
	for(int i=1; i<=nticks; ++i) {
		float xstart, xend, ystart, yend;
		// X axis tick
		xstart = xend = plot_offset_.first + i*xtick;
		ystart = yend = plot_offset_.second + plot_size_.second;

		if(i%4)
			yend -= 3;  // small tick
		else {
			yend -= 6; // big tick
			// Verical dashed gray line
			Fl_Color col = fl_color();
			fl_color(200,200,200);
			fl_line_style(FL_DOT);
			fl_line(xstart, ystart - 6, xend, plot_offset_.second);
			fl_color(col);
			fl_line_style(0);

			drawXTickValue(i*xtick_range + xrange.first, make_pair(xstart, ystart));
		}
		fl_line(xstart, ystart, xend, yend);

		// Y axis tick
		xstart = xend = plot_offset_.first;
		ystart = yend = plot_offset_.second + plot_size_.second - i*ytick;

		if(i%4)
			xend += 3;  // small tick
		else {
			xend += 6; // big tick
			// Horizontal dashed gray line
			Fl_Color col = fl_color();
			fl_color(200,200,200);
			fl_line_style(FL_DOT);
			fl_line(xstart + 6, ystart, plot_offset_.first + plot_size_.first, yend);
			fl_color(col);
			fl_line_style(0);

			drawYTickValue(i*ytick_range + yrange.first, make_pair(xstart, ystart));
		}
		fl_line(xstart, ystart, xend, yend);
	}
	fl_end_line();
}

void Fl_Plot::drawBackground() {
	// White background
	fl_color(255, 255, 255);
	fl_rectf(0, 0, window()->w(), window()->h());
}

void Fl_Plot::drawLabels() {
	int texth=16, yoffset=0, idx=0;
	int xstart = plot_offset_.first+plot_size_.first+30;
	int ystart;

	Fl_Color orig_col = fl_color();
	fl_begin_line();

	for(auto& lbl : labels_) {
		++idx;

		fl_color(FL_BLACK);

		ystart = plot_offset_.second + yoffset;
		fl_draw(lbl.second.c_str(), xstart + 30, ystart + texth/2);

		fl_color(palette_[idx%palette_.size()]);
		fl_line(xstart, ystart+texth/4, xstart+20, ystart+texth/4);

		yoffset += texth;
	}
	fl_end_line();
	fl_color(orig_col);
}

void Fl_Plot::initScaleToPlot() {
	current_xrange_ = auto_xrange_ ? xrange_auto_ : xrange_;
	current_yrange_ = auto_yrange_ ? yrange_auto_ : yrange_;

	current_xscale_ = plot_size_.first/(current_xrange_.second - current_xrange_.first);
	current_yscale_ = plot_size_.second/(current_yrange_.first - current_yrange_.second);

}

void Fl_Plot::scaleToPlot(const PointXY& in_point, PointXY& out_point) {
	out_point.first = plot_offset_.first + current_xscale_*(in_point.first - current_xrange_.first);
	out_point.second = plot_offset_.second + current_yscale_*(in_point.second - current_yrange_.second);
}

PointXY Fl_Plot::scaleToGraph(const PointXY& point) {
	PointXY ret;
	Paird xrange, yrange;
	xrange = auto_xrange_ ? xrange_auto_ : xrange_;
	yrange = auto_yrange_ ? yrange_auto_ : yrange_;
	ret.first = xrange.first + (xrange.second - xrange.first) * (point.first - plot_offset_.first)/plot_size_.first;
	ret.second = yrange.second - (yrange.second - yrange.first) * (point.second - plot_offset_.second)/plot_size_.second;

	return ret;
}
void Fl_Plot::printPoint(const PointXY& point) {
	cout << "(" << point.first << "," << point.second << ")";
}


void Fl_Plot::drawXTickValue(float num, const PointXY& point) {

	char str[10];
	sprintf(str, "%.2f", num);
	int textw = 0, texth = 0;
	fl_measure(str, textw, texth);
	fl_draw(str, point.first-textw/2, point.second+texth);
}

void Fl_Plot::drawYTickValue(float num, const PointXY& point) {

	char str[10];
	sprintf(str, "%.2f", num);
	int textw = 0, texth = 0;
	fl_measure(str, textw, texth);
	fl_draw(str, point.first-textw-5, point.second+texth/2-2);
}
