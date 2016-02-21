#include <iostream>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include "core.h"
#include <stdio.h>
#include <sys/time.h>

Map::Map(int x, int y, char *map) : x_(x), y_(y) {
    std::cout << "x " << x << " y " << y << std::endl;
    std::cout << map << std::endl;
    int len = strlen(map);
    if (len != x * y)
        std::cout << "initialize failed" << std::endl;

    int index = 0;
    for (int i = 0; i < x; i ++)
        for (int f = 0; f < y; f ++) {
            raw_info_[i][f] = ('1' == map[index] ? true : false);
            index += 1;
            key_point_info_[i][f] = false;
            color_info_[i][f] = -1;
        }
}

void Map::Display() {
    std::cout << "=========================================================" << std::endl << std::endl;
    for (int i = 0 ;i < x_; i ++) {
        for (int f = 0 ;f < y_; f++) {
            if (true == raw_info_[i][f])
                std::cout << "# " ;
            else
                std::cout << "* " ;
        }
        std::cout << std::endl;
    }
    std::cout << "=========================================================" << std::endl << std::endl;
}

bool Map::IsPointOccupy(int aim_x, int aim_y) {
    if (aim_x < 0 || aim_x >= x_)
        return true;
    if (aim_y < 0 || aim_y >= y_)
        return true;
    return raw_info_[aim_x][aim_y];
}

int Map::MarkKeyPoint()
{
    key_count_ = 0;
    
    for (int i = 0 ;i < x_; i ++)
        for (int f = 0; f < y_; f ++) {

            if (true == IsPointOccupy(i ,f))
                continue;

            std::vector <int> empty_direction;
            empty_direction.clear();
            for (int k = 0; k < 4; k ++) {

                int next_x = i + direction[k][0]; 
                int next_y = f + direction[k][1]; 

                if (false == IsPointOccupy(next_x, next_y))
                    empty_direction.push_back(k);

            }

            if (2 != empty_direction.size())
                continue;

            /*
            if (1 == (empty_direction[0] + empty_direction[1]) % 2)
                continue;
            */

            // this point is key point
            key_point_info_[i][f] = true;
            color_info_[i][f] = key_count_ ++;
        }
    
    block_count_ = key_count_;

    return true;
}

void Map::ColorBlock(int now_x, int now_y, int color) {
    
    if (true == IsPointColor(now_x, now_y))
        return ;
    color_info_[now_x][now_y] = color;

    for (int i = 0 ;i < 4; i ++) {
        int next_x = now_x + direction[i][0];
        int next_y = now_y + direction[i][1];

        ColorBlock(next_x, next_y, color);
    }
    return ;
}

/* true mean colored
 * fales mean not colored
 */
bool Map::IsPointColor(int aim_x, int aim_y) {
    if (true == IsPointOccupy(aim_x, aim_y) )
        return true;

    if (-1 == color_info_[aim_x][aim_y])
        return false;
    return true;
}

void Map::DisplayColorMap() {
    std::cout << "==============================================" << std::endl;
    std::cout << "start display colored map" << std::endl;
    std::cout << "==============================================" << std::endl;
    for (int i = 0; i < x_; i ++) {
        for (int f = 0; f < y_; f ++) {
            if (true == raw_info_[i][f])
                std::cout << "    #" ;
            else if (color_info_[i][f] < key_count_)
                printf("%5d", color_info_[i][f]);
            else
                printf("%5d", color_info_[i][f]);
        }
        std::cout << std::endl;
    }

    std::cout << "requirement" << std::endl;
    for (size_t i = 0; i < requirement_.size(); i ++) { 
        std::cout << requirement_[i] << " ";
    }
    std::cout << std::endl;

    return ;
}


bool Map::ColorTheMap() {

    for (int i = 0 ;i < x_; i ++)
        for (int f = 0 ;f < y_; f ++) {
            if (true == IsPointColor(i, f))
                continue;

            ColorBlock(i, f, block_count_);
            block_count_ += 1;

        }

    return true;
}

void Map::FindSolutionInBlock() {
    for (int i = key_count_; i < block_count_; i ++) {
        block_list_[i] = new Block(i, this);
       // block_list_[i]->Display();

        block_list_[i]->FindSolutionOutside();
    }

    return ;
}

bool Map::IsSearchFinish() {
    /*
    //根据当前的枚举状态判断是不是能全部块填满
    
    //先判断所有的key point要填满
    for (int i = 0; i < x_; i++) 
        for (int f = 0; f < y_; f++) {
            if (color_info_[i][f] == WALL)
                continue;
            if (color_info_[i][f] < key_count_)
                return false;
        }

    //再判断所有的Block能不能填满
    for (int i = key_count_; i < block_count_; i ++) {
        if (false == block_list_[i]->IsSolutionFinish(block_action_[i]))
            return false;
    }
    */

    //直接扫一遍 看是否由未填充
    for (int i = 0; i < x_; i++) 
        for (int f = 0; f < y_; f++) {
            if (color_info_[i][f] != WALL)
                return false;
        }
    return true;
}

//判断在前方的点是WALL还是 key point 还是块内
int Map::SearchColor(int aim_x, int aim_y) {
    if (aim_x < 0 || aim_x >= x_)
        return WALL;
    if (aim_y < 0 || aim_y >= y_)
        return WALL;

    if (WALL == color_info_[aim_x][aim_y])
        return WALL;
    if (color_info_[aim_x][aim_y] < key_count_)
        return color_info_[aim_x][aim_y];

    //前方那个点在块内，需要根据块内的Action来判断是否可以进入
    return color_info_[aim_x][aim_y];
}

//文本格式为三行 第一行x 第二行y 第三行path
void Map::GenerateAnswer() {
    std::cout << "================================================================" << std::endl;
    std::cout << "Find Answer" << std::endl;
    std::cout << "================================================================" << std::endl;
    std::cout << "x=" << stack_.begin()->x_ << " y=" << stack_.begin()->y_ << std::endl;

    std::vector<int> path;

    for (std::list< Action >::iterator it = stack_.begin(); it != stack_.end(); it ++) {

        //如果是key point 当中的Action
        if (ActionNormal == it->type_) {
            if (-1 == it->dir_)
                continue;
            if (false == path.empty() && path[path.size() - 1] == it->dir_)
                continue;
            path.push_back(it->dir_);
        }
        //如果是在块当中的Action
        else {
            for (size_t f = 0; f < it->path_.size(); f ++) {
                int dir = it->path_[f];

                if (-1 == dir)
                    continue;
                if (false == path.empty() && path[path.size() - 1] == dir)
                    continue;
                path.push_back(dir);
            }
        }
    }

    std::cout << "path=" ;
    for (size_t i = 0; i < path.size(); i ++) {
        std::cout << dir2name[path[i]] ;
    }
    std::cout << std::endl;

    //写入到文件当中
    FILE *file = fopen("answer", "w");

    fprintf(file, "%d\n%d\n", stack_.begin()->x_, stack_.begin()->y_);
    for (size_t i = 0; i < path.size(); i ++) {
        fprintf(file, "%c", dir2name[path[i]]);
    }
    fprintf(file, "\n");

    fclose(file);
}

//进行hash 如果已经存在 返回true 代表可以剪枝了
bool Map::IsHash(int now_x, int now_y, int dir) {
    std::string key;
    for (int i = 0; i < x_; i ++) 
        for (int f = 0; f < y_; f ++) 
            key += (WALL == color_info_[i][f] ? "1" : "0");

    char info[256];
    snprintf(info, sizeof(info), "%5d%5d%2d", now_x, now_y, dir);
    key += info;

    //std::cout << "kdjfkdsjf " << key << std::endl;

    std::unordered_map<std::string, bool> ::iterator it = hash_map_.find(key);
    if (hash_map_.end() != it)
        return true;
    hash_map_[key] = true;
    return false;
}

//还是采用精确枚举，能找到每一个Action
bool Map::DFSPath() {

    status_node_count_ += 1;
    if (status_node_count_ % 1000 == 0) {
        timeval now;
        gettimeofday(&now, NULL);
        double cost = now.tv_sec - start_.tv_sec + (now.tv_usec - start_.tv_usec) / 1000000.0;

        printf("[INFO] MakeAnswer handle %d nodes, per second handle %lf nodes\n",
               status_node_count_, status_node_count_ / cost);
        printf("[INFO] MakeAnswer cost %lf second\n", cost);
    }

    //DisplayColorMap();

    //是否结束
    if ( true == IsSearchFinish()) {
        GenerateAnswer();
        return true;
    }
    //做出最新的Action 找到当前的状态 
    //在块内还是在关键点
    
    Action now = *stack_.rbegin();

    //还在key point
    if (ActionNormal == now.type_) {

        //进行hash
        //printf("  AAAAAA");
        if (true == IsHash(now.x_, now.y_, now.dir_)) {
            return false;
        }

        int next_x, next_y, ret;
        if (-1 != now.dir_) {
            next_x = now.x_ + direction[now.dir_][0];
            next_y = now.y_ + direction[now.dir_][1];
            ret = SearchColor(next_x, next_y);
        }

        std::vector<int> available_direction;

        //起点的情况, 或者前方是墙，可以自由选择
        if (-1 == now.dir_ || WALL == ret) {
            for (int dir = 0; dir < 4; dir++)
                available_direction.push_back(dir);
        }
        //只能行走原先方向
        else 
            available_direction.push_back(now.dir_);

        for (size_t i = 0; i < available_direction.size(); i ++) {
            int dir = available_direction[i];

            int next_x = now.x_ + direction[dir][0];
            int next_y = now.y_ + direction[dir][1];

            int forward_color = SearchColor(next_x, next_y);

            //是墙， GG
            if (WALL == forward_color)
                continue;

            //key point
            if (forward_color < key_count_) {

                Action action_in;
                action_in.type_ = ActionNormal;
                action_in.x_ = next_x;
                action_in.y_ = next_y;
                action_in.dir_ = dir;
                action_in.color_ = color_info_[next_x][next_y];

                //改造起始点
                stack_.push_back(action_in);
                requirement_.push_back(color_info_[next_x][next_y]);
                color_info_[next_x][next_y] = WALL;

                if (true == DFSPath())
                    return true;

                //恢复现场
                color_info_[next_x][next_y] = action_in.color_;
                stack_.pop_back();
                requirement_.pop_back();

            }
            //进入到块内了
            else {
                int color = color_info_[next_x][next_y];
                std::vector< std::vector< Action> > next_solution =
                    block_list_[color]->FindNextSolution(block_action_[color]);

                size_t index = block_action_[color].size();

                for (size_t f = 0; f < next_solution.size(); f ++) {
                    //应该是还未填满 应该不会出现这种情况
                    if (next_solution[f].size() <= index)
                        continue;

                    Action choose = next_solution[f][index];

                    //进入块内的点都不对，pass
                    if (choose.x_ != next_x || choose.y_ != next_y)
                        continue;

                    //进来的 key point 也要相同
                    if (choose.key_point_ != now.color_)
                        continue;

                    //判断后继是否合法，包括起始点和requirement
                    size_t requirement_checker = 0;

                    
                    for (; requirement_checker < choose.requirement_.size(); requirement_checker ++) {
                        std::vector<int> ::iterator it = std::find(requirement_.begin(),
                                requirement_.end(), choose.requirement_[requirement_checker]);
                        if (requirement_.end() == it) {
                            break;
                        }
                    }
                    if (requirement_checker < choose.requirement_.size() ) {
                        continue;
                    }
                    
                    //使用这个Action 需要更新地图上面的点
                    //肯定不会是ActionStart 节点了, 所以肯定是进入ActionIn
                    //更新现场
                    std::vector<std::pair<int, int> > points = SetToWall(choose.x_, choose.y_, 
                            choose.path_);
                    stack_.push_back(choose);
                    block_action_[color].push_back(choose);

                    //choose.Display();
                    //进行hash
                    //printf("  BBBB");
                    if (true == IsHash(points.rbegin()->first ,points.rbegin()->second ,
                                *choose.path_.rbegin())) {
                        //还原现场
                        SetToColor(points, color);
                        stack_.pop_back();
                        block_action_[color].pop_back();
                        continue;
                    }

                    if (true == DFSPath())
                        return true;

                    //还原现场
                    SetToColor(points, color);
                    stack_.pop_back();
                    block_action_[color].pop_back();

                }

            }
        }

        return false;
    }

    //当前是在块中, 开始处理 找到合法后继 枚举出口
    int color = now.color_;
    int index = block_action_[color].size();
    std::vector< std::vector< Action> > next_solution = block_list_[color]->FindNextSolution(
            block_action_[color]);

    //一定是ActionOut 也有可能是ActionEnd  一般情况下只有一个
    for (size_t i = 0; i < next_solution.size(); i ++) {

        Action choose = next_solution[i][index];

        //如果能够匹配上，这个函数开始的Finish就会结束，所以这里就不用判断了
        if (ActionEnd == choose.type_) 
            continue;

        int dir = choose.dir_;
        int next_x = choose.x_ + direction[dir][0];
        int next_y = choose.y_ + direction[dir][1];

        //判断出去的那个点有没有被覆盖
        if (WALL == SearchColor(next_x, next_y))
            continue;

        //可以出去 
        //这里要添加两个Action 一个是OutAction 一个是Normal
        //第一个
        stack_.push_back(choose);
        block_action_[color].push_back(choose);

        //第二个
        Action action_out;
        action_out.type_ = ActionNormal;
        action_out.x_ = next_x;
        action_out.y_ = next_y;
        action_out.dir_ = dir;
        action_out.color_ = choose.key_point_;
        
        stack_.push_back(action_out);
        requirement_.push_back(color_info_[next_x][next_y]);
        color_info_[next_x][next_y] = WALL;

        if (true == DFSPath())
            return true;

        //恢复现场
        color_info_[next_x][next_y] = choose.key_point_;
        requirement_.pop_back();
        //弹出 ActionNormal
        stack_.pop_back();
        //弹出 ActionOut
        stack_.pop_back();

        block_action_[color].pop_back();
    }

    return false;
}

/*
 * 搜索的路径放在stack_当中，其中 dir = -1,代表这个是key point作为起点，
 * 否则 dir 代表到达该点的方向，x, y, key_point 作为当前位置的坐标
 * 如果是块内部作为起点， 不会设定具体的坐标，因为具体的坐标与后面的枚举
 * 并没有什么关系,其余的块内Action 并不会含有路径信息,因为内部的路径与
 * 后面的枚举也没有关系。
 */
// 遇到一些问题，还是以确定的Action进行搜索
// 搜索放在color_info_ 上面进行, -1为墙，其余为key point和 块
bool Map::MakeAnswer() {

    //把stack_ 清空
    stack_.clear();
    for (size_t i = 0; i < MAX_BLOCK; i ++)
        block_action_[i].clear();
    requirement_.clear();
    hash_map_.clear();

    //从key point 开始找解
    for (int i = 0; i < x_; i ++) {
        for (int f = 0; f < y_; f ++) {
            
            if (WALL == color_info_[i][f] || color_info_[i][f] >= key_count_)
                continue;

            std::cout << "[MAKE ANSWER] start key point " << color_info_[i][f] << std::endl;

            //这个点是key point
            Action action_start;
            action_start.type_ = ActionNormal;
            action_start.x_ = i;
            action_start.y_ = f;
            action_start.dir_ = -1;
            action_start.color_ = color_info_[i][f];

            //改造起始点
            stack_.push_back(action_start);
            requirement_.push_back(color_info_[i][f]);
            color_info_[i][f] = WALL;

            if (true == DFSPath())
                return true;

            //恢复现场
            color_info_[i][f] = stack_.rbegin()->color_;
            requirement_.pop_back();
            stack_.pop_back();
        }
    }

    std::cout << "[MAKE ANSWER] start block" << std::endl;

    //从块内开始找解
    for (int color = key_count_; color < block_count_; color ++) {
        std::vector< std::vector< Action > > head_start = block_list_[color]->FindSolutionStartHere();
        
        //遍历所有起点
        for (size_t f = 0; f <  head_start.size(); f ++) {
            //加载现场
            Action choose = head_start[f][0];
            std::vector< std::pair<int, int> > points = SetToWall(choose.x_, choose.y_, choose.path_);
            block_action_[color].push_back(choose);
            stack_.push_back(choose);

            if (true == DFSPath())
                return true;

            //恢复现场
            stack_.pop_back();
            block_action_[color].pop_back();
            SetToColor(points, color);
        }
    }

    return false;
}

std::vector < std::pair<int, int> > 
Map::SetToWall(int start_x, int start_y, std::vector<int> path) {

    std::vector <std::pair<int, int> > result;

    /*
    std::cout << "ask to set from (" << start_x << ", " << start_y << ") " << std::endl;
    for (size_t i = 0; i < path.size(); i ++) {
        std::cout << path[i] ;
    }
    std::cout << std::endl;
    */

    color_info_[start_x][start_y] = WALL;
    result.push_back(std::make_pair(start_x, start_y));

    for (size_t i = 0; i < path.size(); i ++) {
        int dir = path[i];

        int next_x = start_x + direction[dir][0];
        int next_y = start_y + direction[dir][1];

        int color = SearchColor(next_x, next_y);
        //前方那个点依旧在块内
        while (color >= key_count_ && color < block_count_) {
            color_info_[next_x][next_y] = WALL;
            result.push_back(std::make_pair(next_x, next_y) );

            start_x = next_x;
            start_y = next_y;

            next_x += direction[dir][0];
            next_y += direction[dir][1];

            color = SearchColor(next_x, next_y);
        }
    }

    return result;
}

void 
Map::SetToColor(std::vector< std::pair<int, int> > points, int color) {
    for (size_t i = 0; i < points.size(); i++)
        color_info_[points[i].first][points[i].second] = color;  
}

int main(int argc, char **argv) {
    //用来断点使用
 //   getchar();

    if (4 != argc) {
        std::cout << "you must input map_str" << std::endl; 
        return 0;
    }

    Map *map = new Map(atoi(argv[1]), atoi(argv[2]), argv[3]);

    map->Display();
    map->MarkKeyPoint();
    map->ColorTheMap();
    map->DisplayColorMap();

    map->FindSolutionInBlock();

    timeval now;
    gettimeofday(&map->start_, NULL);

    if (false ==  map->MakeAnswer()) {
        std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        std::cout << "ERROR Solution Fail" << std::endl;
        std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        return -1;
    }
    
    gettimeofday(&now, NULL);
    double cost = now.tv_sec - map->start_.tv_sec + (now.tv_usec - map->start_.tv_usec) / 1000000.0;

    printf("[INFO] MakeAnswer handle %d nodes, per second handle %lf nodes\n",
           map->status_node_count_, map->status_node_count_ / cost);
    printf("[INFO] MakeAnswer cost %lf second\n", cost);

    return 0;
}
