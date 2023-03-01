#include <cerrno>
#include <cstdlib>
#include <ios>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <thread>
#include <queue>
#include <assert.h>
#include <list>
#include <map>
#include <tuple>
#include <set>
#include <functional>
#include <memory>

int width, height;
std::vector<std::vector<int>> map_;
std::vector<std::vector<int>> isKey;

/* 这是题目的格式
var width = 5; var height = 3; var boardStr = "......X......X."
GET /coil/?x=4&y=2&path=ULDRUR 
题目的答案横竖索引从0开始
.....
.X...
...X.
*/

void parse_mapstr(std::string map_str) {
	/*
	为了方便处理，所以在周围一圈加上一圈障碍物，如下
	######
	#....#
	#....#
	######
	*/
	auto count = 0;
	assert(map_str.size() == height * width);
	std::vector<int> emptyWidth(width + 2, 1);
	for (auto i = 0; i < height + 2; i++)
		map_.push_back(emptyWidth);
	
	for (auto i = 1; i <= height; i++) {
		for (auto f = 1; f <= width; f++) {
			map_[i][f] = map_str[count++] == 'X' ? 1 : 0;
		}
	}
}

void display_keymap() {
	for(auto i = 0; i < height + 2; i ++) {
		for (auto f = 0; f < width + 2; f ++) {
			if (map_[i][f])
				std::cout << '#';
			else if (isKey[i][f])
				std::cout << 'k';
			else 
				std::cout << '.';
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void display_map(std::vector<std::vector<int>> map) {
	//std::cout.setf(std::ios_base::hex, std::ios_base::basefield);
	for(auto i = 0; i < height + 2; i ++) {
		for (auto f = 0; f < width + 2; f ++) {
			std::cout.width(4);
			std::cout << map[i][f];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	//std::cout.unsetf(std::ios_base::hex);
}

std::array<std::array<int, 2>, 4> dir{-1, 0, 0, 1, 1, 0, 0, -1};
std::array<std::string, 4> dir_name{"U", "R", "D", "L"};

int twoCount = 0;
constexpr int blockStart = 700;
int greaterTwoCount = blockStart;

std::vector<std::vector<int>> id;

void split_block() {
	//init for id and degree;
	std::vector<int> emptyWidth(width + 2, 0);
	std::vector<int> negaWidth(width + 2, -1);

	for (auto i = 0; i < height + 2; i++)
		id.push_back(negaWidth);

	for (auto i = 0; i < height + 2; i++)
		isKey.push_back(emptyWidth);

	// caculate key point
	/*  一共八种情况
	#K#

	#
	K
	#

	#K.    .K#
	..#    #..

	#.     .#
	K.     K.
	.#     #.

	#..    ..#
	.K.    .K.
	..#    #..
	*/
	constexpr std::array<std::pair<int, int>, 16> keyPointFeature  {{{0, -1}, {0, 1},
																	{-1, 0}, {1, 0},
																	{0, -1}, {1, 1},
																	{1, -1}, {0, 1},
																	{-1, 0}, {1, 1},
																	{-1, 1}, {1, 0},
																	{-1,-1}, {1, 1},
																	{-1, 1}, {1,-1}}};

	//mark keyPoint
	for (auto i = 1; i <= height; i++) {
		for (auto f = 1; f <= width; f++) {
			for (auto ki = 0; ki < 16; ki += 2) {
				auto nx0 = i + keyPointFeature[ki].first;
				auto ny0 = f + keyPointFeature[ki].second;
				auto nx1 = i + keyPointFeature[ki+1].first;
				auto ny1 = f + keyPointFeature[ki+1].second;

				if (map_[nx0][ny0] && map_[nx1][ny1]) {
					isKey[i][f] = 1;
				}
			}
		}
	}
	display_keymap();

	auto bfs = [](int x, int y, bool key, int &markId) {
		std::queue<int> q;
		q.push(x);
		q.push(y);
		id[x][y] = markId;
		while(!q.empty()) {
			auto x = q.front();
			q.pop();
			auto y = q.front();
			q.pop();
			for (auto d : dir) {
				auto xx = x + d[0];
				auto yy = y + d[1];
				if (id[xx][yy] == -1 && !map_[xx][yy] &&
					!(isKey[xx][yy] ^ key)) {
					id[xx][yy] = markId;
					q.push(xx);
					q.push(yy);
				}
			}
		}
		markId ++;
	};

	// caculate id;
	// [0, twoCount) stand for block that degree equal 2
	// [blockStart, twoCount+greaterTwoCount) stand for block that degree 
	// greater than 2
	for (auto i = 1; i <= height; i++) {
		for (auto f = 1; f <= width; f++) {
			if (map_[i][f] || id[i][f] != -1)
				continue;

			if (isKey[i][f]) {
				bfs(i, f, true, twoCount);
			}
			else {
				bfs(i, f, false, greaterTwoCount);
			}
		}
	}

	display_map(id);

	/*
		对于keyblock如果连接到同一个id的，为了减少复杂度，则认为它也是该id
		参见level1的左上右下
		XX..
		....
		...X
	*/
	std::map<int, int> nearby_ids;

	for (auto i = 1; i <= height; i++) {
		for (auto f = 1; f <= width; f++) {
			auto keyId = id[i][f];

			if (isKey[i][f]) {
				for (auto d : dir) {
					auto xx = i + d[0];
					auto yy = f + d[1];
					auto blockId = id[xx][yy];
					if (blockId >= blockStart) {
						if (nearby_ids[keyId] == 0) {
							nearby_ids[keyId] = blockId;
						}
						else if (nearby_ids[keyId] != blockId) {
							nearby_ids[keyId] = -1;
						}
					}
				}
			}
		}
	}

	for (auto i = 0; i < twoCount; i++) {
		if (nearby_ids[i] != -1) {
			for (auto x = 1; x <= height; x++)
				for (auto y = 1; y <= width; y++)
					if (id[x][y] == i)
						id[x][y] = nearby_ids[i];
		}
	}
	display_map(id);
}

enum SolutionType {
	in,
	out, 
};

class partSolution {
public:
	struct path {
		path(int x, int y, int d) : x(x), y(y), ds{d} {}
		int x, y;
		std::list<int>ds;
	};
	bool isFinish = false;
	bool nextFinish = false;
	std::list<path> s;
	std::map<std::tuple<int, int, int>, std::shared_ptr<partSolution>> next; // in 2 out, out 2 in;
};

std::map<int, partSolution> psStart;
std::map<int, partSolution> psPassby;
std::map<int, std::list<std::pair<int, int>>> points;
std::map<int, std::list<std::tuple<int, int, int>>> entrys;

void block_solution_check(int blockId, bool start);

int solutionC = 0;

void find_block_solution() {
	auto summary = []() {
		auto totalblocksize = 0;
		auto totalblockentry = 0;
		for (auto i = blockStart; i < greaterTwoCount; i++) {
			totalblockentry += entrys[i].size();
			totalblocksize += points[i].size();
			std::cout << "size = " << points[i].size() << " entry = " << entrys[i].size() << std::endl;
		}
		auto blocksize = greaterTwoCount - blockStart;
		std::cout << "ave block size = " << 1.0*totalblocksize / blocksize << 
			" ave block entry " << 1.0*totalblockentry / blocksize << std::endl;


	};

	auto getProxy = [](partSolution &stage, std::tuple<int, int, int> key) {
		auto outIter = stage.next.find(key);
		if (outIter != stage.next.end()) {
			return outIter->second;
		}
		return std::make_shared<partSolution>();
	};

	auto setProxy = [](partSolution &stage, std::tuple<int, int, int> key, std::shared_ptr<partSolution> &n) {
		auto outIter = stage.next.find(key);
		if (outIter == stage.next.end() && (n->nextFinish || n->isFinish)) {
			stage.next[key] = n;
			stage.nextFinish = true;
		}
	};

	std::list<partSolution::path> s;
	std::function<void(int, int, int, int, partSolution&, int)> dfs = 
		[&dfs, &s, &getProxy, &setProxy](int blockId, int x, int y, int d, partSolution &ps, int total) {
		if (map_[x][y]) { // visited
			return;
		}

		total ++;
		map_[x][y] = 1;

		if (total == points[blockId].size()) { // success
			ps.isFinish = true;
			ps.s = s;
			solutionC ++;
			// 继续，如果可以走到外面的话在out设为finish
		}
		
		//one step
		auto xx = x + dir[d][0];
		auto yy = y + dir[d][1];
		std::list<int> newds;
		if (map_[xx][yy]) { //换方向
			for (auto newd : {d + 1, d + 3}) {
				newd = newd % 4;
				newds.push_back(newd);
			}
		}
		else { //直走
			newds.push_back(d);
		}

		for (auto nd : newds) {
				xx = x + dir[nd][0];
				yy = y + dir[nd][1];
				if (!map_[xx][yy]) { // empty
					//更新path
					auto p = s.rbegin();
					p->ds.push_back(nd);


					if (id[xx][yy] == blockId) { // 块内行走
						dfs(blockId, xx, yy, nd, ps, total);
					}
					else { //走到块外，重新寻找入口
						//如果填充满，走到块外就别回来了

						auto outKey = std::make_tuple(xx, yy, nd);
						auto pOut = getProxy(ps, outKey);

						if (total == points[blockId].size()) {
							pOut->isFinish = true;
							pOut->s = s;
							solutionC ++;
							continue;
						}

						map_[xx][yy] = 1;
						for (auto i : entrys[blockId]) {
							//新增path，开始新的一段路径
							auto ix = std::get<0>(i);
							auto iy = std::get<1>(i);
							auto id = std::get<2>(i);

							auto fromX = ix - dir[id][0];
							auto fromY = iy - dir[id][1];
							if (map_[fromX][fromY]) {
								continue;
							}
							map_[fromX][fromY] = 1;

							s.push_back(partSolution::path(ix, iy, id));

							auto pIn = getProxy(*pOut, i);
							
							dfs(blockId, ix, iy, id, *pIn, total);

							setProxy(*pOut, i, pIn);

							s.pop_back();
							map_[fromX][fromY] = 0;
						}
						map_[xx][yy] = 0;

						setProxy(ps, outKey, pOut);
					}

					p->ds.pop_back();
				}
		}
	
		map_[x][y] = 0;
	};

	for (auto i = 1; i <= height; i ++)
		for (auto f = 1; f <= width; f++) {
			//预处理出所有的合法入口，与块内所有点
			auto blockId = id[i][f];
			if (blockId != -1) {
				points[blockId].push_back(std::make_pair(i, f));
				//枚举入口
				for (auto d = 0; d < 4; d++) {
					auto xx = i + dir[d][0];
					auto yy = f + dir[d][1];
					auto stepId = id[xx][yy];
					if (stepId != -1 && stepId != blockId) {
						entrys[stepId].push_back(std::make_tuple(xx, yy, d));
					}
				}
			}
		}

	summary();
				
	for (auto blockId = blockStart; blockId < greaterTwoCount; blockId ++) {
		//枚举block内任一点作为起点
		std::cout << "start slove partSolution " << blockId << " size = " << points[blockId].size() <<
			 " entry = " << entrys[blockId].size() << std::endl;
		for (auto p : points[blockId]) {
			auto x = p.first;
			auto y = p.second;
			//std::cout << "now at " << x << " " << y << std::endl;

			for (auto d = 0; d < 4; d++) {
				auto xx = x + dir[d][0];
				auto yy = y + dir[d][1];

				//内部出发
				//第一步就撞墙，跳过。
				if (!map_[xx][yy])
				{
					s.clear();
					s.push_back(partSolution::path(x, y, d));
					auto &root = psStart[blockId];
					auto inKey = std::make_tuple(x, y, d);
					auto pIn = getProxy(root, inKey);
					dfs(blockId, x, y, d, *pIn, 0);
					setProxy(root, inKey, pIn);
				}

				//外部进入
				auto fromX = x - dir[d][0];
				auto fromY = y - dir[d][1];
				if (!map_[fromX][fromY] && (id[fromX][fromY] != blockId))
				{
					//std::cout << "start dfs out" << std::endl;
					map_[fromX][fromY] = 1;
					s.clear();
					s.push_back(partSolution::path(x, y, d));
					auto &root = psPassby[blockId];
					auto inKey = std::make_tuple(x, y, d);
					auto pIn = getProxy(root, inKey);
					dfs(blockId, x, y, d, *pIn, 0);
					setProxy(root, inKey, pIn);
					map_[fromX][fromY] = 0;
				}
			}
			std::cout << "solution = " << solutionC << std::endl;
			solutionC = 0;
		}

		block_solution_check(blockId, true);
		block_solution_check(blockId, false);
	}
}

std::tuple<int, int, std::string> solve() {
	std::vector<bool> needCheck(greaterTwoCount+1, false);

	auto dfs = []() {

	};
	
	for (auto i = 1; i <= height; i++) {
		for (auto f = 1; f <= width; f++) {
			auto blockId=  id[i][f];
			if (!blockId || !needCheck[blockId])
				continue;
			needCheck[blockId] = true;
			/*
			if (degree[i][f] == 2) {
				dfs();
			}
			else if (degree[i][f] > 2) {
				dfs();
			}
			*/
		}
	}

	return std::make_tuple(0, 0, "");
}

void block_solution_check(int blockId, bool start) {
	auto display_solution = [](std::list<partSolution::path> s, bool in) {
		std::cout << "find one solution in: " << in << std::endl;
		for (auto ss : s) {
			std::cout << ss.x << " " << ss.y << " ";
			for (auto d : ss.ds) {
				std::cout << dir_name[d];
			}
			std::cout << std::endl << std::endl;
		}
	};

	auto solutionCount = 0;
	std::function<void(partSolution &, bool)> dfs = 
		[&solutionCount, &dfs, blockId, &display_solution](partSolution &ps, bool in) {
		// check nextFinish
		if (ps.nextFinish) {
			auto ret = false;
			for (auto i : ps.next) {
				ret |= i.second->nextFinish;
				ret |= i.second->isFinish;
			}
			assert(ret);
		}
		assert(ps.isFinish || ps.nextFinish);
		if (ps.isFinish) {
			//display_solution(ps.s, in);
			solutionCount ++;
		}
		for (auto &n : ps.next) {
			dfs(*n.second, !in);
		}
	};

	auto &ps = start ? psStart[blockId] : psPassby[blockId];
	std::cout << "check block id = " << blockId << (start ? " start" : " passBy") << " nextFinish = " << ps.nextFinish << std::endl;
	dfs(ps, false);
	std::cout << "total solution count " << solutionCount << std::endl;
}

int main() {
	/* get map from stdin
	 * format width height map_str
	 */
	std::string map_str;
	std::cin >> width >> height >> map_str;
	parse_mapstr(map_str);
	std::cout << "RUN" << std::endl;
	display_map(map_);

	split_block();
	std::cout << "split block finish" << std::endl;
	display_map(id);
	std::cout << twoCount << " " << (greaterTwoCount - blockStart) << std::endl;

	find_block_solution();
	auto solution = solve();
	return 0;
}
