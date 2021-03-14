#include <sampler/textureSynthesis.hpp>
#include <cafel/utils/imageOutput.hpp>

CAFEL_NAMESPACE_BEGIN

TextureSynthesis::TextureSynthesis(SceneNode *node) : TextureSampler(node)
{
	std::string v = node->getParam("numX");
	numX = atoi(v.c_str());
	v = node->getParam("numY");
	numY = atoi(v.c_str());
	v = node->getParam("overLapSize");
	OverLapSize = atoi(v.c_str());
	v = node->getParam("tileSize");
	TileSize = atoi(v.c_str());

	DefaultSizeX = texture.width;
	DefaultSizeY = texture.height;

	textureOrigin = texture;

	ivDataMat source(Vector_zero(int, 3), texture.width, texture.height);
	for (int i = 0; i < textureOrigin.height; i++)// row
		for (int j = 0; j < textureOrigin.width; j++)// col
		{
			source[i][j].x = textureOrigin.data[(i*textureOrigin.width + j)*textureOrigin.nrComponents];
			source[i][j].y = textureOrigin.data[(i*textureOrigin.width + j)*textureOrigin.nrComponents + 1];
			source[i][j].z = textureOrigin.data[(i*textureOrigin.width + j)*textureOrigin.nrComponents + 2];
		}

	int delta = TileSize - OverLapSize;
	ivDataMat dest(Vector_zero(int, 3), delta * numX + OverLapSize, delta * numY + OverLapSize);

	generateInit(dest, source, TileSize, TileSize, DefaultSizeX, DefaultSizeY);
	test(dest, source);

	texture.width = dest.col;
	texture.height = dest.row;
	unsigned char *arr = new unsigned char[texture.height *texture.width *texture.nrComponents];
	for (int i = 0; i < texture.height; i++)// row
		for (int j = 0; j < texture.width; j++)// col
			for (int k = 0; k < texture.nrComponents; k++)
				arr[(i*texture.width + j)*texture.nrComponents + k] = dest[i][j][k];

	texture.data = arr;

	if (node->hasParam("save"))
	{
		ImageOutput img(node->getParam("save"));
		img.output(texture.data, texture.width, texture.height);
	}
}

//��ԭͼ�г�ȡ��СΪtilesize��һ�飬������dest��
void TextureSynthesis::generateInit(ivDataMat &dest, ivDataMat &source, int tileSizeW, int tileSizeH, int sourceSizeW, int sourceSizeH)
{
	dest.copyFrom(source, 0, 0, Warp::get()->rand(sourceSizeW - tileSizeW), Warp::get()->rand(sourceSizeH - tileSizeH), tileSizeW, tileSizeH);
}

//x y�ǵ�ǰ���꣬tag����Ƿ���������mask����Ƿ���Ƹ�����
void TextureSynthesis::solve(std::set<iVector2> & points, std::vector<std::vector<bool>> &mask, int x, int y, std::vector<std::vector<bool>> &tags)
{
	if (x < 0 || y < 0)
		return;
	mask[x][y] = true;
	tags[x][y] = true;
	int xn[][2] = { -1,0,0,-1 };
	for (int i = 0; i < 2; ++i)
	{//������һ����������
		int tmp_x = x + xn[i][0];
		int tmp_y = y + xn[i][1];
		if (tmp_x < 0 || tmp_y < 0)
			continue;
		iVector2 p(tmp_x, tmp_y);
		if (points.count(p) || tags[tmp_x][tmp_y])//����߽���Ѿ�������
			continue;
		solve(points, mask, tmp_x, tmp_y, tags);
	}
}

// 1�����ŵ�
void TextureSynthesis::generateMask(ivDataMat &area_1, ivDataMat &area_2, ivDataMat &c_area_1, ivDataMat &c_area_2, std::vector<std::vector<bool>> &mask)
{
	std::vector<iVector2> line_v;
	std::vector<iVector2> line_h;
	// �������߽�ĵ�һ��
	int idx;
	int dis = inf;
	for (int i = 0; i < OverLapSize; ++i)
	{
		//��������֮��ľ���ֵ��
		int tmp_0 = abs(area_1[0][i][0] - c_area_1[0][i][0]);
		int tmp_1 = abs(area_1[0][i][1] - c_area_1[0][i][1]);
		int tmp_2 = abs(area_1[0][i][2] - c_area_1[0][i][2]);
		int tmp = sqrt(tmp_0*tmp_0 + tmp_1 * tmp_1 + tmp_2 * tmp_2);
		if (tmp < dis)
		{
			dis = tmp;
			idx = i;
		}//�����С���������
	}
	line_v.push_back(iVector2(0, idx));
	//�������߽��������
	for (int i = 1; i < TileSize; ++i)
	{
		int tmp_idx;
		dis = inf;
		for (int j = idx - 1; j <= idx + 1; ++j)
		{
			if (j < 0 || j >= OverLapSize)
				continue;
			int tmp_0 = abs(area_1[i][j][0] - c_area_1[i][j][0]);
			int tmp_1 = abs(area_1[i][j][1] - c_area_1[i][j][1]);
			int tmp_2 = abs(area_1[i][j][2] - c_area_1[i][j][2]);
			int tmp = sqrt(tmp_0*tmp_0 + tmp_1 * tmp_1 + tmp_2 * tmp_2);
			if (tmp < dis)
			{
				dis = tmp;
				tmp_idx = j;
			}
		}
		idx = tmp_idx;
		line_v.push_back(iVector2(i, idx));
	}
	// ��߽�ĵ�һ��
	dis = inf;
	for (int i = 0; i < OverLapSize; ++i)
	{
		int tmp_0 = abs(area_2[i][0][0] - c_area_2[i][0][0]);
		int tmp_1 = abs(area_2[i][0][1] - c_area_2[i][0][1]);
		int tmp_2 = abs(area_2[i][0][2] - c_area_2[i][0][2]);
		int tmp = sqrt(tmp_0*tmp_0 + tmp_1 * tmp_1 + tmp_2 * tmp_2);
		if (tmp < dis)
		{
			dis = tmp;
			idx = i;
		}
	}
	line_h.push_back(iVector2(idx, 0));
	for (int i = 1; i < TileSize; ++i)
	{
		int tmp_idx;
		dis = inf;
		for (int j = idx - 1; j <= idx + 1; ++j)
		{
			if (j < 0 || j >= OverLapSize)
				continue;
			int tmp_0 = abs(area_2[j][i][0] - c_area_2[j][i][0]);
			int tmp_1 = abs(area_2[j][i][1] - c_area_2[j][i][1]);
			int tmp_2 = abs(area_2[j][i][2] - c_area_2[j][i][2]);
			int tmp = sqrt(tmp_0*tmp_0 + tmp_1 * tmp_1 + tmp_2 * tmp_2);
			if (tmp < dis)
			{
				dis = tmp;
				tmp_idx = j;
			}
		}
		idx = tmp_idx;
		line_h.push_back(iVector2(idx, i));
	}

	// Ѱ�Һ��ݱ߽��ߵĽ���
	int pos_h, pos_v;
	for (int i = 0; i < TileSize; ++i)
		for (int j = 0; j < TileSize; ++j)
		{
			//ĳ����ͬʱ�ں�߽���ݱ߽���
			if (line_v[i].x == line_h[j].x && line_v[i].y == line_h[j].y)
			{
				pos_v = i;
				pos_h = j;
				break;
			}
		}

	std::set<iVector2> points;
	//���߽�����points��
	points.insert(line_v[pos_v]);
	for (int i = pos_v + 1; i < TileSize; ++i)
		points.insert(line_v[i]);
	for (int i = pos_h + 1; i < TileSize; ++i)
		points.insert(line_h[i]);
	// ��Ѱmask��trueΪҪ����
	std::vector<std::vector<bool>> tags(TileSize, std::vector<bool>(TileSize, 0));
	solve(points, mask, TileSize - 1, TileSize - 1, tags);
}

void TextureSynthesis::test(ivDataMat &dest, ivDataMat &source)
{
	int delta = TileSize - OverLapSize;
	// ������ʼ����ĵ�һ�У�ÿ�κ���һ�鹫���ص�����
	for (int i = 1; i < numX; i++)
	{
		//��dest��һ����ȡ�ص���
		ivDataMat area(dest, i*delta, 0, OverLapSize, TileSize);
		double dis = inf;
		int x, y;
		//��source����λ�����������Ƶ�
		for (int j = 0; j < DefaultSizeY - TileSize; ++j)
			for (int k = 0; k < DefaultSizeX - TileSize; ++k)
			{
				ivDataMat area_t(source, k, j, OverLapSize, TileSize);
				ivDataMat res;
				absdiff(area, area_t, res);
				iVector re = res.sum();
				auto tmp_dis = sqrt(re[0] * re[0] + re[1] * re[1] + re[2] * re[2]);
				if (tmp_dis < dis)
				{
					dis = tmp_dis;
					x = k;
					y = j;
				}
			}
		dest.copyFrom(source, i*delta, 0, x, y, TileSize, TileSize);
	}

	// ��һ��
	for (int i = 1; i < numY; i++)
	{
		ivDataMat area(dest, 0, i*delta, TileSize, OverLapSize);
		double dis = inf;
		int x, y;
		for (int m = 0; m <= DefaultSizeY - TileSize; ++m)
			for (int n = 0; n <= DefaultSizeX - TileSize; ++n)
			{
				ivDataMat area_t(source, n, m, TileSize, OverLapSize);
				ivDataMat res;
				absdiff(area, area_t, res);
				iVector re = res.sum();
				auto tmp_dis = sqrt(re[0] * re[0] + re[1] * re[1] + re[2] * re[2]);
				if (tmp_dis < dis)
				{
					dis = tmp_dis;
					x = n;
					y = m;
				}
			}
		dest.copyFrom(source, 0, i*delta, x, y, TileSize, TileSize);
	}

	// �м䲿��
	#pragma omp parallel for schedule(dynamic,1)      // OpenMP
	for (int i = 1; i < numY; i++)
	{
		printf("\rSynthesising %5.2f%%", 100.0*i / (numY - 1));
		for (int j = 1; j < numX; j++)
		{
			ivDataMat area_1(dest, j*delta, i*delta, OverLapSize, TileSize);// ���ŵ�
			ivDataMat area_2(dest, j*delta, i*delta, TileSize, OverLapSize);// ���ŵ�
			ivDataMat area_0(dest, j*delta, i*delta, OverLapSize, OverLapSize);// С����
			double dis = inf;
			int x, y;
			for (int m = 0; m <= DefaultSizeY - TileSize; ++m)
				for (int n = 0; n <= DefaultSizeX - TileSize; ++n)
				{
					ivDataMat c_area_1(source, n, m, OverLapSize, TileSize);
					ivDataMat c_area_2(source, n, m, TileSize, OverLapSize);
					ivDataMat c_area_0(source, n, m, OverLapSize, OverLapSize);
					ivDataMat res;
					absdiff(c_area_1, area_1, res);
					iVector re = res.sum();
					auto tmp_dis = sqrt(re[0] * re[0] + re[1] * re[1] + re[2] * re[2]);
					absdiff(c_area_2, area_2, res);
					re = res.sum();
					tmp_dis += sqrt(re[0] * re[0] + re[1] * re[1] + re[2] * re[2]);
					absdiff(c_area_0, area_0, res);
					re = res.sum();
					tmp_dis += sqrt(re[0] * re[0] + re[1] * re[1] + re[2] * re[2]);
					if (tmp_dis < dis)
					{
						dis = tmp_dis;
						x = n;
						y = m;
					}
				}
			ivDataMat c_area_1(source, x, y, OverLapSize, TileSize);
			ivDataMat c_area_2(source, x, y, TileSize, OverLapSize);
			std::vector<std::vector<bool>> mask(TileSize, std::vector<bool>(TileSize, 0));
			generateMask(area_1, area_2, c_area_1, c_area_2, mask);// ��������
			for (int m = 0; m < TileSize; ++m)// x col
				for (int n = 0; n < TileSize; ++n)// y row
				{
					if (mask[m][n]) // λ������Ĳ��ָ���
						dest[i*delta + n][j*delta + m] = source[y + n][x + m];
				}
		}
	}
}

void TextureSynthesis::absdiff(const ivDataMat& a, const ivDataMat& b, ivDataMat& res)
{
	int row = a.row;
	int col = a.col;
	res.init(Vector_zero(int, 3), col, row);
	for (int i = 0; i < row; i++)
		for (int j = 0; j < col; j++)
		{
			iVector k = a[i][j] - b[i][j];
			res[i][j] = iVector(abs(k.x), abs(k.y), abs(k.z));
		}			
}

CAFEL_NAMESPACE_END