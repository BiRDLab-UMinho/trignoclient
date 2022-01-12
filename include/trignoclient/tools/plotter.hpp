#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_PLOTTER_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_PLOTTER_HPP_

#include <utility>
#include <string>
#include <mutex>
#include <opencv2/opencv.hpp>                  // cv::Mat
#include "core/sensor.hpp"                     // trigno::sensor::List
#include "core/duration.hpp"                   // trigno::Duration
#include "core/sequence.hpp"                   // trigno::Sequence
#include "tools/iterative.hpp"                 // trigno::tools::Iterative<>
#include "tools/basic_sequence_processor.hpp"  // trigno::tools::BasicSequenceProcessor

#define DEFAULT_PLOT_HEIGHT 500
#define DEFAULT_PLOT_WIDTH  1000

namespace trigno::tools {

/// @cond
// struct PlottedRange : public CvPlot::Series {
//     Sequence::Range _range;

//     explicit PlottedRange(Sequence::ConstRange range, const std::string& lineSpec = "-") : CvPlot::Series(lineSpec),  _range(range) {};

//     void render(CvPlot::RenderTarget &renderTarget) override {
//         // initialize to initial point!
//         auto prev = renderTarget.project(cv::Point2f({_range[0].key, _range[0].get()}));

//         for (size_t idx = 1; idx < _range.size(); idx++) {
//             // draw line from previous sample
//             auto next = renderTarget.project(cv::Point2f({_range[idx].key, _range[idx].get()}));
//             cv::line(renderTarget.innerMat(), prev, next, color);
//             prev = next;
//         }

//         // auto p1 = renderTarget.project({ _rect.x, _rect.y });
//         // auto p2 = renderTarget.project({ _rect.x + _rect.width, _rect.y + _rect.height });
//         // cv::rectangle(renderTarget.innerMat(), cv::Rect2d(p1, p2), cv::Scalar(0, 0, 255), 3);

//     }

//     // Series &blue = axes.create<Series>(std::vector<int>{1, 2}, "-b").setLineWidth(20);

//     // bool getBoundingRect(cv::Rect2d &rect) override {
//     //     rect = _rect;
//     //     return true;
//     // }
// };
/// @endcond

//------------------------------------------------------------------------------
/// @brief      Tool that sequentially plots a sequence of frames.
///
/// @note       Useful when live visual feedback is required during or after data acquisition.
///
class Plotter : public BasicSequenceProcessor {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Plot window configuration.
    ///
    /// @note       At this stage, only name & size are configurable.
    ///             Other configuration options (axes, ticks, multi-plot, etc) show be declared as part of Window.
    ///
    struct Window {
        bool close_on_stop = false;
        std::string name;
        std::string title_format = "S%lu [%s]";
        size_t height = DEFAULT_PLOT_HEIGHT;
        size_t width = DEFAULT_PLOT_WIDTH;
        bool autofitx = true;
        bool autofity = true;
        std::pair< float, float > xlim = { -1, 1 };
        std::pair< float, float > ylim = { -0.005,  0.005 };
        Duration refresh_interval;
        // ...
        explicit Window(const std::string& name_ = "Trigno Plotter", const Duration& refresh_interval = Duration(100));
    };

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    /// @param[in]  window  Configuration of plotting window. Default parametrization fitted to raw EMG plotting common requirements.
    ///
    explicit Plotter(const Window& window = Window());

    //--------------------------------------------------------------------------
    /// @brief      Destroys the object.
    ///
    ~Plotter();

    //--------------------------------------------------------------------------
    /// @brief      Window configuration.
    ///
    Window window;

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Plots a sequence sub-range (multiple frames) with samples from given *sensors* (if available in input data).
    ///
    /// @param[in]  range    Source data, as range over source sequence.
    /// @param[in]  window   Plot window configuration (name/title & dimensions).
    /// @param[in]  sensors  Sensors whose samples to plot. Defaults to all sensors i.e. active sensors are plotted.
    ///
    /// @note       Static function, can be used to plot custom sequences/ranges.
    ///
    static cv::Mat draw(const Sequence::Range& range, const Window& window = Window(), const sensor::List& sensors = sensor::all);

    //--------------------------------------------------------------------------
    /// @brief      Reads & saves a single *DataFrame* from the data client
    ///
    /// @param[in]  range    Range of frames to plot.
    /// @param[in]  sensors  Sensors w/ data to plot. Defaults to all available sensors.
    ///
    void execute(Sequence::Range&& range, const sensor::List& sensors = sensor::all) override;

    //--------------------------------------------------------------------------
    /// @brief      Un-looped function called after execute() loop.
    ///
    void stop() override;

    //--------------------------------------------------------------------------
    /// @brief      Class friendship w/ Iterative< Plotter > allows its members access to Exported protected members.
    ///
    friend class Iterative< Plotter >;
};



//------------------------------------------------------------------------------
/// @brief      Un-looped function called before execute() loop.
///
/// @note       Specialization of Iterative< >::start() for Plotter processor type.
///
template < /* specialization for iterative (continuous) plotter */ >
void Iterative< Plotter >::start();

//------------------------------------------------------------------------------
/// @brief      Un-looped function called after execute() loop.
///
/// @note       Specialization of Iterative< >::start() for Plotter processor type.
///             Called after iterative plotting ends (no more data available / timed out).
///
template < /* specialization for iterative (continuous) plotter */ >
void Iterative< Plotter >::stop();

}  // namespace trigno::tools

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_PLOTTER_HPP_
