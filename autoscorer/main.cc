#include <iostream>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <cassert>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstring>

using namespace std;

template<class T> void loopRange(int end1, int end2, T f) {
    const int from = min(end1, end2);
    const int to = max(end1, end2);
    for (int i = from; i <= to; ++i) {
        f(i);
    }
}

struct DisjointSet {
    vector<int> root;
    vector<int> size;

    DisjointSet(int n) : root(n), size(n, 1) {
        for (int i = 0; i < n; ++i) {
            root[i] = i;
        }
    }

    int get(int i) {
        if (root[i] == i) return i;
        return root[i] = get(root[i]);
    }

    bool unite(int i, int j) {
        const int ri = get(i);
        const int rj = get(j);
        if (ri == rj) {
            return false;
        }
        if (size[ri] > size[rj]) {
            root[rj] = root[ri];
            size[ri] += size[rj];
        } else {
            root[ri] = root[rj];
            size[rj] += size[ri];
        }
        return true;
    }

    bool remove(int i) {
        const int ri = get(i);
        if (i == ri) {
            // Unable to remove this node safely because it's root of a subtree.
            return false;
        }
        size[ri]--;
        root[i] = i;
        size[i] = 1;
        return true;
    }

    void reset() {
        const int N = root.size();
        for (int i = 0; i < N; ++i) {
            root[i] = i;
            size[i] = 1;
        }
    }
};

struct Coord {
    int x, y, z;

    bool isNeardistance() const {
        return mlen() <= 2 && clen() == 1;
    }

    bool isShortDistance() const {
        if (x == 0 && y == 0 && abs(z) <= 5) return true;
        if (x == 0 && abs(y) <= 5 && z == 0) return true;
        if (abs(x) <= 5 && y == 0 && z == 0) return true;
        return false;
    }

    bool isLongDistance() const {
        if (x == 0 && y == 0 && abs(z) <= 15) return true;
        if (x == 0 && abs(y) <= 15 && z == 0) return true;
        if (abs(x) <= 15 && y == 0 && z == 0) return true;
        return false;
    }

    bool isFarDistance() const {
        return 0 < clen() && clen() <= 30;
    }

    int mlen() const {
        return abs(x) + abs(y) + abs(z);
    }

    int clen() const {
        return max(abs(x), max(abs(y), abs(z)));
    }

    friend ostream& operator <<(ostream &os, const Coord &c) {
        return os << "(" << c.x << ", " << c.y << ", " << c.z << ")";
    }
};

struct CoordHasher {
    size_t operator()(const Coord &c) const {
        return c.x * 250 * 250 + c.y * 250 + c.z;
    }
};

Coord operator+(const Coord &c1, const Coord &c2) {
    return Coord{c1.x + c2.x, c1.y + c2.y, c1.z + c2.z};
}

Coord operator-(const Coord &c1, const Coord &c2) {
    return Coord{c1.x - c2.x, c1.y - c2.y, c1.z - c2.z};
}

bool operator ==(const Coord &c1, const Coord &c2) {
    return c1.x == c2.x && c1.y == c2.y && c1.z == c2.z;
}

bool operator !=(const Coord &c1, const Coord &c2) {
    return !(c1 == c2);
}

bool inRange(int a, int x, int b) {
    if (a > b) return inRange(b, x, a);
    return a <= x && x <= b;
}

struct Region {
    Coord lb, rt;

    vector <Coord> corners() const {
        return vector < Coord > {
                lb,
                Coord{lb.x, lb.y, rt.z},
                Coord{lb.x, rt.y, lb.z},
                Coord{lb.x, rt.y, rt.z},
                Coord{rt.x, lb.y, lb.z},
                Coord{rt.x, lb.y, rt.z},
                Coord{rt.x, rt.y, lb.z},
                rt,
        };
    }

    bool contains(const Coord &p) const {
        return inRange(lb.x, p.x, rt.x) && inRange(lb.y, p.y, rt.y) && inRange(lb.z, p.z, rt.z);
    }

    bool intersects(const Region &r) const {
        for (auto c : r.corners()) {
            if (contains(c)) return true;
        }
        for (auto c : corners()) {
            if (r.contains(c)) return true;
        }
        return false;
    }

    Region canonical() const {
        return Region {
            Coord { min(lb.x, rt.x), min(lb.y, rt.y), min(lb.z, rt.z) },
            Coord { max(lb.x, rt.x), max(lb.y, rt.y), max(lb.z, rt.z) }
        };
    }

    int dim() const {
        int res = 0;
        if (lb.x != rt.x) ++res;
        if (lb.y != rt.y) ++res;
        if (lb.z != rt.z) ++res;
        return res;
    }
};

struct RegionHasher {
    size_t operator()(const Region &r) const {
        size_t res = r.lb.x;
        res = res * 250 + r.lb.y;
        res = res * 250 + r.lb.z;
        res = res * 250 + r.rt.x;
        res = res * 250 + r.rt.y;
        res = res * 250 + r.rt.z;
        return res;
    }
};

bool operator ==(const Region &r1, const Region &r2) {
    const Region rc1 = r1.canonical();
    const Region rc2 = r2.canonical();
    return rc1.lb == rc2.lb && rc1.rt == rc2.rt;
}

enum Harmonics {
    HIGH,
    LOW,
};

struct NanoBot {
    int id;
    set<int> seeds;
    Coord position;
};

struct Model {
    int size;
    bool field[250][250][250];

    Model(ifstream& in) {
        unsigned char b1;
        in.read(reinterpret_cast<char*>(&b1), sizeof(b1));
        size = b1;

        const int bytes = (size * size * size + 7) / 8;
        int x = 0, y = 0, z = 0;
        for (int i = 0; i < bytes; i++) {
           in.read(reinterpret_cast<char*>(&b1), 1);
            for (int j = 0; j < 8 && x < size; j++) {
                field[x][y][z] = (b1 >> j) & 1;
	            z++;
	            if (z == size) z = 0, y++;
	            if (y == size) y = 0, x++;
            }
        }
    }

    Model(int size): size(size) {
        for (int i = 0; i < 250; ++i) {
            for (int j = 0; j < 250; ++j) {
                for (int k = 0; k < 250; ++k) {
                    field[i][j][k] = false;
                }
            }
        }
    }
};

struct State {
    map<int, NanoBot> bots;
    bool field[250][250][250];
    long long energy;
    Harmonics harmonics;
    int r;
    DisjointSet ds;
    int ground;
    int filled;
    Model* sourceModel;
    Model* targetModel;
    bool needReground;
    bool skipSanityCheck;

    State(Model* sourceModel, Model* targetModel, bool skipSanityCheck)
        : energy(0), harmonics(LOW), r(sourceModel->size), ds(r * r * r + 1), ground(r * r * r), filled(0),
          sourceModel(sourceModel), targetModel(targetModel), needReground(false), skipSanityCheck(false) {
        for (int i = 0; i < 250; ++i) {
            for (int j = 0; j < 250; ++j) {
                for (int k = 0; k < 250; ++k) {
                    field[i][j][k] = false;
                }
            }
        }
        for (int i = 0; i < r; ++i) {
            for (int j = 0; j < r; ++j) {
                for (int k = 0; k < r; ++k) {
                    if (sourceModel->field[i][j][k]) {
                        fill(Coord{i, j, k});
                    }
                }
            }
        }
        set<int> seeds;
        for (int i = 2; i <= 40; ++i) {
            seeds.insert(i);
        }
        bots[1] = NanoBot{1, seeds};
    }

    const NanoBot &bot(int i) const {
        auto it = bots.find(i);
        assert(it != bots.end());
        return it->second;
    }

    NanoBot &bot(int i) {
        auto it = bots.find(i);
        assert(it != bots.end());
        return it->second;
    }

    int botCount() const {
        return bots.size();
    }

    bool botExistsAt(const Coord &p) const {
        for (auto entry : bots) {
            if (entry.second.position == p) {
                return true;
            }
        }
        return false;
    }

    NanoBot &botAt(const Coord &p) {
        for (auto entry : bots) {
            if (entry.second.position == p) {
                return bots[entry.first];
            }
        }
        assert(false);
    }

    void addBot(const NanoBot &bot) {
        assert(bots.find(bot.id) == bots.end());
        bots[bot.id] = bot;
    }

    void removeBot(int botId) {
        bots.erase(botId);
    }

    void clearBots() {
        bots.clear();
    }

    bool isValidPoint(const Coord &p) const {
        if (p.x < 0 || r <= p.x) return false;
        if (p.y < 0 || r <= p.y) return false;
        if (p.z < 0 || r <= p.z) return false;
        return true;
    }

    bool isFilled(const Coord &p) const {
        assert(isValidPoint(p));
        return field[p.x][p.y][p.z];
    }

    int hashCoord(const Coord &p) {
        return p.x * r * r + p.y * r + p.z;
    }

    void connect(const Coord &p1, const Coord &p2) {
        ds.unite(hashCoord(p1), hashCoord(p2));
    }

    void connectToGround(const Coord &p1) {
        ds.unite(hashCoord(p1), ground);
    }

    void fill(const Coord &p, bool isFill = true) {
        assert(isValidPoint(p));
        if (isFill) {
            if (field[p.x][p.y][p.z] == false) {
                field[p.x][p.y][p.z] = true;
                filled++;
            }

            for (auto c : {Coord{1, 0, 0}, Coord{-1, 0, 0}, Coord{0, 1, 0}, Coord{0, -1, 0}, Coord{0, 0, 1}, Coord{0, 0, -1}}) {
                Coord q = p + c;
                if (isValidPoint(q) && field[q.x][q.y][q.z]) {
                    connect(p, q);
                }
            }
            if (p.y == 0) {
                connectToGround(p);
            }
        } else {
            field[p.x][p.y][p.z] = false;
            --filled;
            if (!skipSanityCheck && (!ds.remove(hashCoord(p)) || !stillConnected(p))) {
                needReground = true;
            }
        }
    }

    bool stillConnected(const Coord &p) {
        static deque<Coord> q;
        static unordered_set<Coord, CoordHasher> targets, visited;
        q.clear();
        targets.clear();
        visited.clear();

        for (auto c : {Coord{1, 0, 0}, Coord{-1, 0, 0}, Coord{0, 1, 0}, Coord{0, -1, 0}, Coord{0, 0, 1}, Coord{0, 0, -1}}) {
            const Coord neighbor = p + c;
            if (isValidPoint(neighbor) && isFilled(neighbor)) {
                targets.insert(neighbor);
            }
        }
        if (targets.size() <= 1) return true;

        q.push_back(*targets.begin());
        visited.insert(*targets.begin());
        targets.erase(targets.begin());
        while (!q.empty()) {
            const Coord cur = q.front();
            q.pop_front();

            if (targets.size() == 0) {
                return true;
            }
            for (auto c : {Coord{1, 0, 0}, Coord{-1, 0, 0}, Coord{0, 1, 0}, Coord{0, -1, 0}, Coord{0, 0, 1}, Coord{0, 0, -1}}) {
                const Coord next = cur + c;
                if (isValidPoint(next) && isFilled(next) && visited.find(next) == visited.end()) {
                    visited.insert(next);
                    auto it = targets.find(next);
                    if (it != targets.end()) {
                        targets.erase(it);
                    }
                    q.push_back(next);
                }
            }
        }
        return false;
    }

    void reground() {
        ds.reset();
        filled = 0;
        for (int x = 0; x < r; ++x) {
            for (int y = 0; y < r; ++y) {
                for (int z = 0; z < r; ++z) {
                    const Coord p{x, y, z};
                    if (field[x][y][z]) {
                        ++filled;
                        if (y == 0) {
                            connectToGround(p);
                        }
                    }
                    for (auto c : {Coord{1, 0, 0}, Coord{-1, 0, 0}, Coord{0, 1, 0}, Coord{0, -1, 0}, Coord{0, 0, 1}, Coord{0, 0, -1}}) {
                        const Coord q = p + c;
                        if (isValidPoint(q) && field[q.x][q.y][q.z]) {
                            connect(p, q);
                        }
                    }
                }
            }
        }
        needReground = false;
    }

    void checkMove(const Coord &coord, const Coord &d) const {
        assert(d.isShortDistance() || d.isLongDistance());
        for (int dx = 0; dx <= d.x; ++dx) {
            for (int dy = 0; dy <= d.y; ++dy) {
                for (int dz = 0; dz <= d.z; ++dz) {
                    assert(!field[coord.x + dx][coord.y + dy][coord.z + dz]);
                }
            }
        }

        for (int dx = 0; dx >= d.x; --dx) {
            for (int dy = 0; dy >= d.y; --dy) {
                for (int dz = 0; dz >= d.z; --dz) {
                    assert(!field[coord.x + dx][coord.y + dy][coord.z + dz]);
                }
            }
        }
    }

    void checkPhysics() {
        assert(ds.size[ds.get(ground)] == filled + 1);
    }

    void checkFinalState() {
        assert(botCount() == 0);
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < r; j++) {
                for (int k = 0; k < r; k++) {
                    assert(field[i][j][k] == targetModel->field[i][j][k]);
                }
            }
        }
    }
};

struct Command {
    virtual void checkPrecondition(const State &state, int botId) = 0;

    virtual vector<Region> volatileRegions(const State &state, int botId) = 0;

    virtual int fusionTarget(const State &state, int botId) {
        return -1;
    }

    virtual void updateState(State &state, int botId) = 0;

    virtual ostream& print(ostream &os) const = 0;

    friend ostream& operator <<(ostream &os, const Command &command) {
        return command.print(os);
    }
};

struct HaltCommand : public Command {
    virtual void checkPrecondition(const State &state, int botId) {
        const Coord zero = Coord{0, 0, 0};
        assert(state.bot(botId).position == zero);
        assert(state.botCount() == 1);
        assert(state.harmonics == LOW);
    }

    virtual vector <Region> volatileRegions(const State &state, int botId) {
        return vector < Region > {
                Region{state.bot(botId).position, state.bot(botId).position}
        };
    }

    virtual void updateState(State &state, int botId) {
        state.removeBot(botId);
    }

    virtual ostream& print(ostream &os) const {
        return os << "<HaltCommand>";
    }
};

struct WaitCommand : public Command {
    virtual void checkPrecondition(const State &state, int botId) {
    }

    virtual vector <Region> volatileRegions(const State &state, int botId) {
        return vector < Region > {
                Region{state.bot(botId).position, state.bot(botId).position}
        };
    }

    virtual void updateState(State &state, int botId) {
    }

    virtual ostream& print(ostream &os) const {
        return os << "<WaitCommand>";
    }
};

struct FlipCommand : public Command {
    virtual void checkPrecondition(const State &state, int botId) {
    }

    virtual vector <Region> volatileRegions(const State &state, int botId) {
        return vector <Region> {
                Region{state.bot(botId).position, state.bot(botId).position}
        };
    }

    virtual void updateState(State &state, int botId) {
        if (state.harmonics == HIGH) {
            state.harmonics = LOW;
        } else {
            state.harmonics = HIGH;
        }
    }

    virtual ostream& print(ostream &os) const {
        return os << "<FlipCommand>";
    }
};

struct SMoveCommand : public Command {
    Coord d;

    SMoveCommand(const Coord &d) : d(d) {
        assert(d.isLongDistance());
    }

    virtual void checkPrecondition(const State &state, int botId) {
        const Coord next = state.bot(botId).position + d;
        assert(state.isValidPoint(next));
        state.checkMove(state.bot(botId).position, d);
    }

    virtual vector<Region> volatileRegions(const State &state, int botId) {
        const Coord next = state.bot(botId).position + d;
        return vector<Region> { Region{state.bot(botId).position, next} };
    }

    virtual void updateState(State &state, int botId) {
        const Coord next = state.bot(botId).position + d;
        state.bot(botId).position = next;
        state.energy += 2 * d.mlen();
    }

    virtual ostream& print(ostream &os) const {
        return os << "<SMoveCommand d=" << d << ">";
    }
};

struct LMoveCommand : public Command {
    Coord d1, d2;

    LMoveCommand(const Coord &d1, const Coord &d2) : d1(d1), d2(d2) {
        assert(d1.isShortDistance());
        assert(d2.isShortDistance());
    }

    virtual void checkPrecondition(const State &state, int botId) {
        const Coord next = state.bot(botId).position + d1;
        assert(state.isValidPoint(next));
        assert(!state.isFilled(next));
        state.checkMove(state.bot(botId).position, d1);

        const Coord nn = next + d2;
        assert(state.isValidPoint(nn));
        assert(!state.isFilled(nn));
        state.checkMove(next, d2);
    }

    virtual vector <Region> volatileRegions(const State &state, int botId) {
        const Coord next = state.bot(botId).position + d1;
        const Coord nn = next + d2;
        return vector < Region > {
                Region{state.bot(botId).position, next},
                Region{next, nn}
        };
    }

    virtual void updateState(State &state, int botId) {
        const Coord next = state.bot(botId).position + d1;
        const Coord nn = next + d2;
        state.bot(botId).position = nn;
        state.energy += 2 * (d1.mlen() + 2 + d2.mlen());
    }

    virtual ostream& print(ostream &os) const {
        return os << "<LMoveCommand d1=" << d1 << ", d2=" << d2 << ">";
    }
};

struct FissionCommand : public Command {
    Coord d;
    int m;

    FissionCommand(const Coord &d, int m) : d(d), m(m) {
        assert(d.isNeardistance());
    }

    virtual void checkPrecondition(const State &state, int botId) {
        const NanoBot &bot = state.bot(botId);
        assert(bot.seeds.size() > 0);
        const Coord next = bot.position + d;
        assert(state.isValidPoint(next));
        assert(!state.isFilled(next));
        assert(m + 1 <= bot.seeds.size());
    }

    virtual vector <Region> volatileRegions(const State &state, int botId) {
        const NanoBot &bot = state.bot(botId);
        const Coord next = bot.position + d;
        return vector <Region> {
                Region{bot.position, bot.position},
                Region{next, next}
        };
    }

    virtual void updateState(State &state, int botId) {
        NanoBot &bot = state.bot(botId);
        auto it = bot.seeds.begin();
        int newId = *it;
        it = bot.seeds.erase(it);

        set<int> newSeeds;
        for (int i = 0; i < m; ++i) {
            newSeeds.insert(*it);
            it = bot.seeds.erase(it);
        }

        state.addBot(NanoBot{newId, newSeeds, bot.position + d});
        state.energy += 24;
    }

    virtual ostream& print(ostream &os) const {
        return os << "<FissionCommand d=" << d << ", m=" << m << ">";
    }
};

struct FillCommand : public Command {
    Coord d;
    bool isFill;

    FillCommand(const Coord &d, bool isFill) : d(d), isFill(isFill) {
        assert(d.isNeardistance());
    }

    virtual void checkPrecondition(const State &state, int botId) {
        const NanoBot &bot = state.bot(botId);
        const Coord next = bot.position + d;
        assert(state.isValidPoint(next));
    }

    virtual vector <Region> volatileRegions(const State &state, int botId) {
        const NanoBot &bot = state.bot(botId);
        const Coord next = bot.position + d;
        return vector < Region > {
                Region{bot.position, bot.position},
                Region{next, next}
        };
    }

    virtual void updateState(State &state, int botId) {
        const NanoBot &bot = state.bot(botId);
        const Coord next = bot.position + d;
        if (state.isFilled(next)) {
            if (isFill) {
                state.energy += 6;
            } else {
                state.fill(next, false);
                state.energy -= 12;
            }
        } else {
            if (isFill) {
                state.fill(next);
                state.energy += 12;
            } else {
                state.energy += 3;
            }
        }
    }

    virtual ostream& print(ostream &os) const {
        return os << "<" << (isFill ? "Fill" : "Void") << "Command d=" << d << ">";
    }
};

struct FusionPCommand : public Command {
    Coord d;

    FusionPCommand(const Coord &d) : d(d) {
        assert(d.isNeardistance());
    }

    virtual void checkPrecondition(const State &state, int botId) {
        const NanoBot &bot = state.bot(botId);
        const Coord next = bot.position + d;
        assert(state.isValidPoint(next));
        assert(state.botExistsAt(next));
    }

    virtual vector <Region> volatileRegions(const State &state, int botId) {
        const NanoBot &bot = state.bot(botId);
        const Coord next = bot.position + d;
        return vector < Region > {
                Region{bot.position, bot.position},
                Region{next, next}
        };
    }

    virtual int fusionTarget(State &state, int botId) {
        const NanoBot &bot = state.bot(botId);
        const Coord next = bot.position + d;
        return state.botAt(next).id;
    }

    virtual void updateState(State &state, int botId) {
        NanoBot &bot = state.bot(botId);
        const Coord next = bot.position + d;
        const NanoBot &target = state.botAt(next);

        bot.seeds.insert(target.seeds.begin(), target.seeds.end());
        bot.seeds.insert(target.id);
        state.removeBot(target.id);

        state.energy -= 24;
    }

    virtual ostream& print(ostream &os) const {
        return os << "<FusionPCommand d=" << d << ">";
    }
};

struct FusionSCommand : public Command {
    Coord d;

    FusionSCommand(const Coord &d) : d(d) {
        assert(d.isNeardistance());
    }

    virtual void checkPrecondition(const State &state, int botId) {
        const NanoBot &bot = state.bot(botId);
        const Coord next = bot.position + d;
        assert(state.isValidPoint(next));
        assert(state.botExistsAt(next));
    }

    virtual vector <Region> volatileRegions(const State &state, int botId) {
        // Delegating to FusionPCommand
        return vector < Region > {};
    }

    virtual int fusionTarget(State &state, int botId) {
        const NanoBot &bot = state.bot(botId);
        const Coord next = bot.position + d;
        return state.botAt(next).id;
    }

    virtual void updateState(State &state, int botId) {
        // Delegating to FusionPCommand. Do nothing.
    }

    virtual ostream& print(ostream &os) const {
        return os << "<FusionSCommand d=" << d << ">";
    }
};

struct GFillCommand : public Command {
    Coord nd, fd;
    bool isFill;

    GFillCommand(const Coord &nd, const Coord &fd, bool isFill) : nd(nd), fd(fd), isFill(isFill) {
        assert(nd.isNeardistance());
        assert(fd.isFarDistance());
    }

    virtual void checkPrecondition(const State &state, int botId) {
        const NanoBot &bot = state.bot(botId);
        const Coord c1 = bot.position + nd;
        const Coord c2 = c1 + fd;
        assert(state.isValidPoint(c1));
        assert(state.isValidPoint(c2));
        const Region fillRegion{ c1, c2 };
        assert(!fillRegion.contains(bot.position));
    }

    virtual vector <Region> volatileRegions(const State &state, int botId) {
        const NanoBot &bot = state.bot(botId);
        const Coord c1 = bot.position + nd;
        const Coord c2 = c1 + fd;
        return vector < Region > {
            Region{bot.position, bot.position},
            Region{c1, c2}
        };
    }

    virtual void updateState(State &state, int botId) {
        const NanoBot &bot = state.bot(botId);
        const Coord c1 = bot.position + nd;
        const Coord c2 = c1 + fd;

        loopRange(c1.x, c2.x, [&](int x) {
            loopRange(c1.y, c2.y, [&](int y) {
                loopRange(c1.z, c2.z, [&](int z) {
                    const Coord c{x, y, z};
                    if (state.isFilled(c)) {
                        if (isFill) {
                            state.energy += 6;
                        } else {
                            state.fill(c, false);
                            state.energy -= 12;
                        }
                    } else {
                        if (isFill) {
                            state.fill(c, true);
                            state.energy += 12;
                        } else {
                            state.energy += 3;
                        }
                    }
                });
            });
        });
    }

    virtual ostream& print(ostream &os) const {
        return os << "<" << (isFill ? "GFill" : "GVoid") << "Command nd=" << nd << ", fd=" << fd << ">";
    }

    Region targetRegion(State &state, int botId) const {
        const NanoBot &bot = state.bot(botId);
        const Coord c1 = bot.position + nd;
        const Coord c2 = c1 + fd;
        return Region{c1, c2}.canonical();
    }
};

struct CommandGroup {
    vector<pair<int, Command*>> components;

    CommandGroup() : components()  {}

    vector<Region> volatileRegions(State &state) const {
        vector<Region> regions;
        for (auto component : components) {
            for (auto region : component.second->volatileRegions(state, component.first)) {
                regions.push_back(region);
            }
        }
        return regions;
    }

    void updateState(State &state) const {
        components[0].second->updateState(state, components[0].first);
    }
};

void runStep(State &state, deque<Command*> &commands, bool skipSanityCheck = false) {
    vector <Region> volatileRegions;
    map<int, Command*> commandMap;
    unordered_map<Region, CommandGroup*, RegionHasher> gfillGroups;

    if (state.harmonics == LOW) {
        state.energy += 3 * state.r * state.r * state.r;
    } else {
        state.energy += 30 * state.r * state.r * state.r;
    }
    state.energy += 20 * state.botCount();

    auto it = commands.begin();
    for (auto botEntry : state.bots) {
        Command *command = *it++;
        const NanoBot &bot = botEntry.second;
        const GFillCommand *gfillCommand = dynamic_cast<GFillCommand*>(command);

        if (!skipSanityCheck) {
            command->checkPrecondition(state, bot.id);
        }

        const vector<Region> newRegions = command->volatileRegions(state, bot.id);
        // Volatile region of GFill command is checked later
        if (!gfillCommand) {
            for (auto region : newRegions) {
                for (auto r : volatileRegions) {
                    assert(!region.intersects(r) && !r.intersects(region));
                }
            }
            for (auto region : newRegions) {
                volatileRegions.push_back(region);
            }
        }

        if (gfillCommand) {
            const Region targetRegion = gfillCommand->targetRegion(state, bot.id);
            CommandGroup *group = gfillGroups[targetRegion];
            if (group == nullptr) {
                group = gfillGroups[targetRegion] = new CommandGroup();
            }
            group->components.emplace_back(bot.id, command);
        } else {
            commandMap[bot.id] = command;
        }
    }

    // Check GFill sanity
    for (auto entry : gfillGroups) {
        const Region targetRegion = entry.first;
        const CommandGroup *group = entry.second;
        assert(group->components.size() >= (1 << targetRegion.dim()));

        // All bots must support different corners of region
        for (auto component : group->components) {
            const GFillCommand *gfillCommand1 = dynamic_cast<GFillCommand*>(component.second);
            assert(gfillCommand1 != nullptr);
            const Coord corner1 = state.bot(component.first).position + gfillCommand1->nd;
            for (auto other : group->components) {
                if (component == other) break; // Shortcut
                const GFillCommand *gfillCommand2 = dynamic_cast<GFillCommand*>(other.second);
                assert(gfillCommand2 != nullptr);
                const Coord corner2 = state.bot(other.first).position + gfillCommand2->nd;
                assert(corner1 != corner2);
            }
        }

        // Check volatile regions with GFill
        const vector<Region> newRegions = group->volatileRegions(state);
        for (auto region : newRegions) {
            for (auto r : volatileRegions) {
                assert(!region.intersects(r) && !r.intersects(region));
            }
        }
        for (auto region: newRegions) {
            volatileRegions.push_back(region);
        }
    }

    // Run GFill
    for (auto entry : gfillGroups) {
        const CommandGroup *group = entry.second;
        group->updateState(state);
    }

    // Run normal commands
    for (auto commandEntry : commandMap) {
        if (dynamic_cast<FusionSCommand*>(commandEntry.second)) {
            // state.bot() would fail if coupled FusionPCommand is processed earlier. Skip here to avoid interference.
            // Anyways sanity of fusion is fully verified by FusionPCommand.
            continue;
        }

        const NanoBot &bot = state.bot(commandEntry.first);
        Command *command = commandEntry.second;
        const int targetId = commandEntry.second->fusionTarget(state, bot.id);
        if (targetId > 0) {
            assert(commandMap[targetId]->fusionTarget(state, targetId) == bot.id);
        }

        command->updateState(state, bot.id);
    }
    commands.erase(commands.begin(), it);

    if (skipSanityCheck) {
        return;
    }

    // Recalculate ground state because it's dirty now due to Void commands
    if (state.needReground) {
        //cout << "need" << endl;
        state.reground();
    }

    // If the harmonics is Low, then all Full voxels of the matrix are grounded.
    if (state.harmonics == LOW) {
        state.checkPhysics();
    }

    // TODO: Check following sanity
    // - The seeds of each active nanobot are disjoint.
    // - The seeds of each active nanobot does not include the identifier of any active nanobot.
    // - The position of each active nanobot is distinct
    for (auto botEntry : state.bots) {
        // Each active nanobot has a different identifier.
        assert(botEntry.first == botEntry.second.id);

        // The position of each active nanobot is Void in the matrix.
        assert(!state.isFilled(botEntry.second.position));
    }
    //cout << state.energy << endl;
    //cout << state.filled << endl;
}

void run(Model* sourceModel, Model* targetModel, deque<Command *> &commands, bool skipSanityCheck = false) {
    State *state = new State(sourceModel, targetModel, skipSanityCheck);
    while (state->botCount() > 0) {
        runStep(*state, commands, skipSanityCheck);
    }
    if (!skipSanityCheck) {
        assert(commands.empty());
        state->checkFinalState();
    }
    
    cout << state->energy << endl;
}

Coord decodeNearDistance(int i) {
    return Coord {
        i / 9 - 1,
        i / 3 % 3 - 1,
        i % 3 - 1
    };
}

Coord decodeShortDistance(int a, int i) {
    if (a == 0b01) {
       return Coord{i - 5, 0, 0};
   } else if (a == 0b10) {
       return Coord{0, i - 5, 0};
   } else if (a == 0b11) {
       return Coord{0, 0, i - 5};
   }
   assert(false);
}

Coord decodeLongDistance(int a, int i) {
   if (a == 0b01) {
       return Coord{i - 15, 0, 0};
   } else if (a == 0b10) {
       return Coord{0, i - 15, 0};
   } else if (a == 0b11) {
       return Coord{0, 0, i - 15};
   }
   assert(false);
}

Coord decodeFarDistance(int x, int y, int z) {
    return Coord{x - 30, y - 30, z - 30};
}

deque<Command*> compile(const char *filename) {
    ifstream in(filename);
    deque<Command*> commands;
    while(!in.eof()) {
        unsigned char b1;
        in.read(reinterpret_cast<char*>(&b1), sizeof(b1));
        if (in.eof()) break;
        if (b1 == 0b11111111) {
            commands.push_back(new HaltCommand());
        } else if (b1 == 0b11111110) {
            commands.push_back(new WaitCommand());
        } else if (b1 == 0b11111101) {
            commands.push_back(new FlipCommand());
        } else if ((b1 & 0x0f) == 0b0100) {
            int a = (b1 & 0x30) >> 4;
            unsigned char b2;
            in.read(reinterpret_cast<char*>(&b2), sizeof(b2));
            int i = (b2 & 0x1f);
            commands.push_back(new SMoveCommand(decodeLongDistance(a, i)));
        } else if ((b1 & 0x0f) == 0b1100) {
            int a1 = (b1 & 0x30) >> 4;
            int a2 = (b1 & 0xc0) >> 6;
            unsigned char b2;
            in.read(reinterpret_cast<char*>(&b2), sizeof(b2));
            int i1 = (b2 & 0x0f);
            int i2 = (b2 & 0xf0) >> 4;
            commands.push_back(new LMoveCommand(decodeShortDistance(a1, i1), decodeShortDistance(a2, i2)));
        } else if ((b1 & 0x07) == 0b111) {
            commands.push_back(new FusionPCommand(decodeNearDistance(b1 >> 3)));
        } else if ((b1 & 0x07) == 0b110) {
            commands.push_back(new FusionSCommand(decodeNearDistance(b1 >> 3)));
        } else if ((b1 & 0x07) == 0b101) {
            unsigned char b2;
            in.read(reinterpret_cast<char*>(&b2), sizeof(b2));
            commands.push_back(new FissionCommand(decodeNearDistance(b1 >> 3), b2));
        } else if ((b1 & 0x07) == 0b011) {
            commands.push_back(new FillCommand(decodeNearDistance(b1 >> 3), true));
        } else if ((b1 & 0x07) == 0b010) {
            commands.push_back(new FillCommand(decodeNearDistance(b1 >> 3), false));
        } else if ((b1 & 0x07) == 0b001) {
            unsigned char b2, b3, b4;
            in.read(reinterpret_cast<char*>(&b2), sizeof(b2));
            in.read(reinterpret_cast<char*>(&b3), sizeof(b3));
            in.read(reinterpret_cast<char*>(&b4), sizeof(b4));
            commands.push_back(new GFillCommand(decodeNearDistance(b1 >> 3), decodeFarDistance(b2, b3, b4), true));
        } else if ((b1 & 0x07) == 0b000) {
            unsigned char b2, b3, b4;
            in.read(reinterpret_cast<char*>(&b2), sizeof(b2));
            in.read(reinterpret_cast<char*>(&b3), sizeof(b3));
            in.read(reinterpret_cast<char*>(&b4), sizeof(b4));
            commands.push_back(new GFillCommand(decodeNearDistance(b1 >> 3), decodeFarDistance(b2, b3, b4), false));
        }
        //cout << *commands.back() << endl;
    }
    return commands;
}

enum RunMode {
    ASSEMBLY,
    DISASSEMBLY,
    REASSEMBLY,
};

struct Options {
    RunMode runMode;
    bool skipSanityCheck;

    int argc;
    char **args;

    Options() : runMode(ASSEMBLY), argc(0), args(nullptr), skipSanityCheck(false) {}
};

Options getOptions(int argc, char **argv) {
    Options options;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--disassembly") == 0) {
            options.runMode = DISASSEMBLY;
        } else if (strcmp(argv[i], "--reassembly") == 0) {
            options.runMode = REASSEMBLY;
        } else if (strcmp(argv[i], "--aperture-science-dangerously-skip-sanity-check") == 0) {
            options.skipSanityCheck = true;
        } else {
            options.argc = argc - i;
            options.args = argv + i;
            break;
        }
    }

    return options;
}

int main(int argc, char **argv) {
    const Options options = getOptions(argc, argv);

    if (options.argc < 2) {
        cerr << "Usage: ./autoscorer [--disassembly] [--reassembly] MDL_FILE(s) NBT_FILE" << endl;
        return 1;
    }
    cerr << "Reading Model..." << endl;
    Model* sourceModel;
    Model* targetModel;
    char **args = options.args;
    if (options.runMode == ASSEMBLY) {
        ifstream in(*args++);
        targetModel = new Model(in);
        sourceModel = new Model(targetModel->size);
    } else if (options.runMode == DISASSEMBLY) {
        ifstream in(*args++);
        sourceModel = new Model(in);
        targetModel = new Model(sourceModel->size);
    } else if (options.runMode == REASSEMBLY) {
        ifstream in1(*args++);
        sourceModel = new Model(in1);
        ifstream in2(*args++);
        targetModel = new Model(in2);
    }
    assert(sourceModel);
    assert(targetModel);

    cerr << "Compiling..." << endl;
    deque <Command*> commands = compile(*args++);
    /*
    for (auto command : commands) {
        cout << (*command) << endl;
    }
     */
    cerr << "Running..." << endl;
    run(sourceModel, targetModel, commands, options.skipSanityCheck);
    return 0;
}
