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
const int MAX_B = 20;
unsigned char buffer[MAX_R * MAX_R * MAX_R / 8];
int R;
bool hermonics;
bool matrix[MAX_R][MAX_R][MAX_R];
bool current[MAX_R][MAX_R][MAX_R];
int sum[MAX_R + 1][MAX_R + 1][MAX_R + 1];
int voxels_x[MAX_R][MAX_R];
int voxels_z[MAX_R][MAX_R];
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

pair<long long, vector <command>> calc(vector <rectangle>& bots, vector <int>& voxels, int direction) {
    long long energy = 0;
    vector <position> ps = {position(0, 0, 0)};
    vector <command> traces;
    
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < R; j++) {
            for (int k = 0; k < R; k++) {
                current[i][j][k] = false;
            }
        }
    }
    
    {
        for (int i = 1; i < bots.size(); i++) {
            maintain(energy, i);
            
            for (int j = 0; j < i - 1; j++) add_trace(energy, traces, wait());
            
            int dx = 1 - direction, dy = 0, dz = direction;
            add_trace(energy, traces, fission(dx, dy, dz, bots.size() - i - 1));
            ps.push_back(position(ps.back().x + dx, ps.back().y + dy, ps.back().z + dz));
        }
    }
    
    {
        vector <vector <command>> all_moves;
        for (int i = 0; i < bots.size(); i++) {
            position target = position(bots[i].x1, 0, bots[i].z1);
            vector <command> moves = get_moves(ps[i], target);
            ps[i] = target;
            reverse(moves.begin(), moves.end());
            all_moves.push_back(moves);
        }
        
        for (int i = 0; ; i++) {
            maintain(energy, bots.size());
            
            bool remaining = false;
            for (int j = 0; j < bots.size(); j++) {
                if (i + j + 1 < bots.size() || all_moves[j].empty()) {
                    add_trace(energy, traces, wait());
                } else {
                    add_trace(energy, traces, all_moves[j].back());
                    all_moves[j].pop_back();
                }
                if (!all_moves[j].empty()) remaining = true;
            }
            if (!remaining) break;
        }
    }
    
    {
        int full = 0;
        UnionFind uf(R);
        vector <int> floors(bots.size());
        vector <deque <command>> remaining_moves(bots.size());
        while (true) {
            maintain(energy, bots.size());
            
            for (int i = 0; i < bots.size(); i++) {
                while (remaining_moves[i].empty() && floors[i] < R - 1) {
                    vector <command> moves = put_floor(bots[i], ps[i], floors[i]++);
                    remaining_moves[i].insert(remaining_moves[i].end(), moves.begin(), moves.end());
                }
                
                if (remaining_moves[i].empty() && floors[i] == R - 1) {
                    floors[i]++;
                    vector <command> moves = get_moves(ps[i], position(ps[i].x, R - 1, ps[i].z));
                    remaining_moves[i].insert(remaining_moves[i].end(), moves.begin(), moves.end());
                }
            }
            
            bool all_grounded = (uf.size(uf.ground()) == full + 1);
            if (all_grounded) {
                for (int i = 0; i < bots.size(); i++) {
                    if (!remaining_moves[i].empty() && remaining_moves[i].front().op == FILL) {
                        if (!grounded(uf, ps[i] + remaining_moves[i].front().p1)) {
                            all_grounded = false;
                            break;
                        }
                    }
                }
            }
            
            if (all_grounded == hermonics) {
                int minimum = 0;
                for (int i = 0; i < bots.size(); i++) {
                    if (voxels[i] < voxels[minimum]) minimum = i;
                }
                hermonics = !hermonics;
                remaining_moves[minimum].push_front(flip());
            }
            
            bool remaining = false;
            for (int i = 0; i < bots.size(); i++) {
                if (remaining_moves[i].empty()) {
                    add_trace(energy, traces, wait());
                } else {
                    if (remaining_moves[i].front().op == FILL) {
                        full++;
                        voxels[i]--;
                        fill(uf, ps[i] + remaining_moves[i].front().p1);
                    }
                    if (remaining_moves[i].front().op == SMOVE) {
                        ps[i] = ps[i] + remaining_moves[i].front().p1;
                    } else if (remaining_moves[i].front().op == LMOVE) {
                        ps[i] = ps[i] + remaining_moves[i].front().p1 + remaining_moves[i].front().p2;
                    }
                    add_trace(energy, traces, remaining_moves[i].front());
                    remaining_moves[i].pop_front();
                }
                if (!remaining_moves[i].empty() || ps[i].y != R - 1) {
                    remaining = true;
                } else {
                    voxels[i] = -1;
                }
            }
            if (!remaining) break;
        }
    }
    
    {
        for (int i = bots.size(); i > 1; i--) {
            vector <command> moves = get_moves(ps[i - 1], position(ps[i - 2].x + 1 - direction, R - 1, ps[i - 2].z + direction));
            for (int j = 0; j < moves.size(); j++) {
                maintain(energy, i);
                
                for (int k = 0; k < i - 1; k++) {
                    if (hermonics) {
                        hermonics = false;
                        add_trace(energy, traces, flip());
                    } else {
                        add_trace(energy, traces, wait());
                    }
                }
                
                add_trace(energy, traces, moves[j]);
            }
            
            maintain(energy, i);
            for (int j = 0; j < i - 2; j++) add_trace(energy, traces, wait());
            int dx = 1 - direction, dy = 0, dz = direction;
            add_trace(energy, traces, fusionp(dx, dy, dz));
            add_trace(energy, traces, fusions(-dx, -dy, -dz));
        }
    }
    
    {
        if (hermonics) {
            maintain(energy, 1);
            hermonics = false;
            add_trace(energy, traces, flip());
        }
        
        vector <command> moves;
        moves = get_moves(ps[0], position(0, R - 1, 0));
        ps[0] = position(0, R - 1, 0);
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
    
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < R; j++) {
            for (int k = j; k < R; k++) {
                voxels_x[j][k] += sum[i][k + 1][R] - sum[i][j][R] - sum[i][k + 1][0] + sum[i][j][0];
                voxels_z[j][k] += sum[i][R][k + 1] - sum[i][R][j] - sum[i][0][k + 1] + sum[i][0][j];
            }
        }
    }
    
    for (int i = 0; i <= R; i++) {
        for (int j = 0; j <= MAX_B; j++) {
            dp_x[i][j][0] = 1e9;
            dp_z[i][j][0] = 1e9;
        }
    }
    
    dp_x[0][0][0] = 0;
    dp_z[0][0][0] = 0;
    
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < MAX_B; j++) {
            for (int k = i + 1; k <= R; k++) {
                if (max(dp_x[i][j][0], voxels_x[i][k - 1]) < dp_x[k][j + 1][0]) {
                    dp_x[k][j + 1][0] = max(dp_x[i][j][0], voxels_x[i][k - 1]);
                    dp_x[k][j + 1][1] = i;
                }
                if (max(dp_z[i][j][0], voxels_z[i][k - 1]) < dp_z[k][j + 1][0]) {
                    dp_z[k][j + 1][0] = max(dp_z[i][j][0], voxels_z[i][k - 1]);
                    dp_z[k][j + 1][1] = i;
                }
            }
        }
    }
    
    long long best_energy = 1e18;
    vector <command> best_traces;
    
    for (int i = min(R / 10, MAX_B); i <= MAX_B; i++) {
        int last = R, b = i;
        vector <rectangle> bots;
        vector <int> voxels;
        
        while (b > 0) {
            int parent = dp_x[last][b][1];
            bots.push_back(rectangle(parent, last - 1, 0, R - 1));
            voxels.push_back(voxels_x[parent][last - 1]);
            last = parent;
            b--;
        }
        
        reverse(bots.begin(), bots.end());
        
        pair<long long, vector <command>> tmp = calc(bots, voxels, 0);
        if (tmp.first < best_energy) {
            best_energy = tmp.first;
            best_traces = tmp.second;
        }
    }
    
    for (int i = min(R / 10, MAX_B); i <= MAX_B; i++) {
        int last = R, b = i;
        vector <rectangle> bots;
        vector <int> voxels;
        
        while (b > 0) {
            int parent = dp_z[last][b][1];
            bots.push_back(rectangle(0, R - 1, parent, last - 1));
            voxels.push_back(parent);
            last = parent;
            b--;
        }
        
        reverse(bots.begin(), bots.end());
        
        pair<long long, vector <command>> tmp = calc(bots, voxels, 1);
        if (tmp.first < best_energy) {
            best_energy = tmp.first;
            best_traces = tmp.second;
        }
    }
    
    cerr << best_energy << endl;
    output(best_traces);
    
    return 0;
}
