// Bondrewd: postprocessing optimizer
// !!! assuming that input commands are valid.

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <map>
#include <algorithm>

using namespace std;


// 6
int adj_dx[] = {1,-1, 0, 0, 0, 0};
int adj_dy[] = {0, 0, 1,-1, 0, 0};
int adj_dz[] = {0, 0, 0, 0, 1,-1};


struct position {
    int x;
    int y;
    int z;

    position() {}
    position(int x, int y, int z) : x(x), y(y), z(z) {}
};

position operator+(const position& p1, const position& p2) {
    return position(p1.x + p2.x, p1.y + p2.y, p1.z + p2.z);
}

position operator-(const position& p1, const position& p2) {
    return position(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
}

bool operator==(const position& p1, const position& p2) {
    return make_tuple(p1.x, p1.y, p1.z) == make_tuple(p2.x, p2.y, p2.z);
}

bool operator!=(const position& p1, const position& p2) {
    return make_tuple(p1.x, p1.y, p1.z) != make_tuple(p2.x, p2.y, p2.z);
}

bool operator<(const position& p1, const position& p2) {
    return make_tuple(p1.x, p1.y, p1.z) < make_tuple(p2.x, p2.y, p2.z);
}

istream& operator>>(istream& s, position& p) {
    s >> p.x >> p.y >> p.z;
    return s;
}

struct region {
    position p1;
    position p2;

    region(position _p1, position _p2): p1(_p1), p2(_p2) {}

    vector<position> internals() {
        vector<position> internals;
        for (int x = min(p1.x, p2.x); x <= max(p1.x, p2.x); ++x) {
            for (int y = min(p1.y, p2.y); y <= max(p1.y, p2.y); ++y) {
                for (int z = min(p1.z, p2.z); z <= max(p1.z, p2.z); ++z) {
                    internals.emplace_back(x, y, z);
                }
            }
        }
        return internals;
    }
};

enum operation {
    HALT,
    WAIT,
    FLIP,
    SMOVE,
    LMOVE,
    FISSION,
    FILL,
    VOID,
    FUSIONP,
    FUSIONS,
    GFILL,
    GVOID,
};

struct command {
    operation op;
    int energy;
    position p1;
    position p2;
    int m;

    command() {}
    command(operation op, int energy) : op(op), energy(energy) {}
    command(operation op, int energy, const position& p1) : op(op), energy(energy), p1(p1) {}
    command(operation op, int energy, const position& p1, const position& p2) : op(op), energy(energy), p1(p1), p2(p2) {}
    command(operation op, int energy, const position& p1, int m) : op(op), energy(energy), p1(p1), m(m) {}
};

int manhattan(int dx, int dy, int dz) {
    return abs(dx) + abs(dy) + abs(dz);
}

int manhattan(const position& p) {
    return manhattan(p.x, p.y, p.z);
}

int chebyshev(int dx, int dy, int dz) {
    return max({abs(dx), abs(dy), abs(dz)});
}

int chebyshev(const position& p) {
    return chebyshev(p.x, p.y, p.z);
}

bool near(const position& p1, const position& p2) {
    int md = manhattan(p1 - p2);
    int cd = chebyshev(p1 - p2);
    return md <= 2 && cd == 1;
}

command halt() {
    return command(HALT, 0);
}

command wait() {
    return command(WAIT, 0);
}

command flip() {
    return command(FLIP, 0);
}

command smove(const position& p) {
    return command(SMOVE, 2 * manhattan(p), p);
}

command smove(int dx, int dy, int dz) {
    return smove(position(dx, dy, dz));
}

command lmove(const position& p1, const position& p2) {
    return command(LMOVE, 2 * (manhattan(p1) + 2 + manhattan(p2)), p1, p2);
}

command lmove(int dx1, int dy1, int dz1, int dx2, int dy2, int dz2) {
    return lmove(position(dx1, dy1, dz1), position(dx2, dy2, dz2));
}

command fusionp(const position& p) {
    return command(FUSIONP, -24, p);
}

command fusionp(int dx, int dy, int dz) {
    return fusionp(position(dx, dy, dz));
}

command fusions(const position& p) {
    return command(FUSIONS, 0, p);
}

command fusions(int dx, int dy, int dz) {
    return fusions(position(dx, dy, dz));
}

command fission(const position& p, int m) {
    return command(FISSION, 24, p, m);
}

command fission(int dx, int dy, int dz, int m) {
    return fission(position(dx, dy, dz), m);
}

command fill(const position& p) {
    return command(FILL, 12, p);
}

command fill(int dx, int dy, int dz) {
    return fill(position(dx, dy, dz));
}

command void_c(const position& p) {
    return command(VOID, -12, p);
}

command void_c(int dx, int dy, int dz) {
    return void_c(position(dx, dy, dz));
}

command gfill(const position& p1, const position& p2) {
    return command(GFILL, 0 /* meaningless*/, p1, p2);
}

command gfill(int dx1, int dy1, int dz1, int dx2, int dy2, int dz2) {
    return gfill(position(dx1, dy1, dz1), position(dx2, dy2, dz2));
}

command gvoid(const position& p1, const position& p2) {
    return command(GVOID, 0 /* meaningless*/, p1, p2);
}

command gvoid(int dx1, int dy1, int dz1, int dx2, int dy2, int dz2) {
    return gvoid(position(dx1, dy1, dz1), position(dx2, dy2, dz2));
}

typedef vector<vector<vector<bool>>> voxels;

voxels empty_voxels(int size) {
    return voxels(size, vector<vector<bool>>(size, vector<bool>(size)));
}

struct Bot {
    position p;
    vector<int> seeds;
    bool active;

    Bot(position _p, vector<int> _s) {
        p = _p;
        seeds = _s;
        active = true;
    }

    Bot(position _p, int total_bots) {
        p = _p;
        for (int i = 1; i < total_bots; ++i) {
            seeds.push_back(i);
        }
        active = true;
    }

    Bot() {
        active = false;
    }

    vector<position> volatileCoordinates(const command &command) const {
        switch (command.op) {
            case HALT:
            case WAIT:
            case FLIP:
                return {p};
            case FISSION:
            case FILL:
            case VOID:
            case FUSIONP:
            case FUSIONS:
                return {p, p + command.p1};
            case GFILL:
            case GVOID: {
                auto p1 = p + command.p1;
                auto p2 = p + command.p2;
                vector<position> vcs = region(p1, p2).internals();
                vcs.push_back(p);
                return vcs;
            }
            case SMOVE: {
                auto d = p + command.p1;
                return region(p, d).internals();
            }
            case LMOVE: {
                auto d1 = p + command.p1;
                auto d2 = d1 + command.p2;
                auto vc1 = region(p, d1).internals();
                auto vc2 = region(d1, d2).internals();
                remove(vc2.begin(), vc2.end(), d1);
                vc1.insert(vc1.end(), vc2.begin(), vc2.end());
                return vc1;
            }
        }
    }
};

const int MAX_BOTS = 20;

struct State {
    bool hermony;
    vector<Bot> bots;
    voxels filled;

    State(int size) {
        // empty. assembly problem
        hermony = false;
        bots = vector<Bot>(MAX_BOTS);
        bots[0] = Bot(position(0, 0, 0), MAX_BOTS);
        filled = empty_voxels(size);
    }

    vector<Bot> activeBots() {
        vector<Bot> actives;
        for (auto &&bot : bots) {
            if (bot.active) {
                actives.push_back(bot);
            }
        }
        return actives;
    }

    vector<position> adj_pos(const position &p) const {
        vector<position> result;
        for (int i = 0; i < 6; ++i) {
            auto pp = p + position(adj_dx[i], adj_dy[i], adj_dz[i]);
            if (0 <= pp.x && pp.x < filled.size() && 0 <= pp.y && pp.y < filled[0].size() && 0 <= pp.z && pp.z < filled[0][0].size()) {
                result.emplace_back(pp);
            }
        }
        return result;
    }
};

struct MultiCommand {
    // commands that are executed simultaneously
    vector<command> commands;

    MultiCommand(vector<command> commands): commands(commands) {}
};

State update(const State& state, const MultiCommand& multiCommand) {
    // update without any sanity check.
    State new_state = state;
    vector<command> commands = multiCommand.commands;
    map<position, int> p2b;
    for (int i = 0; i < state.bots.size(); ++i) {
        auto bot = state.bots[i];
        if (bot.active) {
            p2b[bot.p] = i;
        }
    }

    int bot_id = 0;
    for (int i = 0; i < commands.size(); ++i, ++bot_id) {
        while (not state.bots[bot_id].active) {
            bot_id++;
        }

        switch (commands[i].op) {
            case HALT:
                break;
            case WAIT:
                break;
            case FLIP:
                new_state.hermony = !state.hermony;
                break;
            case SMOVE:
                new_state.bots[bot_id].p = state.bots[bot_id].p + commands[i].p1;
                break;
            case LMOVE:
                new_state.bots[bot_id].p = state.bots[bot_id].p + commands[i].p1 + commands[i].p2;
                break;
            case FISSION: {
                int new_bot_id = state.bots[bot_id].seeds[0];
                auto ptr = state.bots[bot_id].seeds.begin() + 1;
                vector<int> sub_seeds(ptr, ptr + commands[i].m);
                vector<int> new_seeds(ptr + commands[i].m, state.bots[bot_id].seeds.end());
                new_state.bots[bot_id].seeds = new_seeds;
                new_state.bots[new_bot_id].active = true;
                new_state.bots[new_bot_id].seeds = sub_seeds;
                new_state.bots[new_bot_id].p = state.bots[bot_id].p + commands[i].p1;
                break;
            }
            case FILL:
            case VOID: {
                auto d = state.bots[bot_id].p + commands[i].p1;
                new_state.filled[d.x][d.y][d.z] = commands[i].op == FILL;
                break;
            }
            case FUSIONP: {
                auto spos = state.bots[bot_id].p + commands[i].p1;
                assert (p2b.find(spos) != p2b.end());
                int sub_id = p2b[spos];
                new_state.bots[bot_id].seeds.push_back(sub_id);
                new_state.bots[bot_id].seeds.insert(new_state.bots[bot_id].seeds.end(),
                                                    state.bots[sub_id].seeds.begin(), state.bots[sub_id].seeds.end());
                sort(new_state.bots[bot_id].seeds.begin(), new_state.bots[bot_id].seeds.end());
                new_state.bots[sub_id].active = false;
                new_state.bots[sub_id].seeds.clear();
                break;
            }
            case FUSIONS:
                // do nothing. everything is done in FUSIONP
                break;
            case GFILL:
            case GVOID: {
                auto p1 = state.bots[bot_id].p + commands[i].p1;
                auto p2 = p1 + commands[i].p2;
                for (auto &&p : region(p1, p2).internals()) {
                    new_state.filled[p.x][p.y][p.z] = commands[i].op == GFILL;
                }
                break;
            }
        }
    }
    return new_state;
}

vector<MultiCommand> group_commands(const vector<command> &serialized_commands) {
    int current_bots = 1;
    auto ptr = serialized_commands.begin();
    vector<MultiCommand> commands;
    while (ptr != serialized_commands.end()) {
        auto multi_command = MultiCommand(vector<command>(ptr, ptr + current_bots));
        ptr += current_bots;

        for (auto &&command : multi_command.commands) {
            if (command.op == FISSION) {
                current_bots += 1;
            } else if (command.op == FUSIONP) {
                current_bots -= 1;
            }
        }
        commands.push_back(multi_command);
    }
    assert (commands.back().commands.size() == 1);
    assert (commands.back().commands[0].op == HALT);
    return commands;
}

vector<command> serialize_commands(const vector<MultiCommand> &commands) {
    vector<command> serialized_commands;
    for (auto &&multi_command : commands) {
        serialized_commands.insert(serialized_commands.end(), multi_command.commands.begin(),
                                   multi_command.commands.end());
    }
    return serialized_commands;
}

vector<command> input(istream &is) {
    vector<command> commands;
    while (true) {
        string mnemonic;
        is >> mnemonic;
        if (mnemonic == "halt") {
            commands.push_back(halt());
            return commands;
        } else if (mnemonic == "wait") {
            commands.push_back(wait());
        } else if (mnemonic == "flip") {
            commands.push_back(flip());
        } else if (mnemonic == "smove") {
            position p;
            is >> p;
            commands.push_back(smove(p));
        } else if (mnemonic == "lmove") {
            position p1, p2;
            is >> p1 >> p2;
            commands.push_back(lmove(p1, p2));
        } else if (mnemonic == "fusionp") {
            position p;
            is >> p;
            commands.push_back(fusionp(p));
        } else if (mnemonic == "fusions") {
            position p;
            is >> p;
            commands.push_back(fusions(p));
        } else if (mnemonic == "fission") {
            position p;
            int n;
            is >> p >> n;
            commands.push_back(fission(p, n));
        } else if (mnemonic == "fill") {
            position p;
            is >> p;
            commands.push_back(fill(p));
        } else if (mnemonic == "void") {
            position p;
            is >> p;
            commands.push_back(void_c(p));
        } else if (mnemonic == "gfill") {
            position p1, p2;
            is >> p1 >> p2;
            commands.push_back(gfill(p1, p2));
        } else if (mnemonic == "gvoid") {
            position p1, p2;
            is >> p1 >> p2;
            commands.push_back(gvoid(p1, p2));
        } else {
            throw "unknown mneamonic " + mnemonic;
        }
    }
}

void output(const vector<command>& traces) {
    for (int i = 0; i < traces.size(); i++) {
        stringstream ss;
        switch (traces[i].op) {
            case HALT:
                cout << "halt" << endl;
                break;
            case WAIT:
                cout << "wait" << endl;
                break;
            case FLIP:
                cout << "flip" << endl;
                break;
            case SMOVE:
                ss << "smove " << traces[i].p1.x << " " << traces[i].p1.y << " " << traces[i].p1.z;
                cout << ss.str() << endl;
                break;
            case LMOVE:
                ss << "lmove " << traces[i].p1.x << " " << traces[i].p1.y << " " << traces[i].p1.z << " " << traces[i].p2.x << " " << traces[i].p2.y << " " << traces[i].p2.z;
                cout << ss.str() << endl;
                break;
            case FUSIONP:
                ss << "fusionp " << traces[i].p1.x << " " << traces[i].p1.y << " " << traces[i].p1.z;
                cout << ss.str() << endl;
                break;
            case FUSIONS:
                ss << "fusions " << traces[i].p1.x << " " << traces[i].p1.y << " " << traces[i].p1.z;
                cout << ss.str() << endl;
                break;
            case FISSION:
                ss << "fission " << traces[i].p1.x << " " << traces[i].p1.y << " " << traces[i].p1.z << " " << traces[i].m;
                cout << ss.str() << endl;
                break;
            case FILL:
                ss << "fill " << traces[i].p1.x << " " << traces[i].p1.y << " " << traces[i].p1.z;
                cout << ss.str() << endl;
                break;
            case VOID:
                ss << "void " << traces[i].p1.x << " " << traces[i].p1.y << " " << traces[i].p1.z;
                cout << ss.str() << endl;
                break;
            case GFILL:
                ss << "gfill " << traces[i].p1.x << " " << traces[i].p1.y << " " << traces[i].p1.z << " " << traces[i].p2.x << " " << traces[i].p2.y << " " << traces[i].p2.z;
                cout << ss.str() << endl;
                break;
            case GVOID:
                ss << "gvoid " << traces[i].p1.x << " " << traces[i].p1.y << " " << traces[i].p1.z << " " << traces[i].p2.x << " " << traces[i].p2.y << " " << traces[i].p2.z;
                cout << ss.str() << endl;
                break;
        }
    }
}

const int MAX_SIZE = 250;
struct Model {
    int size;
    bool field[MAX_SIZE][MAX_SIZE][MAX_SIZE];

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

struct Step {
    State state;
    MultiCommand multiCommand;

    Step(State& s, MultiCommand& m): state(s), multiCommand(m) {}

    voxels volatileCoordinates() {
        voxels vcs = state.filled;
        auto activeBots = state.activeBots();
        for (int i = 0; i < activeBots.size(); ++i) {
            for (auto &&vc : activeBots[i].volatileCoordinates(multiCommand.commands[i])) {
                vcs[vc.x][vc.y][vc.z] = true;
            }
        }
        return vcs;
    }

    map<int, command> botCommands() {
        map<int, command> botCommands;
        int ptr = 0;
        for (int bot_id = 0; bot_id < state.bots.size(); ++bot_id) {
            if (!state.bots[bot_id].active) {
                continue;
            }
            botCommands[bot_id] = multiCommand.commands[ptr++];
        }
        return botCommands;
    };

    map<int, int> botCommandIds() {
        map<int, int> ids;
        int ptr = 0;
        for (int bot_id = 0; bot_id < state.bots.size(); ++bot_id) {
            if (!state.bots[bot_id].active) {
                continue;
            }
            ids[bot_id] = ptr++;
        }
        return ids;
    };
};

//============= OPTIMIZER STAGES ====================

vector<Step> eagerExecution(vector<Step> &steps) {
    vector<Step> new_steps = steps;
    for (int turn = (int)new_steps.size() - 2; turn >= 0; --turn) {
        auto activeBots = new_steps[turn + 1].state.activeBots();
        for (int bot_id = 0; bot_id < activeBots.size(); ++bot_id) {
            if (!new_steps[turn].state.bots[bot_id].active) {
                continue;
            }
            auto prev_commands = new_steps[turn].botCommands();  // TODO: this can be out of this loop.
            auto prev_command = prev_commands[bot_id];
            if (prev_command.op != WAIT) {
                continue;
            }

            auto command_id = new_steps[turn].botCommandIds()[bot_id];
            auto command = new_steps[turn + 1].multiCommand.commands[command_id];
            if (command.op == HALT || command.op == WAIT || command.op == FLIP) {
                continue;
            }
            auto vcs = new_steps[turn].volatileCoordinates();  // TODO: this can be out of this loop.
            auto& bot = new_steps[turn].state.bots[bot_id];
            auto new_vcs = bot.volatileCoordinates(command);
            bool ok = true;
            for (auto &&vc : new_vcs) {
                if (vc != bot.p && vcs[vc.x][vc.y][vc.z]) {
                    ok = false;
                    break;
                }
            }
            if (not ok) {
                // vc collision
                continue;
            }
            // This command can be moved!
            int prevCommandId = new_steps[turn].botCommandIds()[bot_id];
            switch (command.op) {
                case SMOVE:
                case LMOVE: {
                    new_steps[turn].multiCommand.commands[prevCommandId] = command;
                    new_steps[turn + 1].multiCommand.commands[command_id] = wait();
                    new_steps[turn].state.bots[bot_id] = new_steps[turn + 1].state.bots[bot_id];
                    break;
                }
                case FISSION: { // TODO: make this togglable.
                    int sub_id = new_steps[turn].state.bots[bot_id].seeds.front();
                    new_steps[turn].multiCommand.commands[prevCommandId] = command;
                    new_steps[turn + 1].multiCommand.commands[command_id] = wait();
                    new_steps[turn].state.bots[bot_id] = new_steps[turn + 1].state.bots[bot_id];
                    new_steps[turn].state.bots[sub_id] = new_steps[turn + 1].state.bots[sub_id];
                    break;
                }
                case FILL:
                case VOID: {
                    auto &oldState = steps[turn].state;
                    auto &newState = steps[turn + 1].state;
                    auto dest = bot.p + command.p1;
                    if (not newState.hermony) { // todo: use ground state for this case.
                        break;
                    }
                    new_steps[turn].multiCommand.commands[prevCommandId] = command;
                    new_steps[turn + 1].multiCommand.commands[command_id] = wait();
                    newState.filled[dest.x][dest.y][dest.z] = command.op == FILL;
                    break;
                }
                case FUSIONP:break;
                case FUSIONS:break;
                case GFILL:
                case GVOID: {
                    auto &newState = steps[turn + 1].state;
                    if (not newState.hermony) {
                        break;
                    }
                    new_steps[turn].multiCommand.commands[prevCommandId] = command;
                    new_steps[turn + 1].multiCommand.commands[command_id] = wait();
                    for (auto &&dest : region(bot.p + command.p1, bot.p + command.p1 + command.p2).internals()) {
                        newState.filled[dest.x][dest.y][dest.z] = command.op == GFILL;
                    }
                    break;
                }
                default:
                    // do nothing
                    break;
            }
        }

    }
    return new_steps;
}

vector<Step> mergeMoves(vector<Step> &steps) {

}

vector<Step> optimizeFusion(vector<Step> &steps) {

}

vector<Step> optimizeFission(vector<Step> &steps) {

}

vector<Step> skipAllWaits(vector<Step> &steps) {
    vector<Step> newSteps;
    for (auto &&step : steps) {
        bool skippable = true;
        for (auto &&command: step.multiCommand.commands) {
            skippable &= command.op == WAIT;
        }
        if (!skippable) {
            newSteps.push_back(step);
        }
    }
    return newSteps;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: ./bondrewd [mdl file] [asm file (optional)]" << endl;
        return 1;
    }

    ifstream in(argv[1]);
    auto *model = new Model(in);
    cerr << "Model loaded. size = " << model->size << endl;

    vector<command> commands;
    if (argc < 3) {
        commands = input(cin);
    } else {
        ifstream in2(argv[2]);
        commands = input(in2);
    }
    vector<MultiCommand> turns = group_commands(commands);
    cerr << "assembly loaded. #turns = " << turns.size() << endl;

    State state(model->size);
    vector<Step> steps;
    for (int i = 0; i < turns.size(); ++i) {
        steps.emplace_back(state, turns[i]);
        state = update(state, turns[i]);
    }
    cerr << "calculated state history" << endl;

    int current_steps;
    int least_loops = 10;
    do {
        current_steps = steps.size();
        steps = eagerExecution(steps);
        steps = skipAllWaits(steps);
        cerr << "optimization iteration done. current #turns " << steps.size() << endl;
    } while (steps.size() < current_steps || least_loops--);
    cerr << "optimization completed. #turns = " << steps.size() << endl;

    vector<MultiCommand> newTurns;
    for (auto &&step : steps) {
        newTurns.emplace_back(step.multiCommand);
    }
    output(serialize_commands(newTurns));
}