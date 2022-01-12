#ifndef TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_EXPORTER_HPP_
#define TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_EXPORTER_HPP_

#include <string>
#include <fstream>
#include <mutex>
#include <atomic>
#include <thread>
#include "std/basic_executor.hpp"              // std::basic_executor
#include "core/sequence.hpp"                   // trigno::Sequence
#include "tools/basic_sequence_processor.hpp"  // trigno::tools::BasicSequenceProcessor
#include "tools/iterative.hpp"                 // trigno::tools::Iterative

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
    /// @brief      Gets the export target file.
    ///
    /// @returns    Local/global path to target file.
    ///
    const std::string& target() const noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Sets the source sequence. Useful to disable frame removal @runtime.
    ///
    /// @param[in]  path     Source sequence. A null pointer disables frame removal.
    ///
    void source(Sequence* data = nullptr) noexcept;

    //--------------------------------------------------------------------------
    /// @brief      Gets source data sequence.
    ///
    /// @returns    Const pointer to source sequence.
    ///
    const Sequence* source() const noexcept;

 protected:
    //--------------------------------------------------------------------------
    /// @brief      Exports a single *DataFrame* from the data client.
    ///
    /// @param[in]  range    Range of frames to export.
    /// @param[in]  sensors  Sensors w/ data to export.. Defaults to all available sensors.
    ///
    /// @note       *sensors* are ignored, current implementation only saves data for all available sensors.
    ///
    void execute(Sequence::Range&& range, const sensor::List& sensors = sensor::all) override;

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

    //--------------------------------------------------------------------------
    /// @brief      Class friendship w/ Iterative< Exporter > allows its members access to Exported protected members.
    ///
    friend class Iterative< Exporter >;
};



//------------------------------------------------------------------------------
/// @brief      Un-looped function called before execute() loop.
///
/// @note       Specialization of Iterative< >::start() for Exporter processor type.
///
template < /* specialization for iterative (continuous) exporter */ >
void Iterative< Exporter >::start();

//------------------------------------------------------------------------------
/// @brief      Check if execution condition is verified.
///
/// @return     True if active, false otherwise.
///
/// @note       Pure virtual, derived types must provide their own implementation.
///
template < /* specialization for iterative (continuous) exporter */ >
bool Iterative< Exporter >::active() const;

}  // namespace trigno::tools

#endif  // TRIGNOCLIENT_INCLUDE_TRIGNOCLIENT_EXPORTER_HPP_
