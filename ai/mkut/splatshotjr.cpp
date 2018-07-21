#include <iostream>
#include <queue>
#include <algorithm>
#include <map>
#include <sstream>
using namespace std;

// 6
int adj_dx[] = {1,-1, 0, 0, 0, 0};
int adj_dy[] = {0, 0, 1,-1, 0, 0};
int adj_dz[] = {0, 0, 0, 0, 1,-1};

// 18
int nd_dx[] = {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,-1,-1,-1,-1,-1};
int nd_dy[] = {1, 0, 0, 0,-1, 1, 1, 1, 0, 0,-1,-1,-1, 1, 0, 0, 0,-1};
int nd_dz[] = {0, 1, 0,-1, 0, 1, 0,-1, 1,-1, 1, 0,-1, 0, 1, 0,-1, 0};

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
    int x, y, z, d;
    PointD() : x(0), y(0), z(0), d(0) {}
    PointD(int x, int y, int z, int d) : x(x), y(y), z(z), d(d) {}
    bool operator<(const PointD& a) const {
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
vector<string> bfs(int R, int W, vector<vector<vector<char> > >& field, Point& s, Point& t, bool fill, Point& next) {
    map<Point, Point> prev;
    map<Point, string> command;
    queue<Point> q;
    q.push(s);
    prev[s] = s;
    command[s] = "";
    // cerr << "s: " << s.x << "," << s.y << "," << s.z << endl;
    // cerr << "t: " << t.x << "," << t.y << "," << t.z << endl;
    // cerr << (fill ? "fill" : "nofill") << endl;
    while (q.size()) {
        Point p = q.front(); q.pop();
        // cerr << p.x << "," << p.y << "," << p.z << endl;
        if (fill) {
            for (int i = 0; i < 18; i++) {
                int dx = nd_dx[i], nx = p.x + dx;
                int dy = nd_dy[i], ny = p.y + dy;
                int dz = nd_dz[i], nz = p.z + dz;
                if (nx == t.x && ny == t.y && nz == t.z) {
                    vector<string> ret;
                    ret.push_back(make_command_3("fill", dx, dy, dz));
                    Point tp(p);
                    while (tp.x != s.x || tp.y != s.y || tp.z != s.z) {
                        ret.push_back(command[tp]);
                        tp = prev[tp];
                    }
                    reverse(ret.begin(), ret.end());
                    next.x = p.x; next.y = p.y; next.z = p.z;
                    return ret;
                }
            }
        } else {
            if (p.x == t.x && p.y == t.y && p.z == t.z) {
                vector<string> ret;
                Point tp(p);
                while (tp.x != s.x || tp.y != s.y || tp.z != s.z) {
                    ret.push_back(command[tp]);
                    tp = prev[tp];
                }
                reverse(ret.begin(), ret.end());
                return ret;
            }
        }
        // smove
        for (int i = 0; i < 6; i++) {
            for (int j = 1; j <= 15; j++) {
                int dx = adj_dx[i] * j, nx = p.x + dx;
                int dy = adj_dy[i] * j, ny = p.y + dy;
                int dz = adj_dz[i] * j, nz = p.z + dz;
                if (nx < 0 || nx >= W || ny < 0 || ny >= R || nz < 0 || nz >= R) {
                    break;
                }
                if (field[nx][ny][nz]) {
                    break;
                }
                Point np(nx, ny, nz);
                if (!prev.count(np)) {
                    prev[np] = p;
                    command[np] = make_command_3("smove", dx, dy, dz);
                    q.push(np);
                }
            }
        }
        // lmove
        for (int i1 = 0; i1 < 6; i1++) {
            for (int k1 = 1; k1 <= 5; k1++) {
                int dx1 = adj_dx[i1] * k1, nx1 = p.x + dx1;
                int dy1 = adj_dy[i1] * k1, ny1 = p.y + dy1;
                int dz1 = adj_dz[i1] * k1, nz1 = p.z + dz1;
                if (nx1 < 0 || nx1 >= W || ny1 < 0 || ny1 >= R || nz1 < 0 || nz1 >= R) {
                    break;
                }
                if (field[nx1][ny1][nz1]) {
                    break;
                }
                for (int i2 = 0; i2 < 6; i2++) {
                    for (int k2 = 1; k2 <= 5; k2++) {
                        int dx2 = adj_dx[i2] * k2, nx2 = nx1 + dx2;
                        int dy2 = adj_dy[i2] * k2, ny2 = ny1 + dy2;
                        int dz2 = adj_dz[i2] * k2, nz2 = nz1 + dz2;
                        if (nx2 < 0 || nx2 >= W || ny2 < 0 || ny2 >= R || nz2 < 0 || nz2 >= R) {
                            break;
                        }
                        if (field[nx2][ny2][nz2]) {
                            break;
                        }
                        Point np(nx2, ny2, nz2);
                        if (!prev.count(np)) {
                            prev[np] = p;
                            command[np] = make_command_6("lmove", dx1, dy1, dz1, dx2, dy2, dz2);
                            q.push(np);
                        }
                    }
                }
            }
        }
    }

    // unreachable
    return vector<string>(1, "> <");
}

vector<vector<string> > computePath(const int R, const int W, vector<PointD>& tasks) {
    vector<vector<string> > ret;
    vector<vector<vector<char> > > field(W, vector<vector<char> >(R, vector<char>(R)));
    Point current(0, 0, 0);
    for (int i = 0; i < tasks.size() + 1; i++) {
        cerr << ".";
        Point t = i == tasks.size() ? Point(0, 0, 0) : tasks[i].to_point();
        vector<string> commands = bfs(R, W, field, current, t, i < tasks.size(), current);
        if (commands.size()) {
            ret.push_back(commands);
        }
        if (i < tasks.size()) {
            field[tasks[i].x][tasks[i].y][tasks[i].z] = 1;
        }
    }
    cerr << endl;
    return ret;
}

int main() {
    unsigned char c;
    cin.read((char *)&c, 1);
    const int R = c;
    cerr << "R = " << R << endl;
    const int bytes = (R * R * R + 7) / 8;
    cerr << "Reading " << bytes << " bytes" << endl;

    bool field[R][R][R]; // x, y, z
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

    int dist[R][R][R];
    {
        queue<Point> q;
        q.push(Point(-1, -1, -1));
        for (int x = 0; x < R; x++) for (int z = 0; z < R; z++) if (field[x][0][z]) q.push(Point(x, 0, z));
        
        bool visited[R][R][R]; for (int x = 0; x < R; x++) for (int y = 0; y < R; y++) for (int z = 0; z < R; z++) visited[x][y][z] = false;
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
                if (next.x < 0 || next.x >= R || next.y < 0 || next.y >= R || next.z < 0 || next.z >= R) continue;
                if (!field[next.x][next.y][next.z]) continue;
                if (visited[next.x][next.y][next.z]) continue;
                q.push(next);
            }
        }
    }

    int N = min(20, R);

    // とりあえずxで雑に分割
    vector<int> x_range(N + 1);
    for (int i = 0; i < N; i++) {
        x_range[i + 1] = R * (i + 1) / N;
    }

    //準備
    for (int i = 0; i < N - 1; ) {
        for (int k = 0; k < i; k++) cout << "wait" << endl;
        cout << "fission 1 0 0 " << (N - i - 2) << endl;
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

    vector<vector<PointD> > tasks(N);
    for (int i = 0; i < N; i++) {
        for (int x = x_range[i]; x < x_range[i+1]; x++) {
            for (int y = 0; y < R; y++) {
                for (int z = 0; z < R; z++) {
                    if (field[x][y][z]) {
                        tasks[i].push_back(PointD(x - x_range[i], y, z, dist[x][y][z]));
                    }
                }
            }
        }
        sort(tasks[i].begin(), tasks[i].end());
    }

    for (int i = 0; i < tasks[17].size(); i++) {
        // cerr << tasks[17][i].x << " " << tasks[17][i].y << " " << tasks[17][i].z << endl;
    }

    cerr << "前計算中..." << endl;
    vector<vector<vector<string> > > paths;
    for (int i = 0; i < N; i++) {
        cerr << "bot-" << i << "/" << N << "(" << tasks[i].size() << ")" << endl;
        paths.push_back(computePath(R, x_range[i + 1] - x_range[i], tasks[i]));
    }

    cerr << "前計算した" << endl;
    // for (int i = 0; i < N; i++) {
    //     cerr << paths[i].size() << endl;
    // }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < paths[i].size(); j++) {
            if (paths[i][j].size() == 1 && paths[i][j][0] == "> <") {
                cerr << "> <";
                return 1;
            }
        }
    }

    vector<int> progress1(N);
    vector<int> progress2(N);
    bool reachable[R][R][R]; for (int x = 0; x < R; x++) for (int y = 0; y < R; y++) for (int z = 0; z < R; z++) reachable[x][y][z] = false;
    for (int x = 0; x < R; x++) for (int z = 0; z < R; z++) reachable[x][0][z] = true;
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
                PointD task = tasks[i][progress1[i]];
                if (reachable[task.x + x_range[i]][task.y][task.z]) {
                    cout << paths[i][p1][p2] << endl;
                    progress2[i]++;
                    stuck = false;
                    for (int k = 0; k < 6; k++) {
                        int dx = adj_dx[k], nx = task.x + dx + x_range[i];
                        int dy = adj_dy[k], ny = task.y + dy;
                        int dz = adj_dz[k], nz = task.z + dz;
                        if (nx >= 0 && nx < R && ny >= 0 && ny < R && nz >= 0 && nz < R) {
                            // cerr << "REACHABLE:" << nx << " " << ny << " " << nz << endl;
                            reachable[nx][ny][nz] = true;
                        }
                    }
                } else {
                    // cerr << "WAITING:" << task.x + x_range[i] << " " << task.y << " " << task.z << " " << task.d << endl;
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
    for (int i = N - 1; i > 0; ) {
        int d = x_range[i] - x_range[i - 1] - 1;
        for (int j = 0; j < d / 15; j++) {
            for (int k = 0; k < i; k++) cout << "wait" << endl;
            cout << "smove -15 0 0" << endl;
        }

        if (d % 15) {
            for (int k = 0; k < i; k++) cout << "wait" << endl;
            cout << "smove -" << d % 15 << " 0 0" << endl;
        }
        i--;

        for (int k = 0; k < i; k++) cout << "wait" << endl;
        cout << "fusionp 1 0 0 " << endl;
        cout << "fusions -1 0 0 " << endl;
    }
    cout << "halt" << endl;


    return 0;
}
