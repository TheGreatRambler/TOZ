#include "packetCreator.hpp"

public class TASProcess {
private:
  std::string currentFilename;
  std::ifstream currentStream;
  bool fileIsCurrentlyDone = false;
  bool isDone;
public:
  TASProcess() {}
  void setTASFile(std::string filename) {
  currentFilename = filename;
  currentStream = new std::ifstream(currentFilename, std::ifstream::in);
  if (!currentStream.is_open()) {
    // Error opening file
  }
  }
  
  void setNextInputs(Inputs currentInputs) {
    std::string currentLine;
    isDone = std::getline(currentStream, currentLine);
    if (currentStream.bad()) {
      // Some sort of error
    }
    // Have the line now
  }
  
  void closeFile() {
    currentStream.close();
  }
}
