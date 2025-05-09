#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <cctype>
#include <chrono>
#include <omp.h>

constexpr size_t BLOCK_SIZE = 1 << 22; // 4MB

void process_chunk(std::ifstream &file, size_t start, size_t end,
                   std::unordered_map<std::string, size_t> &local_map, size_t &local_count)
{
    size_t chunk_size = end - start;
    std::vector<char> buffer(chunk_size);

    file.seekg(start);
    file.read(buffer.data(), chunk_size);
    size_t bytes_read = file.gcount();

    std::string word;
    for (size_t i = 0; i < bytes_read; ++i)
    {
        char c = buffer[i];
        if (std::isalnum(static_cast<unsigned char>(c)))
            word += std::tolower(c);
        else if (!word.empty())
        {
            ++local_map[word];
            ++local_count;
            word.clear();
        }
    }

    if (!word.empty())
    {
        ++local_map[word];
        ++local_count;
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " file.txt\n";
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream test_file(filename, std::ios::binary | std::ios::ate);
    if (!test_file)
    {
        std::cerr << "Cannot open file.\n";
        return 1;
    }

    size_t file_size = test_file.tellg();
    test_file.close();

    auto start_time = std::chrono::high_resolution_clock::now();

    int num_threads = omp_get_max_threads();
    size_t num_blocks = (file_size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    std::vector<std::unordered_map<std::string, size_t>> thread_maps(num_blocks);
    std::vector<size_t> word_counts(num_blocks, 0);

#pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < num_blocks; ++i)
    {
        size_t start = i * BLOCK_SIZE;
        size_t end = std::min(start + BLOCK_SIZE, file_size);

        std::ifstream file(filename, std::ios::binary);
        if (!file)
            continue;
        if (i != 0)
        {
            file.seekg(start);
            char c;
            while (start < end && file.get(c) && std::isalnum(static_cast<unsigned char>(c)))
                ++start;
        }
        if (end < file_size && i != num_blocks - 1)
        {
            file.seekg(end);
            char c;
            while (end < file_size && file.get(c) && std::isalnum(static_cast<unsigned char>(c)))
                ++end;
        }
        process_chunk(file, start, end, thread_maps[i], word_counts[i]);
    }

    std::unordered_map<std::string, size_t> global_map;
    size_t total_words = 0;

    for (size_t i = 0; i < num_blocks; ++i)
    {
        for (const auto &pair : thread_maps[i])
            global_map[pair.first] += pair.second;
        total_words += word_counts[i];
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << "Total word count: " << total_words << '\n';
    std::cout << "Unique words: " << global_map.size() << '\n';
    std::cout << "Elapsed time: " << elapsed.count() << " seconds\n";
    std::cout << "Words found:\n";
    for (const auto &pair : global_map)
        std::cout << pair.first << ": " << pair.second << '\n';

    return 0;
}
