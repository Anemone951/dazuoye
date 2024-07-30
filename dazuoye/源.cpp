#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

string randomword(int length) {
	string word;
	static const char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
	for (int i = 0; i < length; i++) {
		word += alphabet[ rand() % (sizeof(alphabet) - 1) ] ;
	}
	return word;
}

string randomline() {
	int num_words = rand() % 11 + 5;
	string line;
	for (int i = 0; i < num_words; i++) {
		if (i > 0)line += " ";
		line += randomword(rand() % 17 + 4);
	}
	return line;
}

void file(const string& filename,int num_lines) {
	ofstream file;
	file.open(filename,ios::out);
	if (!file.is_open()) {
		cerr << "Unable to open file: " << filename << endl;
		return;
	}
	cout << "Writing " << num_lines << " lines to " << filename << endl;
	for (int i = 0; i < num_lines; i++) {
		file << randomline() << "\n";
	}
	file.close();
}

int main() {
	srand(static_cast<unsigned int>(time(0)));

	unsigned int num_files = 20;
	int min_lines = 500000;
	int max_lines = 4000000;
	vector<int> files;

	while (files.size() < num_files) {
		int num_lines = rand() % (max_lines - min_lines + 1) + min_lines;
		files.push_back(num_lines);
	}

	while (*max_element(files.begin(),files.end())- *min_element(files.begin(), files.end())<2000000)
	{
		files[rand() % num_files] = rand() % (max_lines - min_lines + 1) + min_lines;
	}

	for (int i = 0; i < num_files; i++) {
		string filename = "random_file_" + to_string(i + 1) + ".txt";
		file(filename, files[i]);
		cout << "Generated" << filename << "with" << files[i] << "lines." << endl;
	}
	return 0;
}