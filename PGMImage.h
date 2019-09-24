// PGMImage.h
// Saves grabresult or pylon image as PGM format
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
#define _CRT_SECURE_NO_WARNINGS // suppress fopen_s warnings for convinience
#endif

#include <pylon/PylonIncludes.h>
#include <cstdio>

namespace PGMImage
{
	bool Save(const char* fileName, const Pylon::CGrabResultPtr& ptrGrabResult, std::string &errorMessage);
	bool Save(const char* fileName, const Pylon::CPylonImage& image, std::string &errorMessage);
}


// *********************************************************************************************************
// DEFINITIONS

bool PGMImage::Save(const char* fileName, const Pylon::CGrabResultPtr& ptrGrabResult, std::string &errorMessage)
{
	try
	{
		if (ptrGrabResult->GetPixelType() == Pylon::PixelType_Undefined)
		{
			errorMessage = "ERROR: PixelType Undefined.";
			return false;
		}
		if (Pylon::IsPacked(ptrGrabResult->GetPixelType()) == true || Pylon::IsYUV(ptrGrabResult->GetPixelType()) == true)
		{
			errorMessage = "ERROR: Packed & YUV Image formats not supported yet.";
			return false;
		}

		std::FILE *const pgmfileOut = std::fopen(fileName, "wb+");

		uint32_t bitDepth = Pylon::BitDepth(ptrGrabResult->GetPixelType());
		uint32_t imageWidth = ptrGrabResult->GetWidth();
		uint32_t imageHeight = ptrGrabResult->GetHeight();
		uint32_t pixelValue = 0;

		if (bitDepth == 8)
		{
			fprintf(pgmfileOut, "P2\n%u %u\n255\n", imageWidth, imageHeight);
			const uint8_t* pBuffer = (uint8_t*)ptrGrabResult->GetBuffer();
			for (uint32_t i = 0; i < imageHeight * imageWidth; i++)
			{
				pixelValue = (uint32_t)pBuffer[i];
				std::fprintf(pgmfileOut, "%u ", pixelValue);
				if (i > 0 && i % imageWidth == 0)
					std::fprintf(pgmfileOut, "\n");
			}
		}
		if (bitDepth == 10)
		{
			std::fprintf(pgmfileOut, "P2\n%u %u\n1024\n", imageWidth, imageHeight);
			const uint16_t* pbuffer = (uint16_t*)ptrGrabResult->GetBuffer();
			for (uint32_t i = 0; i < imageHeight * imageWidth; i++)
			{
				pixelValue = (uint32_t)pbuffer[i];
				std::fprintf(pgmfileOut, "%u ", pixelValue);
				if (i > 0 && i % imageWidth == 0)
					std::fprintf(pgmfileOut, "\n");
			}
		}
		if (bitDepth == 12)
		{
			std::fprintf(pgmfileOut, "P2\n%u %u\n4095\n", imageWidth, imageHeight);
			const uint16_t* pbuffer = (uint16_t*)ptrGrabResult->GetBuffer();
			for (uint32_t i = 0; i < imageHeight * imageWidth; i++)
			{
				pixelValue = (uint32_t)pbuffer[i];
				std::fprintf(pgmfileOut, "%u ", pixelValue);
				if (i > 0 && i % imageWidth == 0)
					std::fprintf(pgmfileOut, "\n");
			}
		}

		std::fclose(pgmfileOut);
		return true;
	}
	catch (GenICam::GenericException &e)
	{
		errorMessage = "An exception occured in SavePGM(): ";
		errorMessage.append(e.GetDescription());
		return false;
	}
	catch (std::exception &e)
	{
		errorMessage = "An exception occured in SavePGM(): ";
		errorMessage.append(e.what());
		return false;
	}
}

bool PGMImage::Save(const char* fileName, const Pylon::CPylonImage& image, std::string &errorMessage)
{
	try
	{
		if (image.GetPixelType() == Pylon::PixelType_Undefined)
		{
			errorMessage = "ERROR: PixelType Undefined.";
			return false;
		}
		if (Pylon::IsPacked(image.GetPixelType()) == true || Pylon::IsYUV(image.GetPixelType()) == true)
		{
			errorMessage = "ERROR: Packed & YUV Image formats not supported yet.";
			return false;
		}

		std::FILE *const pgmfileOut = std::fopen(fileName, "wb+");

		uint32_t bitDepth = Pylon::BitDepth(image.GetPixelType());
		uint32_t imageWidth = image.GetWidth();
		uint32_t imageHeight = image.GetHeight();
		uint32_t pixelValue = 0;

		if (bitDepth == 8)
		{
			std::fprintf(pgmfileOut, "P2\n%u %u\n255\n", imageWidth, imageHeight);
			const uint8_t* pBuffer = (uint8_t*)image.GetBuffer();
			for (uint32_t i = 0; i < imageHeight * imageWidth; i++)
			{
				pixelValue = (uint32_t)pBuffer[i];
				std::fprintf(pgmfileOut, "%u ", pixelValue);
				if (i > 0 && i % imageWidth == 0)
					std::fprintf(pgmfileOut, "\n");
			}
		}
		if (bitDepth == 10)
		{
			std::fprintf(pgmfileOut, "P2\n%u %u\n1024\n", imageWidth, imageHeight);
			const uint16_t* pbuffer = (uint16_t*)image.GetBuffer();
			for (uint32_t i = 0; i < imageHeight * imageWidth; i++)
			{
				pixelValue = (uint32_t)pbuffer[i];
				std::fprintf(pgmfileOut, "%u ", pixelValue);
				if (i > 0 && i % imageWidth == 0)
					std::fprintf(pgmfileOut, "\n");
			}
		}
		if (bitDepth == 12)
		{
			std::fprintf(pgmfileOut, "P2\n%u %u\n4095\n", imageWidth, imageHeight);
			const uint16_t* pbuffer = (uint16_t*)image.GetBuffer();
			for (uint32_t i = 0; i < imageHeight * imageWidth; i++)
			{
				pixelValue = (uint32_t)pbuffer[i];
				std::fprintf(pgmfileOut, "%u ", pixelValue);
				if (i > 0 && i % imageWidth == 0)
					std::fprintf(pgmfileOut, "\n");
			}
		}

		std::fclose(pgmfileOut);
		return true;
	}
	catch (GenICam::GenericException &e)
	{
		errorMessage = "An exception occured in SavePGM(): ";
		errorMessage.append(e.GetDescription());
		return false;
	}
	catch (std::exception &e)
	{
		errorMessage = "An exception occured in SavePGM(): ";
		errorMessage.append(e.what());
		return false;
	}
}

// *********************************************************************************************************
#endif