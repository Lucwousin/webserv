#pragma once
#include <ctime>
#include <iostream>
#include <iomanip>
#include <chrono>

class Log {
 public:
  Log() = delete;
  ~Log() = delete;
  Log(const Log&) = delete;
  Log& operator=(const Log&) = delete;

  enum Level {
   ERROR,
   WARN,
   INFO,
   DEBUG,
   TRACE
  };

  static constexpr Level log_level = TRACE;

  template<Level lvl>
  using log_enable_t = typename std::enable_if<(lvl <= log_level), void>::type;
  template<Level lvl>
  using log_disable_t = typename std::enable_if<(lvl > log_level), void>::type;

  /**
   * Log some stuff to a terminal
   *
   * if lvl is WARN or ERROR, logs to stderr, otherwise log to stdout
   * This function does not prefix the output with the log level.
   */
  template<Level lvl = INFO, typename... Ts>
  static inline log_enable_t<lvl> log(const Ts&... args) {
    constexpr std::ostream& out = (lvl == ERROR || lvl == WARN) ? std::cerr : std::cout;

    print_time(out);
    // This looks and feels dirty... The , is the comma operator so this just creates an empty array
    // but prints all arguments as a byproduct
    __attribute__((unused)) int unused[] = {((void) (out << args), 0)...};
  }

  // noop
  template<Level lvl = INFO, typename... Ts>
  static inline log_disable_t<lvl> log(const Ts&...) {}

  /*
   * All these functions log with a certain debug level.
   * the output will be prefixed with the level:
   *
   * info("Surf's up", ',', " hi ", 5, '\n')
   * would print "Surf's up, hi 5\n" to stdout (unless log level is set to ERROR or WARN
   */
  template<typename... Ts> static inline void error(const Ts&... args) { log<ERROR>("[ERROR] ", args...); }
  template<typename... Ts> static inline void warn(const Ts&... args) { log<WARN>("[WARN]  ", args...); }
  template<typename... Ts> static inline void info(const Ts&... args) { log<INFO>("[INFO]  ", args...); }
  template<typename... Ts> static inline void debug(const Ts&... args) { log<DEBUG>("[DEBUG] ", args...); }
  template<typename... Ts> static inline void trace(const Ts&... args) { log<TRACE>("[TRACE] ", args...); }

 private:
  static void print_time(std::ostream& to) {
    using namespace std::chrono;
    auto now = system_clock::now();
    int ms = (int)(duration_cast<milliseconds>(now.time_since_epoch()) % 1000).count();
    time_t timer = system_clock::to_time_t(now);
    to << std::put_time(std::localtime(&timer), "%d/%m %T.") << std::setw(3) << std::setfill('0') << ms << std::setw(0) << " ";
  }
};
