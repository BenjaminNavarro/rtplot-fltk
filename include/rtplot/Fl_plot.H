#ifndef FLPLOT_H
#define FLPLOT_H

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>

#include <utility>
#include <map>
#include <list>
#include <vector>
#include <mutex>
#include <set>

class Fl_Plot : virtual public Fl_Widget {
public:
	typedef std::pair<float,float> Paird;
	typedef Paird PointXY;
	Fl_Plot(int x, int y, int w, int h, const char *label = 0);

	void refresh();

	void addPoint(int curve, float x, float y);
	void removeFirstPoint(int curve);
	void displayLabels();
	void hideLabels();
	void toggleLabels();

	void setSubdivisions(int sub);
	void setXRange(float min, float max);
	void setYRange(float min, float max);
	void setXLabel(const std::string& label);
	void setYLabel(const std::string& label);
	void setCurveLabel(int curve, const std::string& label);
	void setAutoXRange();
	void setAutoYRange();

	static void display_labels_btn_clb(Fl_Widget *w, void *obj);

protected:
	void draw();
	int handle(int event);

private:

	void drawAxes();
	void drawBackground();
	void drawLabels();
	void initScaleToPlot();
	void scaleToPlot(const PointXY& in_point, PointXY& out_point);
	PointXY scaleToGraph(const PointXY& point);
	void printPoint(const PointXY& point);
	void drawXTickValue(float num, const PointXY& point);
	void drawYTickValue(float num, const PointXY& point);

	std::map<int, std::list<PointXY> > curves_;
	std::map<int, std::pair<std::multiset<float>, std::multiset<float> > > ordered_lists_;
	std::map<int, std::string> labels_;
	std::mutex curves_lock_;
	std::vector<Fl_Color> palette_;
	Paird xrange_;
	Paird yrange_;
	Paird xrange_auto_;
	Paird yrange_auto_;
	std::pair<int, int> xbounds_;
	std::pair<int, int> ybounds_;
	PointXY plot_offset_;
	Paird plot_size_;
	Paird init_plot_size_;
	Paird plot_margins_;
	int label_area_width_;
	int subdivisions_;
	std::string xlabel_;
	std::string cursor_coordinates_;
	bool auto_xrange_;
	bool auto_yrange_;
	bool display_labels_;
	bool toggle_labels_;
	bool display_cursor_coordinates_;

	Fl_Button* display_labels_btn_;
	std::string display_labels_btn_text_;
};

#endif
