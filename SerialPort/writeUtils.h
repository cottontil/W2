//
// Created by Aaron Ye on 8/13/22.
//

#ifndef W2_WRITEUTILS_H
#define W2_WRITEUTILS_H

#define WORD_BYTES 4
#define WRITE "w"
#define READ "r"
#define WAIT 50

#include <string>
#include <vector>

namespace writeUtils {

    void writeBytes(long long address, unsigned char *buffer, int bytes);

    void writeWord(long long address, unsigned char* word);

    const char* formattedReadInstruction(long long address, int length);

    std::string byteToAsciiString(unsigned char c);

    void uploadBin(long long address, std::vector<unsigned char>& buffer);

}


#endif //W2_WRITEUTILS_H
