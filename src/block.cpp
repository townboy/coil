#include "core.h"
#include "stdio.h"

#include <queue>
#include <sys/time.h>

//目前没有New出来的内存
Block::~Block() {
    
}

Block::Block(int color, Map *whole_map) : whole_map_(whole_map), color_(color) {
    status_node_count_ = 0;

    for (int i = 0; i < whole_map_->x_; i ++)
        for (int f = 0; f < whole_map_->y_; f ++) {
            //如果该点是墙 或者 关键点
            if (whole_map_->color_info_[i][f] < whole_map_->key_count_)
                color_map_[i][f] = whole_map_->color_info_[i][f];
            //如果是其他颜色
            else if (whole_map_->color_info_[i][f] != color_)
                color_map_[i][f] = WALL;
            //如果是自己颜色
            else 
                color_map_[i][f] = SELF;
        }
}

void Block::Display() {
    std::cout << "=======================================================" << std::endl;
    std::cout << "start display Block " << color_ << std::endl;
    std::cout << "=======================================================" << std::endl;
    for (int i = 0; i < whole_map_->x_; i ++) {
        for (int f = 0; f < whole_map_->y_; f ++) {
            if (SELF == color_map_[i][f])
                printf("  S");
            else if (WALL == color_map_[i][f])
                printf("  W");
            else
                printf("%3d", color_map_[i][f]);

        }
        std::cout << std::endl;
    }
}

void Block::FindSolutionOutside() {
    //将状态置为在外状态
    
    out_requirement_.clear();
    
    struct timeval start, now;

    gettimeofday(&start, NULL);
    StatusOutside();
    gettimeofday(&now, NULL);

    double cost = now.tv_sec - start.tv_sec + (now.tv_usec - start.tv_usec) / 1000000.0;
    std::cout << "[INFO] BLOCK " << color_ << " cost " << cost << " second" << std::endl;
    std::cout << "HANDLE " << status_node_count_ << " NODE Per Second handle " << 
        status_node_count_ / cost << " Nodes" << std::endl ;
    printf("[INFO] BLOCK %d find %d solutions\n", color_, head_outside.size());
}

/*  寻找一个可以进入内部的点和方向
 *  可以外部结束
 */
void Block::StatusOutside() {
   
    status_node_count_ += 1;

    if (true == IsConnectCut(-1, -1))
        return ;

    //走到外面结束了 并不需要加End Action。因为
    //EndAction 特指是在块内结束不再走出。
    if (true == IsEnd()) {
        OnFindSolution();
    }

    for (int i = 0; i < whole_map_->x_; i ++)
        for (int f = 0; f < whole_map_->y_; f ++) {

            //如果该点不是key point GG 
            if (false == IsKeyPoint(i, f))
                continue;

            for (int dir = 0; dir < 4; dir ++) {
               
                int inside_x = i + direction[dir][0];
                int inside_y = f + direction[dir][1];
               
                if (false == IsInsidePoint(inside_x, inside_y))
                    continue;
                
                // 开始处理合理的进入type = 1 action
                Action record = Action();

                record.color_ = this->color_;
                record.dir_ = dir;

                record.type_ = ActionIn;
                record.key_point_ = color_map_[i][f];
                record.x_ = inside_x;
                record.y_ = inside_y;
                record.path_.push_back(dir);

                //将这一条设为墙
                std::vector<std::pair<int ,int> > color_point = SetToWall(i, f, dir);
                //将进来的这个关键点也变成墙 
                int from_key_point = color_map_[i][f];
                color_map_[i][f] = WALL;

                stack_.push_back(record);

                //进入内部状态处理
                int now_x = color_point[color_point.size() - 1].first;
                int now_y = color_point[color_point.size() - 1].second;
                StatusInside(now_x, now_y, dir);

                //将墙变为块
                SetToSelf(color_point);
                stack_.pop_back();
                //将key point复原
                color_map_[i][f] = from_key_point;
            }
        }
}

void Block::SetToSelf(std::vector<std::pair<int, int> >color_point) {
    for (size_t i = 0; i < color_point.size(); i ++) {
        int x = color_point[i].first;
        int y = color_point[i].second;
        color_map_[x][y] = SELF; 
        out_requirement_.pop_back();
    }
}

std::vector<std::pair<int, int> > Block::SetToWall(int x, int y, int dir) {
    int next_x = x + direction[dir][0];
    int next_y = y + direction[dir][1];

    std::vector<std::pair<int, int> > color_point;

    while (true == IsInsidePoint(next_x, next_y)) {
        color_map_[next_x][next_y] = WALL;
        color_point.push_back(std::make_pair(next_x, next_y));
        out_requirement_.push_back(std::make_pair(next_x, next_y));

        next_x += direction[dir][0];
        next_y += direction[dir][1];
    }

    return color_point;
}

bool Block::IsKeyPoint(int x, int y) {
    if (x < 0 || x >= whole_map_->x_)
        return false;
    if (y < 0 || y >= whole_map_->y_)
        return false;
    if (color_map_[x][y] < 0)
        return false;
    return true;
}

bool Block::IsInsidePoint(int x,int y) {
    if (x < 0 || x >= whole_map_->x_)
        return false;
    if (y < 0 || y >= whole_map_->y_)
        return false;
    //只有-2才是内部的点, -1 为墙
    if (SELF == color_map_[x][y])
        return true;
    return false;
}

bool Block::IsEnd() {
    for (int i = 0; i < whole_map_->x_; i ++) 
        for (int f = 0; f < whole_map_->y_; f ++) {
            if (SELF == color_map_[i][f])
                return false;
        }
    return true;
}

//从该点进行探索
void Block::StatusInside(int now_x, int now_y, int old_dir) {

    //std::cout << "depth" << stack_.size() << std::endl;
    status_node_count_ += 1;

    /*
    if (true == IsHash(now_x, now_y, old_dir))
        return ;
    */
    if (true == IsConnectCut(now_x, now_y))
        return ;

    //该块已经在内部走完了，加上一个End Action 貌似结束点也没有什么用
    if (true == IsEnd()) {
        Action end_action;
        end_action.type_ = ActionEnd;
        end_action.color_ = this->color_;
        stack_.push_back(end_action);

        //找到一个解了
        OnFindSolution();

        stack_.pop_back();
    }

    int forward_x, forward_y;
    bool forward_is_keypoint;
    if (-1 != old_dir) {
        forward_x = now_x + direction[old_dir][0];
        forward_y = now_y + direction[old_dir][1];
        forward_is_keypoint = IsKeyPoint(forward_x, forward_y);
    }

    //对四个方向进行枚举 可以走出去结束
    for (int dir = 0; dir < 4; dir ++) {
        int next_x = now_x + direction[dir][0];
        int next_y = now_y + direction[dir][1];

        //该方向可走
        if (true == IsInsidePoint(next_x, next_y) || true == IsKeyPoint(next_x, next_y)) {

            int result = IsWallOrKeypoint(now_x, now_y, dir);

            //走过的路变成墙
            std::vector<std::pair<int, int> > color_point = SetToWall(now_x, now_y, dir);
            int end_x;
            int end_y;
            if (false == color_point.empty()) {
                end_x = color_point[color_point.size() - 1].first;
                end_y = color_point[color_point.size() - 1].second;
            }
            else {
                end_x = now_x;
                end_y = now_y;
            }

            //如果走到底是墙 保存状态继续拐
            //if (WALL == result)
            if (false == color_point.empty())
            {
                //走过的路径添加进去
                stack_[stack_.size() - 1].path_.push_back(dir);
                //拐弯加上需求点
                if (-1 != old_dir && dir != old_dir && true == forward_is_keypoint )
                    stack_[stack_.size() - 1].requirement_.push_back(color_map_[forward_x][forward_y]);

                StatusInside(end_x, end_y, dir);

                if (-1 != old_dir && dir != old_dir && true == forward_is_keypoint )
                    stack_[stack_.size() - 1].requirement_.pop_back();
                stack_[stack_.size() - 1].path_.pop_back();


                //恢复现场
                SetToSelf(color_point);
            }
            //如果走到底是key point StatusOutside() 必须要走到边缘才能走出去
            if (WALL != result && true == color_point.empty()) {

                //判断之前有没有从该点出去过
                size_t index = 1;
                for (; index < stack_.size(); index ++)
                    if (stack_[index].type_ == ActionOut && stack_[index].key_point_ == result)
                        break;
                if (index != stack_.size())
                    continue;
                
                //从该点出去 
                Action outside_action ;

                outside_action.color_ = this->color_;
                outside_action.dir_ = dir;

                outside_action.type_ = ActionOut;
                outside_action.key_point_ = result;
                outside_action.x_ = end_x;
                outside_action.y_ = end_y;
                outside_action.path_.push_back(dir);
                outside_action.out_requirement_ = this->out_requirement_;

                if (-1 != old_dir && dir != old_dir && true == forward_is_keypoint)
                    stack_[stack_.size() - 1].requirement_.push_back(color_map_[forward_x][forward_y]);

                //走过的路径添加进去
                if (false == color_point.empty() && dir != old_dir)
                    stack_[stack_.size() - 1].path_.push_back(dir);

                stack_.push_back(outside_action);

                StatusOutside();
                //恢复现场
                SetToSelf(color_point);

                stack_.pop_back();

                if (false == color_point.empty() && dir != old_dir)
                    stack_[stack_.size() - 1].path_.pop_back();

                if (-1 != old_dir && dir != old_dir && true == forward_is_keypoint)
                    stack_[stack_.size() - 1].requirement_.pop_back();

            }
        }
    }

}

int Block::IsWallOrKeypoint(int x, int y, int dir) {
    int next_x = x + direction[dir][0];
    int next_y = y + direction[dir][1];
    //如果是内部点的话就往下走
    while (true == IsInsidePoint(next_x, next_y)) {
        next_x += direction[dir][0];
        next_y += direction[dir][1];
    }
    //在地图外面了。是墙
    if (next_x < 0 || next_x >= whole_map_->x_)
        return WALL;
    if (next_y < 0 || next_y >= whole_map_->y_)
        return WALL;
    return color_map_[next_x][next_y];
}

void Block::OnFindSolution() {
    //把解直接记录 分两种类型进行记录
    if (ActionStart == stack_[0].type_) {
        /*
        std::cout << "Start Display One Solution" << std::endl << std::endl << std::endl;
        for (size_t i = 0; i < stack_.size(); i ++) {
            stack_[i].Display();
        }
        std::cout << "End Display One Solution" << std::endl << std::endl << std::endl;
        */
        head_start.push_back(stack_);
    }
    else
        head_outside.push_back(stack_);

}

void Block::StatusStart() {
    is_hash_sum_ = 0;
    is_hash_succ_ = 0;

    for (int i = 0; i < whole_map_->x_; i ++) 
        for (int f = 0; f < whole_map_->y_; f ++) {
            if (SELF != color_map_[i][f])
                continue;

            //就从该点开始
            Action action_start;

            action_start.color_ = this->color_;
            action_start.dir_ = -1;

            action_start.type_ = ActionStart;
            action_start.x_ = i;
            action_start.y_ = f;
            
            stack_.push_back(action_start);
            color_map_[i][f] = WALL;
            out_requirement_.push_back(std::make_pair(i, f));

            printf("[MAKE ANSWER] start find solution start from (%d, %d)\n", i, f);
            StatusInside(i, f, -1);

            out_requirement_.pop_back();
            color_map_[i][f] = SELF;
            stack_.pop_back();

            hash_map_.clear();

            //printf("[Calculate] IsHash() %lf\n", 1 - 1.0 * is_hash_succ_ /is_hash_sum_ );
        }
}

std::vector< std::vector<Action> > Block::FindSolutionStartHere() {
    out_requirement_.clear();
    StatusStart();
    printf("[DEBUG] block %d find %d start solution\n", color_, head_start.size());
    return head_start;
}

void Action::Display() {
    std::cout << "======================" << std::endl;
    std::cout << "Display Action" << std::endl;
    std::cout << "======================" << std::endl;

    if (ActionNormal == type_) {
        std::cout << "TYPE Normal  Color: " << color_ << "  x: " << x_ << "  y: " << y_ << " dir: " << dir_; 
    }
    else if (ActionIn == type_) {
        std::cout << "TYPE  IN  Key Point: " << key_point_ << "  x: " << x_ << "  y: " << y_; 
    }
    else if (ActionOut == type_) {
        std::cout << "TYPE  OUT Key Point: " << key_point_ << "  x: " << x_ << "  y: " << y_; 
    }
    else if (ActionStart == type_) {
        std::cout << "TYPE START " << "  x: " << x_ << "  y: " << y_; 
    }
    else {
        std::cout << "TYPE END  ";
    }
    std::cout << std::endl << "PATH = ";
    
    for (size_t i = 0 ; i < path_.size(); i ++) {
        std::cout << dir2name[path_[i]];
    }

    std::cout << std::endl << "Requirement Key Point = ";
    
    for (size_t i = 0 ; i < requirement_.size(); i ++) {
        std::cout << requirement_[i] << " ";
    }

    std::cout << std::endl;
}

bool Block::IsSolutionFinish(std::list< Action > solution, std::vector< Action > *out) {
    //至少要有两个Action 并且是偶数个
    if (true == solution.empty() || 0 != solution.size() % 2)
        return false;

    //判断这个块是否是全图的起点
    std::vector< std::vector< Action> > head;
    if (ActionStart == solution.begin()->type_) {
        head = head_start;
    }
    if (ActionIn == solution.begin()->type_) {
        head = head_outside;
    }

    for (size_t i = 0; i < head.size(); i ++) {
        //Action数量不一样 肯定不行
        if (solution.size() != head[i].size())
            continue;

        size_t index = 0;
        std::list<Action> ::iterator it = solution.begin();
        for (; index < head[i].size(); index ++, it ++) {
            if (false == head[i][index].IsEqual(*it) )
                break;
        }
        if (index == head[i].size()) {
            if (NULL != out) {
                out->clear();
                for (size_t f = 0; f < head[i].size(); f ++) {
                    out->push_back(head[i][f]);
                }
            }
            return true;
        }
    }
    
    return false;
}

//对象是具体解，传入的foo是枚举量
bool Action::IsEqual( Action foo) {
    if (type_ != foo.type_)
        return false;

    //比较颜色
    if (color_ != foo.color_)
        return false;

    std::unordered_map<int, bool> requirement_hash;

    for (size_t i = 0; i < foo.requirement_.size(); i ++)
        requirement_hash[foo.requirement_[i]] = true;

    //比较requirement 能不能被满足
    for (size_t i = 0; i < requirement_.size(); i ++) {
        std::unordered_map<int, bool>::iterator it = requirement_hash.find(requirement_[i]);
        if (requirement_hash.end() == it)
            return false;
    }

    std::map<std::pair<int, int>, bool> out_requirement_hash;

    for (size_t i = 0; i < out_requirement_.size(); i ++)
        out_requirement_hash[out_requirement_[i]] = true;

    //比较out_requirement 能不能被满足
    for (size_t i = 0; i < foo.out_requirement_.size(); i ++) {
        std::map<std::pair<int, int>, bool>::iterator it = 
            out_requirement_hash.find(foo.out_requirement_[i]);

        if (out_requirement_hash.end() == it)
            return false;
    }
    
    //此时肯定没有key point 被填充
    if (ActionStart == type_) {
        if (false == requirement_.empty())
            return false;
        return true;
    }

    if (ActionIn == type_) {
        if (key_point_ != foo.key_point_)
            return false;
        if (dir_ != foo.dir_)
            return false;
        if (x_ != foo.x_ || y_ != foo.y_)
            return false;
        return true;
    }

    if (ActionOut == type_) {
        if (key_point_ != foo.key_point_)
            return false;
        if (dir_ != foo.dir_)
            return false;
        if (x_ != foo.x_ || y_ != foo.y_)
            return false;
        return true;
    }

    if (ActionEnd == type_) {
        return true;
    }

    return false;
}

std::vector< std::vector < Action> > Block::FindNextSolution(std::list< Action > path) {

    std::vector< std::vector< Action > > result;
    std::vector< std::vector< Action > > head;

    if (false == path.empty() && ActionStart == path.begin()->type_)
        head = head_start;
    else 
        head = head_outside;

    std::unordered_map<std::string, int> avail_exit;
    avail_exit.clear();
    size_t path_index = path.size();

    for (size_t i = 0; i < head.size(); i ++) {

        if (path_index >= head[i].size())
            continue;

        char exit_hash[128];
        snprintf(exit_hash, sizeof(exit_hash), "%d%5d%d", head[i][path_index].type_,
            head[i][path_index].key_point_, head[i][path_index].dir_);

        if (avail_exit.end() != avail_exit.find(exit_hash))
            continue;

        size_t index = 0;
        std::list< Action >::iterator it = path.begin();
        for (; path.end() != it; it ++, index ++) {
            //现在要求严格相同
            //if (false == it->IsEqual(head[i][index]) )
            if (false == head[i][index].IsEqual(*it))
                break;
        }

        //走完了 全部相同
        if (it == path.end()) {
            result.push_back(head[i]);
            avail_exit[exit_hash] = 1;
        }

    }

    return result;
}

bool Block::IsConnectCut(int now_x, int now_y) {
    int backup_map[MAX_GRID][MAX_GRID];
    memcpy(backup_map, color_map_, sizeof(color_map_));

    std::queue<std::pair<int, int> > color_queue;
    for (int i = 0; i < whole_map_->x_; i ++) 
        for (int f = 0; f < whole_map_->y_; f ++)
            //这就是key point
            if (backup_map[i][f] >= 0)
                color_queue.push(std::make_pair(i ,f));

    if (-1 != now_x && -1 != now_y)
        color_queue.push(std::make_pair(now_x, now_y));

    while(false == color_queue.empty()) {
        std::pair<int, int> &now = color_queue.front();
        color_queue.pop();

        int x = now.first;
        int y = now.second;

        for (int dir = 0; dir < 4; dir ++) {
            int next_x = x + direction[dir][0];
            int next_y = y + direction[dir][1];

            if (next_x < 0 || next_x >= whole_map_->x_)
                continue;
            if (next_y < 0 || next_y >= whole_map_->y_)
                continue;
            
            if (SELF == backup_map[next_x][next_y]) {
                backup_map[next_x][next_y] = WALL;
                color_queue.push(std::make_pair(next_x, next_y));
            }
        }
    }

    for (int i = 0; i < whole_map_->x_; i ++) 
        for (int f = 0; f < whole_map_->y_; f ++)
            //存在一个点不能被染到
            if (SELF == backup_map[i][f])
                return true;
    return false;
}

bool Block::IsHash(int now_x, int now_y, int dir) {

    is_hash_sum_ += 1;

    std::string key;
    for (int i = 0; i < whole_map_->x_; i ++)
        for (int f = 0; f < whole_map_->y_; f ++)
            key += (WALL == color_map_[i][f] ? "1" : "0");

    char info[128];
    for (size_t i = 0; i < stack_.size(); i ++) {
        snprintf(info, sizeof(info), "%d%5d%5d%5d", stack_[i].type_, stack_[i].key_point_, 
                stack_[i].x_, stack_[i].y_);
        key += info;
    }

    snprintf(info, sizeof(info), "%5d%5d%2d", now_x, now_y, dir);
    key += info;

    std::unordered_map<std::string, bool> ::iterator it = hash_map_.find(key);
    if (hash_map_.end() != it)
        return true;
    is_hash_succ_ += 1;
    hash_map_[key] = true;
    return false;
}
