#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_PLOTTER_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_PLOTTER_HPP_

#include <string>
#include <mutex>
#include <opencv2/opencv.hpp>
#include "sensor.hpp"                    // trigno::sensor::List
#include "sequence.hpp"                  // trigno::Sequence
#include "basic_sequence_processor.hpp"  // trigno::tools::BasicSequenceProcessor<>

#define DEFAULT_PLOT_HEIGHT 760
#define DEFAULT_PLOT_WIDTH  1280

namespace trigno::tools {

//------------------------------------------------------------------------------
/// @brief      Tool that sequentially plots a sequence of frames.
///
/// @note       Useful when live visual feedback is required during data acquisition.
///
class Plotter : public BasicSequenceProcessor {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Plot object type.
    ///
    using Axes = cv::Mat;

    //--------------------------------------------------------------------------
    /// @brief      Plot window configuration.
    ///
    /// @note       At this stage, only name & size are configurable.
    ///             Other configuration options (axes, ticks, multi-plot, etc) show be declared as part of Window.
    ///
    struct Window {
        std::string name;
        size_t height;
        size_t width;
        // ...
        explicit Window(const std::string& name_ = "", size_t height_ = DEFAULT_PLOT_HEIGHT, size_t width_ = DEFAULT_PLOT_WIDTH);
    };

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    /// @param[in]  in                Input sequence to plot.
    /// @param[in]  n_frames          Number of frames to plot. If < size of input sequence, the last *n_frames* frames are plotted.
    /// @param[in]  window            Plot window configuration (name/title & dimensions).
    /// @param[in]  refresh_interval  Refresh rate (number of new samples to force an i.e. batch size). Defaults to 0 seconds (no update/refresh, single plot).
    /// @param[in]  sensors           Sensors to plot. Defaults to all sensors i.e. active sensors are plotted.
    ///
    explicit Plotter(const Window& window = Window(), const std::duration& refresh_interval = std::duration(0.1));

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
    static Axes draw(const Sequence::Range& range, const Window& window = Window(), const sensor::List& sensors = sensor::all);

    //--------------------------------------------------------------------------
    /// @brief      Reads & saves a single *DataFrame* from the data client
    ///
    void execute() override;

    //--------------------------------------------------------------------------
    /// @brief      Window configuration.
    ///
    Window _window;

    //--------------------------------------------------------------------------
    /// @brief      Time interval between consecutive updates to plot.
    ///
    std::duration _refresh_interval;
};

}  // namespace trigno::tools

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_PLOTTER_HPP_
