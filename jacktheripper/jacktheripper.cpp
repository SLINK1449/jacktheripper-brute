#include <iostream>
#include <ctime>
#include <vector>
#include <thread>
#include <atomic>
#include <string>

const char* chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-_=+[]{}|;:'\\\",.<>?/`~";
const int ALPHABET_SIZE = 94; // 26 lowercase + 26 uppercase + 10 digits + 32 symbols
std::atomic<bool> found(false);
std::string target = "10";

// Holds the starting prefixes to distribute to threads
std::vector<std::string> prefixes;

// Match two char arrays directly (no std::string creation)
bool match(const char* a, const char* b, int len) {
    for (int i = 0; i < len; ++i) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

// Generate all string combinations up to given prefix length (e.g., "aa", "ab", ...)
void generatePrefixes(std::string current, int depth, int maxDepth) {
    if (depth == maxDepth) {
        prefixes.push_back(current);
        return;
    }

    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        generatePrefixes(current + chars[i], depth + 1, maxDepth);
    }
}

// Brute-force from a given prefix (fill the rest with combinations)
void bruteFromPrefix(std::string base) {
    int len = target.length();
    const char* t = target.c_str();
    char guess[13]; // Max 12 letters + null terminator
    guess[len] = '\0';

    // Copy base into guess
    for (int i = 0; i < base.length(); ++i) {
        guess[i] = base[i];
    }
    for (int i = base.length(); i < len; ++i) {
        guess[i] = 'a'; // Fill remaining positions with 'a'
    }

    // Brute-force the remaining letters
    while (!found.load()) {
        if (match(guess, t, len)) {
            found = true;
            std::cout << "Found: " << guess << std::endl;
            return;
        }

        // Increment like base-26 counter
        int pos = len - 1;
        while (pos >= base.length()) {
            if (guess[pos] == 'z') {
                guess[pos] = 'a';
                pos--;
            } else {
                guess[pos]++;
                break;
            }
        }

        if (pos < base.length()) break; // Finished this prefix range
    }
}

int main() {

    clock_t start ,end;
    double elapsed_time;
    start = clock();// Start timing
    int prefixLength = 2; // You can increase this to 3 or 4 for faster multi-threading
    generatePrefixes("", 0, prefixLength);

    const int THREADS = std::thread::hardware_concurrency(); // Use all available cores
    std::vector<std::thread> workers;
    int total = prefixes.size();

    // Divide the prefixes between threads
    for (int t = 0; t < THREADS; ++t) {
        workers.emplace_back([=]() {
            for (int i = t; i < total && !found.load(); i += THREADS) {
                bruteFromPrefix(prefixes[i]);
            }
        });
    }

    for (auto& th : workers) {
        th.join();
    }

    if (!found.load()) {
        std::cout << "Target not found." << std::endl;
    }
for (int i = 0; i < 100000000; i++)
{
    int x=i*i;
}
end = clock(); // End timing
    elapsed_time = double(end - start) / CLOCKS_PER_SEC; // Calculate elapsed time
    std::cout << "Elapsed time: " << elapsed_time << " seconds" << std::endl; // Print elapsed time
    return 0;
}
