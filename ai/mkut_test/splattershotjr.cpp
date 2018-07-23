#include <iostream>
#include <queue>
#include <algorithm>
#include <map>
#include <sstream>
#include <set>
using namespace std;

// 6
int adj_dx[] = {1,-1, 0, 0, 0, 0};
int adj_dy[] = {0, 0, 1,-1, 0, 0};
int adj_dz[] = {0, 0, 0, 0, 1,-1};

// 18
int nd_dx[] = {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,-1,-1,-1,-1,-1};
int nd_dy[] = {1, 0, 0, 0,-1, 1, 1, 1, 0, 0,-1,-1,-1, 1, 0, 0, 0,-1};
int nd_dz[] = {0, 1, 0,-1, 0, 1, 0,-1, 1,-1, 1, 0,-1, 0, 1, 0,-1, 0};

bool field[256][256][256]; // x, y, z
int dist[256][256][256];
int sum[256][256][256];
int dp_x[256][21][2];
int dp_z[256][21][2];
int voxels_x[256][256];
int voxels_z[256][256];

bool connected[256];
int total_by_h[255];

class UnionFind {
    public:
    UnionFind(int R) : R(R) {
        int n = R * R * R + 1;
        parent = (int *)malloc(sizeof(int) * n);
        for (int i = 0; i < n; i++) parent[i] = -1;
    }

    ~UnionFind() {
        free(parent);
    }

    int find(int x) {
        if (parent[x] < 0) return x;

        return parent[x] = find(parent[x]);
    }

    int find(int x, int y, int z) {
        return find(x * R * R + y * R + z);
    }

    int ground() {
        return find(R, 0, 0);
    }

    void unite(int x, int y) {
        x = find(x);
        y = find(y);

        if (x == y) return;

        if (parent[x] < parent[y]) {
            parent[x] += parent[y];
            parent[y] = x;
        } else {
            parent[y] += parent[x];
            parent[x] = y;
        }
    }

    void unite(int x1, int y1, int z1, int x2, int y2, int z2) {
        return unite(find(x1, y1, z1), find(x2, y2, z2));
    }

    int size(int x) {
        return -parent[find(x)];
    }

    int size(int x1, int y1, int z1) {
        return size(find(x1, y1, z1));
    }

    private:
    int R;
    int *parent;
};

struct Point {
    int x, y, z;
    Point() : x(0), y(0), z(0) {}
    Point(int x, int y, int z) : x(x), y(y), z(z) {}
    bool operator<(const Point& a) const {
        if (x != a.x) return x < a.x;
        if (y != a.y) return y < a.y;
        if (z != a.z) return z < a.z;
        return false;
    }
};

struct PointD {
    int x, y, z, d, retry;
    PointD() : x(0), y(0), z(0), d(0), retry(0) {}
    PointD(int x, int y, int z, int d, int retry) : x(x), y(y), z(z), d(d), retry(retry) {}
    bool operator<(const PointD& a) const {
        if (retry != a.retry) return retry < a.retry;
        if (d != a.d) return d < a.d;
        if (x != a.x) return x < a.x;
        if (y != a.y) return y < a.y;
        if (z != a.z) return z < a.z;
        return false;
    }
    Point to_point() {
        return Point(x, y, z);
    }
};

struct AStarPoint {
    int x, y, z, c, ec;
    AStarPoint() : x(0), y(0), z(0), c(0), ec(0) {}
    AStarPoint(int x, int y, int z, int c, int ec) : x(x), y(y), z(z), c(c), ec(ec) {}
    AStarPoint(Point p, int c, int ec) : x(p.x), y(p.y), z(p.z), c(c), ec(ec) {}
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

string make_command_3(string name, int arg1, int arg2, int arg3) {
    stringstream ss;
    ss << name << " " << arg1 << " " << arg2 << " " << arg3;
    return ss.str();
}

string make_command_6(string name, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6) {
    stringstream ss;
    ss << name << " " << arg1 << " " << arg2 << " " << arg3 << " " << arg4 << " " << arg5 << " " << arg6;
    return ss.str();
}

int man_d(Point& a, Point& b) {
    return abs(abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z) - 1);
}

vector<vector<vector<char> > > alive_zone(const int R, int W, int H, vector<vector<vector<char> > >& field2, set<Point>& additional) {
    vector<vector<vector<char> > > alive(W, vector<vector<char> >(H, vector<char>(R)));
    queue<Point> q;
    q.push(Point(0, H - 1, 0));
    alive[0][H - 1][0] = 1;

    while (q.size()) {
        Point p = q.front(); q.pop();
        // cerr << p.x << "," << p.y << "," << p.z << endl;

        for (int i = 0; i < 6; i++) {
            int dx = adj_dx[i], nx = p.x + dx;
            int dy = adj_dy[i], ny = p.y + dy;
            int dz = adj_dz[i], nz = p.z + dz;
            if (nx < 0 || nx >= W || ny < 0 || ny >= H || nz < 0 || nz >= R) {
                continue;
            }
            Point np(nx, ny, nz);
            if (field2[nx][ny][nz] || additional.count(np)) {
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

vector<vector<string> > bfs(int R, int W, int H, vector<vector<vector<char> > >& field2, Point& s, vector<PointD>& tasks, int taskId) {
    bool fill = taskId < tasks.size();
    Point t = fill ? tasks[taskId].to_point() : Point(0, H - 1, 0);
    // cerr << taskId << "/" << tasks.size() << (fill ? "(fill)" : "") << ": " << t.x << " " << t.y << " " << t.z << endl;
    map<Point, Point> prev;
    map<Point, string> command;
    priority_queue<AStarPoint> q;
    AStarPoint init(s, 0, man_d(s, t));
    q.push(init);
    prev[s] = s;
    command[s] = "";

    map<Point, vector<string> > atari;
    set<Point> additional;
    int rush = 0;
    if (fill) {
        // cerr << "W=" << W << " H=" << H << " R=" << R << endl;
        for (; taskId + rush < tasks.size(); rush++) {
            additional.insert(tasks[taskId + rush].to_point());
            vector<vector<vector<char> > > alive_zone_after = alive_zone(R, W, H, field2, additional);
            bool dead = false;
            for (int i = taskId + rush + 1; i < tasks.size(); i++) {
                // cerr << tasks[i].x << "," << tasks[i].y << "," << tasks[i].z << endl;
                if (!alive_zone_after[tasks[i].x][tasks[i].y][tasks[i].z]) {
                    dead = true;
                    break;
                }
            }
            if (dead) {
                break;
            }
            map<Point, vector<string> > newAtari;
            for (int i = 0; i < 18; i++) {
                int dx = nd_dx[i], nx = tasks[taskId + rush].x + dx;
                int dy = nd_dy[i], ny = tasks[taskId + rush].y + dy;
                int dz = nd_dz[i], nz = tasks[taskId + rush].z + dz;
                Point np(nx, ny, nz);
                if (nx >= 0 && nx < W && ny >= 0 && ny < H && nz >= 0 && nz < R && alive_zone_after[nx][ny][nz]) {
                    if (rush == 0 || atari.count(np)) {
                        newAtari[np] = atari[np];
                        newAtari[np].push_back(make_command_3("fill", -dx, -dy, -dz));
                    }
                }
            }
            if (newAtari.size()) {
                atari = newAtari;
            } else {
                break;
            }
        }
        if (rush == 0) {
            return vector<vector<string> >(1, vector<string>(1, "> <"));
        }
    } else {
        atari[t] = vector<string>();
    }
    bool found = false;
    while (q.size()) {
        AStarPoint p = q.top(); q.pop();
        // cerr << p.x << "," << p.y << "," << p.z << endl;
        Point p_p = p.to_point();
        if (atari.count(p_p)) {
            if (fill) {
                vector<vector<string> > ret(1);
                ret[0].push_back(atari[p_p][0]);
                Point tp(p_p);
                while (tp.x != s.x || tp.y != s.y || tp.z != s.z) {
                    ret[0].push_back(command[tp]);
                    tp = prev[tp];
                }
                reverse(ret[0].begin(), ret[0].end());
                for (int i = 1; i < rush; i++) {
                    ret.push_back(vector<string>(1, atari[p_p][i]));
                }
                s.x = p.x; s.y = p.y; s.z = p.z;
                return ret;
            } else {
                vector<vector<string> > ret(1);
                Point tp(p_p);
                while (tp.x != s.x || tp.y != s.y || tp.z != s.z) {
                    ret[0].push_back(command[tp]);
                    tp = prev[tp];
                }
                reverse(ret[0].begin(), ret[0].end());
                return ret;
            }
        }
        if (found) continue;
        // smove
        for (int i = 0; i < 6; i++) {
            for (int j = 1; j <= 15; j++) {
                int dx = adj_dx[i] * j, nx = p.x + dx;
                int dy = adj_dy[i] * j, ny = p.y + dy;
                int dz = adj_dz[i] * j, nz = p.z + dz;
                if (nx < 0 || nx >= W || ny < 0 || ny >= H || nz < 0 || nz >= R) {
                    break;
                }
                if (field2[nx][ny][nz]) {
                    break;
                }
                Point np(nx, ny, nz);
                if (!prev.count(np)) {
                    prev[np] = p_p;
                    command[np] = make_command_3("smove", dx, dy, dz);
                    q.push(AStarPoint(np, p.c + 1, p.c + 1 + man_d(np, t)));
                    if (atari.count(np)) {
                        found = true;
                    }
                }
            }
        }
        // lmove
        for (int i1 = 0; i1 < 6; i1++) {
            for (int k1 = 1; k1 <= 5; k1++) {
                int dx1 = adj_dx[i1] * k1, nx1 = p.x + dx1;
                int dy1 = adj_dy[i1] * k1, ny1 = p.y + dy1;
                int dz1 = adj_dz[i1] * k1, nz1 = p.z + dz1;
                if (nx1 < 0 || nx1 >= W || ny1 < 0 || ny1 >= H || nz1 < 0 || nz1 >= R) {
                    break;
                }
                if (field2[nx1][ny1][nz1]) {
                    break;
                }
                for (int i2 = 0; i2 < 6; i2++) {
                    for (int k2 = 1; k2 <= 5; k2++) {
                        int dx2 = adj_dx[i2] * k2, nx2 = nx1 + dx2;
                        int dy2 = adj_dy[i2] * k2, ny2 = ny1 + dy2;
                        int dz2 = adj_dz[i2] * k2, nz2 = nz1 + dz2;
                        if (nx2 < 0 || nx2 >= W || ny2 < 0 || ny2 >= H || nz2 < 0 || nz2 >= R) {
                            break;
                        }
                        if (field2[nx2][ny2][nz2]) {
                            break;
                        }
                        Point np(nx2, ny2, nz2);
                        if (!prev.count(np)) {
                            prev[np] = p_p;
                            command[np] = make_command_6("lmove", dx1, dy1, dz1, dx2, dy2, dz2);
                            q.push(AStarPoint(np, p.c + 1, p.c + 1 + man_d(np, t)));
                            if (atari.count(np)) {
                                found = true;
                            }
                        }
                    }
                }
            }
        }
    }

    // unreachable
    return vector<vector<string> >(1, vector<string>(1, "> <"));
}

vector<vector<string> > computePath(const int R, const int W, int const H, vector<PointD> tasks, vector<PointD>& newTasks) {
    vector<vector<string> > ret;
    vector<vector<vector<char> > > field2(W, vector<vector<char> >(H, vector<char>(R)));
    Point current(0, 0, 0);
    int renzoku = 0;
    for (int i = 0; i < tasks.size() + 1; i++) {
        // if (i < tasks.size()) cerr << tasks[i].x << "," << tasks[i].y << "," << tasks[i].z << ":" << tasks[i].d << endl;
        int rush;
        vector<vector<string> > commands = bfs(R, W, H, field2, current, tasks, i);
        if (commands.size() == 1 && commands[0].size() == 1 && commands[0][0] == "> <") {
            tasks.push_back(tasks[i]);
            cerr << "*";
            renzoku++;
            if (renzoku >= tasks.size() - i) {
                ret.push_back(vector<string>(1, "> <"));
                cerr << endl;
                return ret;
            }
        } else {
            for (int k = 0; k < commands.size(); k++) {
                if (commands[k].size()) {
                    ret.push_back(commands[k]);
                    if (i + k < tasks.size()) {
                        newTasks.push_back(tasks[i + k]);
                        field2[tasks[i + k].x][tasks[i + k].y][tasks[i + k].z] = 1;
                    }
                }
                cerr << (k == 0 ? "." : "_");
            }
            i += commands.size() - 1;
            renzoku = 0;
        }
    }
    cerr << endl;
    return ret;
}

void calcConnected(int R) {
    UnionFind uf(R);
    int full = 0;
    for (int y = 0; y < R; y++) {
        for (int x = 0; x < R; x++) {
            for (int z = 0; z < R; z++) {
                if (field[x][y][z]) {
                    full++;
                    if (y == 0) {
                        uf.unite(uf.ground(), uf.find(x, y, z));
                    } else {
                        for (int k = 0; k < 6; k++) {
                            if (adj_dy[k] > 0) /* 上 */ continue;
                            int nx = x + adj_dx[k];
                            int ny = y + adj_dy[k];
                            int nz = z + adj_dz[k];
                            if (field[nx][ny][nz]) {
                                uf.unite(x, y, z, nx, ny, nz);
                            }
                        }
                    }
                }
            }
        }
        total_by_h[y + 1] = full;
        connected[y] = uf.size(uf.ground()) == full + 1;
    }
}

int main() {
    unsigned char c;
    cin.read((char *)&c, 1);
    const int R = c;
    cerr << "R = " << R << endl;
    const int bytes = (R * R * R + 7) / 8;
    cerr << "Reading " << bytes << " bytes" << endl;

    {
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

    int N = min(20, R);
    int retry_count = 0;

    cerr << "読み込みおわり" << endl;

    calcConnected(R);

    cerr << "連結計算おわり" << endl;

    // 分裂
    for (int i = 0; i < N - 1; i++) {
        for (int k = 0; k < i; k++) cout << "wait" << endl;
        cout << "fission 1 0 0 " << (N - i - 2) << endl;
    }

    bool reachable[R][R][R]; for (int x = 0; x < R; x++) for (int y = 0; y < R; y++) for (int z = 0; z < R; z++) reachable[x][y][z] = false;
    for (int x = 0; x < R; x++) for (int z = 0; z < R; z++) reachable[x][0][z] = true;

    int h_low = 0;
    for (int h_high = min(R, h_low + 5); h_high <= R; h_high++) {
        set<Point> waiting;
retry:
        int H = h_high - h_low + 1;
        // 連結チェック
        if (!connected[h_high - 1]) {
            continue;
        }
        if (total_by_h[h_high] - total_by_h[h_low] == 0) {
            continue;
        }
        cerr << "高さ: " << h_low << " -- " << h_high << endl;

        // == kawatea slice == /
        for (int i = h_low; i < h_high; i++) {
            for (int j = 0; j < R; j++) {
                for (int k = 0; k < R; k++) {
                    sum[i][j + 1][k + 1] = sum[i][j + 1][k] + sum[i][j][k + 1] - sum[i][j][k];
                    if (field[j][i][k]) sum[i][j + 1][k + 1]++;
                }
            }
        }

        for (int i = h_low; i < h_high; i++) {
            for (int j = 0; j < R; j++) {
                for (int k = j; k < R; k++) {
                    voxels_x[j][k] += sum[i][k + 1][R] - sum[i][j][R] - sum[i][k + 1][0] + sum[i][j][0];
                    voxels_z[j][k] += sum[i][R][k + 1] - sum[i][R][j] - sum[i][0][k + 1] + sum[i][0][j];
                }
            }
        }

        for (int i = 0; i <= R; i++) {
            for (int j = 0; j <= N; j++) {
                dp_x[i][j][0] = 1e9;
                dp_z[i][j][0] = 1e9;
            }
        }

        dp_x[0][0][0] = 0;
        dp_z[0][0][0] = 0;

        for (int i = 0; i < R; i++) {
            for (int j = 0; j < N; j++) {
                for (int k = i + 1; k <= R; k++) {
                    if (max(dp_x[i][j][0], voxels_x[i][k - 1]) < dp_x[k][j + 1][0]) {
                        dp_x[k][j + 1][0] = max(dp_x[i][j][0], voxels_x[i][k - 1]);
                        dp_x[k][j + 1][1] = i;
                    }
                    if (max(dp_z[i][j][0], voxels_z[i][k - 1]) < dp_z[k][j + 1][0]) {
                        dp_z[k][j + 1][0] = max(dp_z[i][j][0], voxels_z[i][k - 1]);
                        dp_z[k][j + 1][1] = i;
                    }
                }
            }
        }
        // == kawatea slice == /

        // とりあえずxで雑に分割
        vector<int> x_range(N + 1);
    //    for (int i = 0; i < N; i++) {
    //        x_range[i + 1] = R * (i + 1) / N;
    //    }

        x_range[N] = R;
        for (int i = N, last = R; i > 0; i--) {
            int parent = dp_x[last][i][1];
            x_range[i - 1] = parent;
            last = parent;
        }

        // for (int i = 0; i < N + 1; i++) cerr << x_range[i] << endl;

        // 準備
        {
            vector<vector<string> > prepare(N);
            for (int i = 0; i < N; i++) {
                for (int k = 0; k < N - i - 1; k++) {
                    prepare[i].push_back("wait");
                }
                int dx = x_range[i] - i;
                for (int k = 0; k < dx / 15; k++) {
                    prepare[i].push_back(make_command_3("smove", 15, 0, 0));
                }
                if (dx % 15) {
                    prepare[i].push_back(make_command_3("smove", dx % 15, 0, 0));
                }
            }

            int max_prep = 0;
            for (int i = 0; i < N; i++) max_prep = max(max_prep, (int) prepare[i].size());

            for (int i = 0; i < max_prep; i++) {
                for (int k = 0; k < N; k++) {
                    if (i < prepare[k].size()) {
                        cout << prepare[k][i] << endl;
                    } else {
                        cout << "wait" << endl;
                    }
                }
            }
        }

        // 距離アップデート
        {
            queue<Point> q;
            q.push(Point(-1, -1, -1));
            for (int x = 0; x < R; x++) for (int z = 0; z < R; z++) if (field[x][h_low][z] && (h_low == 0 || field[x][h_low - 1][z])) q.push(Point(x, h_low, z));

            bool visited[R][R][R]; for (int x = 0; x < R; x++) for (int y = h_low; y < h_high; y++) for (int z = 0; z < R; z++) visited[x][y][z] = false;
            int d = 0;
            while (q.size() > 1) {
                Point p = q.front(); q.pop();
                if (p.x == -1) {
                    d++;
                    q.push(p);
                    continue;
                }
                if (visited[p.x][p.y][p.z]) {
                    continue;
                }
                dist[p.x][p.y][p.z] = d;
                visited[p.x][p.y][p.z] = true;
                for (int i = 0; i < 6; i++) {
                    Point next(p.x + adj_dx[i], p.y + adj_dy[i], p.z + adj_dz[i]);
                    if (next.x < 0 || next.x >= R || next.y < h_low || next.y >= h_high || next.z < 0 || next.z >= R) continue;
                    if (!field[next.x][next.y][next.z]) continue;
                    if (visited[next.x][next.y][next.z]) continue;
                    q.push(next);
                }
            }
        }

        vector<vector<PointD> > tasks(N);
        for (int i = 0; i < N; i++) {
            for (int x = x_range[i]; x < x_range[i+1]; x++) {
                for (int y = h_low; y < h_high; y++) {
                    for (int z = 0; z < R; z++) {
                        if (field[x][y][z]) {
                            tasks[i].push_back(PointD(x - x_range[i], y - h_low, z, dist[x][y][z], waiting.count(Point(x, y, z))));
                        }
                    }
                }
            }
            sort(tasks[i].begin(), tasks[i].end());
        }

        cerr << "前計算中..." << endl;
        vector<vector<vector<string> > > paths;
        vector<vector<PointD> > newTasks(N);
        for (int i = 0; i < N; i++) {
            cerr << "bot-" << i << "/" << N << "(" << tasks[i].size() << ")" << endl;
            paths.push_back(computePath(R, x_range[i + 1] - x_range[i], H, tasks[i], newTasks[i]));
        }

        cerr << "前計算 おわった" << endl;
        // for (int i = 0; i < N; i++) {
        //     cerr << paths[i].size() << endl;
        // }

    //    for (int i = 0; i < N; i++) {
    //        for (int j = 0; j < paths[i].size(); j++) {
    //            if (paths[i][j].size() == 1 && paths[i][j][0] == "> <") {
    //                cerr << "> <" << endl;
    //                cerr << "bot-" << i << endl;
    //                for (int k = 0; k <= j; k++) {
    //                    cerr << tasks[i][k].x << " " << tasks[i][k].y << " " << tasks[i][k].z << endl;
    //                }
    //                return 1;
    //            }
    //        }
    //    }

        vector<int> progress1(N);
        vector<int> progress2(N);
        bool running = true;

        int Z = 0;
        while (running) {
            bool stuck = true;
            // cerr << "turn:" << (Z++) << endl;
            for (int i = 0; i < N; i++) {
                int p1 = progress1[i], p2 = progress2[i];
                // cerr << i << ": " << p1 << "/" << paths[i].size() << "," << p2 << "/" << (p1 == paths[i].size() ? 0 : paths[i][p1].size()) << endl;
                if (p1 == paths[i].size()) {
                    // 終わってる
                    cout << "wait" << endl;
                    continue;
                } else if (p1 != paths[i].size() - 1 && p2 == paths[i][p1].size() - 1) {
                    // fill
                    PointD task = newTasks[i][progress1[i]];
                    if (reachable[task.x + x_range[i]][task.y + h_low][task.z]) {
                        cout << paths[i][p1][p2] << endl;
                        progress2[i]++;
                        stuck = false;
                        for (int k = 0; k < 6; k++) {
                            int dx = adj_dx[k], nx = task.x + dx + x_range[i];
                            int dy = adj_dy[k], ny = task.y + dy + h_low;
                            int dz = adj_dz[k], nz = task.z + dz;
                            if (nx >= 0 && nx < R && ny >= 0 && ny < R && nz >= 0 && nz < R) {
                                // cerr << "REACHABLE:" << nx << " " << ny << " " << nz << endl;
                                reachable[nx][ny][nz] = true;
                            }
                        }
                        waiting.erase(task.to_point());
                    } else {
                        cerr << "WAITING:" << task.x + x_range[i] << " " << task.y + h_low << " " << task.z << " " << task.d << endl;
                        waiting.insert(task.to_point());
                        cout << "wait" << endl;
                    }
                } else {
                    // move
                    cout << paths[i][p1][p2] << endl;
                    progress2[i]++;
                        stuck = false;
                }
                if (progress2[i] == paths[i][p1].size()) {
                    progress1[i]++;
                    progress2[i] = 0;
                }
            }
            if (stuck) {
                cerr << "> <!!" << endl;
                retry_count++;
                if (retry_count < 10) goto retry;
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
        {
            vector<vector<string> > prepare(N);
            for (int i = 0; i < N; i++) {
                for (int k = 0; k < i; k++) {
                    prepare[i].push_back("wait");
                }
                int dx = x_range[i] - i;
                for (int k = 0; k < dx / 15; k++) {
                    prepare[i].push_back(make_command_3("smove", -15, 0, 0));
                }
                if (dx % 15) {
                    prepare[i].push_back(make_command_3("smove", -(dx % 15), 0, 0));
                }
            }

            int max_prep = 0;
            for (int i = 0; i < N; i++) max_prep = max(max_prep, (int) prepare[i].size());

            for (int i = 0; i < max_prep; i++) {
                for (int k = 0; k < N; k++) {
                    if (i < prepare[k].size()) {
                        cout << prepare[k][i] << endl;
                    } else {
                        cout << "wait" << endl;
                    }
                }
            }
        }

        h_low = h_high;
    }
    // 融合
    for (int i = N - 2; i >= 0; i--) {
        for (int k = 0; k < i; k++) cout << "wait" << endl;
        cout << "fusionp 1 0 0 " << endl;
        cout << "fusions -1 0 0 " << endl;
    }

    //初期位置に戻る
    {
        for (int i = 0; i < h_low / 15; i++) {
            cout << "smove 0 -15 0" << endl;
        }
        if (h_low % 15) {
            cout << "smove 0 -" << (h_low % 15) << " 0" << endl;
        }
    }
    cout << "halt" << endl;

    return 0;
}
