#include <string>
#include <thread>
#include <exception>
#include <vector>
#define CVPLOT_HEADER_ONLY
#include "CvPlot/cvplot.h"
#include "core/duration.hpp"                   // trigno::Duration
#include "core/sequence.hpp"                   // trigno::Sequence
#include "tools/basic_sequence_processor.hpp"  // trigno::tools::BasicSequenceProcessor
#include "tools/plotter.hpp"

namespace cv {

void waitKeyOrClose(const std::string& window_name, size_t wait_ms = 100) {
    while (cv::getWindowProperty(window_name, cv::WND_PROP_VISIBLE) > 0.0) {
        auto key = cv::waitKey(100);
        if (key == 27 /* ESC */) {
            break;
        }
    }
}

}  // namespace cv


namespace trigno::tools {

Plotter::Window::Window(const std::string& name_, const Duration& refresh_interval_) :
    name(name_),
    refresh_interval(refresh_interval_) {
        /* ... */
}



Plotter::Plotter(const Window& window) :
    BasicSequenceProcessor(),
    window(window) {
        /* ... */
}



Plotter::~Plotter() {
    /* ... */
    // cv::destroyWindow(window.name);
    cv::destroyAllWindows();
}



cv::Mat Plotter::draw(const Sequence::Range& range, const Window& window, const sensor::List& sensors) {
    if (!range.size()) {
        throw std::invalid_argument("");
    }
    // initialize empty axes container
    std::vector< CvPlot::Axes > axes;
    // initialize data vectors
    std::vector< double > values;
    std::vector< double > times;
    // available styles -> upper limit on number of channels, which is not an issue as Trigno sensors have at max 4 channels
    std::vector< std::string > styles = {"-r", "-b", "-g", "-k", "-c", "-y", ".r", ".b", ".g", ".k", ".c", ".y"};

    for (const auto& id : sensors) {
        // extract sensor data (use first frame as reference -> sensor samples not present on first frame are not plotted)
        // @note Sequence implements sensor check on append/add operation, thus we can assume all frames in sequence share content type
        auto p = range[0].get().find(id);
        if (p >= 0) {
            // initialize empty axes
            axes.emplace_back(CvPlot::makePlotAxes());
            // draw plot for each channel
            for (int idx = 0; idx < range[0].get()[p].size(); idx++) {
                auto n = Sequence::extract(range, id, &values, &times, idx);
                axes.back().create< CvPlot::Series >(times, values, styles[idx]);
            }
            // format axes
            char title[2 * window.title_format.size()];
            // sprintf(title, window.title_format.data(), id, range[0].get().key(p).data());
            snprintf(title, 2 * window.title_format.size(), window.title_format.data(), id, range[0].get().key(p).data());
            axes.back().title(title);
            axes.back().setXTight();
            axes.back().setYTight();
            axes.back().setTightBox();
            if (!window.autofitx) {
                axes.back().setXLim({ window.xlim.first, window.xlim.second });
            }
            if (!window.autofity) {
                axes.back().setYLim({ window.ylim.first, window.ylim.second });
            }
        }
    }
    // if no plots created (no active sensors), return empty image
    if (!axes.size()) {
        return cv::Mat3b(0, 0);
    }
    // render plots (build output image)
    cv::Mat3b mat(window.height, window.width);
    auto subplot_height = window.height / axes.size();
    size_t idx = 0;
    for (const auto& plot : axes) {
        auto roi = cv::Rect(0, (idx++) * subplot_height, window.width, subplot_height);
        plot.render(mat(roi));
    }
    // show window
    // 1ms wait to ensure window is shown!
    cv::imshow(window.name, mat);
    cv::waitKey(1);
    return mat;
}



void Plotter::execute(Sequence::Range&& range, const sensor::List& sensors) {
    if (range.size() > 0) {
        // delegate to static implementation
        draw(range, window, sensors);
        // sleep for desired refresh time. avoids excessive & unnecessary plot updates (less performant)
        std::this_thread::sleep_for(window.refresh_interval);
    }
}



void Plotter::stop() {
    // close or hold figure @ end of plotting
    if (!window.close_on_stop) {
        cv::waitKeyOrClose(window.name);
    }
}



template < /* specialization for iterative (continuous) plotter */ >
void Iterative< Plotter >::start() {
    // disable blocking on displayed plot for continuous iteration
    _executor.window.close_on_stop = true;
}



template < /* specialization for iterative (continuous) plotter */ >
void Iterative< Plotter >::stop() {
    cv::waitKeyOrClose(_executor.window.name);
}

}  // namespace trigno::network
