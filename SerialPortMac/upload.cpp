//
// Created by Aaron Ye on 1/8/23.
//

#include "utils.h"

#define BAUD_RATE 9600

int main(int argc, const char * argv[]) {

    if (argc == 1 || (utils::find_args(argv, argc, "-h")) || utils::find_args(argv, argc, "-help")) {
        string help =
                "Please run this tools with arguments\n"
                "Usage:\tUploader <fullpath> [options]\n"
                "\t\t-v\t--visible\t显示详细输入输出\n"
                "\t\t  \t--noreset\t寻找到W2开发板之后，不再次等待板重置，直接进入上载过程\n"
                "\t\t-h\t--help\t帮助菜单\n"
                "\t\t-r\t--run\t在上传之后立即运行程序\n";
        printf("%s", help.c_str());
        return 0;
    }

    SerialPort sp;
    const char* port = utils::getPort();
    cout << "port = " << port << "\n";
    sp.open_port(port, BAUD_RATE);

    {
        std::this_thread::sleep_for (std::chrono::milliseconds (500));
        sp.write_bytes("\n",1); // start
        cout << "sent start\n";
        std::this_thread::sleep_for (std::chrono::milliseconds (500));
    }

    utils::set_up_flash(sp);

    long long address = 0x10000000;
    {
        printf("file = %s\n", argv[1]);
        vector<unsigned char> buffer;
        utils::read_bin(buffer, argv[1]);
        printf("finished reading from bin\n");
        utils::upload_bin(sp, address, buffer);
        printf("finished uploading buffer\n");
    }

    if ((utils::find_args(argv, argc, "-r")) || utils::find_args(argv, argc, "-run")) {
        std::this_thread::sleep_for (std::chrono::milliseconds (100));
        printf("jumping the code\n");
        utils::jump(sp,address);
        printf("finished jump\n");
    }

    sp.close_port();
    return 0;
}