#include "miniz.h"
#include <iostream>
#include <string.h>
#include <fstream>
#include <vector>
#include <stdio.h>

#define my_max(a,b) (((a) > (b)) ? (a) : (b))
#define my_min(a,b) (((a) < (b)) ? (a) : (b))
#define BUF_SIZE (1024 * 1024)

static unsigned char s_inbuf[BUF_SIZE];
static unsigned char s_outbuf[BUF_SIZE];

using namespace std;

int compressFile(const char* sourceFilename, const char* destFilename)
{
	int level = Z_BEST_COMPRESSION;
	long file_loc = 0;
	unsigned int infile_size = 0;

	// 打开源文件
	FILE* pInfile = fopen(sourceFilename, "rb");
	if (!pInfile)
	{
		printf("无法打开源文件: %s\n", sourceFilename);
		return -1;
	}

	//Determine input file's size.
	fseek(pInfile, 0, SEEK_END);
	file_loc = ftell(pInfile);
	fseek(pInfile, 0, SEEK_SET);

	if ((file_loc < 0) || ((mz_uint64)file_loc > INT_MAX))
	{
		// This is not a limitation of miniz or tinfl, but this example.
		printf("File is too large to be processed by this example.\n");
		return EXIT_FAILURE;
	}

	infile_size = (unsigned int)file_loc;

	// 打开目标文件
	FILE* pOutfile = fopen(destFilename, "wb");
	if (!pOutfile)
	{
		printf("无法打开目标文件: %s\n", destFilename);
		fclose(pOutfile);
		return -1;
	}

	printf("Input file size: %u\n", infile_size);

	z_stream stream;

	// Init the z_stream
	memset(&stream, 0, sizeof(stream));
	stream.next_in = s_inbuf;
	stream.avail_in = 0;
	stream.next_out = s_outbuf;
	stream.avail_out = BUF_SIZE;

	// Compression.
	unsigned int infile_remaining = infile_size;

	if (deflateInit(&stream, level) != Z_OK)
	{
		printf("deflateInit() failed!\n");
		return EXIT_FAILURE;
	}

	for (; ; )
	{
		int status;
		if (!stream.avail_in)
		{
			// Input buffer is empty, so read more bytes from input file.
			unsigned int n = my_min(BUF_SIZE, infile_remaining);

			if (fread(s_inbuf, 1, n, pInfile) != n)
			{
				printf("Failed reading from input file!\n");
				return EXIT_FAILURE;
			}

			stream.next_in = s_inbuf;
			stream.avail_in = n;

			infile_remaining -= n;
			//printf("Input bytes remaining: %u\n", infile_remaining);
		}

		status = deflate(&stream, infile_remaining ? Z_NO_FLUSH : Z_FINISH);

		if ((status == Z_STREAM_END) || (!stream.avail_out))
		{
			// Output buffer is full, or compression is done, so write buffer to output file.
			unsigned int n = BUF_SIZE - stream.avail_out;
			if (fwrite(s_outbuf, 1, n, pOutfile) != n)
			{
				printf("Failed writing to output file!\n");
				return EXIT_FAILURE;
			}
			stream.next_out = s_outbuf;
			stream.avail_out = BUF_SIZE;
		}

		if (status == Z_STREAM_END)
			break;
		else if (status != Z_OK)
		{
			printf("deflate() failed with status %i!\n", status);
			return EXIT_FAILURE;
		}
	}

	if (deflateEnd(&stream) != Z_OK)
	{
		printf("deflateEnd() failed!\n");
		return EXIT_FAILURE;
	}

	fclose(pInfile);
	if (EOF == fclose(pOutfile))
	{
		printf("Failed writing to output file!\n");
		return EXIT_FAILURE;
	}

	printf("Total input bytes: %u\n", (mz_uint32)stream.total_in);
	printf("Total output bytes: %u\n", (mz_uint32)stream.total_out);
	printf("Success.\n");
	return EXIT_SUCCESS;
}

int decompressFile(const char* sourceFilename, const char* destFilename)
{
	int level = Z_BEST_COMPRESSION;
	long file_loc = 0;
	unsigned int infile_size = 0;

	// 打开源文件
	FILE* pInfile = fopen(sourceFilename, "rb");
	if (!pInfile)
	{
		printf("无法打开源文件: %s\n", sourceFilename);
		return -1;
	}

	//Determine input file's size.
	fseek(pInfile, 0, SEEK_END);
	file_loc = ftell(pInfile);
	fseek(pInfile, 0, SEEK_SET);

	if ((file_loc < 0) || ((mz_uint64)file_loc > INT_MAX))
	{
		// This is not a limitation of miniz or tinfl, but this example.
		printf("File is too large to be processed by this example.\n");
		return EXIT_FAILURE;
	}

	infile_size = (unsigned int)file_loc;

	// 打开目标文件
	FILE* pOutfile = fopen(destFilename, "wb");
	if (!pOutfile)
	{
		printf("无法打开目标文件: %s\n", destFilename);
		fclose(pOutfile);
		return -1;
	}

	printf("Input file size: %u\n", infile_size);

	// Decompression.
	unsigned int infile_remaining = infile_size;

	z_stream stream;
	// Init the z_stream
	memset(&stream, 0, sizeof(stream));
	stream.next_in = s_inbuf;
	stream.avail_in = 0;
	stream.next_out = s_outbuf;
	stream.avail_out = BUF_SIZE;

	if (inflateInit(&stream))
	{
		printf("inflateInit() failed!\n");
		return EXIT_FAILURE;
	}

	for (; ; )
	{
		int status;
		if (!stream.avail_in)
		{
			// Input buffer is empty, so read more bytes from input file.
			unsigned int n = my_min(BUF_SIZE, infile_remaining);

			if (fread(s_inbuf, 1, n, pInfile) != n)
			{
				printf("Failed reading from input file!\n");
				return EXIT_FAILURE;
			}

			stream.next_in = s_inbuf;
			stream.avail_in = n;

			infile_remaining -= n;
		}

		status = inflate(&stream, Z_SYNC_FLUSH);

		if ((status == Z_STREAM_END) || (!stream.avail_out))
		{
			// Output buffer is full, or decompression is done, so write buffer to output file.
			unsigned int n = BUF_SIZE - stream.avail_out;
			if (fwrite(s_outbuf, 1, n, pOutfile) != n)
			{
				printf("Failed writing to output file!\n");
				return EXIT_FAILURE;
			}
			stream.next_out = s_outbuf;
			stream.avail_out = BUF_SIZE;
		}

		if (status == Z_STREAM_END)
			break;
		else if (status != Z_OK)
		{
			printf("inflate() failed with status %i!\n", status);
			return EXIT_FAILURE;
		}
	}

	if (inflateEnd(&stream) != Z_OK)
	{
		printf("inflateEnd() failed!\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int main()
{
	//bool ret = compressFile("E:/2.mp4", "E:/2.mz");
	bool ret = decompressFile("E:/2.mz", "E:/3.mp4");
	return 0;
}

int main2()
{
	const  char* source = "This is a string that we want to compress.This is a string that we want to compress";
	unsigned long sourceLen = strlen(source);
	unsigned long destLen = mz_compressBound(sourceLen);
	unsigned char* dest = (unsigned char*)malloc(destLen);

	if (mz_compress(dest, &destLen, (unsigned char *)source, sourceLen) != MZ_OK) 
	{
		printf("Compression failed.\n");
		free(dest);
		return 1;
	}

	printf("Original size: %lu\n", sourceLen);
	printf("Compressed size: %lu\n", destLen);

	// 解压缩数据
	char* uncomp = (char*)malloc(sourceLen);
	unsigned long uncompLen = sourceLen;
	if (mz_uncompress((unsigned char *)uncomp, &uncompLen, (unsigned char *)dest, destLen) != MZ_OK) 
	{
		printf("Decompression failed.\n");
		free(dest);
		free(uncomp);
		return 1;
	}

	printf("Decompressed string: %s\n", uncomp);

	// 释放内存
	free(dest);
	free(uncomp);

	return 0;
}

