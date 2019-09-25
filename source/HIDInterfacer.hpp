#pragma once

#include <stdio.h> 
#include <fcntl.h> 
#include <errno.h>

#include "packetStructure.hpp"

namespace HID {
  int hidgStreamFileNum;
  void Open() {
    // Open for both reading and writing
    // Also open in binary mode
    // C system calls need to be used because they are low level
    // 0666 is permission
    hidgStreamFileNum = open("/dev/hidg0", O_RDWR | O_DIRECT | O_SYNC, 0666);
  }
  
  void Close() {
    close(hidgStreamFileNum);
  }
  
  void Write(char* data) {
    // Write with C write command
    write(hidgStreamFileNum, &data, strlen(data));
  }
}
