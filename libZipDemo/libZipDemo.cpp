#include <zip.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>

void compress(const std::vector<std::string>& filenames, const std::string& zipFilename)
{
	int error = 0;
	zip_t* zip = zip_open(zipFilename.c_str(), ZIP_CREATE | ZIP_EXCL, &error);

	if (zip == nullptr) 
	{
		zip_error_t ziperror;
		zip_error_init_with_code(&ziperror, error);
		std::cerr << "Failed to open output file " << zipFilename << ": " << zip_error_strerror(&ziperror) << std::endl;
		zip_error_fini(&ziperror);
		return;
	}

	for (const auto& filename : filenames) 
	{
		std::ifstream file(filename, std::ios::binary);
		std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});

		zip_source_t* source = zip_source_buffer(zip, buffer.data(), buffer.size(), 0);
		if (source == nullptr)
		{
			std::cerr << "Failed to create zip source for " << filename << std::endl;
			continue;
		}

		if (zip_file_add(zip, filename.c_str(), source, ZIP_FL_OVERWRITE) < 0) 
		{
			std::cerr << "Failed to add file " << filename << " to zip: " << zip_strerror(zip) << std::endl;
			zip_source_free(source);
		}
	}

	zip_close(zip);
}

void decompress(const std::string& zipFilename, const std::string& outputFolder)
{
	//没有目录先创建目录。
	int error;
	zip_t* zip = zip_open(zipFilename.c_str(), 0, &error);
	if (zip == nullptr)
	{
		zip_error_t ziperror;
		zip_error_init_with_code(&ziperror, error);
		std::cerr << "Failed to open zip file " << zipFilename << ": " << zip_error_strerror(&ziperror) << std::endl;
		zip_error_fini(&ziperror);
		return;
	}

	zip_int64_t numEntries = zip_get_num_entries(zip, 0);
	for (zip_int64_t i = 0; i < numEntries; i++) 
	{
		zip_file_t* zfile = zip_fopen_index(zip, i, 0);
		zip_stat_t zstat;
		zip_stat_index(zip, i, 0, &zstat);

		std::string outputPath = outputFolder + "/" + zstat.name;
		std::ofstream outputFile(outputPath, std::ios::binary);

		std::vector<char> buffer(zstat.size);
		zip_fread(zfile, buffer.data(), buffer.size());
		outputFile.write(buffer.data(), buffer.size());
	}

	zip_close(zip);
}

int main() {
	const char* zip_filename = "./example.zip";

	std::vector<std::string> filenames;
	filenames.push_back("./test1.txt");
	filenames.push_back("./zip.dll");

	// 压缩文件
	compress(filenames, zip_filename);

	// 解压缩文件
	decompress(zip_filename, "./d");
	
	return 0;
}
