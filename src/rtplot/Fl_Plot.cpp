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

typedef Fl_Plot::PointXY PointXY;
typedef Fl_Plot::Paird Paird;

constexpr int _plot_margin_left = 90;
constexpr int _plot_margin_top = 30;
constexpr int _plot_margin_right = 40;
constexpr int _plot_margin_bottom = 60;

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

	xrange_ = std::make_pair(0.0, 10.0);
	yrange_ = std::make_pair(0.0, 10.0);

	xbounds_ = std::make_pair(0, 0);
	ybounds_ = std::make_pair(0, 0);

	label_area_width_ = 0;
	display_labels_ = false;
	toggle_labels_ = false;

	subdivisions_ = 6;

	auto_xrange_ = false;
	auto_yrange_ = false;

	display_cursor_coordinates_ = false;

	display_labels_btn_text_ = "+";
	display_labels_btn_ = new Fl_Button(x+w+5, y, 20, 20, display_labels_btn_text_.c_str());
	display_labels_btn_->callback(&Fl_Plot::display_labels_btn_clb, this);

	if(label) {
		plot_name_ = std::string(label);
	}
}


void Fl_Plot::refresh() {
	redraw();
}

void Fl_Plot::addPoint(int curve, float x, float y) {
	auto& data = curves_data_[curve];

	if(data.points.size() == data.max_points) {
		removeFirstPoint(curve);
	}

	std::lock_guard<std::mutex> lock(data.lock_);

	data.points.push_back(std::make_pair(x,y));
	data.ordered_list.first.insert(x);
	data.ordered_list.second.insert(y);

	if(auto_xrange_) {
		float min_val = std::numeric_limits<float>::infinity();
		float max_val = -std::numeric_limits<float>::infinity();
		for(auto& curve_data : curves_data_) {
			auto& list = curve_data.second.ordered_list.first; // xvalues
			if(list.empty()) {
				continue;
			}
			float c_min = *list.begin();
			float c_max = *(--list.end());
			min_val = std::min(min_val, c_min);
			max_val = std::max(max_val, c_max);
		}
		xrange_auto_.first = min_val;
		xrange_auto_.second = max_val;
	}
	if(auto_yrange_) {
		float min_val = std::numeric_limits<float>::infinity();
		float max_val = -std::numeric_limits<float>::infinity();
		for(auto& curve_data : curves_data_) {
			auto& list = curve_data.second.ordered_list.second; // yvalues
			if(list.empty()) {
				continue;
			}
			float c_min = *list.begin();
			float c_max = *(--list.end());
			min_val = std::min(min_val, c_min);
			max_val = std::max(max_val, c_max);
		}
		yrange_auto_.first = min_val;
		yrange_auto_.second = max_val;
	}

}

void Fl_Plot::removeFirstPoint(int curve) {
	CurveData* data;
	try {
		data = &(curves_data_.at(curve)); // check for existance
		if(data->points.begin() == data->points.end()) // check if non-empty
			return;
	}
	catch (const std::out_of_range& oor) {
		std::cerr << "Curve " << curve << " doesn't exist, can't remove a point from it\n";
		return;
	}

	std::lock_guard<std::mutex> lock(data->lock_);

	auto& points = data->points;
	auto& ordered_list = data->ordered_list;

	PointXY removed_point = points.front();

	points.pop_front();

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
		for(auto& curve : curves_data_) {
			auto& lbl = curve.second.label;
			int texth=0, textw=0;
			fl_measure(lbl.c_str(), textw, texth);
			max_text_width = std::max(max_text_width, textw);
		}
		label_area_width_ = max_text_width;
		if(label_area_width_)
			label_area_width_ += 40;

		refresh();
	}
}
void Fl_Plot::hideLabels() {
	if(display_labels_) {
		display_labels_ = false;
		display_labels_btn_text_ = "+";
		label_area_width_ = 0;

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
	xrange_ = std::make_pair(min, max);
	auto_xrange_ = false;
}

void Fl_Plot::setYRange(float min, float max) {
	yrange_ = std::make_pair(min, max);
	auto_yrange_ = false;
}

void Fl_Plot::setXLabel(const std::string& label) {
	xlabel_ = label;
}

void Fl_Plot::setYLabel(const std::string& label) {
	ylabel_ = label;
}

void Fl_Plot::setPlotName(const std::string& name) {
	plot_name_ = name;
}

void Fl_Plot::setCurveLabel(int curve, const std::string& label) {
	curves_data_[curve].label = label;
}

void Fl_Plot::setAutoXRange() {
	auto_xrange_ = true;
}

void Fl_Plot::setAutoYRange() {
	auto_yrange_ = true;
}

void Fl_Plot::setMaxPoints(int curve, size_t count) {
	curves_data_[curve].max_points = count;
}

void Fl_Plot::setMaxPoints(size_t count) {
	for(auto& data: curves_data_) {
		data.second.max_points = count;
	}
}

void Fl_Plot::display_labels_btn_clb(Fl_Widget *w, void *obj) {
	static_cast<Fl_Plot*>(obj)->toggleLabels();
}

void Fl_Plot::draw() {
	Fl_Color init_color = fl_color();

	if(toggle_labels_) {
		toggle_labels_ = false;
		if(display_labels_) {
			hideLabels();
		}
		else {
			displayLabels();
		}
	}

	plot_size_ = std::make_pair(w() - _plot_margin_left - _plot_margin_right - label_area_width_, h() - _plot_margin_top - _plot_margin_bottom);
	plot_offset_ = std::make_pair(x() + _plot_margin_left, y() + _plot_margin_top);
	display_labels_btn_->resize(x()+w()-_plot_margin_right + 10, y() + 10, 20, 20);

	if(display_labels_)
		drawLabels();


	drawAxes();

	// Avoid drawing outside of the plot area
	fl_push_clip(plot_offset_.first, plot_offset_.second, plot_size_.first, plot_size_.second);
	int idx = 0;
	initScaleToPlot();
	for(auto& data: curves_data_) {
		data.second.lock_.lock();
		auto& c = data.second.points;
		++idx;
		if(c.size() > 1) {
			PointXY prev, curr;
			auto it = c.begin();

			scaleToPlot(*it, prev);

			fl_color(palette_[idx%palette_.size()]);
			fl_begin_line();
			for(++it; it != c.end(); ++it) {
				scaleToPlot(*it, curr);
				fl_line(prev.first, prev.second, curr.first, curr.second);
				prev = curr;
			}
			fl_end_line();
		}
		data.second.lock_.unlock();
	}
	fl_pop_clip();

	if(display_cursor_coordinates_) {
		PointXY p = scaleToGraph(last_cursor_position_);
		fl_color(FL_BLACK);
		fl_draw((std::to_string(p.first) + ", " + std::to_string(p.second)).c_str(), x()+10, y()+h()-10);
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
		last_cursor_position_ = std::make_pair(Fl::event_x(), Fl::event_y());
		ret_code = 1;
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
	fl_draw(90, ylabel_.c_str(), x()+10+texth/2, plot_offset_.second+(plot_size_.second+textw)/2);

	textw = texth = 0;
	fl_measure(xlabel_.c_str(), textw, texth);
	fl_draw(xlabel_.c_str(), plot_offset_.first+(plot_size_.first-textw)/2, plot_offset_.second+plot_size_.second+40);

	fl_measure(plot_name_.c_str(), textw, texth);
	fl_draw(plot_name_.c_str(), plot_offset_.first+(plot_size_.first-textw)/2, plot_offset_.second - texth/2);

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

			drawXTickValue(i*xtick_range + xrange.first, std::make_pair(xstart, ystart));
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

			drawYTickValue(i*ytick_range + yrange.first, std::make_pair(xstart, ystart));
		}
		fl_line(xstart, ystart, xend, yend);
	}
	fl_end_line();
}

void Fl_Plot::drawLabels() {
	int texth=16, yoffset=0, idx=0;
	int xstart = plot_offset_.first+plot_size_.first+10;
	int ystart;

	fl_push_clip(xstart, y(), xstart+label_area_width_, h());

	Fl_Color orig_col = fl_color();
	fl_begin_line();

	for(auto& data : curves_data_) {
		auto& lbl = data.second.label;
		++idx;

		fl_color(FL_BLACK);

		ystart = plot_offset_.second + yoffset;
		fl_draw(lbl.c_str(), xstart + 30, ystart + texth/2);

		fl_color(palette_[idx%palette_.size()]);
		fl_line(xstart, ystart+texth/4, xstart+20, ystart+texth/4);

		yoffset += texth;
	}
	fl_end_line();
	fl_color(orig_col);

	fl_pop_clip();
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
	ret.first = xrange.first + (xrange.second - xrange.first) * (point.first - x())/plot_size_.first;
	ret.second = yrange.second - (yrange.second - yrange.first) * (point.second - plot_offset_.second)/plot_size_.second;

	return ret;
}
void Fl_Plot::printPoint(const PointXY& point) {
	std::cout << "(" << point.first << "," << point.second << ")";
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
