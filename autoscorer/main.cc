#include <iostream>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <cassert>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

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

Coord operator+(const Coord &c1, const Coord &c2) {
    return Coord{c1.x + c2.x, c1.y + c2.y, c1.z + c2.z};
}

Coord operator-(const Coord &c1, const Coord &c2) {
    return Coord{c1.x - c2.x, c1.y - c2.y, c1.z - c2.z};
}

bool operator ==(const Coord &c1, const Coord &c2) {
    return c1.x == c2.x && c1.y == c2.y && c1.z == c2.z;
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
};

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
    Model* model;

    State(Model* model) : energy(0), harmonics(LOW), r(model->size), ds(r * r * r + 1), ground(r * r * r), filled(0), model(model) {
        for (int i = 0; i < 250; ++i) {
            for (int j = 0; j < 250; ++j) {
                for (int k = 0; k < 250; ++k) {
                    field[i][j][k] = false;
                }
            }
        }
        set<int> seeds;
        for (int i = 2; i <= 20; ++i) {
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

    void fill(const Coord &p) {
        assert(isValidPoint(p));
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
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < r; j++) {
                for (int k = 0; k < r; k++) {
                    assert(field[i][j][k] == model->field[i][j][k]);
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
        state.clearBots();
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
        assert(d1.isLongDistance());
        assert(d2.isLongDistance());
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
        assert(d.isShortDistance());
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

    FillCommand(const Coord &d) : d(d) {
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
            state.energy += 6;
        } else {
            state.fill(next);
            state.energy += 12;
        }
    }

    virtual ostream& print(ostream &os) const {
        return os << "<FillCommand d=" << d << ">";
    }
};

struct FusionPCommand : public Command {
    Coord d;

    FusionPCommand(const Coord &d) : d(d) {
        assert(d.isShortDistance());
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
        assert(d.isShortDistance());
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

void runStep(State &state, deque<Command*> &commands) {
    vector <Region> volatileRegions;
    map<int, Command*> commandMap;

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

        command->checkPrecondition(state, bot.id);
        const vector<Region> newRegions = command->volatileRegions(state, bot.id);
        for (auto region : newRegions) {
            for (auto r : volatileRegions) {
                assert(!region.intersects(r) && !r.intersects(region));
            }
        }
        for (auto region : newRegions) {
            volatileRegions.push_back(region);
        }
        commandMap[bot.id] = command;
    }

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
}

void run(Model* model, deque<Command *> &commands) {
    State *state = new State(model);
    while (state->botCount() > 0) {
        runStep(*state, commands);
    }
    state->checkFinalState();
    
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

deque<Command*> compile(const string filename) {
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
            commands.push_back(new FillCommand(decodeNearDistance(b1 >> 3)));
        }
        //cout << *commands.back() << endl;
    }
    return commands;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        cerr << "Usage: ./autoscorer [mdl file] [nbt file]" << endl;
        return 1;
    }
    cerr << "Reading Model..." << endl;
    ifstream in(argv[1]);
    Model* model = new Model(in);
    cerr << "Compiling..." << endl;
    deque <Command*> commands = compile(string(argv[2]));
    /*
    for (auto command : commands) {
        cout << (*command) << endl;
    }
     */
    cerr << "Running..." << endl;
    run(model, commands);
    return 0;
}
