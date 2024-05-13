#include <iostream>
#include <zlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <vector>

using namespace std;

bool compressFile(const std::string& sourceFilename, const std::string& compressedFilename) 
{
	std::ifstream source(sourceFilename, std::ios_base::binary);
	std::ofstream dest(compressedFilename, std::ios_base::binary);

	if (!source || !dest)
	{
		std::cerr << "Could not open source or destination file." << std::endl;
		return false;
	}

	z_stream zs;
	memset(&zs, 0, sizeof(zs));

	if (deflateInit(&zs, Z_DEFAULT_COMPRESSION) != Z_OK) 
	{
		std::cerr << "deflateInit failed while compressing." << std::endl;
		return false;
	}

	// 压缩缓冲区
	const size_t bufSize = 128 * 1024;
	std::vector<char> inBuffer(bufSize);
	std::vector<char> outBuffer(bufSize);

	int deflateStatus;
	do 
	{
		// 读取源文件内容到缓冲区
		source.read(inBuffer.data(), inBuffer.size());
		zs.next_in = reinterpret_cast<Bytef*>(inBuffer.data());
		zs.avail_in = source.gcount();	//实际读取字节数

		do 
		{
			zs.next_out = reinterpret_cast<Bytef*>(outBuffer.data());
			zs.avail_out = outBuffer.size();	//next_out剩余可用空间

			deflateStatus = deflate(&zs, source.eof() ? Z_FINISH : Z_NO_FLUSH);

			if (deflateStatus == Z_STREAM_ERROR)
			{
				std::cerr << "Deflate failed during compression." << std::endl;
				deflateEnd(&zs);
				return false;
			}

			// 写入压缩数据到目标文件
			int have = outBuffer.size() - zs.avail_out;
			dest.write(outBuffer.data(), have);
		} while (zs.avail_out == 0);

	} while (deflateStatus != Z_STREAM_END);

	deflateEnd(&zs);
	return true;
}

bool decompressFile(const std::string& compressedFilename, const std::string& destFilename) 
{
	std::ifstream source(compressedFilename, std::ios_base::binary);
	std::ofstream dest(destFilename, std::ios_base::binary);

	if (!source || !dest)
	{
		std::cerr << "Could not open source or destination file." << std::endl;
		return false;
	}

	z_stream zs;
	memset(&zs, 0, sizeof(zs));

	if (inflateInit(&zs) != Z_OK)
	{
		std::cerr << "inflateInit failed while decompressing." << std::endl;
		return false;
	}

	const size_t bufSize = 128 * 1024;
	std::vector<char> inBuffer(bufSize);
	std::vector<char> outBuffer(bufSize);

	int inflateStatus;
	do 
	{
		source.read(inBuffer.data(), inBuffer.size());
		zs.next_in = reinterpret_cast<Bytef*>(inBuffer.data());
		zs.avail_in = source.gcount();

		do
		{
			zs.next_out = reinterpret_cast<Bytef*>(outBuffer.data());
			zs.avail_out = outBuffer.size();

			inflateStatus = inflate(&zs, Z_NO_FLUSH);

			if (inflateStatus == Z_NEED_DICT || inflateStatus == Z_DATA_ERROR || inflateStatus == Z_MEM_ERROR)
			{
				std::cerr << "Inflate failed during decompression." << std::endl;
				inflateEnd(&zs);
				return false;
			}

			int have = outBuffer.size() - zs.avail_out;
			dest.write(outBuffer.data(), have);
		} while (zs.avail_out == 0);
	} while (inflateStatus != Z_STREAM_END);

	inflateEnd(&zs);
	return true;
}

int main()
{
	bool ret = compressFile("E:/2.mp4", "E:/2.zlib");
	//bool ret = decompressFile("E:/2.zlib", "E:/3.mp4");
	return 0;
}

int main2()
{
	const char* input = "This is a string that we want to compress.This is a string that we want to compress";
	unsigned long sourceLen = strlen(input);
	unsigned char* compressed = NULL;
	unsigned long compressedLen = sourceLen * 1.001 + 12;

	compressed = (unsigned char*)realloc(compressed,compressedLen);
	if (compressed == NULL)
	{
		printf("Failed to allocate memory for compressed data.\n");
		return -1;
	}
	
	// 压缩数据
	int ret = compress(compressed, &compressedLen, (const Bytef*)input, sourceLen);
	if (Z_OK == ret)
	{
		printf("compress success!!!\n");
		printf("Original size: %lu\n", sourceLen);
		printf("Compressed size: %lu\n", compressedLen);
	}

	unsigned char* decompressed = NULL;
	unsigned long decompressedLen = sourceLen;

	decompressed = (unsigned char*)realloc(decompressed,decompressedLen);
	// 解压缩数据
	uncompress(decompressed, &decompressedLen, compressed, compressedLen);

	printf("Decompressed string: %s\n", decompressed);

	// 释放内存
	free(compressed);

	return 0;
}