#include <bits/stdc++.h>

using namespace std;

const int RMAX = 250;
bool field[RMAX][RMAX][RMAX]; // x, y, z
int R, NUM;

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

    int cnt = 0;

    bot b = bot {{0, 0, 0}, {}};

    b.flip();
    b.smove({1, 0, 0});
    b.lmove({0, 1, 0}, {0, 0, 1});

    int dz = 1;
    while (b.pos.y < R - 1) {
        int dx = 1;
        while((dz > 0 && b.pos.z < R - 1) || (dz < 0 && b.pos.z > 0)) {
            while ((dx > 0 && b.pos.x < R - 1) || (dx < 0 && b.pos.x > 0)) {
                if (field[b.pos.x][b.pos.y - 1][b.pos.z]) {
                    b.fill({0, -1, 0});
                    if (++cnt == NUM) goto X;
                }
                b.smove({dx, 0, 0});
            }
            b.smove({0, 0, dz});
            dx = -dx;
        }
        b.smove({0, 1, 0});
        dz = -dz;
    }
    X:
    if (b.pos.y != R - 1) b.smove({0, 1, 0});

    while(b.pos.x > 0) b.smove({-(min(b.pos.x, 15)), 0, 0});
    while(b.pos.z > 0) b.smove({0, 0, -min(b.pos.z, 15)});
    while(b.pos.y > 0) b.smove({0, -min(b.pos.y, 15), 0});
    b.flip();
    b.halt();

    return 0;
}