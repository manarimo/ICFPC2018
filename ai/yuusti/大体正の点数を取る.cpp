#include <bits/stdc++.h>

using namespace std;

const int RMAX = 250;
bool field[RMAX][RMAX][RMAX]; // x, y, z
bool placed[RMAX][RMAX][RMAX]; // x, y, z
int R, NUM;

bool high = false;

struct P {
    int x, y, z;
};

P operator+(const P &a, const P &b) {
    return P{a.x + b.x, a.y + b.y, a.z + b.z};
}

P operator-(const P &a, const P &b) {
    return P{a.x - b.x, a.y - b.y, a.z - b.z};
}

ostream& operator<<(ostream& os, const P& p) {
    os << p.x << ' ' << p.y << ' ' << p.z;
    return os;
}

struct bot {
    P pos;
    set<int> seed;

    void halt() {
        cout << "halt" << '\n';
    }

    void wait() {
        cout << "wait" << '\n';
    }

    void flip() {
        high = !high;
        cout << "flip" << '\n';
    }

    void smove(P lld) {
        assert((lld.x == 0) + (lld.y == 0) + (lld.z == 0) == 2);
        pos = pos + lld;
        cout << "smove " << lld << '\n';
    }

    void lmove(P sld1, P sld2) {
        assert((sld1.x == 0) + (sld1.y == 0) + (sld1.z == 0) == 2);
        assert((sld2.x == 0) + (sld2.y == 0) + (sld2.z == 0) == 2);
        pos = pos + sld1 + sld2;
        cout << "lmove " << sld1 << ' ' << sld2 << '\n';
    }

    void fusion(bot &other) {
        fusion(other, true);
    }

    void fusion(bot &other, bool p) {
        cout << "fusion" << (p ? "p":"s") << other.pos - pos << '\n';
        cout << "fusion" << (p ? "s":"p") << pos - other.pos << '\n';
        auto &par = p ? seed : other.seed;
        auto &ch = p ? other.seed : seed;
        for (auto e : ch) {
            par.insert(e);
        }
    }

    void fission(P nd, int m) {
        // TODO
//        cout << "fission " << nd << ' ' << m << '\n';
    }

    void fill(P d) {
        cout << "fill " << d << '\n';
    }
};

struct UnionFind {
    vector<int> par, sz;

    UnionFind(int n) {
        par.resize(n);
        sz.resize(n);
        for (int i = 0; i < n; ++i) {
            par[i] = i;
            sz[i] = 1;
        }
    }

    int find(int x) {
        if (x == par[x]) return x;
        return par[x] = find(par[x]);
    }

    void unite(int x, int y) {
        x = find(x);
        y = find(y);
        par[x] = y;
        if (x == y) return;
        sz[y] += sz[x];
    }

    int size(int x) {
        return sz[find(x)];
    }
};

int f(P p) {
    return p.x * R * R + p.y * R + p.z;
}

int dx[] = {0, 1, 0, -1, 0};
int dz[] = {1, 0, -1, 0, 0};
int dy[] = {0, 0, 0, 0, -1};

void input() {
    cin.tie(0);
    ios::sync_with_stdio(false);

    unsigned char c;
    cin.read((char *) &c, 1);
    R = c;
    cerr << "R = " << R << endl;
    const int bytes = (R * R * R + 7) / 8;
    cerr << "Reading " << bytes << " bytes" << endl;

    {
        int x = 0, y = 0, z = 0;
        for (int i = 0; i < bytes; i++) {
            cin.read((char *) &c, 1);
            for (int j = 0; j < 8 && x < R; j++) {
                field[x][y][z] = (c >> j) & 1;
                if (field[x][y][z]) ++NUM;
                z++;
                if (z == R) z = 0, y++;
                if (y == R) y = 0, x++;
            }
        }
    }
    cerr << NUM << endl;
}

int main() {
    input();

    queue<P> q;
    for (int y = 0; y < R; ++y) {
        for (int x = 0; x < R; ++x) {
            for (int z = 0; z < R; ++z) {
                if (field[x][y][z]) {
                    q.push({x, y, z});
                }
            }
        };
    }

    UnionFind uf(R * R * R);

    bot b = bot {{0, 0, 0}, {}};

    b.smove({0, 1, 0});

    int cnt = 0;
    while(!q.empty()) {
        auto p = q.front(); q.pop();
        ++cnt;
        placed[p.x][p.y][p.z] = true;
        while (p.y + 1 > b.pos.y) {
            b.smove({0, 1, 0});
        }
        while(p.x - b.pos.x > 0) b.smove({min(p.x - b.pos.x, 15), 0, 0});
        while(p.x - b.pos.x < 0) b.smove({-min(b.pos.x - p.x, 15), 0, 0});
        while(p.z - b.pos.z > 0) b.smove({0, 0, min(p.z - b.pos.z, 15)});
        while(p.z - b.pos.z < 0) b.smove({0, 0, -min(b.pos.z - p.z, 15)});

        if (p.y == 0) {
            uf.unite(f(p), f({0, 0, 0}));
        } else {
            for (int i = 0; i < 5; ++i) {
                P adj = p + P{dx[i], dy[i], dz[i]};
                if (placed[adj.x][adj.y][adj.z]) {
                    uf.unite(f(p), f(adj));
                }
            }
        }
        if ((uf.size(f(p)) != cnt + 1) && !high) b.flip();
        b.fill({0, -1, 0});
        if ((uf.size(f(p)) == cnt + 1) && high) b.flip();
    }

    while(b.pos.x > 0) b.smove({-(min(b.pos.x, 15)), 0, 0});
    while(b.pos.z > 0) b.smove({0, 0, -min(b.pos.z, 15)});
    while(b.pos.y > 0) b.smove({0, -min(b.pos.y, 15), 0});
    if (high) b.flip();
    b.halt();

    return 0;
}
