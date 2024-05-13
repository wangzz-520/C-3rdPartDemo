#include <iostream>
#include <snappy.h>
#include <string>
#include <iostream>
#include <fstream>

// 压缩文件
bool compressFile(const std::string& input_filename, const std::string& output_filename) 
{
	std::ifstream input(input_filename, std::ios::binary);
	if (!input.is_open()) 
	{
		std::cerr << "Failed to open input file." << std::endl;
		return false;
	}

	std::string input_data((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	input.close();

	std::string compressed;
	snappy::Compress(input_data.data(), input_data.size(), &compressed);

	std::ofstream output(output_filename, std::ios::binary);
	if (!output.is_open()) {
		std::cerr << "Failed to open output file." << std::endl;
		return false;
	}

	output.write(compressed.data(), compressed.size());
	output.close();

	return true;
}

// 解压缩文件
bool decompressFile(const std::string& input_filename, const std::string& output_filename) 
{
	std::ifstream input(input_filename, std::ios::binary);
	if (!input.is_open())
	{
		std::cerr << "Failed to open input file." << std::endl;
		return false;
	}

	std::string compressed((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	input.close();

	std::string decompressed;
	if (!snappy::Uncompress(compressed.data(), compressed.size(), &decompressed)) 
	{
		std::cerr << "Failed to decompress the data." << std::endl;
		return false;
	}

	std::ofstream output(output_filename, std::ios::binary);
	if (!output.is_open())
	{
		std::cerr << "Failed to open output file." << std::endl;
		return false;
	}

	output.write(decompressed.data(), decompressed.size());
	output.close();

	return true;
}

int main() 
{
	const std::string input_file = "E:/2.mp4";
	const std::string compressed_file = "E:/2.snappy";
	const std::string decompressed_file = "E:/3.mp4";

	// 压缩文件
	if (compressFile(input_file, compressed_file)) {
		std::cout << "File compressed successfully." << std::endl;
	}
	else {
		std::cerr << "Failed to compress file." << std::endl;
		return 1;
	}

	// 解压缩文件
	if (decompressFile(compressed_file, decompressed_file)) {
		std::cout << "File decompressed successfully." << std::endl;
	}
	else {
		std::cerr << "Failed to decompress file." << std::endl;
		return 1;
	}

	return 0;
}

int main2()
{
	// 原始数据
	std::string input = "This is a string that we want to compress using Snappy.";

	// 压缩数据
	std::string compressed;
	snappy::Compress(input.data(), input.size(), &compressed);

	std::cout << "Compressed size: " << compressed.size() << std::endl;

	// 解压缩数据
	std::string decompressed;
	if (!snappy::Uncompress(compressed.data(), compressed.size(), &decompressed)) 
	{
		std::cerr << "Failed to decompress the data." << std::endl;
		return 1;
	}

	std::cout << "Decompressed data: " << decompressed << std::endl;

	return 0;
}