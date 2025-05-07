import os

LOREM = (
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
    "Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. "
    "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. "
    "Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. "
    "Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.\n"
)

def generate_large_lorem_file(filename, target_size_gb=20):
    target_size_bytes = target_size_gb * 1024**3  # 20 GB
    block = (LOREM * 1000).encode('utf-8')  # ~60KB per block
    block_size = len(block)

    written = 0
    with open(filename, 'wb') as f:
        while written < target_size_bytes:
            f.write(block)
            written += block_size
            if written % (1024**3) < block_size:
                print(f'Progress: {written / 1024**3:.2f} GB written')

    print(f'✅ File "{filename}" generated with size: {written / 1024**3:.2f} GB')

if __name__ == "__main__":
    generate_large_lorem_file("lorem_20GB.txt", target_size_gb=20)
