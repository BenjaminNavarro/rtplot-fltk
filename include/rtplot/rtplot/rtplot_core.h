#pragma once

#include <utility>
#include <map>
#include <deque>
#include <vector>
#include <mutex>
#include <set>
#include <limits>

class RTPlotCore {
public:
	using Paird = std::pair<float,float>;
	using PointXY = Paird;

	RTPlotCore();
	virtual ~RTPlotCore();

	virtual void refresh() = 0;

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

protected:
	enum class LineStyle {
		Solid,
		Dotted
	};
	enum class MouseEvent {
		EnterWidget,
		LeaveWidget,
		MoveInsideWidget
	};
	enum class Colors {
		Black,
		White,
		Gray
	};

	virtual size_t getWidth() = 0;
	virtual size_t getHeiht() = 0;
	virtual size_t getXPosition() = 0;
	virtual size_t getYPosition() = 0;
	virtual void pushClip(const PointXY& start, const Paird& size) = 0;
	virtual void popClip() = 0;
	virtual void startLine() = 0;
	virtual void drawLine(const PointXY& start, const PointXY& end) = 0;
	virtual void endLine() = 0;
	virtual void setLineStyle(LineStyle style) = 0;
	virtual void drawText(const std::string& text, const PointXY& position, int angle = 0) = 0;
	virtual Paird measureText(const std::string& text) = 0;
	virtual void setColor(size_t palette_idx) = 0;
	virtual void setColor(Colors color) = 0;
	virtual void saveColor() = 0;
	virtual void restoreColor() = 0;
	virtual void handleWidgetEvent(MouseEvent event, PointXY cursor_position) final;
	virtual void labelsToggleButtonCallback() final;

private:

	void drawPlot();
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
	std::vector<uint32_t> palette_;
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

	std::string display_labels_btn_text_;
};
