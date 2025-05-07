#include <iostream>
#include <fstream>
#include <cctype>
#include <vector>
#include <chrono>

size_t wc_fast(std::istream &is)
{
    constexpr size_t BUF_SIZE = 1 << 20; // 1MB buffer
    std::vector<char> buffer(BUF_SIZE);
    size_t count = 0;
    bool in_word = false;

    while (is)
    {
        is.read(buffer.data(), buffer.size());
        std::streamsize bytesRead = is.gcount();

        for (std::streamsize i = 0; i < bytesRead; ++i)
        {
            if (std::isspace(static_cast<unsigned char>(buffer[i])))
            {
                in_word = false;
            }
            else if (!in_word)
            {
                in_word = true;
                ++count;
            }
        }
    }

    return count;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " file.txt\n";
        return 1;
    }

    std::ifstream f(argv[1], std::ios::in | std::ios::binary);
    if (!f)
    {
        std::cerr << "Cannot open file.\n";
        return 1;
    }

    std::ios::sync_with_stdio(false); // acelerar std::cout
    std::cin.tie(nullptr);            // desacoplar cout/cin

    auto start = std::chrono::high_resolution_clock::now();

    size_t word_count = wc_fast(f);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Word count: " << word_count << '\n';
    std::cout << "Elapsed time: " << elapsed.count() << " seconds\n";

    return 0;
}
