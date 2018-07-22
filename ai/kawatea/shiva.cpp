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

bool grounded(const region& box) {
    UnionFind uf(R);
    int voxel = 0;
    static int dx[3] = {1, 0, 0};
    static int dy[3] = {0, 1, 0};
    static int dz[3] = {0, 0, 1};
    
    for (int x = box.p1.x; x <= box.p2.x; x++) {
        for (int y = box.p1.y; y <= box.p2.y; y++) {
            for (int z = box.p1.z; z <= box.p2.z; z++) {
                if (!matrix[x][y][z]) continue;
                
                voxel++;
                
                if (y == 0) uf.unite(uf.find(x, y, z), uf.ground());
                
                for (int i = 0; i < 3; i++) {
                    int nx = x + dx[i], ny = y + dy[i], nz = z + dz[i];
                    if (matrix[nx][ny][nz]) uf.unite(x, y, z, nx, ny, nz);
                }
            }
        }
    }
    
    return uf.size(uf.ground()) == voxel + 1;
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

vector <command> dig(position p) {
    int origin = p.y;
    vector <command> traces;
    
    while (true) {
        bool found = false;
        for (int y = p.y; y > 0; y--) {
            if (matrix[p.x][y - 1][p.z]) {
                found = true;
                vector <command> moves = get_moves(position(0, y - p.y, 0));
                traces.insert(traces.end(), moves.begin(), moves.end());
                traces.push_back(Void(0, -1, 0));
                matrix[p.x][y - 1][p.z] = false;
                p.y = y;
                break;
            }
        }
        if (!found) break;
    }
    
    vector <command> moves = get_moves(position(0, origin - p.y, 0));
    traces.insert(traces.end(), moves.begin(), moves.end());
    
    return traces;
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

vector<command> calc_thin(const region& box, const position& p) {
    vector <command> traces;
    
    {
        vector <command> moves = get_moves(box.p1 + position(-1, 0, -1));
        for (int i = 0; i < moves.size(); i++) {
            traces.push_back(moves[i]);
        }
        
        traces.push_back(Fission(1, 0, 0, 1));
        
        vector <command> moves_x = get_moves(position(p.x + 1, 0, 0));
        for (int i = 0; i < moves_x.size(); i++) {
            traces.push_back(Wait());
            traces.push_back(moves_x[i]);
        }
        
        traces.push_back(Fission(0, 0, 1, 0));
        traces.push_back(Fission(0, 0, 1, 0));
        
        vector <command> moves_z = get_moves(position(0, 0, p.z - 1));
        for (int i = 0; i < moves_z.size(); i++) {
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(moves_z[i]);
            traces.push_back(moves_z[i]);
        }
    }
    
    traces.push_back(GVoid(1, 0, 1, p.x, p.y, p.z));
    traces.push_back(GVoid(-1, 0, 1, -p.x, p.y, p.z));
    traces.push_back(GVoid(-1, 0, 1, -p.x, p.y, -p.z));
    traces.push_back(GVoid(1, 0, 1, p.x, p.y, -p.z));
    
    {
        vector <command> moves_z = get_moves(position(0, 0, -p.z + 1));
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
        
        vector <command> moves_x = get_moves(position(-p.x - 1, 0, 0));
        for (int i = 0; i < moves_x.size(); i++) {
            traces.push_back(Wait());
            traces.push_back(moves_x[i]);
        }
        
        traces.push_back(FusionP(1, 0, 0));
        traces.push_back(FusionS(-1, 0, 0));
        
        vector <command> moves = get_moves(-box.p1 - position(-1, 0, -1));
        for (int i = 0; i < moves.size(); i++) {
            traces.push_back(moves[i]);
        }
    }
    
    traces.push_back(Halt());
    
    return traces;
}

vector <command> calc_small(const region& box, const position& p) {
    vector <command> traces;
    
    {
        vector <command> moves = get_moves(box.p1 + position(-1, 0, -1));
        for (int i = 0; i < moves.size(); i++) {
            traces.push_back(moves[i]);
        }
        
        traces.push_back(Fission(1, 0, 0, 3));
        
        vector <command> moves_x = get_moves(position(p.x + 1, 0, 0));
        for (int i = 0; i < moves_x.size(); i++) {
            traces.push_back(Wait());
            traces.push_back(moves_x[i]);
        }
        
        traces.push_back(Fission(0, 1, 0, 1));
        traces.push_back(Fission(0, 1, 0, 1));
        
        vector <command> moves_y = get_moves(position(0, p.y - 1, 0));
        for (int i = 0; i < moves_y.size(); i++) {
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(moves_y[i]);
            traces.push_back(moves_y[i]);
        }
        
        traces.push_back(Fission(0, 0, 1, 0));
        traces.push_back(Fission(0, 0, 1, 0));
        traces.push_back(Fission(0, 0, 1, 0));
        traces.push_back(Fission(0, 0, 1, 0));
        
        vector <command> moves_z = get_moves(position(0, 0, p.z - 1));
        for (int i = 0; i < moves_z.size(); i++) {
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(moves_z[i]);
            traces.push_back(moves_z[i]);
            traces.push_back(Wait());
            traces.push_back(moves_z[i]);
            traces.push_back(moves_z[i]);
        }
    }
    
    traces.push_back(GVoid(1, 0, 1, p.x, p.y, p.z));
    traces.push_back(GVoid(-1, 0, 1, -p.x, p.y, p.z));
    traces.push_back(GVoid(-1, 0, 1, -p.x, -p.y, p.z));
    traces.push_back(GVoid(-1, 0, 1, -p.x, -p.y, -p.z));
    traces.push_back(GVoid(-1, 0, 1, -p.x, p.y, -p.z));
    traces.push_back(GVoid(1, 0, 1, p.x, -p.y, p.z));
    traces.push_back(GVoid(1, 0, 1, p.x, -p.y, -p.z));
    traces.push_back(GVoid(1, 0, 1, p.x, p.y, -p.z));
    
    {
        vector <command> moves_z = get_moves(position(0, 0, -p.z + 1));
        for (int i = 0; i < moves_z.size(); i++) {
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(moves_z[i]);
            traces.push_back(moves_z[i]);
            traces.push_back(Wait());
            traces.push_back(moves_z[i]);
            traces.push_back(moves_z[i]);
        }
        
        traces.push_back(FusionP(0, 0, 1));
        traces.push_back(FusionP(0, 0, 1));
        traces.push_back(FusionP(0, 0, 1));
        traces.push_back(FusionS(0, 0, -1));
        traces.push_back(FusionS(0, 0, -1));
        traces.push_back(FusionP(0, 0, 1));
        traces.push_back(FusionS(0, 0, -1));
        traces.push_back(FusionS(0, 0, -1));
        
        vector <command> moves_y = get_moves(position(0, -p.y + 1, 0));
        for (int i = 0; i < moves_y.size(); i++) {
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(moves_y[i]);
            traces.push_back(moves_y[i]);
        }
        
        traces.push_back(FusionP(0, 1, 0));
        traces.push_back(FusionP(0, 1, 0));
        traces.push_back(FusionS(0, -1, 0));
        traces.push_back(FusionS(0, -1, 0));
        
        vector <command> moves_x = get_moves(position(-p.x - 1, 0, 0));
        for (int i = 0; i < moves_x.size(); i++) {
            traces.push_back(Wait());
            traces.push_back(moves_x[i]);
        }
        
        traces.push_back(FusionP(1, 0, 0));
        traces.push_back(FusionS(-1, 0, 0));
        
        vector <command> moves = get_moves(-box.p1 - position(-1, 0, -1));
        for (int i = 0; i < moves.size(); i++) {
            traces.push_back(moves[i]);
        }
    }
    
    traces.push_back(Halt());
    
    return traces;
}

vector <command> calc_large(const region& box, const position& p) {
    int cx = (p.x + 29) / 30, cz = (p.z + 29) / 30;
    vector <command> traces;
    
    if (cx <= 1 || cz <= 1) {
        traces = get_moves(box.p1 + position(-1, box.p2.y + 1, -1));
    } else {
        int bots = 1;
        position origin = box.p1 + position(30, box.p2.y + 1, 30);
        
        traces = get_moves(origin);
        
        for (int i = 0; i < cx - 2; i++) {
            for (int j = 0; j < i; j++) traces.push_back(Wait());
            traces.push_back(Fission(1, 0, 0, (cz - 1) * (cx - 2 - i) - 1));
            
            bots++;
            
            vector <command> moves = get_moves(position(29, 0, 0));
            for (int j = 0; j < moves.size(); j++) {
                for (int k = 0; k < bots; k++) {
                    if (k + 1 == bots) {
                        traces.push_back(moves[j]);
                    } else {
                        traces.push_back(Wait());
                    }
                }
            }
        }
        
        for (int i = 0; i < cz - 2; i++) {
            if (i == 0) {
                for (int j = 0; j < bots; j++) traces.push_back(Fission(0, 0, 1, cz - 3 - i));
            } else {
                for (int j = 0; j < cx - 1; j++) traces.push_back(Wait());
                for (int j = cx - 1; j < bots; j++) {
                    if ((j - (cx - 1)) % i == i - 1) {
                        traces.push_back(Fission(0, 0, 1, cz - 3 - i));
                    } else {
                        traces.push_back(Wait());
                    }
                }
            }
            
            bots += cx - 1;
            
            vector <command> moves = get_moves(position(0, 0, 29));
            for (int j = 0; j < moves.size(); j++) {
                for (int k = 0; k < bots; k++) {
                    if (k >= cx - 1 && (k - (cx - 1)) % (i + 1) == i) {
                        traces.push_back(moves[j]);
                    } else {
                        traces.push_back(Wait());
                    }
                }
            }
        }
        
        if (cx == 2 && cz == 2) {
            traces.push_back(Fission(-1, 0, 0, 0));
        } else {
            traces[traces.size() - bots] = Fission(-1, 0, 0, 0);
        }
        
        vector <vector<command>> all_moves;
        for (int i = 0; i < bots; i++) {
            position p;
            if (i < cx - 1) {
                p = origin + position(i * 30, 0, 0);
            } else {
                int dx = cx - 2 - (i - (cx - 1)) / (cz - 2);
                int dz = (i - (cx - 1)) % (cz - 2) + 1;
                p = origin + position(dx * 30, 0, dz * 30);
            }
            vector <command> moves = dig(p);
            reverse(moves.begin(), moves.end());
            all_moves.push_back(moves);
        }
        while (true) {
            bool remaining = false;
            bool updated = false;
            for (int i = 0; i < bots; i++) {
                if (all_moves[i].empty()) {
                    traces.push_back(Wait());
                } else {
                    if (all_moves[i].back().op == VOID) updated = true;
                    traces.push_back(all_moves[i].back());
                    all_moves[i].pop_back();
                }
                if (!all_moves[i].empty()) remaining = true;
            }
            if (!hermonics && updated && !grounded(box)) {
                hermonics = true;
                traces.push_back(Flip());
            } else {
                traces.push_back(Wait());
            }
            if (!remaining) break;
        }
        
        int num = traces.size();
        
        for (int i = cz - 3; i >= 0; i--) {
            vector <command> moves = get_moves(position(0, 0, -29));
            for (int j = 0; j < moves.size(); j++) {
                for (int k = 0; k < bots; k++) {
                    if (k >= cx - 1 && (k - (cx - 1)) % (i + 1) == i) {
                        traces.push_back(moves[j]);
                    } else {
                        traces.push_back(Wait());
                    }
                }
            }
            
            if (i == 0) {
                for (int j = 0; j < bots / 2; j++) traces.push_back(FusionP(0, 0, 1));
                for (int j = 0; j < bots / 2; j++) traces.push_back(FusionS(0, 0, -1));
            } else {
                for (int j = 0; j < cx - 1; j++) traces.push_back(Wait());
                for (int j = cx - 1; j < bots; j++) {
                    if ((j - (cx - 1)) % (i + 1) == i - 1) {
                        traces.push_back(FusionP(0, 0, 1));
                    } else if ((j - (cx - 1)) % (i + 1) == i) {
                        traces.push_back(FusionS(0, 0, -1));
                    } else {
                        traces.push_back(Wait());
                    }
                }
            }
            
            bots -= cx - 1;
        }
        
        for (int i = 0; i < cx - 2; i++) {
            vector <command> moves = get_moves(position(-29, 0, 0));
            for (int j = 0; j < moves.size(); j++) {
                for (int k = 0; k < bots; k++) {
                    if (k + 1 == bots) {
                        traces.push_back(moves[j]);
                    } else {
                        traces.push_back(Wait());
                    }
                }
            }
            
            for (int j = 0; j < bots; j++) {
                if (j + 2 == bots) {
                    traces.push_back(FusionP(1, 0, 0));
                } else if (j + 1 == bots) {
                    traces.push_back(FusionS(-1, 0, 0));
                } else {
                    traces.push_back(Wait());
                }
            }
            
            bots--;
        }
        
        if (cx == 2 && cz == 2) {
            traces.push_back(FusionP(-1, 0, 0));
            traces.push_back(FusionS(1, 0, 0));
        } else {
            traces[num] = FusionP(-1, 0, 0);
            traces.insert(traces.begin() + num + (cx - 1) * (cz - 1), FusionS(1, 0, 0));
        }
        
        {
            vector <command> moves = get_moves(position(-31, 0, -31));
            traces.insert(traces.end(), moves.begin(), moves.end());
        }
    }
    
    {
        traces.push_back(Fission(1, 0, 0, 3));
        
        vector <command> moves_x = get_moves(position(min(30, p.x + 1), 0, 0));
        for (int i = 0; i < moves_x.size(); i++) {
            traces.push_back(Wait());
            traces.push_back(moves_x[i]);
        }
        
        traces.push_back(Fission(0, 0, 1, 1));
        traces.push_back(Fission(0, 0, 1, 1));
        
        vector <command> moves_z1 = get_moves(position(0, 0, min(29, p.z)));
        vector <command> moves_z2 = get_moves(position(0, 0, min(30, p.z + 1)));
        if (moves_z1.size() < moves_z2.size()) moves_z1.push_back(Wait());
        for (int i = 0; i < moves_z1.size(); i++) {
            if (i == 0) {
                traces.push_back(SMove(0, -1, 0));
            } else {
                traces.push_back(Wait());
            }
            traces.push_back(Wait());
            traces.push_back(moves_z2[i]);
            traces.push_back(moves_z1[i]);
        }
        
        traces.push_back(Fission(0, -1, 0, 0));
        traces.push_back(Fission(0, -1, 0, 0));
        traces.push_back(Wait());
        traces.push_back(Fission(0, -1, 0, 0));
        
        vector <command> moves_y1 = get_moves(position(0, -min(30, p.y), 0));
        vector <command> moves_y2 = get_moves(position(0, -min(29, p.y - 1), 0));
        if (moves_y2.size() < moves_y1.size()) moves_y2.push_back(Wait());
        for (int i = 0; i < moves_y1.size(); i++) {
            if (i + 1 == moves_y1.size()) {
                traces.push_back(Fission(0, 1, 0, 0));
            } else {
                traces.push_back(Wait());
            }
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(moves_y1[i]);
            traces.push_back(Wait());
            traces.push_back(moves_y1[i]);
            traces.push_back(moves_y2[i]);
        }
    }
    
    {
        int z = 0;
        while (true) {
            int x = 0;
            int dz = min(29, p.z + 1 - z);
            while (true) {
                int y = p.y + 1;
                int dx = min(29, p.x + 1 - x);
                
                traces.push_back(Wait());
                traces.push_back(Wait());
                traces.push_back(Fission(0, -1, 0, 0));
                traces.push_back(Wait());
                traces.push_back(Wait());
                traces.push_back(Wait());
                traces.push_back(Wait());
                traces.push_back(Wait());
                
                vector <command> moves_fission = get_moves(position(0, -min(30, p.y), 0));
                for (int i = 0; i < moves_fission.size(); i++) {
                    for (int j = 0; j < 9; j++) {
                        if (i == 0 && j == 1) {
                            traces.push_back(SMove(0, -1, 0));
                        } else if (i == 0 && j == 2) {
                            traces.push_back(LMove(-1, 0, 0, 0, 0, -1));
                        } else if (j == 3) {
                            traces.push_back(moves_fission[i]);
                        } else {
                            traces.push_back(Wait());
                        }
                    }
                }
                
                while (true) {
                    int dy = min(30, y - 1);
                    
                    traces.push_back(GVoid(1, 0, 1, dx, -dy, dz));
                    traces.push_back(GVoid(-1, 0, 1, -dx, -dy, dz));
                    traces.push_back(GVoid(0, -1, 0, -dx, -dy, -dz));
                    traces.push_back(GVoid(-1, 0, -1, -dx, dy, -dz));
                    traces.push_back(GVoid(-1, 0, 1, -dx, dy, dz));
                    traces.push_back(GVoid(1, -1, 0, dx, -dy, -dz));
                    traces.push_back(GVoid(1, 0, 0, dx, dy, -dz));
                    traces.push_back(GVoid(1, 0, 1, dx, dy, dz));
                    
                    for (int i = 0; i <= dx; i++) {
                        for (int j = 0; j <= dy; j++) {
                            for (int k = 0; k <= dz; k++) {
                                int nx = box.p1.x + x + i;
                                int ny = y - dy - 1 + j;
                                int nz = box.p1.z + z + k;
                                matrix[nx][ny][nz] = false;
                            }
                        }
                    }
                    if (grounded(box) == hermonics) {
                        hermonics = !hermonics;
                        traces.push_back(Flip());
                    } else {
                        traces.push_back(Wait());
                    }
                    
                    if (y <= 31) break;
                    
                    dy = min(30, y - dy - 1);
                    y -= dy;
                    
                    vector <command> moves_y = get_moves(position(0, -dy, 0));
                    for (int i = 0; i < moves_y.size(); i++) {
                        for (int j = 0; j < 8; j++) traces.push_back(moves_y[i]);
                        traces.push_back(Wait());
                    }
                }
                
                vector <command> moves_y = get_moves(position(0, p.y + 1 - y, 0));
                for (int i = 0; i < moves_y.size(); i++) {
                    for (int j = 0; j < 8; j++) traces.push_back(moves_y[i]);
                    traces.push_back(Wait());
                }
                
                vector <command> moves_fusion = get_moves(position(0, min(30, p.y), 0));
                for (int i = 0; i < moves_fusion.size(); i++) {
                    for (int j = 0; j < 9; j++) {
                        if (i + 1 == moves_fusion.size() && j == 1) {
                            traces.push_back(SMove(0, 1, 0));
                        } else if (i + 1 == moves_fusion.size() && j == 2) {
                            traces.push_back(LMove(1, 0, 0, 0, 0, 1));
                        } else if (j == 3) {
                            traces.push_back(moves_fusion[i]);
                        } else {
                            traces.push_back(Wait());
                        }
                    }
                }
                
                traces.push_back(Wait());
                traces.push_back(Wait());
                traces.push_back(FusionP(0, -1, 0));
                traces.push_back(FusionS(0, 1, 0));
                traces.push_back(Wait());
                traces.push_back(Wait());
                traces.push_back(Wait());
                traces.push_back(Wait());
                traces.push_back(Wait());
                
                if (p.x - x <= 29) break;
                
                dx = min(30, p.x - (x + dx));
                x += dx;
                
                vector <command> moves_x = get_moves(position(dx, 0, 0));
                for (int i = 0; i < moves_x.size(); i++) {
                    for (int j = 0; j < 7; j++) traces.push_back(moves_x[i]);
                    traces.push_back(Wait());
                }
            }
            
            vector <command> moves_x = get_moves(position(-x, 0, 0));
            for (int i = 0; i < moves_x.size(); i++) {
                for (int j = 0; j < 7; j++) traces.push_back(moves_x[i]);
                traces.push_back(Wait());
            }
            
            if (p.z - z <= 29) break;
            
            dz = min(30, p.z - (z + dz));
            z += dz;
            
            vector <command> moves_z = get_moves(position(0, 0, dz));
            for (int i = 0; i < moves_z.size(); i++) {
                for (int j = 0; j < 7; j++) traces.push_back(moves_z[i]);
                traces.push_back(Wait());
            }
        }
        
        vector <command> moves_z = get_moves(position(0, 0, -z));
        for (int i = 0; i < moves_z.size(); i++) {
            for (int j = 0; j < 7; j++) traces.push_back(moves_z[i]);
            traces.push_back(Wait());
        }
    }
    
    {
        
        vector <command> moves_y1 = get_moves(position(0, min(30, p.y), 0));
        vector <command> moves_y2 = get_moves(position(0, min(29, p.y - 1), 0));
        if (moves_y2.size() < moves_y1.size()) moves_y2.push_back(Wait());
        for (int i = 0; i < moves_y1.size(); i++) {
            if (i == 0) {
                traces.push_back(FusionP(0, 1, 0));
            } else {
                traces.push_back(Wait());
            }
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(moves_y1[i]);
            traces.push_back(Wait());
            traces.push_back(moves_y1[i]);
            traces.push_back(moves_y2[i]);
            if (i == 0) traces.push_back(FusionS(0, -1, 0));
        }
        
        traces.push_back(FusionP(0, -1, 0));
        traces.push_back(FusionP(0, -1, 0));
        traces.push_back(Wait());
        traces.push_back(FusionS(0, 1, 0));
        traces.push_back(FusionP(0, -1, 0));
        traces.push_back(FusionS(0, 1, 0));
        traces.push_back(FusionS(0, 1, 0));
        
        vector <command> moves_z1 = get_moves(position(0, 0, -min(29, p.z)));
        vector <command> moves_z2 = get_moves(position(0, 0, -min(30, p.z + 1)));
        if (moves_z1.size() < moves_z2.size()) moves_z1.push_back(Wait());
        for (int i = 0; i < moves_z1.size(); i++) {
            if (i == 0) {
                traces.push_back(SMove(0, 1, 0));
            } else {
                traces.push_back(Wait());
            }
            traces.push_back(Wait());
            traces.push_back(moves_z2[i]);
            traces.push_back(moves_z1[i]);
        }
        
        traces.push_back(FusionP(0, 0, 1));
        traces.push_back(FusionP(0, 0, 1));
        traces.push_back(FusionS(0, 0, -1));
        traces.push_back(FusionS(0, 0, -1));
        
        vector <command> moves_x = get_moves(position(-min(30, p.x + 1), 0, 0));
        for (int i = 0; i < moves_x.size(); i++) {
            traces.push_back(Wait());
            traces.push_back(moves_x[i]);
        }
        
        traces.push_back(FusionP(1, 0, 0));
        traces.push_back(FusionS(-1, 0, 0));
    }
    
    {
        vector <command> moves = get_moves(-box.p1 - position(-1, box.p2.y + 1, -1));
        for (int i = 0; i < moves.size(); i++) traces.push_back(moves[i]);
    }
    
    traces.push_back(Halt());
    
    return traces;
}

vector<command> calc(const region& box) {
    vector <command> traces;
    
    position p = box.p2 - box.p1;
    
    if (p.y == 0) {
        return calc_thin(box, p);
    } else if (p.x <= 30 && p.y <= 30 && p.z <= 30) {
        return calc_small(box, p);
    } else {
        return calc_large(box, p);
    }
}

int main() {
    read_input();
    
    output(calc(bounding_box()));
    
    return 0;
}
