#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>

using namespace std;

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

struct rectangle {
    int x1;
    int x2;
    int z1;
    int z2;
    
    rectangle(int x1, int x2, int z1, int z2) : x1(x1), x2(x2), z1(z1), z2(z2) {}
};

enum operation {
    HALT,
    WAIT,
    FLIP,
    SMOVE,
    LMOVE,
    FISSION,
    FILL,
    FUSIONP,
    FUSIONS
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

class UnionFind {
    public:
    UnionFind(int R) : R(R) {
        int n = R * R * R + 1;
        parent = (int *)malloc(sizeof(int) * n);
        for (int i = 0; i < n; i++) parent[i] = -1;
    }
    
    ~UnionFind() {
        free(parent);
    }
    
    int find(int x) {
        if (parent[x] < 0) return x;
        
        return parent[x] = find(parent[x]);
    }
    
    int find(int x, int y, int z) {
        return find(x * R * R + y * R + z);
    }
    
    int ground() {
        return find(R, 0, 0);
    }
    
    void unite(int x, int y) {
        x = find(x);
        y = find(y);
        
        if (x == y) return;
        
        if (parent[x] < parent[y]) {
            parent[x] += parent[y];
            parent[y] = x;
        } else {
            parent[y] += parent[x];
            parent[x] = y;
        }
    }
    
    void unite(int x1, int y1, int z1, int x2, int y2, int z2) {
        return unite(find(x1, y1, z1), find(x2, y2, z2));
    }
    
    int size(int x) {
        return -parent[find(x)];
    }
    
    int size(int x1, int y1, int z1) {
        return size(find(x1, y1, z1));
    }
    
    private:
    int R;
    int *parent;
};

const int MAX_R = 250;
const int MAX_B = 40;
unsigned char buffer[MAX_R * MAX_R * MAX_R / 8];
int R;
bool hermonics;
bool occupied[MAX_R][MAX_R];
bool matrix[MAX_R][MAX_R][MAX_R];
bool current[MAX_R][MAX_R][MAX_R];
int sum[MAX_R + 1][MAX_R + 1][MAX_R + 1];
int voxels_x[MAX_R][MAX_R][MAX_R];
int voxels_z[MAX_R][MAX_R][MAX_R];
int dp_x[MAX_R + 1][MAX_B + 1][2];
int dp_z[MAX_R + 1][MAX_B + 1][2];

void read_input() {
    cin.read((char*)buffer, 1);
    R = buffer[0];
    cin.read((char*)buffer, (R * R * R + 7) / 8);
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < R; j++) {
            for (int k = 0; k < R; k++) {
                int pos = i * R * R + j * R + k;
                matrix[i][j][k] = ((buffer[pos / 8] >> (pos % 8)) & 1);
            }
        }
    }
    
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < R; j++) {
            for (int k = 0; k < R; k++) {
                sum[i][j + 1][k + 1] = sum[i][j + 1][k] + sum[i][j][k + 1] - sum[i][j][k];
                if (matrix[j][i][k]) sum[i][j + 1][k + 1]++;
            }
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
            stringstream ss;
            ss << "fill " << traces[i].p1.x << " " << traces[i].p1.y << " " << traces[i].p1.z;
            cout << ss.str() << endl;
            break;
        }
    }
}

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

bool grounded(UnionFind& uf, const position& p) {
    if (p.y == 0) return true;
    static int dx[6] = {1, -1, 0, 0, 0, 0};
    static int dy[6] = {0, 0, 1, -1, 0, 0};
    static int dz[6] = {0, 0, 0, 0, 1, -1};
    for (int i = 0; i < 6; i++) {
        int x = p.x + dx[i];
        int y = p.y + dy[i];
        int z = p.z + dz[i];
        if (uf.find(x, y, z) == uf.ground()) return true;
    }
    return false;
}

void fill(UnionFind& uf, const position& p) {
    current[p.x][p.y][p.z] = true;
    if (p.y == 0) {
        uf.unite(uf.find(p.x, p.y, p.z), uf.ground());
    } else {
        static int dx[6] = {1, -1, 0, 0, 0, 0};
        static int dy[6] = {0, 0, 1, -1, 0, 0};
        static int dz[6] = {0, 0, 0, 0, 1, -1};
        for (int i = 0; i < 6; i++) {
            int x = p.x + dx[i];
            int y = p.y + dy[i];
            int z = p.z + dz[i];
            if (current[x][y][z]) uf.unite(p.x, p.y, p.z, x, y, z);
        }
    }
}

void maintain(long long& energy, int bots) {
    energy += (hermonics ? 30 : 3) * R * R * R + 20 * bots;
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

void add_trace(long long& energy, vector <command>& traces, const command& command) {
    energy += command.energy;
    traces.push_back(command);
}

bool is_occupied(const position& p, const command& c, bool update) {
    if (c.op == SMOVE) {
        int d = max(abs(c.p1.x), abs(c.p1.z));
        int dx = (c.p1.x == 0 ? 0 : c.p1.x / abs(c.p1.x));
        int dz = (c.p1.z == 0 ? 0 : c.p1.z / abs(c.p1.z));
        for (int i = 1; i <= d; i++) {
            if (occupied[p.x + dx * i][p.z + dz * i]) return true;
            if (update) occupied[p.x + dx * i][p.z + dz * i] = true;
        }
    } else {
        int d1 = max(abs(c.p1.x), abs(c.p1.z));
        int dx1 = (c.p1.x == 0 ? 0 : c.p1.x / abs(c.p1.x));
        int dz1 = (c.p1.z == 0 ? 0 : c.p1.z / abs(c.p1.z));
        for (int i = 1; i <= d1; i++) {
            if (occupied[p.x + dx1 * i][p.z + dz1 * i]) return true;
            if (update) occupied[p.x + dx1 * i][p.z + dz1 * i] = true;
        }
        
        int d2 = max(abs(c.p2.x), abs(c.p2.z));
        int dx2 = (c.p2.x == 0 ? 0 : c.p2.x / abs(c.p2.x));
        int dz2 = (c.p2.z == 0 ? 0 : c.p2.z / abs(c.p2.z));
        for (int i = 1; i <= d2; i++) {
            if (occupied[p.x + dx1 * d1 + dx2 * i][p.z + dz1 * d1 + dz2 * i]) return true;
            if (update) occupied[p.x + dx1 * d1 + dx2 * i][p.z + dz1 * d1 + dz2 * i] = true;
        }
    }
    return false;
}

vector <command> get_moves(const position& p1, const position& p2) {
    bool fx = false, fy = false, fz = false;
    position p = p2 - p1;
    vector <command> traces;
    
    if (abs(p.x) % 15 > 0 && abs(p.x) % 15 <= 5) fx = true;
    if (abs(p.y) % 15 > 0 && abs(p.y) % 15 <= 5) fy = true;
    if (abs(p.z) % 15 > 0 && abs(p.z) % 15 <= 5) fz = true;
    
    if (fy && fx) {
        int dy = min(max(p.y, -5), 5);
        int dx = min(max(p.x, -5), 5);
        fy = fx = false;
        p.y -= dy;
        p.x -= dx;
        traces.push_back(lmove(0, dy, 0, dx, 0, 0));
    } else if (fy && fz) {
        int dy = min(max(p.y, -5), 5);
        int dz = min(max(p.z, -5), 5);
        fy = fz = false;
        p.y -= dy;
        p.z -= dz;
        traces.push_back(lmove(0, dy, 0, 0, 0, dz));
    }
    
    while (p.y != 0) {
        int dy = min(max(p.y, -15), 15);
        p.y -= dy;
        traces.push_back(smove(0, dy, 0));
    }
    
    if (fx && fz) {
        int dx = min(max(p.x, -5), 5);
        int dz = min(max(p.z, -5), 5);
        p.x -= dx;
        p.z -= dz;
        traces.push_back(lmove(dx, 0, 0, 0, 0, dz));
    }
    
    while (p.x != 0) {
        int dx = min(max(p.x, -15), 15);
        p.x -= dx;
        traces.push_back(smove(dx, 0, 0));
    }
    
    while (p.z != 0) {
        int dz = min(max(p.z, -15), 15);
        p.z -= dz;
        traces.push_back(smove(0, 0, dz));
    }
    
    return traces;
}

vector <command> put_floor(const rectangle& rect, const position& p, int floor) {
    long long best_energy;
    vector <command> best_traces;
    
    for (int dx = -1; dx <= 1; dx++) {
        for (int dz = -1; dz <= 1; dz++) {
            if (dx == 0 || dz == 0) continue;
            
            position now = p;
            vector <position> positions;
            vector <command> traces;
            for (int x = (dx == 1 ? rect.x1 : rect.x2); rect.x1 <= x && x <= rect.x2; x += dx) {
                bool found = false;
                for (int z = (dz == 1 ? rect.z1 : rect.z2); rect.z1 <= z && z <= rect.z2; z += dz) {
                    if (matrix[x][floor][z]) {
                        found = true;
                        positions.push_back(position(x, floor + 1, z));
                    }
                }
                if (found) dz *= -1;
            }
            
            for (int i = 0; i < positions.size(); i++) {
                if (i + 1 < positions.size() && manhattan(positions[i] - positions[i + 1]) <= 1) {
                    vector <command> moves = get_moves(now, positions[i + 1]);
                    now = positions[i + 1];
                    traces.insert(traces.end(), moves.begin(), moves.end());
                    traces.push_back(fill((positions[i] - now) + position(0, -1, 0)));
                    traces.push_back(fill(0, -1, 0));
                    if (i + 2 < positions.size() && manhattan(positions[i + 1] - positions[i + 2]) <= 1) {
                        traces.push_back(fill((positions[i + 2] - now) + position(0, -1, 0)));
                        i += 2;
                    } else {
                        i++;
                    }
                } else {
                    vector <command> moves = get_moves(now, positions[i]);
                    now = positions[i];
                    traces.insert(traces.end(), moves.begin(), moves.end());
                    traces.push_back(fill(0, -1, 0));
                }
            }
            
            long long energy = 0;
            for (int i = 0; i < traces.size(); i++) energy += traces[i].energy;
            if (best_traces.empty() || traces.size() < best_traces.size() || (traces.size() == best_traces.size() && energy < best_energy)) {
                best_energy = energy;
                best_traces = traces;
            }
        }
    }
    
    for (int dx = -1; dx <= 1; dx++) {
        for (int dz = -1; dz <= 1; dz++) {
            if (dx == 0 || dz == 0) continue;
            
            position now = p;
            vector <position> positions;
            vector <command> traces;
            for (int z = (dz == 1 ? rect.z1 : rect.z2); rect.z1 <= z && z <= rect.z2; z += dz) {
                bool found = false;
                for (int x = (dx == 1 ? rect.x1 : rect.x2); rect.x1 <= x && x <= rect.x2; x += dx) {
                    if (matrix[x][floor][z]) {
                        found = true;
                        positions.push_back(position(x, floor + 1, z));
                    }
                }
                if (found) dx *= -1;
            }
            
            for (int i = 0; i < positions.size(); i++) {
                if (i + 1 < positions.size() && manhattan(positions[i] - positions[i + 1]) <= 1) {
                    vector <command> moves = get_moves(now, positions[i + 1]);
                    now = positions[i + 1];
                    traces.insert(traces.end(), moves.begin(), moves.end());
                    traces.push_back(fill((positions[i] - now) + position(0, -1, 0)));
                    traces.push_back(fill(0, -1, 0));
                    if (i + 2 < positions.size() && manhattan(positions[i + 1] - positions[i + 2]) <= 1) {
                        traces.push_back(fill((positions[i + 2] - now) + position(0, -1, 0)));
                        i += 2;
                    } else {
                        i++;
                    }
                } else {
                    vector <command> moves = get_moves(now, positions[i]);
                    now = positions[i];
                    traces.insert(traces.end(), moves.begin(), moves.end());
                    traces.push_back(fill(0, -1, 0));
                }
            }
            
            long long energy = 0;
            for (int i = 0; i < traces.size(); i++) energy += traces[i].energy;
            if (best_traces.empty() || traces.size() < best_traces.size() || (traces.size() == best_traces.size() && energy < best_energy)) {
                best_energy = energy;
                best_traces = traces;
            }
        }
    }
    
    return best_traces;
}

void spread(long long& energy, vector <command>& traces, vector <position>& ps, const vector <rectangle>& rect, int floor, int direction, bool should_flip) {
    vector <position> targets;
    vector <vector <command>> all_moves;
    for (int i = 0; i < ps.size(); i++) {
        int x = min(max(ps[i].x, rect[i].x1), rect[i].x2), y = floor, z = min(max(ps[i].z, rect[i].z1), rect[i].z2);
        position target(x, y, z);
        targets.push_back(target);
        vector <command> moves = get_moves(ps[i], target);
        reverse(moves.begin(), moves.end());
        all_moves.push_back(moves);
    }
    
    while (true) {
        maintain(energy, ps.size());
        
        for (int i = 0; i < ps.size(); i++) occupied[ps[i].x][ps[i].z] = true;
        
        vector <int> signs;
        for (int i = 0; i < ps.size(); i++) signs.push_back(direction == 0 ? targets[i].x - ps[i].x : targets[i].z - ps[i].z);
        
        bool remaining = false;
        vector <bool> determined(ps.size(), false);
        vector <command> commands(ps.size());
        for (int i = 0; i < ps.size(); i++) {
            for (int j = 0; j < ps.size(); j++) {
                if (determined[j]) continue;
                if ((signs[j] < 0 && (j == 0 || determined[j - 1] || signs[j - 1] >= 0)) || signs[j] == 0 || (signs[j] > 0 && (j + 1 == ps.size() || determined[j + 1] || signs[j + 1] <= 0))) {
                    determined[j] = true;
                    if (all_moves[j].empty() || is_occupied(ps[j], all_moves[j].back(), false)) {
                        if (should_flip) {
                            should_flip = false;
                            hermonics = false;
                            commands[j] = flip();
                        } else {
                            commands[j] = wait();
                        }
                    } else {
                        is_occupied(ps[j], all_moves[j].back(), true);
                        if (all_moves[j].back().op == SMOVE) {
                            ps[j] = ps[j] + all_moves[j].back().p1;
                        } else {
                            ps[j] = ps[j] + all_moves[j].back().p1 + all_moves[j].back().p2;
                        }
                        commands[j] = all_moves[j].back();
                        all_moves[j].pop_back();
                    }
                    if (!all_moves[j].empty()) remaining = true;
                    break;
                }
            }
        }
        
        for (int i = 0; i < commands.size(); i++) add_trace(energy, traces, commands[i]);
        
        for (int i = 0; i < ps.size(); i++) {
            for (int dx = -15; dx <= 15; dx++) {
                for (int dz = -15; dz <= 15; dz++) {
                    int x = ps[i].x + dx, z = ps[i].z + dz;
                    if (x >= 0 && x < R && z >= 0 && z < R) occupied[x][z] = false;
                }
            }
        }
        
        if (!remaining) break;
    }
}

pair<long long, vector <command>> calc_i(int B, const vector <int>& mutations, vector <vector <rectangle>>& all_bots, vector <vector <int>>& all_voxels, int direction) {
    long long energy = 0;
    vector <command> traces;
    vector <position> ps = {position(0, 0, 0)};
    
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < R; j++) {
            for (int k = 0; k < R; k++) {
                current[i][j][k] = false;
            }
        }
    }
    
    
    {
        for (int i = 1; i < B; i++) {
            maintain(energy, i);
            
            for (int j = 0; j < i - 1; j++) add_trace(energy, traces, wait());
            
            int dx = 1 - direction, dy = 0, dz = direction;
            add_trace(energy, traces, fission(dx, dy, dz, B - i - 1));
            ps.push_back(position(ps.back().x + dx, ps.back().y + dy, ps.back().z + dz));
        }
    }
    
    int full = 0;
    UnionFind uf(R);
    vector <int> floors(B);
    for (int i = 0; i + 1 < mutations.size(); i++) {
        spread(energy, traces, ps, all_bots[i], mutations[i], direction, uf.size(uf.ground()) == full + 1 && hermonics);
        
        vector <deque <command>> remaining_moves(B);
        while (true) {
            maintain(energy, B);
            
            for (int j = 0; j < B; j++) {
                while (remaining_moves[j].empty() && floors[j] < mutations[i + 1]) {
                    vector <command> moves = put_floor(all_bots[i][j], ps[j], floors[j]++);
                    remaining_moves[j].insert(remaining_moves[j].end(), moves.begin(), moves.end());
                }
                
                if (remaining_moves[j].empty() && floors[j] == mutations[i + 1]) {
                    vector <command> moves = get_moves(ps[j], position(ps[j].x, mutations[i + 1], ps[j].z));
                    remaining_moves[j].insert(remaining_moves[j].end(), moves.begin(), moves.end());
                }
            }
            
            bool all_grounded = (uf.size(uf.ground()) == full + 1);
            if (all_grounded) {
                for (int j = 0; j < B; j++) {
                    if (!remaining_moves[j].empty() && remaining_moves[j].front().op == FILL) {
                        if (!grounded(uf, ps[j] + remaining_moves[j].front().p1)) {
                            all_grounded = false;
                            break;
                        }
                    }
                }
            }
            
            if (all_grounded == hermonics) {
                int minimum = 0;
                for (int j = 0; j < B; j++) {
                    if (all_voxels[i][j] < all_voxels[i][minimum]) minimum = j;
                }
                hermonics = !hermonics;
                remaining_moves[minimum].push_front(flip());
            }
            
            bool remaining = false;
            for (int j = 0; j < B; j++) {
                if (remaining_moves[j].empty()) {
                    add_trace(energy, traces, wait());
                } else {
                    if (remaining_moves[j].front().op == FILL) {
                        full++;
                        all_voxels[i][j]--;
                        fill(uf, ps[j] + remaining_moves[j].front().p1);
                    }
                    if (remaining_moves[j].front().op == SMOVE) {
                        ps[j] = ps[j] + remaining_moves[j].front().p1;
                    } else if (remaining_moves[j].front().op == LMOVE) {
                        ps[j] = ps[j] + remaining_moves[j].front().p1 + remaining_moves[j].front().p2;
                    }
                    add_trace(energy, traces, remaining_moves[j].front());
                    remaining_moves[j].pop_front();
                }
                if (!remaining_moves[j].empty() || ps[j].y != mutations[i + 1]) {
                    remaining = true;
                } else {
                    all_voxels[i][j] = -1;
                }
            }
            if (!remaining) break;
        }
    }
    
    {
        while (ps.size() > 1) {
            vector <vector <command>> all_moves(ps.size());
            for (int i = ps.size() - 1; i > 0; i -= 2) {
                int dx = 1 - direction, dy = 0, dz = direction;
                all_moves[i] = get_moves(ps[i], position(ps[i - 1].x + dx, mutations.back(), ps[i - 1].z + dz));
                all_moves[i].push_back(fusions(-dx, -dy, -dz));
                reverse(all_moves[i].begin(), all_moves[i].end());
                all_moves[i - 1].push_back(fusionp(dx, dy, dz));
                while (all_moves[i - 1].size() < all_moves[i].size()) all_moves[i - 1].push_back(wait());
            }
            
            while (true) {
                maintain(energy, ps.size());
                
                bool remaining = false;
                for (int i = 0; i < all_moves.size(); i++) {
                    bool fusions = false;
                    if (all_moves[i].empty() || all_moves[i].back().op == WAIT) {
                        if (hermonics) {
                            hermonics = false;
                            add_trace(energy, traces, flip());
                        } else {
                            add_trace(energy, traces, wait());
                        }
                        if (!all_moves[i].empty()) all_moves[i].pop_back();
                    } else {
                        if (all_moves[i].back().op == FUSIONS) fusions = true;
                        add_trace(energy, traces, all_moves[i].back());
                        all_moves[i].pop_back();
                    }
                    if (!all_moves[i].empty()) remaining = true;
                    if (fusions) {
                        ps.erase(ps.begin() + i);
                        all_moves.erase(all_moves.begin() + i);
                        i--;
                    }
                }
                if (!remaining) break;
            }
        }
    }
    
    {
        if (hermonics) {
            maintain(energy, 1);
            hermonics = false;
            add_trace(energy, traces, flip());
        }
        
        vector <command> moves;
        moves = get_moves(ps[0], position(0, mutations.back(), 0));
        ps[0] = position(0, mutations.back(), 0);
        for (int i = 0; i < moves.size(); i++) {
            maintain(energy, 1);
            add_trace(energy, traces, moves[i]);
        }
        moves = get_moves(ps[0], position(0, 0, 0));
        ps[0] = position(0, 0, 0);
        for (int i = 0; i < moves.size(); i++) {
            maintain(energy, 1);
            add_trace(energy, traces, moves[i]);
        }
        
        maintain(energy, 1);
        add_trace(energy, traces, halt());
    }
    
    return make_pair(energy, traces);
}

int main() {
    read_input();
    
    vector <int> mutations = {0};
    for (int i = 1; i < R; i++) {
        if (sum[i][R][R] == 0) {
            mutations.push_back(i);
            break;
        }
        
        if (i - mutations.back() == 4) mutations.push_back(i);
    }
    
    for (int i = 0; i < mutations.size(); i++) {
        for (int j = 0; j < R; j++) {
            for (int k = 0; k < R; k++) {
                voxels_x[i][j][k] = 0;
                voxels_z[i][j][k] = 0;
            }
        }
    }
    
    for (int i = 0; i + 1 < mutations.size(); i++) {
        for (int j = mutations[i]; j < mutations[i + 1]; j++) {
            for (int k = 0; k < R; k++) {
                for (int l = k; l < R; l++) {
                    voxels_x[i][k][l] += sum[j][l + 1][R] - sum[j][k][R] - sum[j][l + 1][0] + sum[j][k][0];
                    voxels_z[i][k][l] += sum[j][R][l + 1] - sum[j][R][k] - sum[j][0][l + 1] + sum[j][0][k];
                }
            }
        }
    }
    
    long long best_energy = 1e18;
    vector <command> best_traces;
    
    for (int B = min(R / 4, MAX_B); B <= min(R, MAX_B); B++) {
        vector <vector <rectangle>> all_bots;
        vector <vector <int>> all_voxels;
        
        for (int i = 0; i + 1 < mutations.size(); i++) {
            for (int j = 0; j <= R; j++) {
                for (int k = 0; k <= B; k++) {
                    dp_x[j][k][0] = 1e9;
                }
            }
            
            dp_x[0][0][0] = 0;
            
            for (int j = 0; j < R; j++) {
                for (int k = 0; k < B; k++) {
                    for (int l = j + 1; l <= R; l++) {
                        if (max(dp_x[j][k][0], voxels_x[i][j][l - 1]) < dp_x[l][k + 1][0]) {
                            dp_x[l][k + 1][0] = max(dp_x[j][k][0], voxels_x[i][j][l - 1]);
                            dp_x[l][k + 1][1] = j;
                        }
                    }
                }
            }
        
            int last = R, b = B;
            vector <rectangle> bots;
            vector <int> voxels;
            
            while (b > 0) {
                int parent = dp_x[last][b][1];
                bots.push_back(rectangle(parent, last - 1, 0, R - 1));
                voxels.push_back(voxels_x[i][parent][last - 1]);
                last = parent;
                b--;
            }
            
            reverse(bots.begin(), bots.end());
            reverse(voxels.begin(), voxels.end());
            
            all_bots.push_back(bots);
            all_voxels.push_back(voxels);
        }
        
        pair<long long, vector <command>> tmp = calc_i(B, mutations, all_bots, all_voxels, 0);
        if (tmp.first < best_energy) {
            best_energy = tmp.first;
            best_traces = tmp.second;
        }
    }
    
    for (int B = min(R / 4, MAX_B); B <= min(R, MAX_B); B++) {
        vector <vector <rectangle>> all_bots;
        vector <vector <int>> all_voxels;
        
        for (int i = 0; i + 1 < mutations.size(); i++) {
            for (int j = 0; j <= R; j++) {
                for (int k = 0; k <= B; k++) {
                    dp_z[j][k][0] = 1e9;
                }
            }
            
            dp_z[0][0][0] = 0;
            
            for (int j = 0; j < R; j++) {
                for (int k = 0; k < B; k++) {
                    for (int l = j + 1; l <= R; l++) {
                        if (max(dp_z[j][k][0], voxels_z[i][j][l - 1]) < dp_z[l][k + 1][0]) {
                            dp_z[l][k + 1][0] = max(dp_z[j][k][0], voxels_z[i][j][l - 1]);
                            dp_z[l][k + 1][1] = j;
                        }
                    }
                }
            }
        
            int last = R, b = B;
            vector <rectangle> bots;
            vector <int> voxels;
            
            while (b > 0) {
                int parent = dp_z[last][b][1];
                bots.push_back(rectangle(0, R - 1, parent, last - 1));
                voxels.push_back(voxels_z[i][parent][last - 1]);
                last = parent;
                b--;
            }
            
            reverse(bots.begin(), bots.end());
            reverse(voxels.begin(), voxels.end());
            
            all_bots.push_back(bots);
            all_voxels.push_back(voxels);
        }
        
        pair<long long, vector <command>> tmp = calc_i(B, mutations, all_bots, all_voxels, 1);
        if (tmp.first < best_energy) {
            best_energy = tmp.first;
            best_traces = tmp.second;
        }
    }
    
    cerr << mutations.size() << endl;
    cerr << best_energy << endl;
    output(best_traces);
    
    return 0;
}
