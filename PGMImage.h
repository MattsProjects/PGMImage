// PGMImage.h
// Saves image buffers as PGM/PPM file format.
// Can be used independently, or with the Pylon Camera Software Suite (by including PylonIncludes.h)
// These file formats are known as the "lowest common denominator" for mono & color images respectively.
// Images in these formats are essentially plain text files of raw, uncompressed pixel values.
// More information can be found here:
// PGM: http://netpbm.sourceforge.net/doc/pgm.html
// PPM: http://netpbm.sourceforge.net/doc/ppm.html
//
// Copyright (c) 2016-2019 Matthew Breit - matt.breit@baslerweb.com or matt.breit@gmail.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef PGMIMAGE_H
#define PGMIMAGE_H

#ifndef LINUX_BUILD
#define WIN_BUILD
#endif

#ifdef WIN_BUILD
#define _CRT_SECURE_NO_WARNINGS // suppress fopen_s warnings for convinience (if this header included first)
#endif

// We will use with Pylon
#include <pylon/PylonIncludes.h>

#include <cstdio>
#include <string>
#include <stdint.h>

namespace PGMImage
{
	struct PixelFormat
	{
		uint32_t BitDepth = 0;
		bool IsUndefined = false;
		bool IsPacked = false;
		bool IsMono = false;
		bool IsBayer = false;
		bool IsYUV = false;
		bool IsRGB = false;
		bool IsBGR = false;
	};

	// Saves an image from a buffer. Adds ".pgm" or ".ppm" extension to the filename as needed.
	static bool Save(std::string &fileName, const void* buffer, uint32_t width, uint32_t height, PGMImage::PixelFormat pgmPixelFormat, std::string &errorMessage);

#ifdef PYLONINCLUDES_H_INCLUDED__
	// Saves an image from a Pylon Grab Result pointer. Adds ".pgm" or ".ppm" extension to filename as needed.
	static bool Save(std::string &fileName, const Pylon::CGrabResultPtr& pGrabResult, std::string &errorMessage);

	// Saves an image from a PylonImage container. Adds ".pgm" or ".ppm" extension to filename as needed.
	static bool Save(std::string &fileName, const Pylon::CPylonImage& Image, std::string &errorMessage);
#endif
}

// *********************************************************************************************************
inline bool PGMImage::Save(std::string &fileName, const void* buffer, uint32_t width, uint32_t height, PGMImage::PixelFormat pixelFormat, std::string &errorMessage)
{
	try
	{
		if (pixelFormat.IsUndefined == true)
		{
			errorMessage = "ERROR: Pixel Format Undefined.";
			return false;
		}
		if (pixelFormat.IsPacked == true || pixelFormat.IsYUV == true || pixelFormat.IsBGR == true)
		{
			errorMessage = "ERROR: Packed, YUV, and BGR image formats not yet supported.";
			return false;
		}

		uint32_t bitDepth = pixelFormat.BitDepth;

		if (bitDepth > 12 || bitDepth < 8)
		{
			errorMessage = "ERROR: Bit depths larger than 12bit and smaller than 8bit are not yet supported.";
			return false;
		}

		if (pixelFormat.IsMono == true || pixelFormat.IsBayer == true)
		{
			fileName.append(".pgm");
			std::FILE *const pgmfileOut = std::fopen(fileName.c_str(), "wb+");

			if (bitDepth == 8)
			{
				fprintf(pgmfileOut, "P2\n%u %u\n255\n", width, height);
				const uint8_t* pBuffer = (uint8_t*)buffer;
				for (uint32_t i = 0; i < width * height; i++)
				{
					uint32_t pixelValue = (uint32_t)pBuffer[i];
					std::fprintf(pgmfileOut, "%u ", pixelValue);
					if (i > 0 && i % width == 0)
						std::fprintf(pgmfileOut, "\n");
				}
			}

			if (bitDepth == 10 || bitDepth == 12)
			{
				if (bitDepth == 10)
					std::fprintf(pgmfileOut, "P2\n%u %u\n1024\n", width, height);
				if (bitDepth == 12)
					std::fprintf(pgmfileOut, "P2\n%u %u\n4095\n", width, height);

				const uint16_t* pbuffer = (uint16_t*)buffer;
				for (uint32_t i = 0; i < width * height; i++)
				{
					uint32_t pixelValue = (uint32_t)pbuffer[i];
					std::fprintf(pgmfileOut, "%u ", pixelValue);
					if (i > 0 && i % width == 0)
						std::fprintf(pgmfileOut, "\n");
				}
			}

			std::fclose(pgmfileOut);
		}

		if (pixelFormat.IsRGB == true)
		{
			fileName.append(".ppm");
			std::FILE *const pgmfileOut = std::fopen(fileName.c_str(), "wb+");

			if (bitDepth == 8)
			{
				fprintf(pgmfileOut, "P3\n%u %u\n255\n", width, height);
				const uint8_t* pBuffer = (uint8_t *)buffer;
				for (uint32_t i = 0; i < (width * height) * 3; i += 3)
				{
					uint32_t pixelValue_c1 = (uint32_t)pBuffer[i];
					uint32_t pixelValue_c2 = (uint32_t)pBuffer[i + 1];
					uint32_t pixelValue_c3 = (uint32_t)pBuffer[i + 2];
					std::fprintf(pgmfileOut, "%u ", pixelValue_c1);
					std::fprintf(pgmfileOut, "%u ", pixelValue_c2);
					std::fprintf(pgmfileOut, "%u ", pixelValue_c3);
					if (i > 0 && i % width == 0)
						std::fprintf(pgmfileOut, "\n");
				}
			}

			std::fclose(pgmfileOut);
		}

		return true;
	}
	catch (std::exception &e)
	{
		errorMessage = "An exception occured in Save(): ";
		errorMessage.append(e.what());
		return false;
	}
}

#ifdef PYLONINCLUDES_H_INCLUDED__
inline bool PGMImage::Save(std::string &fileName, const Pylon::CGrabResultPtr &pGrabResult, std::string &errorMessage)
{
	Pylon::EPixelType pylonFormat = pGrabResult->GetPixelType();

	if (pylonFormat == Pylon::PixelType_Undefined)
	{
		errorMessage = "ERROR: Pixel Format Undefined.";
		return false;
	}

	PGMImage::PixelFormat pgmFormat;

	if (pylonFormat == Pylon::PixelType_Undefined)
		pgmFormat.IsUndefined = true;
	if (Pylon::IsMono(pylonFormat))
		pgmFormat.IsMono = true;
	if (Pylon::IsBayer(pylonFormat))
		pgmFormat.IsBayer = true;
	if (Pylon::IsYUV(pylonFormat))
		pgmFormat.IsYUV = true;
	if (Pylon::IsRGB(pylonFormat))
		pgmFormat.IsRGB = true;
	if (Pylon::IsBGR(pylonFormat))
		pgmFormat.IsBGR = true;

	pgmFormat.BitDepth = Pylon::BitDepth(pylonFormat);
	pgmFormat.IsPacked = Pylon::IsPacked(pylonFormat);

	if (PGMImage::Save(fileName, pGrabResult->GetBuffer(), pGrabResult->GetWidth(), pGrabResult->GetHeight(), pgmFormat, errorMessage) == true)
		return true;
	else
		return false;
}

inline bool PGMImage::Save(std::string &fileName, const Pylon::CPylonImage& Image, std::string &errorMessage)
{
	Pylon::EPixelType pylonFormat = Image.GetPixelType();

	if (pylonFormat == Pylon::PixelType_Undefined)
	{
		errorMessage = "ERROR: Pixel Format Undefined.";
		return false;
	}

	PGMImage::PixelFormat pgmFormat;

	if (pylonFormat == Pylon::PixelType_Undefined)
		pgmFormat.IsUndefined = true;
	if (Pylon::IsMono(pylonFormat))
		pgmFormat.IsMono = true;
	if (Pylon::IsBayer(pylonFormat))
		pgmFormat.IsBayer = true;
	if (Pylon::IsYUV(pylonFormat))
		pgmFormat.IsYUV = true;
	if (Pylon::IsRGB(pylonFormat))
		pgmFormat.IsRGB = true;
	if (Pylon::IsBGR(pylonFormat))
		pgmFormat.IsBGR = true;

	pgmFormat.BitDepth = Pylon::BitDepth(pylonFormat);
	pgmFormat.IsPacked = Pylon::IsPacked(pylonFormat);

	if (PGMImage::Save(fileName, Image.GetBuffer(), Image.GetWidth(), Image.GetHeight(), pgmFormat, errorMessage) == true)
		return true;
	else
		return false;
}
#endif
// *********************************************************************************************************
#endif