#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_SCALER_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_SCALER_HPP_

#include <mutex>
#include "basic_sequence_processor.hpp"  // trigno::tools:BasicSequenceProcessor
#include "sensor.hpp"                    // trigno::sensor::List

namespace trigno::tools {

//------------------------------------------------------------------------------
/// @brief      Simple processor tool that scales frame sample according to a given sensor-specific scale factors.
///
/// @note       Useful to normalize raw data (e.g. using measured MVC).
///
class Scaler : public BasicSequenceProcessor {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance with given *scale* factor.
    ///
    /// @param[in]  scale  Frame instance with scale factors to apply to sequence frames.
    ///
    explicit Scaler(float scale);

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Exports a single Frame from the data client.
    ///
    void execute() override;

    //--------------------------------------------------------------------------
    /// @brief      Scale factors to apply to sequence values (as a multi-sensor frame).
    ///
    Frame _scale;

    //--------------------------------------------------------------------------
    /// @brief      Mutex to claim thread ownership during execution.
    ///
    /// @note       Required as scaling values entails modifying non-atomic types in-place.
    ///
    std::mutex _mutex;
};

}  // namespace trigno::tools

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_EXPORTER_HPP_
