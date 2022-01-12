#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_MVC_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_MVC_HPP_

#include <initializer_list>
#include "sequence_metrics.hpp"  // trigno::tools::RMS, trigno::tools::Maximum
#include "frame.hpp"             // trigno::Frame
#include "basic_protocol.hpp"    // trigno::tools::BasicProtocol

namespace trigno::tools {

//------------------------------------------------------------------------------
/// @brief      Single- or multi-sensor MVC (Maximum Voluntary Contraction) acquisition protocol.
///
class MVC : public BasicProtocol {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance.
    ///
    /// @param[in]  sensors                The sensors
    /// @param[in]  filter_window          The filter window. Defaults to 0.125 seconds.
    /// @param[in]  filter_window_overlap  The filter window overlap. Defaults to 0.0625 seconds (1/2 window).
    /// @param[in]  repetitions            The repetitions. Defaults to 3 attempts.
    ///
    explicit MVC(const std::initializer_list< uint >& sensors, float filter_window = 0.125, float filter_window_overlap = 0.065, int repetitions = 3);

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Execute MVC protocol (data acquisition).
    ///
    void execute() override;

    //--------------------------------------------------------------------------
    /// @brief      Finalize MVC protocol (data plotting, post-processing & exporting).
    ///
    void finalize() override;

    //--------------------------------------------------------------------------
    /// @brief      RMS filter to extract signal envelope.
    ///
    Filter< RMS > _envelope;

    //--------------------------------------------------------------------------
    /// @brief      RMS filter to extract signal envelope.
    ///
    Maximum _peak_detector;
};

}  // namespace trigno::tools

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_MVC_HPP_
