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
    
    position(int x, int y, int z) : x(x), y(y), z(z) {}
};

struct rectangle {
    int x1;
    int x2;
    int z1;
    int z2;
    
    rectangle(int x1, int x2, int z1, int z2) : x1(x1), x2(x2), z1(z1), z2(z2) {}
};

const int MAX_R = 250;
const int MAX_B = 20;
int R;
bool hermonics;
bool matrix[MAX_R][MAX_R][MAX_R];
int sum[MAX_R + 1][MAX_R + 1][MAX_R + 1];
int voxels_x[MAX_R][MAX_R];
int voxels_z[MAX_R][MAX_R];
int dp_x[MAX_R + 1][MAX_B + 1][2];
int dp_z[MAX_R + 1][MAX_B + 1][2];

void read_input() {
    unsigned char buffer[MAX_R * MAX_R * MAX_R / 8];
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

int manhattan(int dx, int dy, int dz) {
    return abs(dx) + abs(dy) + abs(dz);
}

void maintain(long long& energy, int bots) {
    energy += (hermonics ? 30 : 3) * R * R * R + 20 * bots;
}

pair<int, string> halt() {
    return make_pair(0, "halt");
}

pair<int, string> wait() {
    return make_pair(0, "wait");
}

pair<int, string> flip() {
    return make_pair(0, "flip");
}

pair<int, string> smove(int dx, int dy, int dz) {
    stringstream ss;
    ss << "smove " << dx << " " << dy << " " << dz;
    return make_pair(2 * manhattan(dx, dy, dz), ss.str());
}

pair<int, string> lmove(int dx1, int dy1, int dz1, int dx2, int dy2, int dz2) {
    stringstream ss;
    ss << "lmove " << dx1 << " " << dy1 << " " << dz1 << " " << dx2 << " " << dy2 << " " << dz2;
    return make_pair(2 * (manhattan(dx1, dy1, dz1) + 2 + manhattan(dx2, dy2, dz2)), ss.str());
}

pair<int, string> fusionp(int dx, int dy, int dz) {
    stringstream ss;
    ss << "fusionp " << dx << " " << dy << " " << dz;
    return make_pair(-24, ss.str());
}

pair<int, string> fusions(int dx, int dy, int dz) {
    stringstream ss;
    ss << "fusions " << dx << " " << dy << " " << dz;
    return make_pair(0, ss.str());
}

pair<int, string> fission(int dx, int dy, int dz, int m) {
    stringstream ss;
    ss << "fission " << dx << " " << dy << " " << dz << " " << m;
    return make_pair(24, ss.str());
}

pair<int, string> fill(int dx, int dy, int dz) {
    stringstream ss;
    ss << "fill " << dx << " " << dy << " " << dz;
    return make_pair(12, ss.str());
}

void add_trace(long long& energy, vector <string>& traces, const pair<int, string>& trace) {
    energy += trace.first;
    traces.push_back(trace.second);
}

vector <pair<int, string>> get_moves(position& p1, const position& p2) {
    vector <pair<int, string>> traces;
    
    while (p1.y != p2.y) {
        int dy = min(max(p2.y - p1.y, -15), 15);
        p1.y += dy;
        traces.push_back(smove(0, dy, 0));
    }
    
    while (p1.x != p2.x) {
        int dx = min(max(p2.x - p1.x, -15), 15);
        p1.x += dx;
        traces.push_back(smove(dx, 0, 0));
    }
    
    while (p1.z != p2.z) {
        int dz = min(max(p2.z - p1.z, -15), 15);
        p1.z += dz;
        traces.push_back(smove(0, 0, dz));
    }
    
    return traces;
}

vector <pair<int, string>> put_floor(const rectangle& rect, position& p, int floor) {
    vector <pair<int, string>> traces;
    
    for (int i = rect.x1; i <= rect.x2; i++) {
        for (int j = rect.z1; j <= rect.z2; j++) {
            if (matrix[i][floor][j]) {
                vector <pair<int, string>> moves = get_moves(p, position(i, floor + 1, j));
                p.x = i;
                p.y = floor + 1;
                p.z = j;
                traces.insert(traces.end(), moves.begin(), moves.end());
                traces.push_back(fill(0, -1, 0));
            }
        }
    }
    
    return traces;
}

pair<long long, vector <string>> calc(vector <rectangle>& bots, vector <int>& voxels, int direction) {
    long long energy = 0;
    vector <position> ps = {position(0, 0, 0)};
    vector <string> traces;
    
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
        vector <vector <pair<int, string>>> all_moves;
        for (int i = 0; i < bots.size(); i++) {
            vector <pair<int, string>> moves = get_moves(ps[i], position(bots[i].x1, 0, bots[i].z1));
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
        
        for (int i = 0; i < traces.size(); i++) {
            if (traces[traces.size() - i - 1] == "wait") {
                hermonics = true;
                traces[traces.size() - i - 1] = "flip";
                break;
            }
        }
    }
    
    {
        vector <int> floors(bots.size());
        vector <deque <pair<int, string>>> remaining_moves(bots.size());
        while (true) {
            maintain(energy, bots.size());
            
            for (int i = 0; i < bots.size(); i++) {
                while (remaining_moves[i].empty() && floors[i] < R - 1) {
                    vector <pair<int, string>> moves = put_floor(bots[i], ps[i], floors[i]++);
                    remaining_moves[i].insert(remaining_moves[i].end(), moves.begin(), moves.end());
                }
                
                if (floors[i] == R - 1) {
                    floors[i]++;
                    vector <pair<int, string>> moves = get_moves(ps[i], position(ps[i].x, R - 1, ps[i].z));
                    remaining_moves[i].insert(remaining_moves[i].end(), moves.begin(), moves.end());
                }
            }
            
            bool remaining = false;
            for (int i = 0; i < bots.size(); i++) {
                if (remaining_moves[i].empty()) {
                    add_trace(energy, traces, wait());
                } else {
                    add_trace(energy, traces, remaining_moves[i].front());
                    remaining_moves[i].pop_front();
                }
                if (!remaining_moves[i].empty() || ps[i].y != R - 1) remaining = true;
            }
            if (!remaining) break;
        }
    }
    
    {
        for (int i = bots.size(); i > 1; i--) {
            vector <pair<int, string>> moves = get_moves(ps[i - 1], position(ps[i - 2].x + 1 - direction, R - 1, ps[i - 2].z + direction));
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
        
        vector <pair<int, string>> moves;
        moves = get_moves(ps[0], position(0, R - 1, 0));
        for (int i = 0; i < moves.size(); i++) {
            maintain(energy, 1);
            add_trace(energy, traces, moves[i]);
        }
        moves = get_moves(ps[0], position(0, 0, 0));
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
    vector <string> best_traces;
    
    for (int i = 1; i <= MAX_B; i++) {
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
        
        pair<long long, vector <string>> tmp = calc(bots, voxels, 0);
        if (tmp.first < best_energy) {
            best_energy = tmp.first;
            best_traces = tmp.second;
        }
    }
    
    for (int i = 1; i <= MAX_B; i++) {
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
        
        pair<long long, vector <string>> tmp = calc(bots, voxels, 1);
        if (tmp.first < best_energy) {
            best_energy = tmp.first;
            best_traces = tmp.second;
        }
    }
    
    for (int i = 0; i < best_traces.size(); i++) cout << best_traces[i] << endl;
    
    return 0;
}
