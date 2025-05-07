#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <omp.h>

constexpr size_t CHUNK_SIZE = 1 << 22; // 4MB

bool is_space(unsigned char c)
{
    return c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v' || c == '\f';
}

size_t count_words_chunk(const char *data, size_t size, bool &starts_with_word, bool &ends_with_word)
{
    size_t count = 0;
    bool in_word = false;

    for (size_t i = 0; i < size; ++i)
    {
        if (is_space((unsigned char)data[i]))
            in_word = false;
        else if (!in_word)
        {
            in_word = true;
            ++count;
        }
    }

    starts_with_word = !is_space((unsigned char)data[0]);
    ends_with_word = !is_space((unsigned char)data[size - 1]);
    return count;
}

size_t wc_parallel(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file)
    {
        std::cerr << "Cannot open file\n";
        return 0;
    }

    size_t file_size = file.tellg();
    file.seekg(0);

    size_t num_chunks = (file_size + CHUNK_SIZE - 1) / CHUNK_SIZE;

    std::vector<size_t> counts(num_chunks);
    std::vector<bool> starts(num_chunks), ends(num_chunks);

#pragma omp parallel for
    for (size_t i = 0; i < num_chunks; ++i)
    {
        size_t offset = i * CHUNK_SIZE;
        size_t size = std::min(CHUNK_SIZE, file_size - offset);
        std::vector<char> buffer(size);

#pragma omp critical
        {
            file.seekg(offset);
            file.read(buffer.data(), size);
        }

        bool starts_with_word, ends_with_word;
        counts[i] = count_words_chunk(buffer.data(), size, starts_with_word, ends_with_word);
        starts[i] = starts_with_word;
        ends[i] = ends_with_word;
    }

    // Adjust counts for word splits between chunks
    for (size_t i = 1; i < num_chunks; ++i)
        if (ends[i - 1] && starts[i])
            counts[i]--; // split word was counted twice

    size_t total = 0;
    for (auto c : counts)
        total += c;
    return total;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " file.txt\n";
        return 1;
    }

    auto start = std::chrono::high_resolution_clock::now();

    size_t count = wc_parallel(argv[1]);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Word count: " << count << '\n';
    std::cout << "Elapsed time: " << elapsed.count() << " seconds\n";

    return 0;
}
