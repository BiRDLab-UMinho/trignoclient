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
#include "sequence.hpp"                  // trigno::Sequence

namespace trigno::tools {

//------------------------------------------------------------------------------
/// @brief      Exporter class that saves multiple frames into a local file.
///
class Exporter : public BasicSequenceProcessor {
 public:
    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance, initializing a file stream from given *path*.
    ///
    /// @param[in]  file  Local target file.
    /// @param[in]  data  Input sequence, required to remove exported frames. Defaults to nullptr i.e. frames are only exported, but kept in memory.
    ///
    explicit Exporter(std::string& path, Sequence* data = nullptr);

    //--------------------------------------------------------------------------
    /// @brief      Constructs a new instance, initializing a file @ given *path* with matching header from *sensors*.
    ///
    /// @param[in]  file     Local target file.
    /// @param[in]  sensors  Sensors used to initialize file header
    /// @param[in]  data     Input sequence, required to remove exported frames. Defaults to nullptr i.e. frames are only exported, but kept in memory.
    ///
    explicit Exporter(std::string& path, const sensor::List& sensors, Sequence* data = nullptr);

    //--------------------------------------------------------------------------
    /// @brief      Destroys the object.    
    ///
    ~Exporter();

    //--------------------------------------------------------------------------
    /// @brief      Initializes a file from system configuration i.e. with headers matching sensor labels.
    ///
    /// @todo       Return type is redundant as std::ofstream has no copy constructor!
    ///
    static std::ofstream file(const std::string& path, const sensor::List& sensors);

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Exports a single *DataFrame* from the data client.
    ///
    void execute() override;

    //--------------------------------------------------------------------------
    /// @brief      Pointer to source sequence.
    ///
    Sequence* _data;

    //--------------------------------------------------------------------------
    /// @brief      Mutex to claim thread ownership during execution.
    ///
    std::mutex _mutex;

    //--------------------------------------------------------------------------
    /// @brief      Target file path.
    ///
    std::string _path;

    //--------------------------------------------------------------------------
    /// @brief      Output file stream.
    ///
    std::ofstream _file;
};

}  // namespace trigno::tools

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_EXPORTER_HPP_
