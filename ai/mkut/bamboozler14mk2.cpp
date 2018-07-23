#include <iostream>
#include <queue>
#include <sstream>
#include <map>
#include <set>
#include <deque>
using namespace std;

struct Point {
    int x, y, z;
    Point() : x(0), y(0), z(0) {}
    Point(int x, int y, int z) : x(x), y(y), z(z) {}
    bool inside(int x_min, int x_max, int y_min, int y_max, int z_min, int z_max) {
        return x_min <= x && x < x_max
            && y_min <= y && y < y_max
            && z_min <= z && z < z_max;
    }
    bool inside(int x_max, int y_max, int z_max) {
        return inside(0, x_max, 0, y_max, 0, z_max);
    }
    bool operator<(const Point& a) const {
        if (x != a.x) return x < a.x;
        if (y != a.y) return y < a.y;
        if (z != a.z) return z < a.z;
        return false;
    }
    bool operator==(const Point& a) const {
        if (x != a.x) return false;
        if (y != a.y) return false;
        if (z != a.z) return false;
        return true;
    }
    bool operator!=(const Point& a) const {
        if (x != a.x) return true;
        if (y != a.y) return true;
        if (z != a.z) return true;
        return false;
    }
    Point operator+(const Point& a) const {
        return Point(x + a.x, y + a.y, z + a.z);
    }
    Point operator-(const Point& a) const {
        return Point(x - a.x, y - a.y, z - a.z);
    }
    Point operator*(const int n) const {
        return Point(x * n, y * n, z * n);
    }
    Point operator-() const {
        return Point(-x, -y, -z);
    }
};

struct AStarPoint {
    int x, y, z, c, ec;
    AStarPoint() : x(0), y(0), z(0), c(0), ec(0) {}
    AStarPoint(Point& p, int c, int ec) : x(p.x), y(p.y), z(p.z), c(c), ec(ec) {}
    AStarPoint(int x, int y, int z, int c, int ec) : x(x), y(y), z(z), c(c), ec(ec) {}
    bool operator<(const AStarPoint& a) const {
        if (ec != a.ec) return ec > a.ec;
        if (c != a.c) return c > a.c;
        if (x != a.x) return x < a.x;
        if (y != a.y) return y < a.y;
        if (z != a.z) return z < a.z;
        return false;
    }
    Point to_point() {
        return Point(x, y, z);
    }
};

struct Command {
    int type; // 1: smove, 2: lmove, 3: fill
    int idx, len, idx2, len2;
    Command() {}
    Command(int type, int idx) : type(type), idx(idx) {}
    Command(int type, int idx, int len) : type(type), idx(idx), len(len) {}
    Command(int type, int idx, int len, int idx2, int len2) : type(type), idx(idx), len(len), idx2(idx2), len2(len2) {}
    Point nextPos(const Point& p);
    vector<Point> reservePos(const Point& p);
    vector<Point> blockPos(const Point& p);
    string to_str();
};

int NUM_ADJ = 6;
int adj_dx[] = {1,-1, 0, 0, 0, 0};
int adj_dy[] = {0, 0, 1,-1, 0, 0};
int adj_dz[] = {0, 0, 0, 0, 1,-1};
Point adjs[] = {
    Point( 1, 0, 0),
    Point(-1, 0, 0),
    Point( 0, 1, 0),
    Point( 0,-1, 0),
    Point( 0, 0, 1),
    Point( 0, 0,-1),
};

int NUM_ND = 18;
int nd_dx[] = {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,-1,-1,-1,-1,-1};
int nd_dy[] = {1, 0, 0, 0,-1, 1, 1, 1, 0, 0,-1,-1,-1, 1, 0, 0, 0,-1};
int nd_dz[] = {0, 1, 0,-1, 0, 1, 0,-1, 1,-1, 1, 0,-1, 0, 1, 0,-1, 0};
Point nds[] = {
    Point( 1, 1, 0),
    Point( 1, 0, 1),
    Point( 1, 0, 0),
    Point( 1, 0,-1),
    Point( 1,-1, 0),
    Point( 0, 1, 1),
    Point( 0, 1, 0),
    Point( 0, 1,-1),
    Point( 0, 0, 1),
    Point( 0, 0,-1),
    Point( 0,-1, 1),
    Point( 0,-1, 0),
    Point( 0,-1,-1),
    Point(-1, 1, 0),
    Point(-1, 0, 1),
    Point(-1, 0, 0),
    Point(-1, 0,-1),
    Point(-1,-1, 0),
};

int R, N;
bool field[256][256][256] = {};
int global_depth[256][256][256] = {};
deque<set<Point> > reservation;
//bool current_model[256][256][256] = {};
int scheduled[256][256][256] = {};
int boxel_score[256][256][256] = {};
//dead_zone[256][256][256] = {};
set<Point> remainings;
set<Point> reachables;
Point bot_pos[40] = {};
queue<Command> exec_plan[40] = {};
bool bot_terminated[40] = {};
int terminated_bot_count = 0;
int scheduledCount = 0;
int boxelCount = 0;
int turn = 1;

Point Command::nextPos(const Point& p) {
    switch (type) {
        case 1:
            return p + adjs[idx] * len;
        case 2:
            return p + adjs[idx] * len + adjs[idx2] * len2;
        case 3:
            return p;
    }
    cerr << "ERROR 1" << endl;
    return p;
}

vector<Point> Command::reservePos(const Point& p) {
    vector<Point> ret;
    switch (type) {
        case 1:
            for (int i = 0; i <= len; i++) {
                ret.push_back(p + adjs[idx] * i);
            }
            return ret;
        case 2:
            for (int i = 0; i <= len; i++) {
                ret.push_back(p + adjs[idx] * i);
            }
            for (int i = 1; i <= len2; i++) {
                ret.push_back(p + adjs[idx] * len + adjs[idx2] * i);
            }
            return ret;
        case 3:
            ret.push_back(p + nds[idx]);
            ret.push_back(p);
            return ret;
    }
    cerr << "ERROR 2" << endl;
    return vector<Point>();
}

vector<Point> Command::blockPos(const Point& p) {
    switch (type) {
        case 1:
        case 2:
            return vector<Point>();
        case 3:
            return vector<Point>(1, p + nds[idx]);
    }
    cerr << "ERROR 4" << endl;
    return vector<Point>();
}

string Command::to_str() {
    stringstream ss;
    switch (type) {
        case 1: ss << "smove "; break;
        case 2: ss << "lmove "; break;
        case 3: ss << "fill "; break;
    }
    if (type == 1 || type == 2) {
        ss << adj_dx[idx] * len << " " << adj_dy[idx] * len << " " << adj_dz[idx] * len;
        if (type == 2) {
            ss << " " << adj_dx[idx2] * len2 << " " << adj_dy[idx2] * len2 << " " << adj_dz[idx2] * len2;
        }
    } else if (type == 3) {
        ss << nd_dx[idx] << " " << nd_dy[idx] << " " << nd_dz[idx];
    }
    return ss.str();
}

int manhattan(const Point& a) {
    return abs(abs(a.x) + abs(a.y) + abs(a.z) - 1);
}

void reserve(const Point& p, int time) {
    cerr << "reserve: global_time=" << (turn + time) << " " << p.x << " " << p.y << " " << p.z << endl;
    while (time >= reservation.size()) reservation.push_back(set<Point>());
    reservation[time].insert(p);
}

bool reserved(const Point&p, int time) {
    if (time >= reservation.size()) return false;
    return scheduled[p.x][p.y][p.z] || reservation[time].count(p);
}

void nextTurn() {
    cerr << "turn end: " << turn << endl;
    reservation.pop_front();
    turn++;
    // if (turn == 20) exit(1);
}

void read_input() {
    unsigned char c;
    cin.read((char *)&c, 1);
    R = c;
    cerr << "R = " << R << endl;
    const int bytes = (R * R * R + 7) / 8;
    cerr << "Reading " << bytes << " bytes" << endl;

    int x = 0, y = 0, z = 0;
    for (int i = 0; i < bytes; i++) {
        cin.read((char *)&c, 1);
        for (int j = 0; j < 8 && x < R; j++) {
            field[x][y][z] = (c >> j) & 1;
            z++;
            if (z == R) z = 0, y++;
            if (y == R) y = 0, x++;
        }
    }
}

vector<vector<vector<char> > > alive_zone(const Point& additional) {
    vector<vector<vector<char> > > alive(R, vector<vector<char> >(R, vector<char>(R)));
    queue<Point> q;
    q.push(Point(0, 0, 0));
    alive[0][0][0] = 1;

    while (q.size()) {
        Point p = q.front(); q.pop();

        for (int i = 0; i < NUM_ADJ; i++) {
            Point np = p + adjs[i];
            if (!np.inside(R, R, R))  continue;
            if (scheduled[np.x][np.y][np.z] || additional == np) continue;
            if (!alive[np.x][np.y][np.z]) {
                alive[np.x][np.y][np.z] = 1;
                q.push(np);
            }
        }
    }
    return alive;
}


int find_path2(int id, const Point& target) {
    Point& pos = bot_pos[id];

    map<Point, Point> prev;
    map<Point, Command> command;
    priority_queue<AStarPoint> q;
    q.push(AStarPoint(pos, 0, manhattan(target - pos)));
    prev[pos] = pos;

    bool found = false;
    while (q.size()) {
        AStarPoint asp = q.top(); q.pop();
        Point p = asp.to_point();
        if (p == target) {
            vector<Command> revExecPlan;
            vector<vector<Point> > revReservePos;
            Point tp(p);
            while (tp != pos) {
                revExecPlan.push_back(command[tp]);
                revReservePos.push_back(command[tp].reservePos(prev[tp]));
                tp = prev[tp];
            }
            for (int i = revExecPlan.size() - 1; i >= 0; i--) {
                exec_plan[id].push(revExecPlan[i]);
            }
            for (int i = revReservePos.size() - 1; i >= 0; i--) {
                for (int j = 0; j < revReservePos[i].size(); j++) {
                    reserve(revReservePos[i][j], revReservePos.size() - 1 - i);
                }
            }
            reserve(p, revReservePos.size());
            return 0;
        }
        if (found) continue;
        // smove
        for (int i = 0; i < NUM_ADJ; i++) {
            for (int j = 1; j <= 15; j++) {
                Point np = p + adjs[i] * j;
                if (!np.inside(R, R, R)) break;
                if (np != pos && reserved(np, asp.c)) break;
                if (!prev.count(np)) {
                    prev[np] = p;
                    command[np] = Command(1, i, j);
                    q.push(AStarPoint(np, asp.c + 1, asp.c + 1 + manhattan(target - np)));
                    if (np == target) {
                        found = true;
                    }
                }
            }
        }
        // lmove
        for (int i1 = 0; i1 < NUM_ADJ; i1++) {
            for (int k1 = 1; k1 <= 5; k1++) {
                Point np1 = p + adjs[i1] * k1;
                if (!np1.inside(R, R, R)) break;
                if (np1 != pos && reserved(np1, asp.c)) break;
                for (int i2 = 0; i2 < 6; i2++) {
                    for (int k2 = 1; k2 <= 5; k2++) {
                        Point np = np1 + adjs[i2] * k2;
                        if (!np.inside(R, R, R)) break;
                        if (np != pos && reserved(np, asp.c)) break;
                        if (!prev.count(np)) {
                            prev[np] = p;
                            command[np] = Command(2, i1, k1, i2, k2);
                            q.push(AStarPoint(np, asp.c + 1, asp.c + 1 + manhattan(target - np)));
                            if (np == target) {
                                found = true;
                            }
                        }
                    }
                }
            }
        }
    }
    cerr << "> <" << endl;
    return 1;
}

void build_score() {
    vector<vector<vector<char> > > alive_zone(R, vector<vector<char> >(R, vector<char>(R)));
    queue<Point> q;
    q.push(Point(0, 0, 0));
    alive_zone[0][0][0] = 1;

    while (q.size()) {
        Point p = q.front(); q.pop();

        for (int i = 0; i < NUM_ADJ; i++) {
            Point np = p + adjs[i];
            if (!np.inside(R, R, R))  continue;
            if (field[np.x][np.y][np.z]) continue;
            if (!alive_zone[np.x][np.y][np.z]) {
                alive_zone[np.x][np.y][np.z] = 1;
                q.push(np);
            }
        }
    }

    for (int x = 0; x < R; x++) for (int y = 0; y < R; y++) for (int z = 0; z < R; z++) if (field[x][y][z]) {
        int score = 0;
        set<Point> visited;
        queue<Point> q;
        q.push(Point(-1, -1, -1));
        q.push(Point(x, y, z));
        visited.insert(Point(x, y, z));
        bool end = false;
        while (q.size() > 1) {
            Point p = q.front(); q.pop();
            if (p.x == -1) {
                if (end) break;
                q.push(p);
                continue;
            }
            if (alive_zone[p.x][p.y][p.z]) {
                end = true;
                continue;
            }
            score++;
            for (int i = 0; i < NUM_ADJ; i++) {
                Point np = Point(x, y, z) + adjs[i];
                if (visited.count(np)) continue;
                if (!np.inside(R, R, R)) continue;
                q.push(np);
                visited.insert(np);
            }
        }
        // cerr << score << endl;
        boxel_score[x][y][z] = score;
    }
}

int find_target(int id) {
    Point& pos = bot_pos[id];

    set<int> possibleScores;
    for (auto r = reachables.begin(); r != reachables.end(); r++) {
        int score = boxel_score[r->x][r->y][r->z];
        possibleScores.insert(score);
    }

    int score_i = 0;
    for (auto score_it = possibleScores.rbegin(); score_it != possibleScores.rend(); score_it++) {
        score_i++;
//        if (score_i == 3) break;
        int targetScore = *score_it;
        map<Point, Command> goal;
        for (auto r = reachables.begin(); r != reachables.end(); r++) {
            if (boxel_score[r->x][r->y][r->z] < targetScore) continue;
//            vector<vector<vector<char> > > alive_zone_after = alive_zone(*r);
            bool ng = false;

//            for (auto it = remainings.begin(); it != remainings.end(); ++it) {
//                if (*it != *r && !alive_zone_after[it->x][it->y][it->z]) {
//                    // cerr << "x _ x" << endl;
//                    // cerr << "予定ブロック: " << it->x << "," << it->y << "," << it->z << "   設置したいブロック: " << r->x << "," << r->y << "," << r->z << endl;
//                    ng = true;
//                    break;
//                }
//            }
//            if (ng) continue;
//
//            for (auto it = reservation.begin(); it != reservation.end(); ++it) {
//                if (*it != pos && !scheduled[it->x][it->y][it->z] && !alive_zone_after[it->x][it->y][it->z]) {
//                    // cerr << "x _ x" << endl;
//                    // cerr << "予約地点: " << it->x << "," << it->y << "," << it->z << "   設置したいブロック: " << r->x << "," << r->y << "," << r->z << endl;
//                    ng = true;
//                    break;
//                }
//            }
//            if (ng) continue;

            for (int i = 0; i < NUM_ND; i++) {
                Point np = nds[i] + *r;
                if (np.inside(R, R, R)) {
                    goal[np] = Command(3, NUM_ND - 1 - i);
                }
            }
        }
        if (!goal.size()) {
            continue;
        }

        map<Point, Point> prev;
        map<Point, Command> command;
        queue<Point> q;
        q.push(pos);
        q.push(Point(-1, -1, -1));
        prev[pos] = pos;

        bool found = false;
//        int limit = 0;
        int time = 0;
        while (q.size() > 1) {
//            limit++;
//            if (limit >= 1000 && !found && score_i == possibleScores.size() - 1) {
//                // cerr << limit;
//                break;
//            }
            Point p = q.front(); q.pop();
            if (p.x == -1) {
                time++;
                q.push(p);
                continue;
            }
            if (goal.count(p)) {
                vector<Point> reservePos = goal[p].reservePos(p);
                bool ng = false;
                //bool flag = turn == 35 && id == 19;
                if (reserved(p, time + 1)) ng = true;
                for (int i = 0; i < reservePos.size(); i++) {
                    //if (flag) cerr << reservePos[i].x << "," << reservePos[i].y << "," << reservePos[i].z << " " << time << endl;
                    if (reserved(reservePos[i], time)) {
                        ng = true;
                        break;
                    }
                }
                if (!ng) {
                    vector<Command> revExecPlan;
                    vector<vector<Point> > revReservePos;
                    revExecPlan.push_back(goal[p]);
                    revReservePos.push_back(reservePos);
                    vector<Point> blockPos = goal[p].blockPos(p);
                    for (int j = 0; j < blockPos.size(); j++) {
                        scheduled[blockPos[j].x][blockPos[j].y][blockPos[j].z] = 1;
                        scheduledCount++;
                        remainings.erase(blockPos[j]);
                        cerr << scheduledCount << "/" << boxelCount << endl;
                        reachables.erase(blockPos[j]);
                    }
                    Point tp(p);
                    while (tp != pos) {
                        revExecPlan.push_back(command[tp]);
                        vector<Point> reservePos = command[tp].reservePos(prev[tp]);
                        revReservePos.push_back(reservePos);
                        tp = prev[tp];
                    }
                    for (int i = revExecPlan.size() - 1; i >= 0; i--) {
                        exec_plan[id].push(revExecPlan[i]);
                    }
                    for (int i = revReservePos.size() - 1; i >= 0; i--) {
                        for (int j = 0; j < revReservePos[i].size(); j++) {
                            reserve(revReservePos[i][j], revReservePos.size() - 1 - i);
                            //if (flag) cerr << revReservePos[i][j].x << "," << revReservePos[i][j].y << "," << revReservePos[i][j].z << " " << revReservePos.size() - 1 - i << endl;
                        }
                    }
                    reserve(p, revReservePos.size());
                    // cerr << "[" << id << "]" << reservePos[0].x << "," << reservePos[0].y << "," << reservePos[0].z << endl;
                    return 0;
                }
            }
            if (found) continue;
            // smove
            for (int i = 0; i < NUM_ADJ; i++) {
                for (int j = 1; j <= 15; j++) {
                    Point np = p + adjs[i] * j;
                    if (!np.inside(R, R, R)) break;
                    if (np != pos && reserved(np, time)) break;
                    if (prev.count(np)) break;
                    if (!prev.count(np)) {
                        prev[np] = p;
                        command[np] = Command(1, i, j);
                        q.push(np);
                        if (goal.count(np)) {
                            vector<Point> reservePos = goal[np].reservePos(np);
                            bool ng = false;
                            if (reserved(np, time + 2)) ng = true;
                            for (int i = 0; i < reservePos.size(); i++) {
                                if (reserved(reservePos[i], time + 1)) {
                                    ng = true;
                                    break;
                                }
                            }
                            if (ng) found = true;
                        }
                    }
                }
            }
            // lmove
            for (int i1 = 0; i1 < NUM_ADJ; i1++) {
                for (int k1 = 1; k1 <= 5; k1++) {
                    Point np1 = p + adjs[i1] * k1;
                    if (!np1.inside(R, R, R)) break;
                    if (np1 != pos && reserved(np1, time)) break;
                    if (prev.count(np1)) break;
                    for (int i2 = 0; i2 < 6; i2++) {
                        for (int k2 = 1; k2 <= 5; k2++) {
                            Point np = np1 + adjs[i2] * k2;
                            if (!np.inside(R, R, R)) break;
                            if (np != pos && reserved(np, time)) break;
                            if (prev.count(np)) break;
                            if (!prev.count(np)) {
                                prev[np] = p;
                                command[np] = Command(2, i1, k1, i2, k2);
                                q.push(np);
                                if (goal.count(np)) {
                                    vector<Point> reservePos = goal[np].reservePos(np);
                                    bool ng = false;
                                    if (reserved(np, time + 2)) ng = true;
                                    for (int i = 0; i < reservePos.size(); i++) {
                                        if (reserved(reservePos[i], time + 1)) {
                                            ng = true;
                                            break;
                                        }
                                    }
                                    if (ng) found = true;
                                }
                            }
                        }
                    }
                }
            }
        }

        // cerr << "見つからない: score=" << targetScore << endl;
    }
    // cerr << "> <" << endl;
    return 1;
}

void plan_bot(int id) {
    if (bot_terminated[id]) return;
    if (exec_plan[id].size() == 0) {
        if (scheduledCount < boxelCount) {
            int res = find_target(id);
            if (res != 0) {
                // cerr << "[" << id << "]: waiting" << endl;
                find_path2(id, Point(id, 0, 0));
                if (exec_plan[id].size() == 0) {
                    // cerr << "[" << id << "]" << "閉じ込められた" << bot_pos[id].x << " " << bot_pos[id].y << " " << bot_pos[id].z << endl;
//                    if (bot_pos[id] == Point(id, 0, 0)) {
//                        bot_terminated[id] = true;
//                        terminated_bot_count++;
//                    }
                }
            }
        } else if (bot_pos[id] != Point(id, 0, 0)) {
            find_path2(id, Point(id, 0, 0));
        } else {
            if (!bot_terminated[id]) {
                bot_terminated[id] = true;
                terminated_bot_count++;
            }
        }
    }
}

bool exec_bot(int id) {
    if (exec_plan[id].size()) {
        Command cmd = exec_plan[id].front();
        exec_plan[id].pop();
        vector<Point> blockPos = cmd.blockPos(bot_pos[id]);
        for (int i = 0; i < blockPos.size(); i++) {
            reachables.erase(blockPos[i]);
            // current_model[blockPos[i].x][blockPos[i].y][blockPos[i].z] = true;
            for (int j = 0; j < NUM_ADJ; j++) {
                Point newReachable = blockPos[i] + adjs[j];
                if (field[newReachable.x][newReachable.y][newReachable.z] && !scheduled[newReachable.x][newReachable.y][newReachable.z]) {
                    reachables.insert(newReachable);
                }
            }
        }
        bot_pos[id] = cmd.nextPos(bot_pos[id]);
        cerr << "(" << id << "): " << cmd.to_str() << " " << bot_pos[id].x << "," << bot_pos[id].y << "," << bot_pos[id].z << endl;
        cout << cmd.to_str() << endl;
        return true;
    } else {
        cerr << "(" << id << "): wait" << endl;
        cout << "wait" << endl;
        return false;
    }
}

int main() {
    read_input();
    cerr << "読み込みおわり" << endl;

    for (int x = 0; x < R; x++) for (int y = 0; y < R; y++) for (int z = 0; z < R; z++) if (field[x][y][z]) {
        boxelCount++;
        remainings.insert(Point(x, y, z));
    }
    for (int x = 0; x < R; x++) for (int z = 0; z < R; z++) if (field[x][0][z]) reachables.insert(Point(x, 0, z));
    build_score();
//return 0;
    N = min(40, R);

    for (int i = 0; i < N; i++) {
        reserve(Point(i, 0, 0), i);
        if (i > 0) {
            reserve(Point(i, 0, 0), i - 1);
        }
    }
    for (int i = 0; i < N - 1; i++) {
        for (int k = 0; k < i; k++) {
            plan_bot(k);
        }
        for (int k = 0; k < i; k++) {
            exec_bot(k);
        }
        cout << "fission 1 0 0 " << (N - 2 - i) << endl;
        // cerr << "reach: " << reachables.size() << endl;
        // cerr << "==========" << endl;
        bot_pos[i+1] = Point(i+1, 0, 0);
        nextTurn();
    }

    while (terminated_bot_count < N) {
        bool ok = false;
        for (int i = 0; i < N; i++) {
            plan_bot(i);
        }
        for (int i = 0; i < N; i++) {
            cerr << (bot_terminated[i] ? '@' : exec_plan[i].size() ? '.' : 'o');
            ok |= exec_bot(i);
        }
        if (!ok && terminated_bot_count < N) {
            cerr << "> <!!" << endl;
            for (int x = 0; x < R; x++) for (int y = 0; y < R; y++) for (int z = 0; z < R; z++) if (field[x][y][z] && !scheduled[x][y][z]) {
                cerr << x << " " << y << " " << z << " score=" << boxel_score[x][y][z] << endl;
            }
            exit(1);
        }
        cerr << endl;
        nextTurn();
        // cerr << "reach: " << reachables.size() << endl;
        // cerr << "==========" << endl;
    }
    // cerr << endl;

    for (int i = N - 2; i >= 0; i--) {
        for (int j = 0; j < i; j++) {
            cout << "wait" << endl;
        }
        cout << "fusionp 1 0 0" << endl;
        cout << "fusions -1 0 0" << endl;
        nextTurn();
    }
    cout << "halt" << endl;
}