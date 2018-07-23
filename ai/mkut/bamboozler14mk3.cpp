#include <iostream>
#include <queue>
#include <sstream>
#include <map>
#include <set>
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
    vector<Point> freePos(const Point& p);
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
int reservation[256][256][256] = {};
bool current_model[256][256][256] = {};
int scheduled[256][256][256] = {};
set<Point> remainings;
set<Point> reachables;
Point bot_pos[40] = {};
queue<Command> exec_plan[40] = {};
bool bot_terminated[40] = {};
int terminated_bot_count = 0;
int scheduledCount = 0;
int boxelCount = 0;

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
            for (int i = 1; i <= len; i++) {
                ret.push_back(p + adjs[idx] * i);
            }
            return ret;
        case 2:
            for (int i = 1; i <= len; i++) {
                ret.push_back(p + adjs[idx] * i);
            }
            for (int i = 1; i <= len2; i++) {
                ret.push_back(p + adjs[idx] * len + adjs[idx2] * i);
            }
            return ret;
        case 3:
            ret.push_back(p + nds[idx]);
            return ret;
    }
    cerr << "ERROR 2" << endl;
    return vector<Point>();
}

vector<Point> Command::freePos(const Point& p) {
    vector<Point> ret;
    switch (type) {
        case 1:
            for (int i = 0; i < len; i++) {
                ret.push_back(p + adjs[idx] * i);
            }
            return ret;
        case 2:
            for (int i = 0; i < len; i++) {
                ret.push_back(p + adjs[idx] * i);
            }
            for (int i = 0; i < len2; i++) {
                ret.push_back(p + adjs[idx] * len + adjs[idx2] * i);
            }
            return ret;
        case 3:
            ret.push_back(p + nds[idx]);
            return ret;
    }
    cerr << "ERROR 3" << endl;
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

void reserve(const Point& p) {
    cerr << "reserve: " << p.x << " " << p.y << " " << p.z << endl;
    reservation[p.x][p.y][p.z]++;
}

void unreserve(const Point& p) {
    cerr << "unreserve: " << p.x << " " << p.y << " " << p.z << endl;
    reservation[p.x][p.y][p.z]--;
}

bool reserved(const Point& p) {
    return reservation[p.x][p.y][p.z] || current_model[p.x][p.y][p.z];
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
            if (current_model[np.x][np.y][np.z] || additional == np) continue;
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
            Point tp(p);
            while (tp != pos) {
                revExecPlan.push_back(command[tp]);
                vector<Point> reservePos = command[tp].reservePos(prev[tp]);
                tp = prev[tp];
                for (int j = 0; j < reservePos.size(); j++) {
                    reserve(reservePos[j]);
                }
            }
            for (int i = revExecPlan.size() - 1; i >= 0; i--) {
                exec_plan[id].push(revExecPlan[i]);
            }
            return 0;
        }
        if (found) continue;
        // smove
        for (int i = 0; i < NUM_ADJ; i++) {
            for (int j = 1; j <= 15; j++) {
                Point np = p + adjs[i] * j;
                if (!np.inside(R, R, R)) break;
                if (reserved(np)) break;
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
                if (reserved(np1)) break;
                for (int i2 = 0; i2 < 6; i2++) {
                    for (int k2 = 1; k2 <= 5; k2++) {
                        Point np = np1 + adjs[i2] * k2;
                        if (!np.inside(R, R, R)) break;
                        if (reserved(np)) break;
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

int find_target(int id) {
    Point& pos = bot_pos[id];

    int max_reachables_surrounds = 0;
    vector<Point> activeReachables;
    for (auto it = reachables.begin(); it != reachables.end(); ++it) {
        int cnt = 0;
        for (int i = 0; i < NUM_ADJ; i++) {
            Point np = adjs[i] + *it;
            if (!np.inside(R, R, R) || current_model[np.x][np.y][np.z]) {
                cnt++;
            }
        }
        if (cnt > max_reachables_surrounds) {
            max_reachables_surrounds = cnt;
            activeReachables = vector<Point>();
        }
        if (cnt >= max_reachables_surrounds) {
            activeReachables.push_back(*it);
        }
    }

    map<Point, Command> goal;
    for (int i = 0; i < activeReachables.size(); i++) {
        for (int i = 0; i < NUM_ND; i++) {
            Point np = nds[i] + activeReachables[i];
            if (np.inside(R, R, R) && (!reserved(activeReachables[i]) || activeReachables[i] == pos) && (!reserved(np) || np == pos)) {
                goal[np] = Command(3, NUM_ND - 1 - i);
            }
        }
    }
    if (!goal.size()) {
        return 1;
    }

    map<Point, Point> prev;
    map<Point, Command> command;
    queue<Point> q;
    q.push(pos);
    prev[pos] = pos;

    bool found = false;
    while (q.size()) {
        Point p = q.front(); q.pop();
        if (goal.count(p)) {
            Point target = goal[p].reservePos(p)[0];
            vector<Command> revExecPlan;
            revExecPlan.push_back(goal[p]);
            vector<Point> reservePos = goal[p].reservePos(p);
            for (int j = 0; j < reservePos.size(); j++) {
                reserve(reservePos[j]);
                scheduled[reservePos[j].x][reservePos[j].y][reservePos[j].z] = 1;
                scheduledCount++;
                cerr << scheduledCount << "/" << boxelCount << endl;
                reachables.erase(reservePos[j]);
            }
            Point tp(p);
            while (tp != pos) {
                revExecPlan.push_back(command[tp]);
                vector<Point> reservePos = command[tp].reservePos(prev[tp]);
                tp = prev[tp];
                for (int j = 0; j < reservePos.size(); j++) {
                    reserve(reservePos[j]);
                }
            }
            for (int i = revExecPlan.size() - 1; i >= 0; i--) {
                exec_plan[id].push(revExecPlan[i]);
            }
            cerr << "[" << id << "]" << reservePos[0].x << "," << reservePos[0].y << "," << reservePos[0].z << endl;
            return 0;
        }
        if (found) continue;
        // smove
        for (int i = 0; i < NUM_ADJ; i++) {
            for (int j = 1; j <= 15; j++) {
                Point np = p + adjs[i] * j;
                if (!np.inside(R, R, R)) break;
                if (reserved(np)) break;
                if (!prev.count(np)) {
                    prev[np] = p;
                    command[np] = Command(1, i, j);
                    q.push(np);
                    if (goal.count(np)) {
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
                if (reserved(np1)) break;
                for (int i2 = 0; i2 < 6; i2++) {
                    for (int k2 = 1; k2 <= 5; k2++) {
                        Point np = np1 + adjs[i2] * k2;
                        if (!np.inside(R, R, R)) break;
                        if (reserved(np)) break;
                        if (!prev.count(np)) {
                            prev[np] = p;
                            command[np] = Command(2, i1, k1, i2, k2);
                            q.push(np);
                            if (goal.count(np)) {
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

void plan_bot(int id) {
    if (exec_plan[id].size() == 0) {
        if (scheduledCount < boxelCount) {
            int res = find_target(id);
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
        cerr << cmd.type << "/" << cmd.idx << ","  << cmd.len << endl;
        cerr << cmd.type << "/" << cmd.idx << ","  << cmd.len << endl;
        cerr << cmd.type << "/" << cmd.idx << ","  << cmd.len << endl;
        exec_plan[id].pop();
        vector<Point> freePos = cmd.freePos(bot_pos[id]);
        for (int i = 0; i < freePos.size(); i++) {
            unreserve(freePos[i]);
        }
        vector<Point> blockPos = cmd.blockPos(bot_pos[id]);
        for (int i = 0; i < blockPos.size(); i++) {
            reachables.erase(blockPos[i]);
            remainings.erase(blockPos[i]);
            current_model[blockPos[i].x][blockPos[i].y][blockPos[i].z] = true;
            for (int j = 0; j < NUM_ADJ; j++) {
                Point newReachable = blockPos[i] + adjs[j];
                if (field[newReachable.x][newReachable.y][newReachable.z] && !scheduled[newReachable.x][newReachable.y][newReachable.z]) {
                    reachables.insert(newReachable);
                }
            }
        }
        bot_pos[id] = cmd.nextPos(bot_pos[id]);
        cout << cmd.to_str() << endl;
        return true;
    } else {
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

    N = min(40, R);

    for (int i = 0; i < N; i++) reserve(Point(i, 0, 0));
    for (int i = 0; i < N - 1; i++) {
        for (int k = 0; k < i; k++) {
            plan_bot(k);
        }
        for (int k = 0; k < i; k++) {
            exec_bot(k);
        }
        cout << "fission 1 0 0 " << (N - 2 - i) << endl;
        cerr << "reach: " << reachables.size() << endl;
        cerr << "==========" << endl;
        bot_pos[i+1] = Point(i+1, 0, 0);
    }

    while (terminated_bot_count < N) {
        bool ok = false;
        for (int i = 0; i < N; i++) {
            plan_bot(i);
        }
        for (int i = 0; i < N; i++) {
            ok |= exec_bot(i);
            cerr << (bot_terminated[i] ? '@' : exec_plan[i].size() ? '.' : 'o');
        }
        if (!ok && terminated_bot_count < N) {
            cerr << "> <!!" << endl;
            exit(1);
        }
        cerr << endl;
        cerr << "reach: " << reachables.size() << endl;
        cerr << "==========" << endl;
    }
    // cerr << endl;

    for (int i = N - 2; i >= 0; i--) {
        for (int j = 0; j < i; j++) {
            cout << "wait" << endl;
        }
        cout << "fusionp 1 0 0" << endl;
        cout << "fusions -1 0 0" << endl;
    }
    cout << "halt" << endl;
}