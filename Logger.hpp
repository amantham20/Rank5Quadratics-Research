#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

class CSVLogger {
public:
    CSVLogger(const std::string& filename) {
      // Check if file exists
      if (!std::filesystem::exists(filename)) {
        std::cout << "File does not exist, creating new file: " << filename << std::endl;
      }

      // Open file for writing and append if it already exists
      file.open(filename, std::ios::out | std::ios::app);

      if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
      }
    }

    ~CSVLogger() {
      if (file.is_open()) {
        file.close();
      }
    }

    template <typename... Args>
    void log(Args... args) {
      if (file.is_open()) {
        writeToFile(std::forward<Args>(args)...);
      }
    }

private:
    std::ofstream file;

    template <typename T, typename... Args>
    void writeToFile(const T& value, const Args&... args) {
      file << value;
      ((file << ", " << args), ...);
      file << std::endl;
    }
};
