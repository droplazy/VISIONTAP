#ifndef MD5_H
#define MD5_H

#include <cstdint>
#include <string>

class MD5 {
public:
    MD5();
    void update(const unsigned char* buf, size_t length);
    void update(const char* buf, size_t length);
    std::string final();

    // 静态方法直接计算文件的MD5
    static std::string calculateFileMD5(const std::string& filepath);

private:
    void transform(const unsigned char block[64]);
    void init();

    uint32_t state[4];
    uint32_t count[2];
    unsigned char buffer[64];
    unsigned char digest[16];

    bool finalized;
};

#endif // MD5_H
