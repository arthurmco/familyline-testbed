#include <cinttypes>
#include <common/logic/input_recorder.hpp>

using namespace familyline::logic;

InputRecorder::InputRecorder(PlayerManager& pm) : pm_(pm)
{
    pm_.addListener(std::bind(&InputRecorder::addAction, this, std::placeholders::_1));

    auto pmap = pm_.getPlayerNames();
    for( auto [id, name] : pmap) {
        pinfo_.push_back(RecordPlayerInfo{(uint64_t)id, name});
    }
}

/**
 * Create the file.
 *
 * Return true if it could create, false if it could not
 */
bool InputRecorder::createFile(std::string_view path)
{
    if (f_) fclose(f_);

    f_ = fopen(path.data(), "w");
    if (!f_)
        return false;

    fprintf(f_, "FREC\n");
    fprintf(f_, "players:\n");
    for (auto& pi : pinfo_) {
        fprintf(f_, "\tid: %" PRIx64 ", name:%s\n", pi.id, pi.name.c_str());
        
    }

    fprintf(f_, "input:\n");
    return true;
}

/**
 * You should pass this function as a callback to the player manager
 * `addListener` function
 */
bool InputRecorder::addAction(PlayerInputAction a)
{
    if (f_)
        fprintf(
            f_, "tick %" PRId32 " player %" PRIx64 " actionindex %zu\n", a.tick, a.playercode,
            a.type.index());

    return (f_ != nullptr);
}
