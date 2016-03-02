#include <iostream>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include "core.h"
#include <stdio.h>
#include <sys/time.h>
#include <map>
#include <queue>

Map::Map(int x, int y, char *map) : x_(x), y_(y) {
    std::cout << "x " << x << " y " << y << std::endl;
 //   std::cout << map << std::endl;
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

//如果该点度小于2 那么就是key point
//将该点填充，如果分割成两个块,那么还是key point
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

            //度小于2
            if (3 > empty_direction.size()) {
                key_point_info_[i][f] = true;
                color_info_[i][f] = key_count_ ++;
                continue;
            }

            //判断连通性 肯定不会是靠边的点
           
            int nine_grid[3][3];

            bool push_flag = false;
            std::queue<std::pair<int, int> > bfs_que;
            
            for (int g = 0; g < 3; g++) {
                int correct_x = i - 1 + g;

                for (int h = 0; h < 3; h ++) {
                    int correct_y = f - 1 + h;
                    
                    if (correct_x < 0 || correct_x >= x_ || correct_y < 0 || correct_y >= y_)
                        nine_grid[g][h] = -1;
                    else
                        nine_grid[g][h] = true == raw_info_[correct_x][correct_y] ? -1 : 0;

                    if (WALL != nine_grid[g][h] && false == push_flag) {
                        bfs_que.push(std::make_pair(g, h));
                        push_flag = true;
                    }
                }
            }

            nine_grid[1][1] = WALL;

            while(false == bfs_que.empty()) {
                int now_x = bfs_que.front().first;
                int now_y = bfs_que.front().second;
                bfs_que.pop();
              
                for (int dir = 0; dir < 4; dir ++) {
                    int next_x = now_x + direction[dir][0];
                    int next_y = now_y + direction[dir][1];

                    if (next_x < 0 || next_x > 2 || next_y < 0 || next_y > 2)
                        continue;

                    if (WALL != nine_grid[next_x][next_y]) {
                        nine_grid[next_x][next_y] = WALL;
                        bfs_que.push(std::make_pair(next_x, next_y));
                    }
                }
            }

            bool empty_flag = false;
            for (int g = 0; g < 3; g ++)
                for (int h = 0; h < 3; h ++)
                    if (WALL != nine_grid[g][h])
                        empty_flag = true;
            
            //还存在被分割的。该点是key point
            if (true == empty_flag) {
                key_point_info_[i][f] = true;
                color_info_[i][f] = key_count_ ++;
            }
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

void Map::ColorCheck() {
    for (int i = 0; i < x_; i ++)
        for (int f = 0; f < y_; f ++) {
            if (WALL == color_info_[i][f] || color_info_[i][f] >= key_count_) 
                continue;
            //是 key point
            std::map<int, int> adjust_block;
            adjust_block.clear();

            CheckDFS(i, f, adjust_block);

            if (1 == adjust_block.size() )
                color_info_[i][f] = adjust_block.begin()->first;
        }
}

void Map::CheckDFS(int now_x, int now_y, std::map<int, int> &adjust_block) {
    int key_point = color_info_[now_x][now_y];
    color_info_[now_x][now_y] = WALL;

    for (int dir = 0; dir < 4; dir ++) {
        int next_x = now_x + direction[dir][0];
        int next_y = now_y + direction[dir][1];

        if (next_x < 0 || next_x >= x_ || next_y < 0 || next_y >= y_)
            continue;

        //是块内
        if (color_info_[next_x][next_y] >= key_count_)
            adjust_block[color_info_[next_x][next_y]] = 1;

        else if (WALL < color_info_[next_x][next_y])
            CheckDFS(next_x, next_y, adjust_block);
    }

    color_info_[now_x][now_y] = key_point;
}

void Map::FindSolutionInBlock() {
    for (int i = key_count_; i < block_count_; i ++) {
        block_list_[i] = new Block(i, this);
       // block_list_[i]->Display();

        block_list_[i]->FindSolutionOutside();
    }
}

//根据当前的枚举状态判断是不是能全部块填满
bool Map::IsSearchFinish() {
    
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
        if (false == block_list_[i]->IsSolutionFinish(block_action_[i], NULL))
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

    int start_x, start_y;
    
    std::vector<int> path;
    int index[MAX_BLOCK];
    std::vector< Action > answer[MAX_BLOCK];

    memset(index, 0, sizeof(index));

    //拿到解
    for (int color = key_count_; color < block_count_; color ++) 
        block_list_[color]->IsSolutionFinish(block_action_[color], &answer[color]);

    if (ActionNormal == stack_.begin()->type_) {
        start_x = stack_.begin()->x_;
        start_y = stack_.begin()->y_;
    }
    else {
        start_x = answer[stack_.begin()->color_][0].x_;
        start_y = answer[stack_.begin()->color_][0].y_;
    }

    std::cout << "x = " << start_x << " y = " << start_y << std::endl;

    for (std::list< Action >::iterator it = stack_.begin(); it != stack_.end(); it ++) {
        //it->Display();

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
            int color = it->color_;
            for (size_t f = 0; f < answer[color][index[color]].path_.size(); f ++) {
                int dir = answer[color][index[color]].path_[f];

                if (-1 == dir)
                    continue;
                if (false == path.empty() && path[path.size() - 1] == dir)
                    continue;
                path.push_back(dir);
            }
            index[color] += 1;
        }
    }

    std::cout << "path=" ;
    for (size_t i = 0; i < path.size(); i ++) {
        std::cout << dir2name[path[i]] ;
    }
    std::cout << std::endl;

    //写入到文件当中
    FILE *file = fopen("answer", "w");

    fprintf(file, "%d\n%d\n", start_x, start_y);
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

bool Map::IsConnectCut() {
    int backup_color[MAX_GRID][MAX_GRID];
    memcpy(backup_color, color_info_, sizeof(color_info_));

    std::queue<std::pair<int, int> > que;
    bool flag = false;
    for (int i = 0; i < x_; i ++)
        for (int f = 0; f < y_; f ++)
            if (false == flag && WALL != backup_color[i][f]) {
                flag = true;
                backup_color[i][f] = WALL;
                que.push(std::make_pair(i, f));
            }

    while (false == que.empty()) {
        int now_x = que.front().first;
        int now_y = que.front().second;
        que.pop();

        for (int dir = 0; dir < 4; dir ++) {
            int next_x = now_x + direction[dir][0];
            int next_y = now_y + direction[dir][1];

            if (next_x < 0 || next_x >= x_ || next_y < 0 || next_y >= y_)
                continue;

            if(WALL == backup_color[next_x][next_y])
                continue;

            backup_color[next_x][next_y] = WALL;
            que.push(std::make_pair(next_x, next_y));
        }
    }

    //还有点不能被染色到，该剪纸了
    for (int i = 0; i < x_; i ++)
        for (int f = 0; f < y_; f ++)
            if (WALL != backup_color[i][f])
                return true;
    return false;
}

//还是采用精确枚举，能找到每一个Action
bool Map::DFSPath() {

    //stack_.rbegin()->Display();

    status_node_count_ += 1;
    //printf("%d\n", status_node_count_);
    if (status_node_count_ % 1000 == 0) {
        timeval now;
        gettimeofday(&now, NULL);
        double cost = now.tv_sec - start_.tv_sec + (now.tv_usec - start_.tv_usec) / 1000000.0;

        printf("[INFO] MakeAnswer handle %d nodes, per second handle %lf nodes\n",
               status_node_count_, status_node_count_ / cost);
        printf("[INFO] MakeAnswer cost %lf second\n", cost);
    }


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

        int forward_x, forward_y, ret;
        if (-1 != now.dir_) {
            forward_x = now.x_ + direction[now.dir_][0];
            forward_y = now.y_ + direction[now.dir_][1];
            ret = SearchColor(forward_x, forward_y);
        }

        std::vector<int> available_direction;

        //起点的情况, 可以自由选择
        if (-1 == now.dir_) {
            for (int dir = 0; dir < 4; dir++)
                available_direction.push_back(dir);
        }
        //只能行走原先方向 和周围两个方向 但是有条件
        else  {
            for (int dir = -1; dir < 2; dir++)
                available_direction.push_back( (now.dir_ + dir + 4) % 4);
        }

        for (size_t i = 0; i < available_direction.size(); i ++) {

            int dir = available_direction[i];

            //如果原先有方向，且和原先方向不一样并且前方是空的key point 是不能转向的
            if (-1 != now.dir_ && dir != now.dir_ && (ret > WALL  && ret < key_count_))
                continue;

            int next_x = now.x_ + direction[dir][0];
            int next_y = now.y_ + direction[dir][1];

            int forward_color = SearchColor(next_x, next_y);

            //是墙， GG
            if (WALL == forward_color)
                continue;

            //如果原先有方向，且当前和原先方向不一样，并且前方是Block, 需要加out_requirement
            if (-1 != now.dir_ && dir != now.dir_ && ret >= key_count_) {
                //如果当前那块都不存在肯定不行
                if (true == block_action_[ret].empty())
                    continue;
                block_action_[ret].rbegin()->out_requirement_.push_back(
                        std::make_pair(forward_x, forward_y));

                //判断是否有解
                std::vector< std::vector <Action > > new_solution = 
                    block_list_[ret]->FindNextSolution(block_action_[ret]);
                if (true == new_solution.empty()) {
                    block_action_[ret].rbegin()->out_requirement_.pop_back();
                    continue;
                }
            }

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
                
                //直接进入
                int color = color_info_[next_x][next_y];
                Action action_in;

                action_in.type_ = ActionIn;
                action_in.requirement_ = requirement_;
                action_in.color_ = color;
                action_in.dir_ = dir;
                action_in.key_point_ = stack_.rbegin()->color_;
                action_in.x_ = next_x;
                action_in.y_ = next_y;

                block_action_[color].push_back(action_in);
                stack_.push_back(action_in);

                if (true == DFSPath())
                    return true;

                //还原现场
                stack_.pop_back();
                block_action_[color].pop_back();

            }

            if (-1 != now.dir_ && dir != now.dir_ && ret >= key_count_)
                block_action_[ret].rbegin()->out_requirement_.pop_back();
        }

        return false;
    }

    //当前是在块中, 开始处理 找到合法后继 枚举出口
    int color = now.color_;
    int index = block_action_[color].size();
    std::vector< std::vector< Action> > next_solution = block_list_[color]->FindNextSolution(
            block_action_[color]);

    std::unordered_map<std::string, int> avail_exit;
    avail_exit.clear();

    for (size_t i = 0; i < next_solution.size(); i ++) {
        if ((int)next_solution[i].size() <= index)
            continue;

        char exit_hash[128];
        snprintf(exit_hash, sizeof(exit_hash), "%d%5d%d", next_solution[i][index].type_, 
                next_solution[i][index].key_point_, next_solution[i][index].dir_);

        if (avail_exit.end() != avail_exit.find(exit_hash))
            continue;
        avail_exit[exit_hash] = 1;

        //for out action, just copy it
        Action choose = next_solution[i][index];

        //检查是否结束
        if (ActionEnd == choose.type_) {
            stack_.push_back(choose);
            block_action_[color].push_back(choose);
            if(true == IsSearchFinish()) {
                GenerateAnswer();
                return true;
            }
            block_action_[color].pop_back();
            stack_.pop_back();
            continue;
        }

        int dir = choose.dir_;
        int next_x = choose.x_ + direction[dir][0];
        int next_y = choose.y_ + direction[dir][1];
        choose.out_requirement_.clear();

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
        
        block_list_[color]->FindSolutionStartHere();

        //加载现场
        Action choose ;
        choose.type_ = ActionStart;
        choose.color_ = color;

        block_action_[color].push_back(choose);
        stack_.push_back(choose);

        if (true == DFSPath())
            return true;

        //恢复现场
        stack_.pop_back();
        block_action_[color].pop_back();
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
    //getchar();

    if (4 != argc) {
        std::cout << "you must input map_str" << std::endl; 
        return 0;
    }

    Map *map = new Map(atoi(argv[1]), atoi(argv[2]), argv[3]);

 //   map->Display();
    map->MarkKeyPoint();
    map->ColorTheMap();
 //   map->DisplayColorMap();

    map->ColorCheck();
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
