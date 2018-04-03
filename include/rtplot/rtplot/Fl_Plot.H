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
#include <deque>
#include <vector>
#include <mutex>
#include <set>
#include <limits>

class Fl_Plot : virtual public Fl_Widget {
public:
	typedef std::pair<float,float> Paird;
	typedef Paird PointXY;
	Fl_Plot(int x = 0, int y = 0, int w = 0, int h = 0, const char *label = 0);

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
	void setPlotName(const std::string& name);
	void setCurveLabel(int curve, const std::string& label);
	void setAutoXRange();
	void setAutoYRange();
	void setMaxPoints(int curve, size_t count);
	void setMaxPoints(size_t count);

	static void display_labels_btn_clb(Fl_Widget *w, void *obj);

protected:
	void draw();
	int handle(int event);

private:

	void drawAxes();
	void drawLabels();
	void initScaleToPlot();
	void scaleToPlot(const PointXY& in_point, PointXY& out_point);
	PointXY scaleToGraph(const PointXY& point);
	void printPoint(const PointXY& point);
	void drawXTickValue(float num, const PointXY& point);
	void drawYTickValue(float num, const PointXY& point);

	struct CurveData {
		CurveData() :
			max_points(std::numeric_limits<size_t>::max())
		{
		}

		std::deque<PointXY> points;
		std::pair<std::multiset<float>, std::multiset<float>> ordered_list;
		std::string label;
		size_t max_points;
		std::mutex lock_;
	};
	std::map<int, CurveData> curves_data_;
	std::vector<Fl_Color> palette_;
	Paird xrange_;
	Paird yrange_;
	Paird xrange_auto_;
	Paird yrange_auto_;
	std::pair<int, int> xbounds_;
	std::pair<int, int> ybounds_;
	PointXY plot_offset_;
	Paird plot_size_;
	int label_area_width_;
	int subdivisions_;
	std::string xlabel_;
	std::string ylabel_;
	std::string plot_name_;
	std::pair<size_t, size_t> last_cursor_position_;
	bool auto_xrange_;
	bool auto_yrange_;
	bool display_labels_;
	bool toggle_labels_;
	bool display_cursor_coordinates_;

	Paird current_xrange_, current_yrange_;
	float current_xscale_, current_yscale_;

	Fl_Button* display_labels_btn_;
	std::string display_labels_btn_text_;
};

#endif
