#include <fstream>
#include <iostream>
#include <filesystem>
#include <mutex>

class CSVLogger {
public:
  CSVLogger(const std::string& filename) {
    if (!std::filesystem::exists(filename)) {
      std::cout << "File does not exist, creating new file: " << filename << std::endl;
    }
    file_.open(filename, std::ios::out | std::ios::app);
    if (!file_.is_open()) {
      std::cerr << "Failed to open file: " << filename << std::endl;
    }
  }

  ~CSVLogger() {
    std::scoped_lock lock(mutex_);
    if (file_.is_open()) file_.close();
  }

  // move-only
  CSVLogger(CSVLogger&& other) noexcept {
    std::scoped_lock lock_other(other.mutex_);
    file_.swap(other.file_);
  }
  CSVLogger& operator=(CSVLogger&& other) noexcept {
    if (this != &other) {
      std::scoped_lock lock_this(mutex_, other.mutex_);
      file_.close();
      file_.swap(other.file_);
    }
    return *this;
  }

  CSVLogger(const CSVLogger&)            = delete;
  CSVLogger& operator=(const CSVLogger&) = delete;

  template <typename... Args>
  void log(Args&&... args) {
    std::scoped_lock lock(mutex_);
    if (!file_.is_open()) return;
    writeToFile(std::forward<Args>(args)...);
    file_.flush();
  }

private:
  std::ofstream file_;
  mutable std::mutex mutex_;

  template <typename T, typename... Rest>
  void writeToFile(const T& value, const Rest&... rest) {
    file_ << value;
    ( (file_ << "," << rest), ... );
    file_ << "\n";
  }
};
