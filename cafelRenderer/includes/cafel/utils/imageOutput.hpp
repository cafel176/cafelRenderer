#pragma once

#if !defined(_CAFEL_IMAGE_OUTPUT_HPP_)
#define _CAFEL_IMAGE_OUTPUT_HPP_

#include <cafel/basic/basic.h>
#include <toojpeg/toojpeg.h>
#include <fstream>
#include <string>

CAFEL_NAMESPACE_BEGIN

const std::string defaultFile = "image.jpg";

class ImageOutput
{
public:
	ImageOutput(std::string filename_ ,int quality_ = 90, bool isRGB_ = true, bool downsample_ = false):
		isRGB(isRGB_),quality(quality_),downsample(downsample_),filename(filename_){}

	bool output(unsigned char *data, int width, int height);

private:
	void changeFile();

	bool isRGB;  // true = RGB image, else false = grayscale
	int quality;    // compression quality: 0 = worst, 100 = best, 80 to 90 are most often used
	bool downsample; // false = save as YCbCr444 JPEG (better quality), true = YCbCr420 (smaller file)
	std::string filename;
};

CAFEL_NAMESPACE_END

#endif 