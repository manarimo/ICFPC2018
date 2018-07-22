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

position operator-(const position& p) {
    return position(-p.x, -p.y, -p.z);
}

struct rectangle {
    int x1;
    int x2;
    int z1;
    int z2;
    
    rectangle(int x1, int x2, int z1, int z2) : x1(x1), x2(x2), z1(z1), z2(z2) {}
};

struct region {
    position p1;
    position p2;
    
    region(const position& p1, const position& p2) : p1(p1), p2(p2) {}
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
    GVOID
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

const int MAX_R = 250;
const int MAX_B = 40;
unsigned char buffer[MAX_R * MAX_R * MAX_R / 8];
int R;
bool hermonics;
bool matrix[MAX_R][MAX_R][MAX_R];

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
            case FUSIONP:
            ss << "fusionp " << traces[i].p1.x << " " << traces[i].p1.y << " " << traces[i].p1.z;
            cout << ss.str() << endl;
            break;
            case FUSIONS:
            ss << "fusions " << traces[i].p1.x << " " << traces[i].p1.y << " " << traces[i].p1.z;
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

void maintain(long long& energy, int bots) {
    energy += (hermonics ? 30 : 3) * R * R * R + 20 * bots;
}

command Halt() {
    return command(HALT, 0);
}

command Wait() {
    return command(WAIT, 0);
}

command Flip() {
    return command(FLIP, 0);
}

command SMove(const position& p) {
    return command(SMOVE, 2 * manhattan(p), p);
}

command SMove(int dx, int dy, int dz) {
    return SMove(position(dx, dy, dz));
}

command LMove(const position& p1, const position& p2) {
    return command(LMOVE, 2 * (manhattan(p1) + 2 + manhattan(p2)), p1, p2);
}

command LMove(int dx1, int dy1, int dz1, int dx2, int dy2, int dz2) {
    return LMove(position(dx1, dy1, dz1), position(dx2, dy2, dz2));
}

command Fission(const position& p, int m) {
    return command(FISSION, 24, p, m);
}

command Fission(int dx, int dy, int dz, int m) {
    return Fission(position(dx, dy, dz), m);
}

command Fill(const position& p) {
    return command(FILL, 12, p);
}

command Fill(int dx, int dy, int dz) {
    return Fill(position(dx, dy, dz));
}

command Void(const position& p) {
    return command(VOID, -12, p);
}

command Void(int dx, int dy, int dz) {
    return Void(position(dx, dy, dz));
}

command FusionP(const position& p) {
    return command(FUSIONP, -24, p);
}

command FusionP(int dx, int dy, int dz) {
    return FusionP(position(dx, dy, dz));
}

command FusionS(const position& p) {
    return command(FUSIONS, 0, p);
}

command FusionS(int dx, int dy, int dz) {
    return FusionS(position(dx, dy, dz));
}

command GFill(const position& p1, const position& p2) {
    return command(GFILL, 0, p1, p2);
}

command GFill(int dx1, int dy1, int dz1, int dx2, int dy2, int dz2) {
    return GFill(position(dx1, dy1, dz1), position(dx2, dy2, dz2));
}

command GVoid(const position& p1, const position& p2) {
    return command(GVOID, 0, p1, p2);
}

command GVoid(int dx1, int dy1, int dz1, int dx2, int dy2, int dz2) {
    return GVoid(position(dx1, dy1, dz1), position(dx2, dy2, dz2));
}

void add_trace(long long& energy, vector <command>& traces, const command& command) {
    energy += command.energy;
    traces.push_back(command);
}

vector <command> get_moves(position p) {
    bool fx = false, fy = false, fz = false;
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
        traces.push_back(LMove(0, dy, 0, dx, 0, 0));
    } else if (fy && fz) {
        int dy = min(max(p.y, -5), 5);
        int dz = min(max(p.z, -5), 5);
        fy = fz = false;
        p.y -= dy;
        p.z -= dz;
        traces.push_back(LMove(0, dy, 0, 0, 0, dz));
    }
    
    while (p.y != 0) {
        int dy = min(max(p.y, -15), 15);
        p.y -= dy;
        traces.push_back(SMove(0, dy, 0));
    }
    
    if (fx && fz) {
        int dx = min(max(p.x, -5), 5);
        int dz = min(max(p.z, -5), 5);
        p.x -= dx;
        p.z -= dz;
        traces.push_back(LMove(dx, 0, 0, 0, 0, dz));
    }
    
    while (p.x != 0) {
        int dx = min(max(p.x, -15), 15);
        p.x -= dx;
        traces.push_back(SMove(dx, 0, 0));
    }
    
    while (p.z != 0) {
        int dz = min(max(p.z, -15), 15);
        p.z -= dz;
        traces.push_back(SMove(0, 0, dz));
    }
    
    return traces;
}

vector <command> get_moves(const position& p1, const position& p2) {
    return get_moves(p2 - p1);
}

region bounding_box() {
    position p1(R, R, R), p2(0, 0, 0);
    for (int x = 0; x < R; x++) {
        for (int y = 0; y < R; y++) {
            for (int z = 0; z < R; z++) {
                if (matrix[x][y][z]) {
                    p1.x = min(p1.x, x);
                    p1.y = min(p1.y, y);
                    p1.z = min(p1.z, z);
                    p2.x = max(p2.x, x);
                    p2.y = max(p2.y, y);
                    p2.z = max(p2.z, z);
                }
            }
        }
    }
    return region(p1, p2);
}

pair<long long, vector<command>> calc(const region& box) {
    long long energy = 0;
    vector <command> traces;
    
    position p = box.p2 - box.p1;
    
    if (p.y == 0) {
        {
            vector <command> moves = get_moves(box.p1 + position(-1, 0, -1));
            for (int i = 0; i < moves.size(); i++) {
                maintain(energy, 1);
                add_trace(energy, traces, moves[i]);
            }
            
            maintain(energy, 1);
            add_trace(energy, traces, Fission(1, 0, 0, 1));
            
            vector <command> moves_x = get_moves(position(p.x + 1, 0, 0));
            for (int i = 0; i < moves_x.size(); i++) {
                maintain(energy, 2);
                add_trace(energy, traces, Wait());
                add_trace(energy, traces, moves_x[i]);
            }
            
            maintain(energy, 2);
            add_trace(energy, traces, Fission(0, 0, 1, 0));
            add_trace(energy, traces, Fission(0, 0, 1, 0));
            
            vector <command> moves_z = get_moves(position(0, 0, p.z - 1));
            for (int i = 0; i < moves_z.size(); i++) {
                maintain(energy, 4);
                add_trace(energy, traces, Wait());
                add_trace(energy, traces, Wait());
                add_trace(energy, traces, moves_z[i]);
                add_trace(energy, traces, moves_z[i]);
            }
        }
        
        maintain(energy, 4);
        add_trace(energy, traces, GVoid(1, 0, 1, p.x, p.y, p.z));
        add_trace(energy, traces, GVoid(-1, 0, 1, -p.x, p.y, p.z));
        add_trace(energy, traces, GVoid(-1, 0, 1, -p.x, p.y, -p.z));
        add_trace(energy, traces, GVoid(1, 0, 1, p.x, p.y, -p.z));
        
        {
            vector <command> moves_z = get_moves(position(0, 0, -p.z + 1));
            for (int i = 0; i < moves_z.size(); i++) {
                maintain(energy, 4);
                add_trace(energy, traces, Wait());
                add_trace(energy, traces, Wait());
                add_trace(energy, traces, moves_z[i]);
                add_trace(energy, traces, moves_z[i]);
            }
            
            maintain(energy, 4);
            add_trace(energy, traces, FusionP(0, 0, 1));
            add_trace(energy, traces, FusionP(0, 0, 1));
            add_trace(energy, traces, FusionS(0, 0, -1));
            add_trace(energy, traces, FusionS(0, 0, -1));
            
            vector <command> moves_x = get_moves(position(-p.x - 1, 0, 0));
            for (int i = 0; i < moves_x.size(); i++) {
                maintain(energy, 2);
                add_trace(energy, traces, Wait());
                add_trace(energy, traces, moves_x[i]);
            }
            
            maintain(energy, 2);
            add_trace(energy, traces, FusionP(1, 0, 0));
            add_trace(energy, traces, FusionS(-1, 0, 0));
            
            vector <command> moves = get_moves(-box.p1 - position(-1, 0, -1));
            for (int i = 0; i < moves.size(); i++) {
                maintain(energy, 1);
                add_trace(energy, traces, moves[i]);
            }
        }
        
        maintain(energy, 1);
        add_trace(energy, traces, Halt());
    } else if (p.x <= 30 && p.y <= 30 && p.z <= 30) {
        {
            vector <command> moves = get_moves(box.p1 + position(-1, 0, -1));
            for (int i = 0; i < moves.size(); i++) {
                maintain(energy, 1);
                add_trace(energy, traces, moves[i]);
            }
            
            maintain(energy, 1);
            add_trace(energy, traces, Fission(1, 0, 0, 3));
            
            vector <command> moves_x = get_moves(position(p.x + 1, 0, 0));
            for (int i = 0; i < moves_x.size(); i++) {
                maintain(energy, 2);
                add_trace(energy, traces, Wait());
                add_trace(energy, traces, moves_x[i]);
            }
            
            maintain(energy, 2);
            add_trace(energy, traces, Fission(0, 1, 0, 1));
            add_trace(energy, traces, Fission(0, 1, 0, 1));
            
            vector <command> moves_y = get_moves(position(0, p.y - 1, 0));
            for (int i = 0; i < moves_y.size(); i++) {
                maintain(energy, 4);
                add_trace(energy, traces, Wait());
                add_trace(energy, traces, Wait());
                add_trace(energy, traces, moves_y[i]);
                add_trace(energy, traces, moves_y[i]);
            }
            
            maintain(energy, 4);
            add_trace(energy, traces, Fission(0, 0, 1, 0));
            add_trace(energy, traces, Fission(0, 0, 1, 0));
            add_trace(energy, traces, Fission(0, 0, 1, 0));
            add_trace(energy, traces, Fission(0, 0, 1, 0));
            
            vector <command> moves_z = get_moves(position(0, 0, p.z - 1));
            for (int i = 0; i < moves_z.size(); i++) {
                maintain(energy, 8);
                add_trace(energy, traces, Wait());
                add_trace(energy, traces, Wait());
                add_trace(energy, traces, Wait());
                add_trace(energy, traces, moves_z[i]);
                add_trace(energy, traces, moves_z[i]);
                add_trace(energy, traces, Wait());
                add_trace(energy, traces, moves_z[i]);
                add_trace(energy, traces, moves_z[i]);
            }
        }
        
        maintain(energy, 8);
        add_trace(energy, traces, GVoid(1, 0, 1, p.x, p.y, p.z));
        add_trace(energy, traces, GVoid(-1, 0, 1, -p.x, p.y, p.z));
        add_trace(energy, traces, GVoid(-1, 0, 1, -p.x, -p.y, p.z));
        add_trace(energy, traces, GVoid(-1, 0, 1, -p.x, -p.y, -p.z));
        add_trace(energy, traces, GVoid(-1, 0, 1, -p.x, p.y, -p.z));
        add_trace(energy, traces, GVoid(1, 0, 1, p.x, -p.y, p.z));
        add_trace(energy, traces, GVoid(1, 0, 1, p.x, -p.y, -p.z));
        add_trace(energy, traces, GVoid(1, 0, 1, p.x, p.y, -p.z));
        
        {
            vector <command> moves_z = get_moves(position(0, 0, -p.z + 1));
            for (int i = 0; i < moves_z.size(); i++) {
                maintain(energy, 8);
                add_trace(energy, traces, Wait());
                add_trace(energy, traces, Wait());
                add_trace(energy, traces, Wait());
                add_trace(energy, traces, moves_z[i]);
                add_trace(energy, traces, moves_z[i]);
                add_trace(energy, traces, Wait());
                add_trace(energy, traces, moves_z[i]);
                add_trace(energy, traces, moves_z[i]);
            }
            
            maintain(energy, 8);
            add_trace(energy, traces, FusionP(0, 0, 1));
            add_trace(energy, traces, FusionP(0, 0, 1));
            add_trace(energy, traces, FusionP(0, 0, 1));
            add_trace(energy, traces, FusionS(0, 0, -1));
            add_trace(energy, traces, FusionS(0, 0, -1));
            add_trace(energy, traces, FusionP(0, 0, 1));
            add_trace(energy, traces, FusionS(0, 0, -1));
            add_trace(energy, traces, FusionS(0, 0, -1));
            
            vector <command> moves_y = get_moves(position(0, -p.y + 1, 0));
            for (int i = 0; i < moves_y.size(); i++) {
                maintain(energy, 4);
                add_trace(energy, traces, Wait());
                add_trace(energy, traces, Wait());
                add_trace(energy, traces, moves_y[i]);
                add_trace(energy, traces, moves_y[i]);
            }
            
            maintain(energy, 4);
            add_trace(energy, traces, FusionP(0, 1, 0));
            add_trace(energy, traces, FusionP(0, 1, 0));
            add_trace(energy, traces, FusionS(0, -1, 0));
            add_trace(energy, traces, FusionS(0, -1, 0));
            
            vector <command> moves_x = get_moves(position(-p.x - 1, 0, 0));
            for (int i = 0; i < moves_x.size(); i++) {
                maintain(energy, 2);
                add_trace(energy, traces, Wait());
                add_trace(energy, traces, moves_x[i]);
            }
            
            maintain(energy, 2);
            add_trace(energy, traces, FusionP(1, 0, 0));
            add_trace(energy, traces, FusionS(-1, 0, 0));
            
            vector <command> moves = get_moves(-box.p1 - position(-1, 0, -1));
            for (int i = 0; i < moves.size(); i++) {
                maintain(energy, 1);
                add_trace(energy, traces, moves[i]);
            }
        }
        
        maintain(energy, 1);
        add_trace(energy, traces, Halt());
    }
    
    return make_pair(energy, traces);
}

int main() {
    read_input();
    
    pair<long long, vector <command>> best = calc(bounding_box());
    cerr << best.first << endl;
    output(best.second);
    
    return 0;
}
