#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <set>
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

void read_input(const string& file) {
    ifstream ifs(file, ios::in | ios::binary);
    ifs.read((char*)buffer, 1);
    R = buffer[0];
    ifs.read((char*)buffer, (R * R * R + 7) / 8);
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

vector <command> get_minus_moves(position p) {
    bool fx = false, fy = false, fz = false;
    vector <command> traces;
    vector <command> last;
    
    if (abs(p.x) % 15 > 0 && abs(p.x) % 15 <= 5) fx = true;
    if (abs(p.y) % 15 > 0 && abs(p.y) % 15 <= 5) fy = true;
    if (abs(p.z) % 15 > 0 && abs(p.z) % 15 <= 5) fz = true;
    
    if (fx && fz) {
        int dx = min(max(p.x, -5), 5);
        int dz = min(max(p.z, -5), 5);
        fx = fz = false;
        p.x -= dx;
        p.z -= dz;
        traces.push_back(LMove(dx, 0, 0, 0, 0, dz));
    }
    
    if (fy && fx) {
        int dy = min(max(p.y, -5), 5);
        int dx = min(max(p.x, -5), 5);
        fy = fx = false;
        p.y -= dy;
        p.x -= dx;
        last.push_back(LMove(dx, 0, 0, 0, dy, 0));
    } else if (fy && fz) {
        int dy = min(max(p.y, -5), 5);
        int dz = min(max(p.z, -5), 5);
        fy = fz = false;
        p.y -= dy;
        p.z -= dz;
        last.push_back(LMove(0, 0, dz, 0, dy, 0));
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
    
    while (p.y != 0) {
        int dy = min(max(p.y, -15), 15);
        p.y -= dy;
        traces.push_back(SMove(0, dy, 0));
    }
    
    for (int i = 0; i < last.size(); i++) traces.push_back(last[i]);
    
    return traces;
}

vector <command> get_moves(position p) {
    if (p.y < 0) return get_minus_moves(p);
    
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

vector <command> calc() {
    vector <command> traces;
    
    for (int y = 0; y < R; y++) {
        fprintf(stderr, "y = %d\n", y);
        fprintf(stderr, "   ");
        for (int z = 0; z < R; z++) fprintf(stderr, " %02d", z);
        fprintf(stderr, "\n");
        for (int x = 0; x < R; x++) {
            fprintf(stderr, "%02d", x);
            for (int z = 0; z < R; z++) {
                fprintf(stderr, "  %d", matrix[x][y][z]);
            }
            fprintf(stderr, "\n");
        }
        fprintf(stderr, "\n");
    }
    
    fprintf(stderr, "READY\n");
    fflush(stderr);
    
    position p1, p2, p3, p4;
    
    while (true) {
        string op;
        int x1, y1, z1, x2, y2, z2;
        
        cin >> op;
        
        if (op == "start") {
            cin >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
            
            traces = get_moves(position(x1, y1, z1));
            
            traces.push_back(Fission(1, 0, 0, 1));
            vector <command> moves_x = get_moves(position(x2 - x1 - 1, 0, 0));
            for (int i = 0; i < moves_x.size(); i++) {
                traces.push_back(Wait());
                traces.push_back(moves_x[i]);
            }
            
            traces.push_back(Fission(0, 0, 1, 0));
            traces.push_back(Fission(0, 0, 1, 0));
            vector <command> moves_z = get_moves(position(0, 0, z2 - z1 - 1));
            for (int i = 0; i < moves_z.size(); i++) {
                traces.push_back(Wait());
                traces.push_back(Wait());
                traces.push_back(moves_z[i]);
                traces.push_back(moves_z[i]);
            }
            
            p1 = position(x1, y1, z1);
            p2 = position(x2, y1, z1);
            p3 = position(x2, y1, z2);
            p4 = position(x1, y1, z2);
        } else if (op == "move") {
            cin >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
            
            position np1 = position(x1, y1, z1);
            position np2 = position(x2, y1, z1);
            position np3 = position(x2, y1, z2);
            position np4 = position(x1, y1, z2);
            
            vector <vector <command>> all_moves;
            all_moves.push_back(get_moves(p1, np1));
            all_moves.push_back(get_moves(p2, np2));
            all_moves.push_back(get_moves(p3, np3));
            all_moves.push_back(get_moves(p4, np4));
            
            while (true) {
                bool remaining = false;
                for (int i = 0; i < all_moves.size(); i++) {
                    if (all_moves[i].empty()) {
                        traces.push_back(Wait());
                    } else {
                        traces.push_back(all_moves[i].front());
                        all_moves[i].erase(all_moves[i].begin());
                    }
                    if (!all_moves[i].empty()) remaining = true;
                }
                if (!remaining) break;
            }
            
            p1 = np1;
            p2 = np2;
            p3 = np3;
            p4 = np4;
        } else if (op == "gfill") {
            cin >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
            
            int dx = x2 - x1;
            int dz = z2 - z1;
            
            position np1 = position(x1, y1, z1);
            position np2 = position(x2, y1, z1);
            position np3 = position(x2, y1, z2);
            position np4 = position(x1, y1, z2);
            
            if (dx == 0 && dz == 0) {
                traces.push_back(Fill(np1 - p1));
                traces.push_back(Wait());
                traces.push_back(Wait());
                traces.push_back(Wait());
            } else if (dx == 0) {
                traces.push_back(GFill(np1 - p1, position(0, 0, dz)));
                traces.push_back(Wait());
                traces.push_back(Wait());
                traces.push_back(GFill(np4 - p4, position(0, 0, -dz)));
            } else if (dz == 0) {
                traces.push_back(GFill(np1 - p1, position(dx, 0, 0)));
                traces.push_back(GFill(np2 - p2, position(-dx, 0, 0)));
                traces.push_back(Wait());
                traces.push_back(Wait());
            } else {
                traces.push_back(GFill(np1 - p1, position(dx, 0, dz)));
                traces.push_back(GFill(np2 - p2, position(-dx, 0, dz)));
                traces.push_back(GFill(np3 - p3, position(-dx, 0, -dz)));
                traces.push_back(GFill(np4 - p4, position(dx, 0, -dz)));
            }
        } else if (op == "gvoid") {
            cin >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
            
            int dx = x2 - x1;
            int dz = z2 - z1;
            
            position np1 = position(x1, y1, z1);
            position np2 = position(x2, y1, z1);
            position np3 = position(x2, y1, z2);
            position np4 = position(x1, y1, z2);
            
            if (dx == 0) {
                traces.push_back(GVoid(np1 - p1, position(0, 0, dz)));
                traces.push_back(Wait());
                traces.push_back(Wait());
                traces.push_back(GVoid(np4 - p4, position(0, 0, -dz)));
            } else {
                traces.push_back(GVoid(np1 - p1, position(dx, 0, dz)));
                traces.push_back(GVoid(np2 - p2, position(-dx, 0, dz)));
                traces.push_back(GVoid(np3 - p3, position(-dx, 0, -dz)));
                traces.push_back(GVoid(np4 - p4, position(dx, 0, -dz)));
            }
        } else if (op == "end") {
            vector <command> moves_z = get_moves(position(0, 0, -(p3.z - p1.z - 1)));
            for (int i = 0; i < moves_z.size(); i++) {
                traces.push_back(Wait());
                traces.push_back(Wait());
                traces.push_back(moves_z[i]);
                traces.push_back(moves_z[i]);
            }
            traces.push_back(FusionP(0, 0, 1));
            traces.push_back(FusionP(0, 0, 1));
            traces.push_back(FusionS(0, 0, -1));
            traces.push_back(FusionS(0, 0, -1));
            
            vector <command> moves_x = get_moves(position(-(p3.x - p1.x - 1), 0, 0));
            for (int i = 0; i < moves_x.size(); i++) {
                traces.push_back(Wait());
                traces.push_back(moves_x[i]);
            }
            traces.push_back(FusionP(1, 0, 0));
            traces.push_back(FusionS(-1, 0, 0));
            
            vector <command> moves = get_moves(-p1);
            for (int i = 0; i < moves.size(); i++) traces.push_back(moves[i]);
            
            traces.push_back(Halt());
            
            break;
        }
    }
    
    return traces;
}

int main(int argc, char **argv) {
    read_input(argv[1]);
    
    output(calc());
    
    return 0;
}
