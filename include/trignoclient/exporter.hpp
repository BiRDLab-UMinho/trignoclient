#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_EXPORTER_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_EXPORTER_HPP_

#include <string>
#include <fstream>
#include <mutex>
#include <atomic>
#include <thread>
#include "std/basic_executor.hpp"        // std::basic_executor
#include "configuration.hpp"             // trigno::network::MultiSensorConfiguration
#include "basic_sequence_processor.hpp"  // trigno::tools::BasicSequenceProcessor
#include "iterative.hpp"                 // trigno::tools::Iterative
#include "sequence.hpp"                  // trigno::Sequence

namespace trigno::tools {

//------------------------------------------------------------------------------
/// @brief      Exporter class that saves multiple frames into a local file.
///
class Exporter : public BasicSequenceProcessor {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance exporting to given *path*.
    ///
    /// @param[in]  path     Path to local target file.
    /// @param[in]  data     Input sequence, required to remove exported frames. Defaults to nullptr i.e. frames are only exported, but kept in memory.
    ///
    explicit Exporter(const std::string& path = "", Sequence* data = nullptr, char delimiter = ',');

    //--------------------------------------------------------------------------
    /// @brief      Sets the export target file.
    ///
    /// @param[in]  path     Path to local target file.
    ///
    void target(const std::string& path) noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Sets the source sequence. Useful to disable frame removal @runtime.
    ///
    /// @param[in]  path     Source sequence. A null pointer disables frame removal.
    ///
    void source(Sequence* data = nullptr) noexcept;

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Exports a single *DataFrame* from the data client.
    ///
    void execute() override;

    //--------------------------------------------------------------------------
    /// @brief      Target file path.
    ///
    std::string _path;

    //--------------------------------------------------------------------------
    /// @brief      Pointer to source sequence.
    ///
    Sequence* _data;

    //--------------------------------------------------------------------------
    /// @brief      Pointer to source sequence.
    ///
    char _delimiter;

    //--------------------------------------------------------------------------
    /// @brief      Mutex to claim thread ownership during execution.
    ///
    std::mutex _mutex;
};



//------------------------------------------------------------------------------
/// @brief      Executes operation, i.e. calls run() on member executor instance.
///
/// @note       In order to use Iterator class w/ Exporter processor, execute() needs to be modified.
///             Range iterator is not incremented as elements are being removed from sequence.
///
template < /* specialization for iterative (continuous) exporter */ >
void Iterative< Exporter >::execute();

}  // namespace trigno::tools

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_EXPORTER_HPP_
