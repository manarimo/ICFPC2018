#include <iostream>
#include <queue>
#include <sstream>
#include <map>
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

class Reservation {
    set<Point> data;
    Reservation() {}

    void reserve(const Point& p) {
        if (data.count(p)) {
            cerr << "Reservation failed." << endl;
            exit(1);
        }
        data.insert(p);
    }

    bool reserved(const Point& p) {
        return data.count(p);
    }
}

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
Point bot_pos[40] = {};
deque<Command> exec_plan[40] = {};
bool bot_terminated[40] = {};
int terminated_bot_count = 0;
set<Point> reachables;

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

void nextTurn() {
    cerr << "turn end: " << turn << endl;
    turn++;
}

int main() {
    read_input();
    cerr << "読み込みおわり" << endl;

    N = min(40, R);

    for (int i = 0; i < N; i++) {
        reserve(Point(i, 0, 0));
    }
    for (int i = 0; i < N - 1; i++) {
        for (int k = 0; k < i; k++) {
            plan_bot(k);
        }
        for (int k = 0; k < i; k++) {
            exec_bot(k);
        }
        cout << "fission 1 0 0 " << (N - 2 - i) << endl;
        bot_pos[i+1] = Point(i+1, 0, 0);
        nextTurn();
    }

    while (terminated_bot_count < N) {
        bool ok = false;
        for (int i = 0; i < N; i++) {
            plan_bot(i);
        }
        for (int i = 0; i < N; i++) {
            ok |= exec_bot(i);
        }
        if (!ok && terminated_bot_count < N) {
            cerr << "> <!!" << endl;
            for (int x = 0; x < R; x++) for (int y = 0; y < R; y++) for (int z = 0; z < R; z++) if (field[x][y][z] && !scheduled[x][y][z]) {
                cerr << x << " " << y << " " << z << " score=" << boxel_score[x][y][z] << endl;
            }
            exit(1);
        }
        nextTurn();
    }

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