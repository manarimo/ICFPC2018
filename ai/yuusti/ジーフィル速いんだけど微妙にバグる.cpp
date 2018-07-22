#include <bits/stdc++.h>

using namespace std;

const int RMAX = 250;
const int SEED = 40;
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

P operator-(const P &a) {
    return P{-a.x, -a.y, -a.z};
}

bool operator==(const P &a, const P &b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool operator!=(const P &a, const P &b) {
    return !(a == b);
}

bool operator<(const P &a, const P &b) {
    return a.x == b.x ? (a.y == b.y ? a.z < b.z : a.y < b.y) : a.x < b.x;
}

ostream &operator<<(ostream &os, const P &p) {
    os << p.x << ' ' << p.y << ' ' << p.z;
    return os;
}

bool validDist(const P &p, int maxD) {
    return ((p.x == 0) + (p.y == 0) + (p.z == 0) == 2)
           && 0 < abs(p.x + p.y + p.z) && abs(p.x + p.y + p.z) <= maxD;
}

bool validSmove(const P &p) {
    return validDist(p, 15);
}

bool validLmove(const P &p1, const P &p2) {
    return validDist(p1, 5) && validDist(p2, 5);
}

enum DIR {
    X, Y, Z
};

enum MOVE {
    SMOVE,
    LMOVE_X_Y,
    LMOVE_Y_Z,
    LMOVE_Z_X,
    INVALID
};

P toSmove(int val, const DIR d) {
    switch (d) {
        case X:
            return {val, 0, 0};
        case Y:
            return {0, val, 0};
        case Z:
            return {0, 0, val};
    }
}

P toSmove(const P &p, const DIR d) {
    switch (d) {
        case X:
            return {p.x, 0, 0};
        case Y:
            return {0, p.y, 0};
        case Z:
            return {0, 0, p.z};
    }
}

P toSmove(const P &p) {
    assert(validSmove(p));
    if (p.x != 0) return toSmove(p.x, X);
    if (p.y != 0) return toSmove(p.y, Y);
    return toSmove(p.z, Z);
}

MOVE checkMove(const P &p) {
    switch ((p.x != 0) + (p.y != 0) + (p.z != 0)) {
        case 1:
            return validSmove(p) ? SMOVE : INVALID;
        case 2:
            if (validLmove(toSmove(p.x, X), toSmove(p.y, Y))) return LMOVE_X_Y;
            if (validLmove(toSmove(p.y, Y), toSmove(p.z, Z))) return LMOVE_Y_Z;
            if (validLmove(toSmove(p.z, Z), toSmove(p.x, X))) return LMOVE_Z_X;
        default:
            return INVALID;
    }
}

pair<P, P> toLmove(const P &p, DIR d1, DIR d2) {
    return make_pair(toSmove(p, d1), toSmove(p, d2));
}

pair<P, P> toLmoveAnyOrder(const P &p) {
    switch (checkMove(p)) {
        case LMOVE_X_Y:
            return make_pair(toSmove(p.x, X), toSmove(p.y, Y));
        case LMOVE_Y_Z:
            return make_pair(toSmove(p.y, Y), toSmove(p.z, Z));
        case LMOVE_Z_X:
            return make_pair(toSmove(p.z, Z), toSmove(p.x, X));
        default:
            assert(false);
    }
}

int vol[RMAX][RMAX][RMAX];
int returnDist[RMAX][RMAX][RMAX];

void fillVolatile(const P &src, const P &dst, int turn) {
    for (int x = min(src.x, dst.x); x <= max(src.x, dst.x); ++x) {
        for (int y = min(src.y, dst.y); y <= max(src.y, dst.y); ++y) {
            for (int z = min(src.z, dst.z); z <= max(src.z, dst.z); ++z) {
                vol[x][y][z] = max(vol[x][y][z], turn + 1);
            }
        }
    }
}

bool failVolatile(const P &src, const P &dst, int turn) {
    for (int x = min(src.x, dst.x); x <= max(src.x, dst.x); ++x) {
        for (int y = min(src.y, dst.y); y <= max(src.y, dst.y); ++y) {
            for (int z = min(src.z, dst.z); z <= max(src.z, dst.z); ++z) {
                if (vol[x][y][z] >= turn) return false;
            }
        }
    }
}

bool in(int x, int y, int z) {
    return 0 <= x && x < R && 0 <= y && y < R && 0 <= z && z < R;
}

bool canEnter(int x, int y, int z, int turn) {
    return in(x, y, z) && vol[x][y][z] < turn;
}

bool canEnter(P p, int turn) {
    return canEnter(p.x, p.y, p.z, turn);
}

const int INF = 1e9;

struct bot {
    P pos;
    int bid;
    vector<int> seeds;
    int seed;
    bool active;

    void halt() {
#ifdef DBG
        cout << bid << ": ";
#endif
        cout << "halt" << '\n';
    }

    void wait(int turn) {
        if (!active) return;
        vol[pos.x][pos.y][pos.z] = max(vol[pos.x][pos.y][pos.z], turn + 2);

#ifdef DBG
        cout << bid << ": ";
#endif
        cout << "wait" << '\n';
    }

    void flip() {
        if (!active) return;
        high = !high;
#ifdef DBG
        cout << bid << ": ";
#endif
        cout << "flip" << '\n';
    }

    void smove(P lld, int turn) {
        if (!active) return;
        assert(validSmove(lld));
        cerr << bid << ':' << pos << "->" << pos + lld << endl;
        assert(canEnter(pos + lld, turn));
        fillVolatile(pos, pos + lld, turn);
        pos = pos + lld;
        vol[pos.x][pos.y][pos.z] = max(vol[pos.x][pos.y][pos.z], turn + 2);
#ifdef DBG
        cout << bid << ": ";
#endif
        cout << "smove " << lld << '\n';
    }

    void lmove(P sld1, P sld2, int turn) {
        if (!active) return;
        assert(validLmove(sld1, sld2));
        cerr << bid << ':' << pos << "->" << pos + sld1 + sld2 << endl;
        assert(canEnter(pos + sld1 + sld2, turn));
        fillVolatile(pos, pos + sld1, turn);
        fillVolatile(pos + sld1, pos + sld1 + sld2, turn);
        pos = pos + sld1 + sld2;
        vol[pos.x][pos.y][pos.z] = max(vol[pos.x][pos.y][pos.z], turn + 2);
#ifdef DBG
        cout << bid << ": ";
#endif
        cout << "lmove " << sld1 << ' ' << sld2 << '\n';
    }

    void fusion(P other, bool p, int turn) {
#ifdef DBG
        cout << bid << ": ";
#endif
        cout << "fusion" << (p ? "p " : "s ") << other - pos << '\n';
        if (p) {
            vol[pos.x][pos.y][pos.z] = max(vol[pos.x][pos.y][pos.z], turn + 2);
        } else {
            active = false;
            vol[pos.x][pos.y][pos.z] = max(vol[pos.x][pos.y][pos.z], turn + 1);
        }
    }

    void fission_manual(P nd, int m) {
#ifdef DBG
        cout << bid << ": ";
#endif
        cout << "fission " << nd << ' ' << m << '\n';
    }

    bot fission(P nd, int m) {
        assert(active);
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
#ifdef DBG
        cout << bid << ": ";
#endif
        cout << "fission " << nd << ' ' << m << '\n';
        return bot({pos + nd, nbid, s, true});
    }

    void fill(P d) {
        if (!active) return;
#ifdef DBG
        cout << bid << ": ";
#endif
        cout << "fill " << d << '\n';
    }

    void gfill(P nd, P fd) {
        assert(active);
#ifdef DBG
        cout << bid << ": ";
#endif
        cout << "gfill " << nd << ' ' << fd << '\n';
    }

    void gvoid(P nd, P fd) {
        assert(active);
#ifdef DBG
        cout << bid << ": ";
#endif
        cout << "gvoid " << nd << ' ' << fd << '\n';
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

vector<int> calcBlockSum(int y, int xs) {
    cerr << "calcBlockSum" << endl;
    vector<int> block(R), sum(R + 1);
    for (int x = 0; x < R; ++x) {
        for (int z = 0; z < R; ++z) {
            int r = z;
            while (r < R - 1 && field[x][y][r] && r - z < 30) ++r;
            ++block[x];
            z = r - 1;
        }
        sum[x + 1] = sum[x] + block[x];
    }
    vector<vector<int>> dp(R + 1, vector<int>(xs + 1, INF)), prv(R + 1, vector<int>(xs + 1, 0));
    dp[0][0] = 0;

    for (int i = 0; i < R; ++i) {
        for (int j = 0; j < xs; ++j) {
            for (int k = i + 1; k <= R; ++k) {
                if (max(dp[i][j], sum[k] - sum[i]) < dp[k][j + 1]) {
                    dp[k][j + 1] = max(dp[i][j], sum[k] - sum[i]);
                    prv[k][j + 1] = i;
                }
            }
        }
    }

    vector<int> res;
    int r = xs, p = R;
    while (r > 0) {
        p = prv[p][r];
        res.push_back(p);
        r--;
    }
    reverse(res.begin(), res.end());
    res.push_back(R);

    return res;
}

vector<int> addNext(vector<queue<P>> &q, int y) {
    cerr << "finding next points" << endl;
    auto range = calcBlockSum(y, SEED / 2);

    for (int x = 0; x < R; ++x) {
        for (int z = 0; z < R; ++z) {
            if (!field[x][y][z]) continue;
            int r = z;
            while (r < R - 1 && field[x][y][r] && r - z < 30) ++r;
            for (int i = 1; i <= SEED / 2; ++i) {
                if (x < range[i]) {
                    q[(i - 1) * 2].push({x, y + 1, z - 1});
                    q[(i - 1) * 2 + 1].push({x, y + 1, r});
                    cerr << "task for " << (i - 1) * 2  << ": " << P{x, y + 1, z - 1} << endl;
                    cerr << "task for " << (i - 1) * 2 + 1  << ": " << P{x, y + 1, r} << endl;
                    z = r - 1;
                    break;
                }
            }
        }
    }
    return range;
}

int dx[] = {0, 1, 0, -1, 0, 0};
int dy[] = {0, 0, 0, 0, -1, 1};
int dz[] = {1, 0, -1, 0, 0, 0};
DIR dM[] = {Z, X, Z, X, Y, Y};

int abs(const P &p) {
    return abs(p.x) + abs(p.y) + abs(p.z);
}

void moveBots(vector<bot> &bots, const vector<P> &dsts, vector<function<bool(bot &)>> reserved, int &turn, bool end = false) {
    set<P> st(dsts.begin(), dsts.end());
    int n = bots.size();
    for (int i = 0; i < n; ++i) {
        if (reserved[i](bots[i])) {
#ifdef DBG
            cerr << "custom action by bot" << bots[i].bid << endl;
#endif
            continue;
        } else if (!bots[i].active) {
#ifdef DBG
            cerr << "deactivated bot: " << i << endl;
#endif
            continue;
        }

        const auto &p = bots[i].pos;
        const auto &dst = dsts[i];
        if (p == dst) {
            bots[i].wait(turn);
            continue;
        }
#ifdef DBG_DETAILED_MOVE
        cerr << "p: " << p << " dst:" << dst << endl;
#endif
        int baseScore = end ? returnDist[p.x][p.y][p.z]: abs(dst - p);
        int scS = baseScore + 10 * end, scL = baseScore + 10 * end;
        P smv = {0, 0, 0};
        pair<P, P> lmv = make_pair(P{0, 0, 0}, P{0, 0, 0});

        for (int d1 = 0; d1 < 6; ++d1) {
            for (int l1 = 1; l1 <= 15; ++l1) {
                int nx = p.x + dx[d1] * l1, ny = p.y + dy[d1] * l1, nz = p.z + dz[d1] * l1;
                if (!canEnter(nx, ny, nz, turn)) break;

                // Evaluate Smove
                int nscS;
                if (end) {
                    nscS = returnDist[nx][ny][nz];
                } else {
                    nscS = abs(dst - P{nx, ny, nz});
                    if (nscS > 0) {
                        for (int d; d < 6; ++d) {
                            int nax = nx + dx[d1], nay = ny + dy[d1], naz = nz + dz[d1];
                            if (!canEnter(nax, nay, naz, turn + 1)) nscS += 1;
                        }
                        if (st.count(P{nx, ny, nz})) nscS += 10;
                    }
                }
#ifdef DBG_DETAILED_MOVE
                cerr << "scS:" << scS << " nscS:" << nscS << endl;
#endif
                if (nscS < scS) {
                    scS = nscS;
                    smv = toSmove(P{nx, ny, nz} - p);
                }

                // Try Lmove
                if (l1 > 5) continue;
                for (int d2 = 0; d2 < 6; ++d2) {
                    if (d1 == d2 || dM[d1] == dM[d2]) continue;
                    for (int l2 = 1; l2 <= 5; ++l2) {
                        int nnx = nx + dx[d2] * l2, nny = ny + dy[d2] * l2, nnz = nz + dz[d2] * l2;
                        if (!canEnter(nnx, nny, nnz, turn - 1)) break;

                        // Evaluate Lmove
                        int nscL;
                        if (end) {
                            nscL = returnDist[nnx][nny][nnz];
                        } else {
                            nscL = abs(dst - P{nnx, nny, nnz});
                            if (nscL > 0) {
                                for (int d; d < 6; ++d) {
                                    int nax = nnx + dx[d1], nay = nny + dy[d1], naz = nnz + dz[d1];
                                    if (!canEnter(nax, nay, naz, turn + 1)) nscL += 1;
                                }
                                if (st.count(P{nnx, nny, nnz})) nscL += 10;
                            }
                        }
#ifdef DBG_DETAILED_MOVE
                        cerr << "scL:" << scL << " nscL:" << nscL << endl;
#endif
                        if (nscL < scL) {
                            scL = nscL;
#ifdef DBG_DETAILED_MOVE
                            cerr << "d1:" << "FRBLDU"[d1] << " d2:" << "FRBLDU"[d2] << endl;
                            cerr << "l1:" << l1 << " l2:" << l2 << endl;
                            cerr << "from:" << p << " to:" << P{nnx, nny, nnz} << " via:" << P{nx, ny, nz} << endl;
#endif
                            lmv = toLmove(P{nnx, nny, nnz} - p, dM[d1], dM[d2]);
                        }
                    }
                }
            }
        }

        if (scS <= scL && validSmove(smv)) bots[i].smove(smv, turn);
        else if (validLmove(lmv.first, lmv.second)) bots[i].lmove(lmv.first, lmv.second, turn);
        else bots[i].wait(turn);
    }
    turn++;
}

const function<bool(bot &)> NOP = [](bot b) { return false; };

void addConectivity(const P &src, const P &dst, UnionFind &uf) {
    for (int x = min(src.x, dst.x); x <= max(src.x, dst.x); ++x) {
        for (int y = min(src.y, dst.y); y <= max(src.y, dst.y); ++y) {
            for (int z = min(src.z, dst.z); z <= max(src.z, dst.z); ++z) {
                placed[x][y][z] = true;
            }
        }
    }
    for (int x = min(src.x, dst.x); x <= max(src.x, dst.x); ++x) {
        for (int y = min(src.y, dst.y); y <= max(src.y, dst.y); ++y) {
            for (int z = min(src.z, dst.z); z <= max(src.z, dst.z); ++z) {
                if (y == 0) {
                    uf.unite(f({x, y, z}), f(P{0, 0, 0}));
                } else {
                    for (int d = 0; d < 6; ++d) {
                        int nx = x + dx[d], ny = y + dy[d], nz = z + dz[d];
                        if (placed[nx][ny][nz]) uf.unite(f({x, y, z}), f(P{nx, ny, nz}));
                    }
                }
            }
        }
    }
}

int main() {
    input();

    UnionFind uf(R * R * R);

    vector<bot> bots(SEED);
    {
        vector<int> seeds(SEED);
        for (int i = 1; i < SEED; ++i) seeds[i] = 1;
        bots[0] = bot{{0, 0, 0}, 0, seeds, SEED, true};
    }

    vector<queue<P>> q(SEED);

    auto range = addNext(q, 0);

    vector<P> dst;
    for (int i = 0; i < SEED / 2; ++i) {
        dst.push_back(P{range[i], rand() % min(R, 20), 0});
        dst.push_back(P{range[i], rand() % min(R, 20), R - 1});
    }

    int turn = 1;

    cerr << "fission start" << endl;
    int activeBots = 1;
    while (activeBots < SEED) {
        cerr << "turn:" << turn << endl;
        auto fission = vector<function<bool(bot &)>>(SEED, NOP);

        for (int i = SEED; i >= 0; --i) {
            if (!bots[i].active) continue;
            cerr << "remaining seed of " << i << ": " << bots[i].seed << endl;
            if (bots[i].seed <= 1) continue;
            auto &p = bots[i].pos;

            P nd;
            for (int i = 0; i < 6; ++i) {
                nd = {dx[i], dy[i], dz[i]};
                if (canEnter(p + nd, turn)) break;
            }

            int m = bots[i].seed > 2 ? bots[i].seed / 2 : 0;
            if (canEnter(p + nd, turn)) {
                fission[i] = [nd, m](bot &b) {
                    cerr << "creating a new bot at " << b.pos << endl;
                    b.fission_manual(nd, m);
                    return true;
                };

                bots[i].seed -= m + 1;
                cerr << "creating..." << activeBots + 1 << "/" << SEED << " with seed " << m << endl;
                cerr << "remaining seed of " << i << ": " << bots[i].seed << endl;
                // creating a new bot
                {
                    int cnt = 0;
                    int nbid = -1;
                    vector<int> s(SEED);
                    for (int j = bots[i].bid + 1; j < SEED && cnt < m + 1; ++j) {
                        if (bots[i].seeds[j]) {
                            bots[i].seeds[j] = 0;
                            if (++cnt == 1) {
                                nbid = j;
                            } else {
                                s[j] = 1;
                            }
                        }
                    }
                    assert(nbid > 0);
                    cerr << "created -> " << nbid << endl;
                    vol[p.x][p.y][p.z] = turn + 2;
                    vol[p.x + nd.x][p.y + nd.y][p.z + nd.z] = turn + 2;
                    bots[nbid] = (bot{p + nd, nbid, s, m + 1, true});
                    fission[nbid] = [](bot &b) {
                        cerr << "bot" << b.bid << " created at " << b.pos << endl;
                        return true;
                    };
                    ++activeBots;
                }
            }
        }

        moveBots(bots, dst, fission, turn);
    }
    cerr << "end fission" << endl;
    cerr << "fission end" << endl;

    int cnt = 0, curY = 1;
    while (cnt < NUM) {
        bool ok = true;
        for (int i = 0; i < SEED; ++i) {
            if (q[i].empty()) {
                dst[i] = P{bots[i].pos.x, curY + 1, bots[i].pos.z};
            } else {
                cerr << "remaining" << endl;
                cerr << bots[i].pos << endl;
                cerr << q[i].front() << endl;
            }
            ok &= q[i].empty();
        }

        if (ok) {
            cerr << "height " << curY << " OK" << endl;
            cerr << "placed " << cnt << "/" << NUM << " so far" << endl;
            range = addNext(q, curY);
            for (int i = 0; i < SEED / 2; ++i) {
                dst.push_back(P{range[i], curY + 1, 0});
                dst.push_back(P{range[i], curY + 1, R - 1});
            }
            curY++;
            continue;
        }
        for (int i = 0; i < SEED; ++i) {
            if (q[i].front() != dst[i]) {
                if (!q[i].empty()) dst[i] = q[i].front();
            }
        }

        auto gfill = vector<function<bool(bot &)>>(SEED, NOP);
        set<int> busy;
        bool flipped = false;
        for (int i = 0; i < SEED / 2; ++i) {
            int a = i * 2, b = i * 2 + 1;
            if (bots[a].pos.y == curY && bots[a].pos == dst[a] && bots[b].pos == dst[b] && busy.size() + 2 < activeBots) {
                P na = {0, -1, 1}, nb = {0, -1, -1};

                if (!failVolatile(bots[b].pos + nb, bots[a].pos + na, turn)) {
                    continue;
                }

                P aedge = bots[a].pos + na;
                P bedge = bots[b].pos + nb;
                if (abs(bedge - aedge) != 0) {
                    cerr << "gfill: " << bedge << " to " <<  aedge << endl;
                    cerr << "gfill: " << bedge - aedge << endl;
                    gfill[a] = [=](bot &botA) {
                        botA.gfill(na, bedge - aedge);
                        return true;
                    };
                    gfill[b] = [=](bot &botB) {
                        botB.gfill(nb, aedge - bedge);
                        return true;
                    };
                    busy.insert(a);
                    busy.insert(b);
                } else {
                    gfill[a] = [=](bot &botA) {
                        botA.fill(na);
                        return true;
                    };
                    busy.insert(a);
                }

                q[a].pop();
                q[b].pop();
                cnt += abs(bedge - aedge) + 1;
                fillVolatile(aedge, bedge, INF);
                addConectivity(bedge, aedge, uf);
            }
        }
        for (int i = 0; i < SEED; ++i) {
            if (busy.count(i)) continue;
            cerr << "uf: " << uf.size(0) << endl;
            cerr << "cnt + 1: " << cnt + 1 << endl;
            if (high && uf.size(0) == cnt + 1 || !high && uf.size(0) < cnt + 1) {
                flipped = true;
                gfill[i] = [](bot &b) {
                    b.flip();
                    return true;
                };
            }
            break;
        }

        moveBots(bots, dst, gfill, turn);
    }


    cerr << "fusion start" << endl;
    // End
    {
        for (int i = 0; i < bots.size(); ++i) {
            dst[i] = {0, 0, 0};
        }
        for (int i = 0; i < R; ++i)
            for (int j = 0; j < R; ++j)
                for (int k = 0; k < R; ++k)
                    returnDist[i][j][k] = 1e9;
        queue<P> q;
        returnDist[0][0][0] = 0;
        q.push({0, 0, 0});
        while (!q.empty()) {
            auto p = q.front();
            q.pop();
            for (int d = 0; d < 6; ++d) {
                int nx = p.x + dx[d], ny = p.y + dy[d], nz = p.z + dz[d];
                if (!in(nx, ny, nz) || returnDist[nx][ny][nz] != 1e9) continue;
                if (vol[nx][ny][nz] > turn + 10) continue;
                returnDist[nx][ny][nz] = returnDist[p.x][p.y][p.z] + 1;
                q.push({nx, ny, nz});
            }
        }
    }

    while (activeBots > 1 || bots[0].pos != P{0, 0, 0}) {
        auto f = vector<function<bool(bot &)>>(bots.size(), NOP);
        for (int i = 0; i < bots.size(); ++i) {
            if (!bots[i].active) continue;
            for (int j = i + 1; j < bots.size(); ++j) {
                if (!bots[j].active) continue;
                if (abs(bots[i].pos - bots[j].pos) == 1) {
                    cerr << "fusion->" << i << '&' << j << endl;
                    cerr << bots[i].pos << ' ' << bots[j].pos << endl;
                    f[i] = [=](bot &b) {
                        b.fusion(bots[j].pos, true, turn);
                        return true;
                    };
                    f[j] = [=](bot &b) {
                        b.fusion(bots[i].pos, false, turn);
                        return true;
                    };
                    bots[j].active = false;
                    --activeBots;
                    break;
                }
            }
        }

        moveBots(bots, dst, f, turn, true);
    }

    bots[0].halt();
    cerr << "fusion end" << endl;

    return 0;
}