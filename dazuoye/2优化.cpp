#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <thread>
#include <mutex>
#include <map>
#include <iomanip>

using namespace std;

// 分段锁的数量
const int NUM_BUCKETS = 128;
vector<mutex> bucket_mutexes(NUM_BUCKETS);

// 根据单词计算其对应的桶索引
int get_bucket_index(const string& word) {
    hash<string> hasher;
    return hasher(word) % NUM_BUCKETS;
}

void count_words(const string& filename, vector<unordered_map<string, int>>& bucket_word_counts, int thread_id, int file_index, int total_files) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Unable to open file: " << filename << endl;
        return;
    }
    string word;
    while (file >> word) {
        int bucket_index = get_bucket_index(word);
        lock_guard<mutex> guard(bucket_mutexes[bucket_index]);
        bucket_word_counts[bucket_index][word]++;
    }
    file.close();

    // 打印处理进度
    {
        static mutex progress_mutex;
        lock_guard<mutex> guard(progress_mutex);
        cout << "Thread " << thread_id << " processed file " << (file_index + 1) << "/" << total_files << ": " << filename << endl;
    }
}

void count_words_in_files(int start, int end, vector<unordered_map<string, int>>& bucket_word_counts, int thread_id, int total_files) {
    for (int i = start; i < end; ++i) {
        string filename = "random_file_" + to_string(i + 1) + ".txt";
        count_words(filename, bucket_word_counts, thread_id, i - start, end - start);
    }
}

int main() {
    unsigned int num_files = 20;
    vector<thread> threads;
    int num_threads = 4; // 后续增加线程数量
    int files_per_thread = num_files / num_threads;

    // 使用分段锁存储单词计数
    vector<unordered_map<string, int>> bucket_word_counts(NUM_BUCKETS);

    for (int i = 0; i < num_threads; ++i) {
        int start = i * files_per_thread;
        int end = (i == num_threads - 1) ? num_files : start + files_per_thread;
        threads.emplace_back(count_words_in_files, start, end, ref(bucket_word_counts), i, num_files);
    }

    for (auto& t : threads) {
        t.join();
    }

    // 合并所有桶的结果
    unordered_map<string, int> word_count;
    for (int i = 0; i < NUM_BUCKETS; ++i) {
        for (const auto& pair : bucket_word_counts[i]) {
            word_count[pair.first] += pair.second;
        }
    }

    // 使用优先队列获取前1000个高频单词
    priority_queue<pair<int, string>> top_words;
    for (const auto& word : word_count) {
        top_words.emplace(word.second, word.first);
    }

    cout << "Top 1000 words:" << endl;
    for (int i = 0; i < 1000 && !top_words.empty(); ++i) {
        auto top = top_words.top();
        top_words.pop();
        cout << setw(4) << (i + 1) << ": " << top.second << " - " << top.first << " times" << endl;
    }

    return 0;
}
