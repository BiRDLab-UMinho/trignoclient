#include <string>
#include <thread>
#include <exception>
#define CVPLOT_HEADER_ONLY
#include "CvPlot/cvplot.h"
#include "sequence.hpp"                  // trigno::Sequence
#include "basic_sequence_processor.hpp"  // trigno::tools::BasicSequenceProcessor
#include "plotter.hpp"

namespace trigno::tools {

Plotter::Window::Window(const std::string& name_, size_t height_, size_t width_) :
    name(name_), height(height_), width(width_) {
        /* ... */
}



Plotter::Plotter(const Window& window, const std::duration& refresh_interval) :
    BasicSequenceProcessor(),
    _refresh_interval(refresh_interval) {
        /* ... */
}



Plotter::Axes Plotter::draw(const Sequence::Range& range, const Window& window, const sensor::List& sensors) {
    auto axes = CvPlot::makePlotAxes();

    // extract sensor data (use fisrt frame as reference -> sensor samples not present on first frame are not plotted)
    // @note Sequence implements sensor check on append/add operation, thus we can assume all frames in sequence share content type
    for (const auto& id : sensors) {
        // @note extract() will fail if given *id* is not present
        try {
            // get values & time stamps
            auto values = Sequence::extract(range, id);
            auto times = values.get_keys< std::vector< float > >();
            // plot
            // axes = CvPlot::plot(times, values);
            // axes.create<CvPlot::Series>(times, values, "-g");
        } catch (const std::out_of_range&) {
            continue;
        }
    }
    // show plot
    // CvPlot::show("mywindow", axes);

    cv::Mat plot = axes.render(window.height, window.width);
    cv::imshow(window.name, plot);
    cv::waitKey();  // consider using cv::getWindowProperty(WND_PROP_VISIBLE);
    return plot;
}



void Plotter::execute() {
    // extract sensor data (use fisrt frame as reference)
    draw(_range, _window, _sensors);
    std::this_thread::sleep_for(_refresh_interval);
    ///
    _range++;
}

}  // namespace trigno::network
