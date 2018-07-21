#include <bits/stdc++.h>

using namespace std;

const int RMAX = 250;
const int SEED = 20;
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

bool operator==(const P &a, const P &b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool operator!=(const P &a, const P &b) {
    return !(a == b);
}

ostream& operator<<(ostream& os, const P& p) {
    os << p.x << ' ' << p.y << ' ' << p.z;
    return os;
}

struct bot {
    P pos;
    int bid;
    vector<int> seeds;
    bool active;

    void halt() {
//        cerr << bid << ": ";
        cout << "halt" << '\n';
    }

    void wait() {
//        cerr << bid << ": ";
        cout << "wait" << '\n';
    }

    void flip() {
        high = !high;
//        cerr << bid << ": ";
        cout << "flip" << '\n';
    }

    void moveXZOrWait(P dst) {
        P &p = pos;
        if(dst.x - pos.x > 0) smove({min(dst.x - pos.x, 15), 0, 0});
        else if(dst.x - pos.x < 0) smove({-min(pos.x - dst.x, 15), 0, 0});
        else if(dst.z - pos.z > 0) smove({0, 0, min(dst.z - pos.z, 15)});
        else if(dst.z - pos.z < 0) smove({0, 0, -min(pos.z - dst.z, 15)});
        else wait();
    }

    void moveOrWait(P dst) {
        P &p = pos;
        if(dst.x - pos.x > 0) smove({min(dst.x - pos.x, 15), 0, 0});
        else if(dst.x - pos.x < 0) smove({-min(pos.x - dst.x, 15), 0, 0});
        else if(dst.y - pos.y > 0) smove({0, min(dst.y - pos.y, 15), 0});
        else if(dst.y - pos.y < 0) smove({0, -min(pos.y - dst.y, 15), 0});
        else if(dst.z - pos.z > 0) smove({0, 0, min(dst.z - pos.z, 15)});
        else if(dst.z - pos.z < 0) smove({0, 0, -min(pos.z - dst.z, 15)});
        else wait();
    }

    void smove(P lld) {
        assert((lld.x == 0) + (lld.y == 0) + (lld.z == 0) == 2);
        pos = pos + lld;
//        cerr << bid << ": ";
        cout << "smove " << lld << '\n';
    }

    void lmove(P sld1, P sld2) {
        assert((sld1.x == 0) + (sld1.y == 0) + (sld1.z == 0) == 2);
        assert((sld2.x == 0) + (sld2.y == 0) + (sld2.z == 0) == 2);
        pos = pos + sld1 + sld2;
//        cerr << bid << ": ";
        cout << "lmove " << sld1 << ' ' << sld2 << '\n';
    }

    void fusion(bot &other, bool p) {
//        cerr << bid << ": ";
        cout << "fusion" << (p ? "p ":"s ") << other.pos - pos << '\n';
        if (p) {
            for (int i = 0; i < SEED; ++i) {
                seeds[i] |= other.seeds[i];
            }
        } else {
            active = false;
        }
    }

    bot fission(P nd, int m) {
        int cnt = 0;
        int nbid = bid + 1;
        vector<int> s(SEED);
        for (int i = bid + 1; i < SEED && cnt < m; ++i) {
            if (seeds[i]) {
                seeds[i] = 0;
                if (++cnt == 1) {
                    nbid = i;
                } else {
                    s[i] = 1;
                }
            }
        }
        assert(cnt == m);
//        cerr << bid << ": ";
        cout << "fission " << nd << ' ' << m << '\n';
        return bot({pos + nd, nbid, s, true});
    }

    void fill(P d) {
//        cerr << bid << ": ";
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

void moveBots(vector<bot> &bots, int i, P dst) {
//    cerr << "move #" << bots[i].pos << " to " << dst << endl;
    while (bots[i].pos != dst) {
        for (int j = 0; j < SEED; ++j) {
            if (bots[j].active) {
                if (i == j) bots[i].moveOrWait(dst);
                else bots[j].wait();
            }
        }
    }
}

vector<int> calcBlockSum(int y) {
    vector<int> block(R), sum(R + 1);
    for (int x = 0; x < R; ++x) {
        for (int z = 0; z < R; ++z) {
            block[x] += field[x][y][z];
        }
        sum[x + 1] = sum[x] + block[x];
    }
    vector<vector<int>> dp(R + 1, vector<int>(SEED + 1, 1e9)), prv(R + 1, vector<int>(SEED + 1, 0));
    dp[0][0] = 0;

    for (int i = 0; i < R; ++i) {
        for (int j = 0; j < SEED; ++j) {
            for (int k = i + 1; k <= R; ++k) {
                if (max(dp[i][j], sum[k] - sum[i]) < dp[k][j + 1]) {
                    dp[k][j + 1] = max(dp[i][j], sum[k] - sum[i]);
                    prv[k][j + 1] = i;
                }
            }
        }
    }

    vector<int> res;
    int r = SEED, p = R;
    while (r > 0) {
        p = prv[p][r];
        res.push_back(p);
//        cerr << "p=" << p << endl;
        r--;
    }
    reverse(res.begin(), res.end());
    res.push_back(R);

//    for (int i = 0; i <= R; ++i) {
//        cerr << "i=" << i << ", sum=" << sum[i] << endl;
//    }
//    for (int i = 0; i < SEED; ++i) {
//        cerr << "border=" << res[i] << endl;
//    }

    return res;
}

vector<int> addNext(vector<queue<P>> &q, int y) {
    cerr << "finding next points" << endl;
    auto range = calcBlockSum(y);

    for (int x = 0; x < R; ++x) {
        for (int z = 0; z < R; ++z) {
            if (field[x][y][z]) {
                for (int i = 1; i <= SEED; ++i) {
                    if (x < range[i]) {
                        q[i - 1].push({x, y, z});
                        break;
                    }
                }
            }
        }
    }
    return range;
}

int main() {
    input();

    UnionFind uf(R * R * R);

    vector<bot> bots(SEED);
    vector<int> seeds(SEED);
    for (int i = 1; i < SEED; ++i) seeds[i] = 1;
    bots[0] = bot {{0, 0, 0}, 0, seeds, true};

    moveBots(bots, 0, {0, 1, 0});

    vector<queue<P>> q(SEED);
    auto basew = addNext(q, 0);

    cerr << "fission" << endl;
    for (int i = 0; i < SEED - 1; ++i) {
        auto &p = bots[i].pos;
        for (int j = 0; j < i; ++j) {
            bots[j].wait();
        }
        bots[i + 1] = bots[i].fission({0, 0, 1}, SEED - 2 - i);
        moveBots(bots, i + 1, {basew[i + 1], 1, bots[i + 1].pos.z});
    }

    cerr << "block" << endl;
    int cnt = 0;
    int curY = 1;
    while (cnt < NUM) {
        bool ok = true;
        for (int i = 0; i < SEED; ++i) {
            ok &= q[i].empty();
        }

        if (ok) {
            auto range = addNext(q, curY);
            queue<int> nxt;
            vector<int> done(SEED);
            for (int i = 0; i < SEED; ++i) {
                nxt.push(i);
            }
            while (!nxt.empty()) {
                int i = nxt.front();
//                cerr <<  bots[i].pos << " to " << P{range[i], curY + 1, bots[i].pos.z} << endl;
                nxt.pop();
                if (i + 1 < SEED && !done[i+1] && bots[i+1].pos.x <= range[i]) {
                    nxt.push(i);
                    continue;
                }
                moveBots(bots, i, {range[i], curY, bots[i].pos.z});
                moveBots(bots, i, {range[i], curY + 1, bots[i].pos.z});
                done[i] = 1;
            }

            ++curY;
        } else {
//            cerr << "place" << endl;
            // Check the status after placing blocks
            for (int i = 0; i < SEED; ++i) {
                if (q[i].empty()) continue;
                auto &p = q[i].front();
                if (bots[i].pos == p + P({0, 1, 0})) {
                    ++cnt;
                    placed[p.x][p.y][p.z] = 1;
                    if (p.y == 0) {
                        uf.unite(f(p), f({0, 0, 0}));
                    } else {
                        for (int d = 0; d < 5; ++d) {
                            P adj = p + P{dx[d], dy[d], dz[d]};
                            if (placed[adj.x][adj.y][adj.z]) {
                                uf.unite(f(p), f(adj));
                            }
                        }
                    }
                }
            }
            // Need flip
            if (uf.size(f({0, 0, 0})) != cnt + 1 && !high) {
                bots[0].flip();
                for (int i = 1; i < SEED; ++i) {
                    bots[i].wait();
                }
            }

            // Place or move
            for (int i = 0; i < SEED; ++i) {
                if (q[i].empty()) {
                    bots[i].wait();
                    continue;
                }
                auto &p = q[i].front();
                if (bots[i].pos == p + P({0, 1, 0})) {
                    bots[i].fill({0, -1, 0});
                    q[i].pop();
                } else {
                    bots[i].moveXZOrWait(p);
                }
            }

            // Should flip
            if (uf.size(f({0, 0, 0})) == cnt + 1 && high) {
                bots[0].flip();
                for (int i = 1; i < SEED; ++i) {
                    bots[i].wait();
                }
            }
        }
    }

    cerr << "fusion" << endl;
    auto &b = bots[0];
    moveBots(bots, 0, {0, b.pos.y, 0});
    for (int i = 1; i < SEED; ++i) {
        moveBots(bots, i, {1, b.pos.y, 1});
        moveBots(bots, i, {0, b.pos.y, 1});
        b.fusion(bots[i], true);
        bots[i].fusion(b, false);
        for (int j = i + 1; j < SEED; ++j) {
            bots[j].wait();
        }
    }

    while(b.pos.x > 0) b.smove({-(min(b.pos.x, 15)), 0, 0});
    while(b.pos.z > 0) b.smove({0, 0, -min(b.pos.z, 15)});
    while(b.pos.y > 0) b.smove({0, -min(b.pos.y, 15), 0});
    b.halt();

    assert(cnt == NUM);

    return 0;
}

