#include "engine_race.h"

namespace polar_race {
RetCode Engine::Open(const std::string &name, Engine **eptr) {
  return EngineRace::Open(name, eptr);
}
Engine::~Engine() {}
} // namespace polar_race
