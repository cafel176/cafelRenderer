#include <cafel/utils/imageOutput.hpp>

CAFEL_NAMESPACE_BEGIN

std::ofstream myFile(defaultFile, std::ios_base::out | std::ios_base::binary);
void myOutput(unsigned char byte)
{
	myFile << byte;
}

void ImageOutput::changeFile()
{
	myFile = std::ofstream(filename, std::ios_base::out | std::ios_base::binary);
}

bool ImageOutput::output(unsigned char *data, int width, int height)
{
	changeFile();
	return TooJpeg::writeJpeg(myOutput, data, width, height, isRGB, quality, downsample);
}

CAFEL_NAMESPACE_END