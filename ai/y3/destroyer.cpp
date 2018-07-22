#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <cassert>
#include <set>
#include <map>
#include <tuple>

using namespace std;
typedef tuple<int, int, int> Pos;
typedef pair<Pos, Pos> Region;

int dx[6] = {1, -1, 0, 0, 0, 0};
int dy[6] = {0, 0, 1, -1, 0, 0};
int dz[6] = {0, 0, 0, 0, 1, -1};

Pos operator+(const Pos& p1, const Pos& p2) {
    return make_tuple(get<0>(p1) + get<0>(p2), get<1>(p1) + get<1>(p2), get<2>(p1) + get<2>(p2));
}

Pos operator-(const Pos& p1, const Pos& p2) {
    return make_tuple(get<0>(p1) - get<0>(p2), get<1>(p1) - get<1>(p2), get<2>(p1) - get<2>(p2));
}

enum operation {
    HALT, // (no corresponding op)
    WAIT, // WAIT
    SMOVE, // SMOVE lld
    LMOVE, // LMOVE sld2 sld1
    FISSION, // FUSIONP, FUSIONS
    FILL, // REMOVE
    FUSIONP, // FISSION
    FUSIONS, // FISSION
    DESTROY // FILL
};

struct command {
    operation op;
    Pos p1;
    Pos p2;
    int m;
    
    command(operation op) : op(op) {}
    command(operation op, Pos p1) : op(op), p1(p1) {}
    command(operation op, Pos p1, Pos p2) : op(op), p1(p1), p2(p2) {}
    command(operation op, Pos p1, int m) : op(op), p1(p1), m(m) {}
};

const int MAX_R = 250;
const int MAX_B = 20;
unsigned char buffer[MAX_R * MAX_R * MAX_R / 8];
int R;
bool hermonics;
bool matrix[MAX_R][MAX_R][MAX_R];
bool vis[MAX_R][MAX_R][MAX_R];
bool removable[MAX_R][MAX_R][MAX_R];
int depth[MAX_R][MAX_R][MAX_R];
int maxY = 0;
int remaining;
set<Pos> available;

void read_input() {
    cin.read((char*)buffer, 1);
    R = buffer[0];
    remaining = 0;
    cin.read((char*)buffer, (R * R * R + 7) / 8);
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < R; j++) {
            for (int k = 0; k < R; k++) {
                int pos = i * R * R + j * R + k;
                matrix[i][j][k] = ((buffer[pos / 8] >> (pos % 8)) & 1);
                if (matrix[i][j][k]) remaining++;
            }
        }
    }
}

void clear_vis() {
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < R; j++) {
            for (int k = 0; k < R; k++) {
                vis[i][j][k] = false;
            }
        }
    }
}

bool in_range(int x, int y, int z) {
  return 0 <= x && x < R && 0 <= y && y < R && 0 <= z && z < R;
}

void dijkstra() {
    clear_vis();
    priority_queue<pair<int, Pos>, vector<pair<int, Pos>>, greater<pair<int, Pos>>> pq;
    for (int i = 0; i < R; i++) {
        for (int k = 0; k < R; k++) {
            if (matrix[i][0][k]) {
	            pq.push({1, make_tuple(i, 0, k)});
            }
        }
    }

  while (!pq.empty()) {
    auto e = pq.top();
    pq.pop();
    int d = e.first;
    Pos c = e.second;
    int x = get<0>(c);
    int y = get<1>(c);
    int z = get<2>(c);

    if (vis[x][y][z]) continue;
    vis[x][y][z] = true;
    depth[x][y][z] = d;
    maxY = max(maxY, y);
    for (int i = 0; i < 6; i++) {
      int nx = x + dx[i];
      int ny = y + dy[i];
      int nz = z + dz[i];
      if (in_range(nx, ny, nz) && matrix[nx][ny][nz]) {
          pq.push({d + 1, make_tuple(nx, ny, nz)});
      }
    }
  }
}

int manhattan(int dx, int dy, int dz) {
    return abs(dx) + abs(dy) + abs(dz);
}

int manhattan(const Pos& p) {
    return manhattan(get<0>(p), get<1>(p), get<2>(p));
}

int chebyshev(int dx, int dy, int dz) {
    return max({abs(dx), abs(dy), abs(dz)});
}

int chebyshev(const Pos& p) {
    return chebyshev(get<0>(p), get<1>(p), get<2>(p));
}

bool near(const Pos& p1, const Pos& p2) {
    int md = manhattan(p1 - p2);
    int cd = chebyshev(p1 - p2);
    return md <= 2 && cd == 1;
}

enum BotState {
    BOT_FISSION,
    BOT_DESTROY,
    BOT_GATHER,
    BOT_FUSION_P,
    BOT_FUSION_S,
    BOT_END
};

set<Pos> volatileCoord;

string outputPos(Pos& pos) {
    stringstream ss;
    ss << get<0>(pos) << " " << get<1>(pos) << " " << get<2>(pos);
    return ss.str();
}

void output(const vector<command*>& traces) {
    for (int i = 0; i < traces.size(); i++) {
        stringstream ss;
        switch (traces[i]->op) {
            case HALT:
            cout << "halt" << endl;
            break;
            case WAIT:
            cout << "wait" << endl;
            break;
            case SMOVE:
            ss << "smove " << outputPos(traces[i]->p1);
            cout << ss.str() << endl;
            break;
            case LMOVE:
            ss << "lmove " << outputPos(traces[i]->p1) << " " << outputPos(traces[i]->p2);
            cout << ss.str() << endl;
            break;
            case FUSIONP:
            ss << "fusionp " << outputPos(traces[i]->p1);
            cout << ss.str() << endl;
            break;
            case FUSIONS:
            ss << "fusions " << outputPos(traces[i]->p1);
            cout << ss.str() << endl;
            break;
            case FISSION:
            ss << "fission " << outputPos(traces[i]->p1) << " " << traces[i]->m;
            cout << ss.str() << endl;
            break;
            case FILL:
            stringstream ss;
            ss << "fill " << outputPos(traces[i]->p1);
            cout << ss.str() << endl;
            break;
        }
    }
}

struct Bot {
    int id;
    Pos position;
    Pos gatherPosition;
    bool active;
    BotState state;

    static map<int, Bot*> botById;

    Bot(int id) : id(id), position(make_tuple(0, 0, 0)), active(false), state(BOT_FISSION) {
        gatherPosition = make_tuple(0, 0, id);
    }

    command* move() {
        if (state == BOT_FISSION && id == MAX_B - 1) {
            state = BOT_DESTROY;
        }
        if (state == BOT_DESTROY /*&& remaining == 0*/) {
            state = BOT_GATHER;
        }
        switch (state) {
            case BOT_FISSION:
                return fission();
            case BOT_DESTROY:
                return destroy();
            case BOT_GATHER:
                return gather();
            case BOT_FUSION_P:
                return fusionP();
            case BOT_FUSION_S:
                return fusionS();
            case BOT_END:
                cerr << "Unexpected status reached." << endl;
                assert(false);
        }
    }

    command* wait() {
        return new command(WAIT);
    }

    command* fission() {
        state = BOT_DESTROY;
        Pos d = make_tuple(0, 0, 1);
        claim(position + d);
        return new command(FISSION, d);
    }

    command* destroy() {
        return new command(WAIT);
    }

    command* gather() {
        if (state == BOT_GATHER && position == gatherPosition) {
            state = (id == (MAX_B - 1)) ? BOT_FUSION_S : BOT_FUSION_P;
        }
        return new command(WAIT);
    }

    command* fusionP() {
        Bot* nextBot = botById[id + 1];
        cerr << "FUSION P!" << endl;
        if (nextBot->state == BOT_END) {
            state = BOT_FUSION_S;
            return fusionS();
        }
        if (nextBot->state == BOT_FUSION_S) {
            nextBot->state = BOT_END;
            nextBot->active = false;
            if (id == 0) {
                state = BOT_END;
                active = false;
            }
            return new command(FUSIONP, make_tuple(0, 0, 1));
        }
        return new command(WAIT);
    }

    command* fusionS() {
        Bot* beforeBot = botById[id - 1];
        cerr << "FUSION S!" << endl;
        cerr << beforeBot->state << endl;
        if (beforeBot->state == BOT_FUSION_P) {
            return new command(FUSIONS, make_tuple(0, 0, -1));
        }
        return new command(WAIT);
    } 

    bool claim(Pos p) {
        if (volatileCoord.find(p) != volatileCoord.end()) {
            return false;
        }
        volatileCoord.insert(p);
        return true;
    }

    bool claim(Pos start, Pos end) {
    }
};

map<int, Bot*> Bot::botById = map<int, Bot*>();

vector<command*> convert(vector<command*> commands) {
    vector<command*> ans;
    reverse(commands.begin(), commands.end());
    int fusionCount = MAX_B - 1;
    for (int i = 0; i < commands.size(); i++) {
        command* command = commands[i];
        switch(command->op) {
        case WAIT:
            ans.push_back(command);
            break;
        case SMOVE:
            ans.push_back(new struct command(SMOVE, command->p1));
            break;
        case LMOVE:
            ans.push_back(new struct command(LMOVE, command->p2, command->p1));
            break;
        case FUSIONP:
            fusionCount--;
            ans.push_back(new struct command(FISSION, command->p1, fusionCount));
            break;
        case FUSIONS:
            break;
        case FISSION:
            ans.push_back(new struct command(FUSIONP, command->p1));
            ans.push_back(new struct command(FUSIONS, make_tuple(0, 0, 0) - command->p1));
        break;
        case DESTROY:
            ans.push_back(new struct command(FILL, command->p1));
            break;
        }
    }
    return ans;
}

int main() {
    cerr << "Read Input" << endl;
    read_input();
    cerr << "Dijkstra" << endl;
    dijkstra();
    cerr << "Main" << endl;

    // main
    vector<Bot*> bots;
    for (int i = MAX_B - 1; i >= 0; i--) {
        Bot* bot = new Bot(i);
        bots.push_back(bot);
        Bot::botById[i] = bot;
    }
    bots[MAX_B - 1]->active = true;

    vector<command*> commands;
    while (bots[MAX_B - 1]->active) {
        /*
        if (available.empty()) {
            cerr << "No removable boxel. I'm gonna die." << endl;
            assert(false);
        }
        */

        volatileCoord.clear();
        for (int i = 0; i < MAX_B; i++) {
            Bot* bot = bots[i];
            if (bot->active) {
                assert(bot->claim(bot->position));  
            }
        }
        for (int i = 0; i < MAX_B; i++) {
            Bot& bot = *(bots[i]);
            if (bot.active) {
                command* command = bot.move();
                if (command->op == FISSION) {
                    bots[i - 1]->position = bot.position + command->p1;
                    bots[i - 1]->active = true;
                }
                cerr << bot.id << " " << command->op << endl;
                commands.push_back(command);
            }
        }
    }

    output(convert(commands));
    return 0;
}