#include <iostream>
#include <fstream>
#include <cctype>
#include <unordered_map>
#include <string>
#include <vector>
#include <chrono>

bool is_separator(unsigned char c)
{
    return std::isspace(c) || std::ispunct(static_cast<unsigned char>(c));
}

std::unordered_map<std::string, size_t> count_word_frequencies(std::istream &is, size_t &total_words)
{
    constexpr size_t BUF_SIZE = 1 << 20; // 1MB
    std::vector<char> buffer(BUF_SIZE);
    std::unordered_map<std::string, size_t> word_freq;
    std::string word;
    total_words = 0;

    while (is)
    {
        is.read(buffer.data(), buffer.size());
        std::streamsize bytesRead = is.gcount();
        for (std::streamsize i = 0; i < bytesRead; ++i)
        {
            unsigned char c = static_cast<unsigned char>(buffer[i]);
            if (std::isalpha(c) || std::isdigit(c))
                word += std::tolower(c);
            else if (!word.empty() && is_separator(c))
            {
                ++word_freq[word];
                ++total_words;
                word.clear();
            }
        }
    }
    if (!word.empty())
    {
        ++word_freq[word];
        ++total_words;
    }

    return word_freq;
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
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    auto start = std::chrono::high_resolution_clock::now();
    size_t total_words = 0;
    auto freq = count_word_frequencies(f, total_words);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Total word count: " << total_words << '\n';
    std::cout << "Unique words: " << freq.size() << '\n';
    std::cout << "Elapsed time: " << elapsed.count() << " seconds\n";
    std::cout << "Words found:\n";
    for (const auto &pair : freq)
        std::cout << pair.first << ": " << pair.second << '\n';
    return 0;
}