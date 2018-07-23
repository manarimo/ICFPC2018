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
    Point operator*(const Point& a) const {
        return Point(x * a.x, y * a.y, z * a.z);
    }
    Point operator*(const int n) const {
        return Point(x * n, y * n, z * n);
    }
    Point operator-() const {
        return Point(-x, -y, -z);
    }
    friend ostream& operator<<(ostream& os, const Point& p) {
        return os << p.x << "," << p.y << "," << p.z;
    }
    template<class T>
    T access(const vector<vector<vector<T> > >& arr) {
        return arr[x][y][z];
    }
};

struct TPoint {
    Point p;
    int t;
    TPoint() {}
    TPoint(Point p, int t) : p(p), t(t) {}
    bool operator<(const TPoint& a) const {
        if (t != a.t) return t > a.t;
        return p < a.p;
    }
};

struct Command {
    int type; // 1: smove, 2: lmove, 3: fill, 4: wait
    int idx, len, idx2, len2;
    Command() {}
    Command(int type) : type(type) {}
    Command(int type, int idx) : type(type), idx(idx) {}
    Command(int type, int idx, int len) : type(type), idx(idx), len(len) {}
    Command(int type, int idx, int len, int idx2, int len2) : type(type), idx(idx), len(len), idx2(idx2), len2(len2) {}
    Point nextPos(const Point& p) const;
    vector<Point> reservePos(const Point& p) const;
    vector<Point> blockPos(const Point& p) const;
    string to_str() const;
};

struct Reservation {
    vector<set<Point> > reserved;
    vector<pair<Point, int> > eternal;
    Reservation(const vector<Point>& initPos) {
        reserved.push_back(set<Point>());
        for (int i = 0; i < initPos.size(); i++) {
            eternal.push_back(make_pair(initPos[i], 0));
            reserved[0].insert(initPos[i]);
        }
    }

    void changeEternal(int id, Point newPos, int t) {
        eternal[id] = make_pair(newPos, t);
    }

    bool isReserved(const Point& pos, int time) {
        for (int i = 0; i < eternal.size(); i++) {
            if (pos == eternal[i].first && time >= eternal[i].second) {
                return true;
            }
        }
        if (time >= reserved.size()) return false;
        return reserved[time].count(pos);
    }

    bool isReservedCommand(const Command& command, const Point& pos, int time) {
        vector<Point> reservePos = command.reservePos(pos);
        for (int i = 0; i < reservePos.size(); i++) {
            if (isReserved(reservePos[i], time)) {
                return true;
            }
        }
        return false;
    }

    void reserve(const Point& pos, int time) {
        // cerr << "a " <<  pos << ":" << time << endl;
        while (time >= reserved.size()) {
            reserved.push_back(set<Point>());
        }
        if (isReserved(pos, time)) {
            cerr << "Reservation failed" << endl;
            exit(1);
        }
        reserved[time].insert(pos);
    }

    void reserveCommand(const Command& command, const Point& pos, int time) {
        vector<Point> reservePos = command.reservePos(pos);
        for (int i = 0; i < reservePos.size(); i++) {
            reserve(reservePos[i], time);
        }
        reserve(command.nextPos(pos), time + 1);
    }

    void reserveCommands(const vector<Command>& commands, Point pos) {
//        for (int i = 0; i < commands.size(); i++) {
//            cerr << commands[i].to_str() << endl;
//        }
        for (int i = 0; i < commands.size(); i++) {
            reserveCommand(commands[i], pos, i);
            pos = commands[i].nextPos(pos);
        }
    }
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

int R, N, turn = 0;
bool field[256][256][256] = {};
int global_depth[256][256][256] = {};

Point bots_arrange[] = {
    Point(0, 0, 0),
    Point(1, 0, 0),
    Point(1, 1, 0),
    Point(1, 1, 1),
    Point(1, 0, 1),
    Point(0, 1, 0),
    Point(0, 1, 1),
    Point(0, 0, 1),
};

Point Command::nextPos(const Point& p) const {
    switch (type) {
        case 1:
            return p + adjs[idx] * len;
        case 2:
            return p + adjs[idx] * len + adjs[idx2] * len2;
        case 3:
            return p;
        case 4:
            return p;
    }
    cerr << "ERROR 1" << endl;
    return p;
}

vector<Point> Command::reservePos(const Point& p) const {
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
        case 4:
            return ret;
    }
    cerr << "ERROR 2" << endl;
    return vector<Point>();
}

vector<Point> Command::blockPos(const Point& p) const {
    switch (type) {
        case 1:
        case 2:
        case 4:
            return vector<Point>();
        case 3:
            return vector<Point>(1, p + nds[idx]);
    }
    cerr << "ERROR 4" << endl;
    return vector<Point>();
}

string Command::to_str() const {
    stringstream ss;
    switch (type) {
        case 1: ss << "smove"; break;
        case 2: ss << "lmove"; break;
        case 3: ss << "fill"; break;
        case 4: ss << "wait"; break;
    }
    if (type == 1 || type == 2) {
        ss << " " << adj_dx[idx] * len << " " << adj_dy[idx] * len << " " << adj_dz[idx] * len;
        if (type == 2) {
            ss << " " << adj_dx[idx2] * len2 << " " << adj_dy[idx2] * len2 << " " << adj_dz[idx2] * len2;
        }
    } else if (type == 3) {
        ss << " " << nd_dx[idx] << " " << nd_dy[idx] << " " << nd_dz[idx];
    }
    return ss.str();
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

string make_command_3p(string name, const Point& p) {
    stringstream ss;
    ss << name << " " << p.x << " " << p.y << " " << p.z;
    return ss.str();
}

string make_command_6p(string name, const Point& p1, const Point& p2) {
    stringstream ss;
    ss << name << " " << p1.x << " " << p1.y << " " << p1.z << " " << p2.x << " " << p2.y << " " << p2.z;
    return ss.str();
}

void make_global_depth() {
    queue<Point> q;
    q.push(Point(-1, -1, -1));
    for (int x = 0; x < R; x++) for (int z = 0; z < R; z++) if (field[x][0][z]) q.push(Point(x, 0, z));
    for (int x = 0; x < R; x++) for (int y = 0; y < R; y++) for (int z = 0; z < R; z++) global_depth[x][y][z] = y == 0 ? 0 : -1;
    int d = 0;
    while (q.size() > 1) {
        Point p = q.front(); q.pop();
        if (p.x == -1) {
            d++;
            q.push(p);
            continue;
        }
        for (int i = 0; i < NUM_ADJ; i++) {
            Point next = p + adjs[i];
            if (!next.inside(R, R, R)) continue;
            if (!field[next.x][next.y][next.z]) continue;
            if (global_depth[next.x][next.y][next.z] != -1) continue;
            global_depth[next.x][next.y][next.z] = d;
            q.push(next);
        }
    }
}

void nextTurn() {
    cerr << "turn end: " << turn << endl;
    turn++;
}

vector<vector<vector<char> > > alive_zone(int W, vector<vector<vector<char> > >& field, set<Point>& additional) {
    vector<vector<vector<char> > > alive(W, vector<vector<char> >(R, vector<char>(R)));
    queue<Point> q;
    q.push(Point(0, 0, 0));
    alive[0][0][0] = 1;

    while (q.size()) {
        Point p = q.front(); q.pop();
        // cerr << p.x << "," << p.y << "," << p.z << endl;

        for (int i = 0; i < 6; i++) {
            int dx = adj_dx[i], nx = p.x + dx;
            int dy = adj_dy[i], ny = p.y + dy;
            int dz = adj_dz[i], nz = p.z + dz;
            if (nx < 0 || nx >= W || ny < 0 || ny >= R || nz < 0 || nz >= R) {
                continue;
            }
            Point np(nx, ny, nz);
            if (field[nx][ny][nz] || additional.count(np)) {
                continue;
            }
            if (!alive[nx][ny][nz]) {
                alive[nx][ny][nz] = 1;
                q.push(np);
            }
        }
    }
    return alive;
}


vector<string> find_path(int W, vector<vector<vector<char> > >& currentModel, Point& s, set<Point> t, Reservation& reservation) {
    map<TPoint, TPoint> prev;
    map<TPoint, Command> command;
    queue<TPoint> q;
    q.push(TPoint(s, 0));

    bool found = false;
    while (q.size()) {
        TPoint p = q.front(); q.pop();
        if (p.t > 10) break;
        if (t.count(p.p)) {
            vector<Command> commands;
            TPoint tp(p);
            while (tp.t > 0) {
                commands.push_back(command[tp]);
                tp = prev[tp];
            }
            reverse(commands.begin(), commands.end());
            reservation.reserveCommands(commands, s);
            s.x = p.p.x; s.y = p.p.y; s.z = p.p.z;
            vector<string> ret;
            for (int i = 0; i < commands.size(); i++) {
                ret.push_back(commands[i].to_str());
            }
            return ret;
        }
        if (found) continue;
        // smove
        for (int i = 0; i < NUM_ADJ; i++) {
            for (int j = 1; j <= 15; j++) {
                Point np = p.p + adjs[i] * j;
                if (!np.inside(W, R, R)) break;
                if (np.access(currentModel)) break;
                if (reservation.isReserved(np, p.t)) break;
                if (reservation.isReserved(np, p.t + 1)) continue;
                TPoint ntp = TPoint(np, p.t + 1);
                if (!prev.count(ntp)) {
                    prev[ntp] = p;
                    command[ntp] = Command(1, i, j);
                    q.push(ntp);
                    if (t.count(np)) {
                        found = true;
                    }
                }
            }
        }
        // lmove
        for (int i1 = 0; i1 < NUM_ADJ; i1++) {
            for (int k1 = 1; k1 <= 5; k1++) {
                Point np1 = p.p + adjs[i1] * k1;
                if (!np1.inside(W, R, R)) break;
                if (np1.access(currentModel)) break;
                if (reservation.isReserved(np1, p.t)) break;
                for (int i2 = 0; i2 < 6; i2++) {
                    if (i1 == i2 || adjs[i1] + adjs[i2] == Point(0, 0, 0)) continue;
                    for (int k2 = 1; k2 <= 5; k2++) {
                        Point np2 = adjs[i2] * k2 + np1;
                        if (!np2.inside(W, R, R)) break;
                        if (np2.access(currentModel)) break;
                        if (reservation.isReserved(np2, p.t)) break;
                        if (reservation.isReserved(np2, p.t + 1)) continue;
                        TPoint ntp = TPoint(np2, p.t + 1);
                        if (!prev.count(ntp)) {
                            prev[ntp] = p;
                            command[ntp] = Command(2, i1, k1, i2, k2);
                            q.push(ntp);
                            if (t.count(np2)) {
                                found = true;
                            }
                        }
                    }
                }
            }
        }
    }

    // unreachable
    cerr << "DEAD END" << endl;
    // return vector<string>();
    exit(1);
}

vector<Point> corners(const Point& a, const Point& b) {
    vector<Point> ret;
    ret.push_back(a);
    ret.push_back(Point(b.x, a.y, a.z));
    ret.push_back(Point(b.x, b.y, a.z));
    ret.push_back(b);
    ret.push_back(Point(b.x, a.y, b.z));
    ret.push_back(Point(a.x, b.y, a.z));
    ret.push_back(Point(a.x, b.y, b.z));
    ret.push_back(Point(a.x, a.y, b.z));
    return ret;
}

struct NextTask {
    Point a, b;
    bool gfill;
    vector<char> active;
    vector<Point> corners;
    vector<set<Point> > goals;
    NextTask() : a(Point(-1, 0, 0)) {}
    NextTask(const Point& a, const Point& b, bool gfill, vector<char>& active, vector<Point>& corners, vector<set<Point> >& goals) : a(a), b(b), gfill(gfill), active(active), corners(corners), goals(goals) {}
};

NextTask next_task(const int W, vector<vector<vector<char> > >& model, vector<vector<vector<char> > >& currentModel, vector<Point>& tasks, int& fromIdx) {
    while (fromIdx < tasks.size() && tasks[fromIdx].access(currentModel)) fromIdx++;
    if (fromIdx == tasks.size()) {
        return NextTask();
    }
    set<Point> additional;
    int renzoku = 0;
    while (true) {
        additional.insert(tasks[fromIdx]);
        vector<char> active(8); active[0] = true;
        Point upper = tasks[fromIdx];
        Point lower = tasks[fromIdx];
        int limit = 0;
        for (; limit < 30; ) {
            Point cand = upper + Point(0, 0, 1);
            if (!cand.inside(W, R, R) || !cand.access(model)) {
                break;
            }
            upper = cand;
            additional.insert(cand);
            active[7] = true;
            limit++;
        }
        for (; limit < 30; ) {
            Point cand = lower + Point(0, 0, -1);
            if (!cand.inside(W, R, R) || !cand.access(model)) {
                break;
            }
            lower = cand;
            additional.insert(cand);
            active[7] = true;
            limit++;
        }
        vector<vector<vector<char> > > zone = alive_zone(W, currentModel, additional);
        vector<set<Point> > goals(8);
        bool dead = false;
        for (int i = fromIdx + 1; i < tasks.size(); i++) {
            if (tasks[i].inside(lower.x, upper.x + 1, lower.y, upper.y + 1, lower.z, upper.z + 1)) {
                continue;
            }
            if (!tasks[i].access(currentModel) && !tasks[i].access(zone)) {
                cerr << tasks[i] << endl;
                dead = true;
                break;
            }
        }
        vector<Point> ps = corners(lower, upper);
        for (int i = 0; i < 8; i++) {
            goals.push_back(set<Point>());
            for (int j = 0; j < NUM_ND; j++) {
                Point p = ps[i] + nds[j];
                if (p.inside(W, R, R) && p.access(zone)) {
                    goals[i].insert(p);
                }
            }
            if (!goals[i].size()) {
                cerr << 2 << endl;
                dead = true;
                break;
            }
        }
        if (dead) {
            tasks.push_back(tasks[fromIdx]);
            fromIdx++;
            renzoku++;
            if (renzoku >= tasks.size() - fromIdx) {
                cerr << "> <" << endl;
                return NextTask();
            }
            continue;
        }
        return NextTask(lower, upper, active[7], active, ps, goals);
    }
}

vector<vector<vector<string> > > computePath(const int W, vector<vector<vector<char> > >& model, vector<Point>& tasks, vector<vector<Point> >& newTasks) {

    vector<vector<vector<char> > > currentModel(W, vector<vector<char> >(R, vector<char>(R)));
    vector<Point> pos(8);
    for (int i = 0; i < 8; i++) pos[i] = bots_arrange[i] * Point(W - 1, R - 1, R - 1);

    vector<vector<vector<string> > >  ret;
    int taskId = 0;
    while (taskId < tasks.size()) {
        Reservation reservation(pos);
        NextTask task = next_task(W, model, currentModel, tasks, taskId);
        if (task.a.x == -1) break;

        vector<vector<string> > team_commands(8);
        int max_len = 0;
        for (int i = 0; i < 8; i++) {
            if (task.active[i]) {
    //            cerr << "[" << i << "]" << endl;
                team_commands[i] = find_path(W, currentModel, pos[i], task.goals[i], reservation);

    //            for (int j = 0; j < team_commands[i].size(); j++) cerr << team_commands[i][j] << endl;
    //            cerr << pos[i] << endl;

                reservation.changeEternal(i, pos[i], team_commands[i].size());
                max_len = max(max_len, (int) team_commands[i].size());
            }
        }
        for (int i = 0; i < 8; i++) {
            while (team_commands[i].size() < max_len) {
                team_commands[i].push_back("wait");
            }
        }
        for (int i = 0; i < 8; i++) {
            if (task.active[i]) {
                Point nd = task.corners[i] - pos[i];
                if (!task.gfill) {
                    team_commands[i].push_back(make_command_3p("fill", nd));
                } else {
                    Point fd = (task.b - task.a) * Point(bots_arrange[i].x == 0 ? 1 : -1, bots_arrange[i].y == 0 ? 1 : -1, bots_arrange[i].z == 0 ? 1 : -1);
                    team_commands[i].push_back(make_command_6p("gfill", nd, fd));
                }
            } else {
                team_commands[i].push_back("wait");
            }
        }
        vector<Point> boxels;
        for (int x = task.a.x; x <= task.b.x; x++) for (int y = task.a.y; y <= task.b.y; y++) for (int z = task.a.z; z <= task.b.z; z++) {
            boxels.push_back(Point(x, y, z));
            currentModel[x][y][z] = 1;
        }
        newTasks.push_back(boxels);
        ret.push_back(team_commands);
    }
    vector<vector<string> > final(8);
    Reservation reservation(pos);
    int max_len = 0;
    for (int i = 0; i < 8; i++) {
        set<Point> goals; goals.insert(bots_arrange[i] * Point(W - 1, R - 1, R - 1));
        final[i] = find_path(W, currentModel, pos[i], goals, reservation);
        reservation.changeEternal(i, pos[i], final[i].size());
        max_len = max(max_len, (int) final[i].size());
    }
    for (int i = 0; i < 8; i++) {
        while (final[i].size() < max_len) {
            final[i].push_back("wait");
        }
    }
    ret.push_back(final);
    return ret;
}

int main() {
    read_input();
    cerr << "読み込みおわり" << endl;

    N = min(5, R / 2);
    // とりあえずxで雑に分割
    vector<int> x_range(N + 1);
    for (int i = 0; i < N; i++) {
        x_range[i + 1] = R * (i + 1) / N;
    }

    make_global_depth();

    cerr << "距離計算おわり" << endl;

    // Robot ID 計算
    vector<pair<int, int> > team_and_id(N * 8);
    for (int i = 0; i < N; i++) {
        team_and_id[i] = make_pair(i, 0);
    }
    for (int teamId = N - 1; teamId >= 0; teamId--) {
        int offset = N + (N - 1 - teamId) * 7;
        for (int i = 0; i < 7; i++) {
            team_and_id[offset + i] = make_pair(teamId, i + 1);
        }
    }

    // 準備
    for (int i = 0; i < N - 1; ) {
        for (int k = 0; k < i; k++) cout << "wait" << endl;
        cout << "fission 1 0 0 " << (N - i - 1) * 8 - 1 << endl;
        i++;

        int d = x_range[i] - x_range[i - 1] - 1;
        for (int j = 0; j < d / 15; j++) {
            for (int k = 0; k < i; k++) cout << "wait" << endl;
            cout << "smove 15 0 0" << endl;
        }

        if (d % 15) {
            for (int k = 0; k < i; k++) cout << "wait" << endl;
            cout << "smove " << d % 15 << " 0 0" << endl;
        }
    }

    // X 軸
    for (int i = 0; i < N; i++) {
        cout << "fission 1 0 0 3" << endl;
    }

    // Y 軸
    for (int i = 0; i < N * 2; i++) {
        cout << "fission 0 1 0 1" << endl;
    }

    // Z 軸
    for (int i = 0; i < N * 4; i++) {
        cout << "fission 0 0 1 0" << endl;
    }

    // 散開
    {
        vector<vector<string> > sankai_command(N * 8);
        for (int i = 0; i < N * 8; i++) {
            int teamId = team_and_id[i].first;
            int localId = team_and_id[i].second;

            Point move = bots_arrange[localId] * Point(x_range[teamId + 1] - x_range[teamId] - 2, R - 2, R - 2);

            vector<string> commands;
            int d;
            d = move.x;
            for (int j = 0; j < d / 15; j++) {
                commands.push_back(make_command_3p("smove", Point(15, 0, 0)));
            }
            if (d % 15) {
                commands.push_back(make_command_3p("smove", Point(d % 15, 0, 0)));
            }

            d = move.y;
            for (int j = 0; j < d / 15; j++) {
                commands.push_back(make_command_3p("smove", Point(0, 15, 0)));
            }
            if (d % 15) {
                commands.push_back(make_command_3p("smove", Point(0, d % 15, 0)));
            }

            d = move.z;
            for (int j = 0; j < d / 15; j++) {
                commands.push_back(make_command_3p("smove", Point(0, 0, 15)));
            }
            if (d % 15) {
                commands.push_back(make_command_3p("smove", Point(0, 0, d % 15)));
            }

            sankai_command[i] = commands;
        }

        int maxSankaiSize = 0;
        for (int i = 0; i < N * 8; i++) maxSankaiSize = max(maxSankaiSize, (int) sankai_command[i].size());
        for (int i = 0; i < N * 8; i++) while (sankai_command[i].size() < maxSankaiSize) sankai_command[i].push_back("wait");

        for (int i = 0; i < maxSankaiSize; i++) {
            for (int j = 0; j < N * 8; j++) {
                cout << sankai_command[j][i] << endl;
            }
        }
    }

    vector<vector<Point>> tasks(N);
    for (int i = 0; i < N; i++) {
        for (int x = x_range[i]; x < x_range[i+1]; x++) {
            for (int y = 0; y < R; y++) {
                for (int z = 0; z < R; z++) {
                    if (field[x][y][z]) {
                        tasks[i].push_back(Point(x - x_range[i], y, z));
                    }
                }
            }
        }
        sort(tasks[i].begin(), tasks[i].end(), [x_range, i] (Point& a, Point& b) {
            int depth_a = global_depth[a.x + x_range[i]][a.y][a.z];
            int depth_b = global_depth[b.x + x_range[i]][b.y][b.z];
            if (depth_a != depth_b) return depth_a < depth_b;
            return a < b;
        });
    }

    cerr << "前計算中..." << endl;
    vector<vector<vector<vector<string> > > > paths;
    vector<vector<vector<Point> > > resultTasks(N);
    for (int i = 0; i < N; i++) {
        cerr << "bot-" << i << "/" << N << endl;
        int W = x_range[i + 1] - x_range[i];
        vector<vector<vector<char> > > model(W, vector<vector<char> >(R, vector<char>(R)));
        for (int x = x_range[i]; x < x_range[i + 1]; x++) for (int y = 0; y < R; y++) for (int z = 0; z < R; z++) if (field[x][y][z]) model[x - x_range[i]][y][z] = 1;
        paths.push_back(computePath(x_range[i + 1] - x_range[i], model, tasks[i], resultTasks[i]));
    }

    cerr << "前計算した" << endl;

    vector<int> progress1(N);
    vector<int> progress2(N);
    vector<vector<vector<char> > > reachables(R, vector<vector<char> >(R, vector<char>(R)));
    for (int x = 0; x < R; x++) for (int z = 0; z < R; z++) reachables[x][0][z] = true;
    bool running = true;

    int Z = 0;
    while (running) {
        bool stuck = true;
        vector<Point> completed;
        vector<int> p1s(N), p2s(N);
        // cerr << "turn:" << (Z++) << endl;
        for (int i = 0; i < N; i++) {
            int teamId = i, localId = 0;
            int p1 = progress1[i], p2 = progress2[i];
            p1s[i] = p1; p2s[i] = p2;
            // cerr << i << ": " << p1 << "/" << paths[i].size() << "," << p2 << "/" << (p1 == paths[i].size() ? 0 : paths[i][p1][localId].size()) << endl;
            if (p1 == paths[teamId].size()) {
                // 終わってる
                p1s[i] = -1;
                continue;
            } else if (p1 != paths[teamId].size() - 1 && p2 == paths[teamId][p1][localId].size() - 1) {
                // fill
                Point offset(x_range[teamId], 0, 0);
                vector<Point>& task = resultTasks[teamId][p1];
                bool reachable = false;
                for (int k = 0; k < task.size(); k++) {
                    if ((task[k] + offset).access(reachables)) reachable = true;
                }
                if (reachable) {
                    progress2[teamId]++;
                    stuck = false;
                    for (int k = 0; k < task.size(); k++) {
                        for (int m = 0; m < NUM_ADJ; m++) {
                            Point p = task[k] + offset + adjs[m];
                            if (p.inside(R, R, R)) {
                                reachables[p.x][p.y][p.z] = 1;
                            }
                        }
                    }
                } else {
                    p1s[i] = -1;
                }
            } else {
                // move
                progress2[teamId]++;
                stuck = false;
            }
            if (progress2[teamId] == paths[teamId][p1][localId].size()) {
                progress1[teamId]++;
                progress2[teamId] = 0;
            }
        }
        for (int i = 0; i < N * 8; i++) {
            int teamId = team_and_id[i].first;
            int localId = team_and_id[i].second;
            if (p1s[teamId] == -1) {
                cout << "wait" << endl;
            } else {
                int p1 = p1s[teamId], p2 = p2s[teamId];
                cout << paths[teamId][p1][localId][p2] << endl;
            }
        }
        if (stuck) {
            cerr << "> <!!" << endl;
            return 2;
        }
        running = false;
        for (int i = 0; i < N; i++) {
            if (progress1[i] < paths[i].size()) {
                running = true;
                break;
            }
        }
    }

    // 後片付け
    // Y軸
    for (int j = 0; j < (R - 2) / 15; j++) {
        for (int i = 0; i < N; i++) {
            cout << "smove 0 15 0" << endl;
        }
        for (int i = 0; i < N; i++) {
            cout << "smove 0 15 0" << endl;
            cout << "wait" << endl;
            cout << "wait" << endl;
            cout << "smove 0 15 0" << endl;
            cout << "wait" << endl;
            cout << "wait" << endl;
            cout << "smove 0 15 0" << endl;
        }
    }
    if ((R - 2) % 15) {
        for (int i = 0; i < N; i++) {
            cout << "smove 0 " << (R - 2) % 15 << " 0" << endl;
        }
        for (int i = 0; i < N; i++) {
            cout << "smove 0 " << (R - 2) % 15 << " 0" << endl;
            cout << "wait" << endl;
            cout << "wait" << endl;
            cout << "smove 0 " << (R - 2) % 15 << " 0" << endl;
            cout << "wait" << endl;
            cout << "wait" << endl;
            cout << "smove 0 " << (R - 2) % 15 << " 0" << endl;
        }
    }
    for (int i = 0; i < N; i++) {
        cout << "fusions 0 1 0" << endl;
    }
    for (int i = 0; i < N; i++) {
        cout << "fusions 0 1 0" << endl;
        cout << "fusionp 0 -1 0" << endl;
        cout << "fusionp 0 -1 0" << endl;
        cout << "fusions 0 1 0" << endl;
        cout << "fusionp 0 -1 0" << endl;
        cout << "fusionp 0 -1 0" << endl;
        cout << "fusions 0 1 0" << endl;
    }

    // Z軸
    for (int j = 0; j < (R - 2) / 15; j++) {
        for (int i = 0; i < N; i++) {
            cout << "wait" << endl;
            cout << "smove 0 0 -15" << endl;
            cout << "wait" << endl;
            cout << "smove 0 0 -15" << endl;
        }
    }
    if ((R - 2) % 15) {
        for (int i = 0; i < N; i++) {
            cout << "wait" << endl;
            cout << "smove 0 0 -" << (R - 2) % 15 << endl;
            cout << "wait" << endl;
            cout << "smove 0 0 -" << (R - 2) % 15 << endl;
        }
    }
    for (int i = 0; i < N; i++) {
        cout << "fusionp 0 0 1" << endl;
        cout << "fusions 0 0 -1" << endl;
        cout << "fusionp 0 0 1" << endl;
        cout << "fusions 0 0 -1" << endl;
    }

    // X軸
    vector<vector<string> > syugo_command(N);
    for (int i = 0; i < N; i++) {
        int w = x_range[i + 1] - x_range[i] - 2;
        for (int j = 0; j < w / 15; j++) {
            syugo_command[i].push_back("smove -15 0 0");
        }
        if (w % 15) {
            syugo_command[i].push_back(make_command_3p("smove", Point(-(w % 15), 0, 0)));
        }
    }
    int max_syugo_command = 0; for (int i = 0; i < N; i++) max_syugo_command = max(max_syugo_command, (int) syugo_command[i].size());

    for (int i = 0; i < max_syugo_command; i++) {
        for (int j = N - 1; j >= 0; j--) {
            if (i < syugo_command[j].size()) {
                cout << syugo_command[j][i] << endl;
            } else {
                cout << "wait" << endl;
            }
            cout << "wait" << endl;
        }
    }

    for (int i = 0; i < N; i++) {
        cout << "fusions -1 0 0" << endl;
        cout << "fusionp 1 0 0" << endl;
    }

    for (int i = N - 1; i > 0; ) {
        int d = x_range[i] - x_range[i - 1] - 1;
        for (int j = 0; j < d / 15; j++) {
            cout << "smove -15 0 0" << endl;
            for (int k = 0; k < i; k++) cout << "wait" << endl;
        }

        if (d % 15) {
            cout << "smove -" << d % 15 << " 0 0" << endl;
            for (int k = 0; k < i; k++) cout << "wait" << endl;
        }
        i--;

        cout << "fusions -1 0 0 " << endl;
        cout << "fusionp 1 0 0 " << endl;
        for (int k = 0; k < i; k++) cout << "wait" << endl;
    }

    for (int i = 0; i < (R - 1) / 15; i++) {
        cout << "smove 0 -15 0" << endl;
    }
    if ((R - 1) % 15) {
        cout << "smove 0 -" << (R - 1) % 15 << " 0" << endl;
    }

    cout << "halt" << endl;
}