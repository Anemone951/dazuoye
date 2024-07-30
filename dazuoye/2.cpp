#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <thread>
#include <mutex>

using namespace std;

mutex count_mutex;

void count_words(const string& filename, unordered_map<string, int>& word_count) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Unable to open file: " << filename << endl;
        return;
    }
    string word;
    while (file >> word) {
        lock_guard<mutex> guard(count_mutex);
        word_count[word]++;
    }
    file.close();
}

void count_words_in_files(int start, int end, unordered_map<string, int>& word_count) {
    for (int i = start; i < end; i++) {
        string filename = "random_file_" + to_string(i + 1) + ".txt";
        count_words(filename, word_count);
    }
}

int main() {
    unsigned int num_files = 20;
    vector<thread> threads;
    int num_threads = 4; // 使用4个线程
    int files_per_thread = num_files / num_threads;

    unordered_map<string, int> word_count;

    for (int i = 0; i < num_threads; i++) {
        int start = i * files_per_thread;
        int end = (i == num_threads - 1) ? num_files : start + files_per_thread;
        threads.emplace_back(count_words_in_files, start, end, ref(word_count));
    }

    for (auto& t : threads) {
        t.join();
    }

    priority_queue<pair<int, string>> top_words;
    for (const auto& word : word_count) {
        top_words.emplace(word.second, word.first);
    }

    cout << "Top 1000 words:" << endl;
    for (int i = 0; i < 1000 && !top_words.empty(); i++) {
        auto top = top_words.top();
        top_words.pop();
        cout << top.second << ": " << top.first << endl;
    }

    return 0;
}
