#pragma once

#if !defined(_CAFEL_TEXTURE_SYNTHESIS_HPP_)
#define _CAFEL_TEXTURE_SYNTHESIS_HPP_

#include <sampler/texture.hpp>
#include <cafel/basic/dataMat.hpp>
#include <vector>
#include <set>

CAFEL_NAMESPACE_BEGIN

class TextureSynthesis : public TextureSampler
{
public:
	// �������������
	TextureSynthesis() {}
	TextureSynthesis(const Texture &c_) : TextureSampler(c_) {}
	TextureSynthesis(SceneNode *node);

	~TextureSynthesis() { TextureSampler::~TextureSampler(); }
private:
	Texture textureOrigin;

	int numX, numY;
	//�ص����С
	int OverLapSize;
	//ѡȡ�Ŀ�Ĵ�С
	int TileSize;
	//ԴͼĬ�ϳߴ�
	int DefaultSizeX, DefaultSizeY;

	void generateInit(ivDataMat &dest, ivDataMat &source, int destSizeW, int destSizeH, int sourceSizeW, int sourceSizeH);
	void solve(std::set<iVector2> & points, std::vector<std::vector<bool>> &mask, int x, int y, std::vector<std::vector<bool>> &tags);
	void generateMask(ivDataMat &area_1, ivDataMat &area_2, ivDataMat &c_area_1, ivDataMat &c_area_2, std::vector<std::vector<bool>> &mask);
	void test(ivDataMat &dest, ivDataMat &source);
	void absdiff(const ivDataMat& a, const ivDataMat& b, ivDataMat& res);
};

CAFEL_NAMESPACE_END

#endif 