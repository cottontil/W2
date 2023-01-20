//
// Created by Aaron Ye on 1/8/23.
//

#include "utils.h"

void wait_for_ack(SerialPort& sp) {
    while(sp.last_read != 0x6) {
//        printf("last read = %x\n", sp.last_read);
        std::this_thread::sleep_for (std::chrono::milliseconds (5));
    }
    sp.last_read = 0x0;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

bool utils::find_args(const char* argv[], int argc, const char* arg) {
    for(ssize_t idx = 0; idx < argc; idx++) {
        if(strcmp(argv[idx], arg) == 0) return 1;
    }
    return 0;
}

std::string byteToAsciiString(unsigned char c) {
    int ascii_val = static_cast<int>(c);
    std::stringstream hex_value;
    if(ascii_val <= 0xf) hex_value << "0";
    hex_value << std::hex << ascii_val;
    return hex_value.str();
}

void write_words(SerialPort& sp, long long address, unsigned char* bytes, int words) {
    std::stringstream stream;
    stream << WRITE << std::hex << address << " " << words << " ";
    for(ssize_t word = 0; word < words; word++) {
        for(ssize_t offset = WORD_BYTES-1; offset >= 0; offset--) {
            stream << byteToAsciiString(*(bytes + offset + word * WORD_BYTES));
        }
        if (word < words-1) {
            stream << " ";
        }
        else {
            stream << "\r";
        }
    }
    std::string req = stream.str();
    sp.write_bytes(req.c_str(), req.size());
    cout << req << "\n";
    wait_for_ack(sp);
}

void write_bytes(SerialPort& sp, long long address, unsigned char* buffer, ssize_t bytes) {
    for(ssize_t offset = 0; offset < bytes; offset += WORD_BYTES) {
        write_words(sp, address, buffer + offset,1);
        address += WORD_BYTES;
    }
}

void write_bytes_fast(SerialPort& sp, long long address, unsigned char* buffer, ssize_t bytes) {
    ssize_t offset = 0;
    while (offset < bytes) {
        if (bytes - offset >= 16) {
            write_words(sp, address + offset, buffer + offset, 16);
            offset += 16 * WORD_BYTES;
        }
        else {
            write_words(sp,address + offset, buffer + offset, 1);
            offset += WORD_BYTES;
        }
    }
}

void utils::upload_bin(SerialPort& sp, long long address, std::vector<unsigned char>& buffer) {
//    write_bytes(sp, address, &buffer[0], buffer.size());
    write_bytes_fast(sp, address, &buffer[0], buffer.size());
}

void utils::read_bin(std::vector<unsigned char>& buffer, const std::string& path) {
    std::ifstream bin_file(path, std::ios::binary);
    if (bin_file.good()) {
        std::vector<uint8_t> v_buf((std::istreambuf_iterator<char>(bin_file)), (std::istreambuf_iterator<char>()));
        buffer = v_buf;
        bin_file.close();
    }
    else throw std::exception();
}

void utils::set_up_flash(SerialPort& sp) {
    sp.write_bytes("a",1); // console mode
    cout << "sent console mode\n";
    std::this_thread::sleep_for (std::chrono::milliseconds (500));
    sp.last_read = 0x0;

    {
        const char* instr = "w1f800004 1 40\r"; // disable special sequence
        sp.write_bytes(instr,strlen(instr));
        cout << "sent special sequence\n";
        wait_for_ack(sp);
    }
    // {
    //     const char* instr = "w1f800b07 1 a5\r"; // unlock flash
    //     sp.write_bytes(instr,strlen(instr));
    //     cout << "unlocked flash\n";
    //     wait_for_ack(sp);
    // }
    // {
    //     const char* instr = "w10300000 1 00\r"; // erase whole flash
    //     sp.write_bytes(instr,strlen(instr));
    //     cout << "erased whole flash\n";
    //     wait_for_ack(sp);
    // }
}

const char* utils::getPort() {
  char lpTargetPath[5000];
  for (int i = 0; i < 255; i++)
  {
      std::string str = "COM" + std::to_string(i);
      DWORD test = QueryDosDevice(str.c_str(), lpTargetPath, 5000);

      if (test != 0) //QueryDosDevice returns zero if it didn't find an object
      {
        return str.c_str();
      }

      if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER) {}
  }

  throw "unable to find serial port\n";
}

void utils::jump(SerialPort& sp, long long address) {
    const char* instr = "j10000000\r";
    sp.write_bytes(instr,strlen(instr));
}
