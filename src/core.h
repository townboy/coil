#ifndef __CORE_H__
#define __CORE_H__

#include <string.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <stack>
#include <algorithm>
#include <list>

#define MAX_GRID  100
#define MAX_BLOCK 1000

//一些常量 -1为墙 -2为块
const int WALL = -1;
const int SELF = -2;

/*
      |0
      |
 <-1     ->3
      |
      |2
枚举当中的四个方向
*/

const char dir2name[8] = "uldr";
const int direction[4][2] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

/*
 * action分两类，进入的， 0，2是这类，出去的，1, 3 是这类
 * 整个完整的解链由0 2 开始，1,3 结束.
 * 路径都是挂在第一类action下面，第二类action下面没有路径
 * 0 从块外进来， 1 从块出去，2 在快内开始 3在块中结束
 */ 

enum ActionType {
    //这种类型使用在枚举路径的时候使用，代表在关键点当中的路径
    ActionNormal = -1,
    ActionIn = 0,
    ActionOut = 1,
    ActionStart = 2,
    ActionEnd = 3,
};

class Map;

class Action {
public:
    bool IsEqual(Action foo);
    void Display();

public:
    //到达当前状态的Direction
    int dir_;
    //到达当前状态的颜色
    int color_;

    //这个Action的类型
    int type_;
    //相关的key point是哪个,对于0, 1 用 从哪里来的，或者是到哪里去
    int key_point_;
    // 当前点坐标
    int x_; 
    int y_;
    //跟着该块走过的路径，只有0, 2有
    std::vector< int > path_;
    //这次Action需要覆盖哪几个点才能满足
    std::vector< int > requirement_;
};

class Block {
public:
    Block() {};
    virtual ~Block();
    Block(int color ,Map *whole_map);
    void Display();

    //预处理从外部开始的solution
    void FindSolutionOutside();
    //预处理从内部开始的solution
    std::vector< std::vector<Action> > FindSolutionStartHere();

    //暂时没有调用
    //给一个解方案，判断在此时的情况下是否能将该块填满
    bool IsSolutionFinish(std::vector< Action> solution);

    //根据现在的解决策略 找到接下去的解
    std::vector< std::vector <Action > > FindNextSolution(std::list<Action> path);

private:
    //搜索了多少状态
    int status_node_count_;

    //进行联通性剪纸 true代表就用进行搜索了
    bool IsConnectCut(int now_x, int now_y);
    //处理在外状态
    void StatusOutside();
    //处理在内状态
    void StatusInside(int now_x, int now_y, int old_dir);
    //处理从内部起点
    void StatusStart();

    //是否是内部点, 如果该点走过之后就把它变成墙
    bool IsInsidePoint(int x, int y);
    //是否是key point
    bool IsKeyPoint(int x, int y);
    //将该点开始的那个方向设为WALL 并不会包括x, y两个点
    std::vector<std::pair<int, int> > SetToWall(int x, int y, int dir);
    //回溯的时候将墙复原
    void SetToSelf(std::vector<std::pair<int, int> > color_point);
    //走到底是墙还是keyPoint
    int IsWallOrKeypoint(int x, int y, int dir);
    //该块走完了
    bool IsEnd();
    //得到一个解 添加到解空间
    void OnFindSolution();
    //hash 状态
    bool IsHash(int now_x, int now_y, int dir);

private:
    //整张地图的信息
    Map *whole_map_;
    //当前的颜色 好像没有什么用
    int color_;
    //处理出来的当前块信息，其中 -1 代表 墙 -2 代表 当前块
    int color_map_[MAX_GRID][MAX_GRID];
    //从外部为起点的解
    //Action *head_outside_;
    std::vector< std::vector<Action> > head_outside;
    //从内部为起点的解
    std::vector< std::vector<Action> > head_start;
    //进行hash过程
    std::unordered_map<std::string, bool> hash_map_;
    //搜索栈 用来记录路径
    std::vector<Action> stack_;
};
 
class Map {
public:
    Map(int x, int y, char *map);
    //标记出 key point 
    int MarkKeyPoint();

    bool ColorTheMap();

    void ColorBlock(int now_x, int now_y, int color);
    //进行显示染色分块之后的图
    void DisplayColorMap();
    //进行显示 原始数据
    void Display();

    void FindSolutionInBlock();
    
    bool IsPointOccupy(int aim_x, int aim_y);

    bool IsPointColor(int aim_x, int aim_y);
    //最后的生成路径
    bool MakeAnswer();
    //进行搜索
    bool DFSPath();
    //是否结束
    bool IsSearchFinish();
    //生成解 并且写入到文件当中
    void GenerateAnswer();

    //如果是墙 return WALL; 否则返回color 在搜索的过程当中调用
    int SearchColor(int aim_x, int aim_y);

    std::vector < std::pair<int, int> > SetToWall(int start_x, int start_y, 
            std::vector<int> path);
    void SetToColor(std::vector< std::pair<int, int> > points, int color);

    bool IsHash(int now_x, int now_y, int dir);

public:
    //统计
    int status_node_count_ ;
    //开始时间
    timeval start_;

    // 迷宫的 行
    int x_;             
    // 迷宫的 列
    int y_;
    // 原始的迷宫数据，true 代表该点被占林， false 代表该点没有被占林
    bool raw_info_[MAX_GRID][MAX_GRID];
    // 图中的关键点信息，暂时只有度为2的点是 key_point
    // true
    bool key_point_info_[MAX_GRID][MAX_GRID];
    // 染色之后的信息，
    int color_info_[MAX_GRID][MAX_GRID];
    // 关键点的数量，[0, key_count_) 是关键点 
    int key_count_;
    
    // 迷宫当中总共的块数，包括关键点在内 [key_count_, block_count) 
    // 是需要处理的大块
    int block_count_;

    // 块数据
    Block *block_list_[MAX_BLOCK];

    //搜索是hash使用
    std::unordered_map<std::string, bool> hash_map_;
    //搜索时的路径数据
    std::list< Action > stack_;
    std::list< Action > block_action_[MAX_BLOCK];

    std::vector<int>requirement_;
};

#endif 
