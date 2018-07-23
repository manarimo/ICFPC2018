#include <cstdio>
#include <cstdlib>
#include <iostream>
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

vector <command> calc(int R, int W) {
    int c = (R + W - 1) / W;
    vector <command> traces;
    
    {
        traces.push_back(Fission(1, 0, 0, 7));
        
        vector <command> moves = get_moves(position(R - W - 3, 0, 0));
        for (int i = 0; i < moves.size(); i++) {
            traces.push_back(Wait());
            traces.push_back(moves[i]);
        }
    }
    
    {
        traces.push_back(Fission(1, 0, 0, 3));
        traces.push_back(Fission(1, 0, 0, 3));
        
        vector <command> moves = get_moves(position(W, 0, 0));
        for (int i = 0; i < moves.size(); i++) {
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(moves[i]);
            traces.push_back(moves[i]);
        }
    }
    
    {
        traces.push_back(Fission(0, 1, 0, 1));
        traces.push_back(Fission(0, 1, 0, 1));
        traces.push_back(Fission(0, 1, 0, 1));
        traces.push_back(Fission(0, 1, 0, 1));
        
        vector <command> moves = get_moves(position(0, W - 2, 0));
        for (int i = 0; i < moves.size(); i++) {
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(moves[i]);
            traces.push_back(moves[i]);
            traces.push_back(Wait());
            traces.push_back(moves[i]);
            traces.push_back(moves[i]);
        }
    }
    
    {
        traces.push_back(Fission(0, 0, 1, 0));
        traces.push_back(Fission(0, 0, 1, 0));
        traces.push_back(Fission(0, 0, 1, 0));
        traces.push_back(Fission(0, 0, 1, 0));
        traces.push_back(Fission(0, 0, 1, 0));
        traces.push_back(Fission(0, 0, 1, 0));
        traces.push_back(Fission(0, 0, 1, 0));
        traces.push_back(Fission(0, 0, 1, 0));
        
        vector <command> moves = get_moves(position(0, 0, W));
        for (int i = 0; i < moves.size(); i++) {
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(moves[i]);
            traces.push_back(moves[i]);
            traces.push_back(Wait());
            traces.push_back(moves[i]);
            traces.push_back(moves[i]);
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(moves[i]);
            traces.push_back(moves[i]);
            traces.push_back(Wait());
            traces.push_back(moves[i]);
            traces.push_back(moves[i]);
        }
    }
    
    {
        int z = 0;
        for (int i = 0; i < c; i++) {
            int y = 0;
            for (int j = 0; j < c; j++) {
                traces.push_back(GFill(1, 0, 1, W - 1, W - 1, W - 1));
                traces.push_back(GFill(1, 0, 1, W - 1, W - 1, W - 1));
                traces.push_back(GFill(-1, 0, 1, -(W - 1), W - 1, W - 1));
                traces.push_back(GFill(-1, 0, 1, -(W - 1), -(W - 1), W - 1));
                traces.push_back(GFill(-1, 0, -1, -(W - 1), -(W - 1), -(W - 1)));
                traces.push_back(GFill(-1, 0, -1, -(W - 1), W - 1, -(W - 1)));
                traces.push_back(GFill(1, 0, 1, W - 1, -(W - 1), W - 1));
                traces.push_back(GFill(1, 0, -1, W - 1, -(W - 1), -(W - 1)));
                traces.push_back(GFill(1, 0, -1, W - 1, W - 1, -(W - 1)));
                traces.push_back(GFill(-1, 0, 1, -(W - 1), W - 1, W - 1));
                traces.push_back(GFill(-1, 0, 1, -(W - 1), -(W - 1), W - 1));
                traces.push_back(GFill(-1, 0, -1, -(W - 1), -(W - 1), -(W - 1)));
                traces.push_back(GFill(-1, 0, -1, -(W - 1), W - 1, -(W - 1)));
                traces.push_back(GFill(1, 0, 1, W - 1, -(W - 1), W - 1));
                traces.push_back(GFill(1, 0, -1, W - 1, -(W - 1), -(W - 1)));
                traces.push_back(GFill(1, 0, -1, W - 1, W - 1, -(W - 1)));
                
                if (j + 1 == c) continue;
                
                int dy = min(W, R - 2 - y - W);
                y += dy;
                vector <command> moves = get_moves(position(0, dy, 0));
                for (int k = 0; k < moves.size(); k++) {
                    for (int l = 0; l < 16; l++) traces.push_back(moves[k]);
                }
            }
            
            if (i + 1 == c) continue;
            
            int dz = min(W, R - 2 - z - W);
            z += dz;
            vector <command> moves = get_moves(position(0, -y, dz));
            for (int j = 0; j < moves.size(); j++) {
                for (int k = 0; k < 16; k++) traces.push_back(moves[j]);
            }
        }
    }
    
    {
        vector <command> moves1 = get_moves(position(0, 1, 0));
        vector <command> moves2 = get_moves(position(0, W - 1, 0));
        if (moves1.size() < moves2.size()) moves1.push_back(Wait());
        for (int i = 0; i < moves1.size(); i++) {
            traces.push_back(moves2[i]);
            traces.push_back(moves2[i]);
            traces.push_back(moves2[i]);
            traces.push_back(moves1[i]);
            traces.push_back(moves1[i]);
            traces.push_back(moves2[i]);
            traces.push_back(moves1[i]);
            traces.push_back(moves1[i]);
            traces.push_back(moves2[i]);
            traces.push_back(moves2[i]);
            traces.push_back(moves1[i]);
            traces.push_back(moves1[i]);
            traces.push_back(moves2[i]);
            traces.push_back(moves1[i]);
            traces.push_back(moves1[i]);
            traces.push_back(moves2[i]);
        }
        
        traces.push_back(FusionS(0, 1, 0));
        traces.push_back(FusionS(0, 1, 0));
        traces.push_back(FusionS(0, 1, 0));
        traces.push_back(FusionP(0, -1, 0));
        traces.push_back(FusionP(0, -1, 0));
        traces.push_back(FusionS(0, 1, 0));
        traces.push_back(FusionP(0, -1, 0));
        traces.push_back(FusionP(0, -1, 0));
        traces.push_back(FusionS(0, 1, 0));
        traces.push_back(FusionS(0, 1, 0));
        traces.push_back(FusionP(0, -1, 0));
        traces.push_back(FusionP(0, -1, 0));
        traces.push_back(FusionS(0, 1, 0));
        traces.push_back(FusionP(0, -1, 0));
        traces.push_back(FusionP(0, -1, 0));
        traces.push_back(FusionS(0, 1, 0));
    }
    
    {
        vector <command> moves = get_moves(position(0, 0, -R + W + 2));
        for (int i = 0; i < moves.size(); i++) {
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(moves[i]);
            traces.push_back(moves[i]);
            traces.push_back(moves[i]);
            traces.push_back(moves[i]);
        }
    }
    
    {
        vector <command> moves1 = get_moves(position(-R + W + 2, 0, 0));
        vector <command> moves2 = get_moves(position(-R + W + 3, 0, 0));
        if (moves2.size() < moves1.size()) moves2.push_back(Wait());
        for (int i = 0; i < moves1.size(); i++) {
            traces.push_back(moves1[i]);
            traces.push_back(moves1[i]);
            traces.push_back(moves2[i]);
            traces.push_back(moves2[i]);
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(Wait());
        }
        traces[traces.size() - 1] = traces[traces.size() - 2] = SMove(1, 0, 0);
    }
    
    {
        vector <command> moves1 = get_moves(position(min(W - 1, R - W * 2 - 3), 0, 0));
        vector <command> moves2 = get_moves(position(W, 0, 0));
        if (moves1.size() < moves2.size()) moves1.push_back(Wait());
        for (int i = 0; i < moves1.size(); i++) {
            traces.push_back(moves1[i]);
            traces.push_back(moves1[i]);
            traces.push_back(moves2[i]);
            traces.push_back(moves2[i]);
            traces.push_back(moves1[i]);
            traces.push_back(moves1[i]);
            traces.push_back(moves2[i]);
            traces.push_back(moves2[i]);
        }
    }
    
    {
        vector <command> moves1 = get_moves(position(0, -R + W + 1, 0));
        for (int i = 0; i < moves1.size(); i++) {
            for (int j = 0; j < 8; j++) traces.push_back(moves1[i]);
        }
        
        traces.push_back(Fission(0, -1, 0, 0));
        traces.push_back(Fission(0, -1, 0, 0));
        traces.push_back(Fission(0, -1, 0, 0));
        traces.push_back(Fission(0, -1, 0, 0));
        traces.push_back(Fission(0, -1, 0, 0));
        traces.push_back(Fission(0, -1, 0, 0));
        traces.push_back(Fission(0, -1, 0, 0));
        traces.push_back(Fission(0, -1, 0, 0));
        
        vector <command> moves2 = get_moves(position(0, -W + 2, 0));
        for (int i = 0; i < moves2.size(); i++) {
            traces.push_back(moves2[i]);
            traces.push_back(moves2[i]);
            traces.push_back(moves2[i]);
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(moves2[i]);
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(moves2[i]);
            traces.push_back(moves2[i]);
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(moves2[i]);
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(moves2[i]);
        }
    }
    
    int x = W + 1;
    {
        while (true) {
            int y = 0;
            int dx = min(W - 1, R - W - 2 - x);
            for (int j = 0; j < c; j++) {
                traces.push_back(GFill(0, 0, 1, dx, W - 1, W - 1));
                traces.push_back(GFill(0, 0, 1, dx, W - 1, W - 1));
                traces.push_back(GFill(0, 0, 1, -dx, W - 1, W - 1));
                traces.push_back(GFill(0, 0, 1, -dx, -(W - 1), W - 1));
                traces.push_back(GFill(0, 0, -1, -dx, -(W - 1), -(W - 1)));
                traces.push_back(GFill(0, 0, -1, -dx, W - 1, -(W - 1)));
                traces.push_back(GFill(0, 0, 1, dx, -(W - 1), W - 1));
                traces.push_back(GFill(0, 0, -1, dx, -(W - 1), -(W - 1)));
                traces.push_back(GFill(0, 0, -1, dx, W - 1, -(W - 1)));
                traces.push_back(GFill(0, 0, 1, -dx, W - 1, W - 1));
                traces.push_back(GFill(0, 0, 1, -dx, -(W - 1), W - 1));
                traces.push_back(GFill(0, 0, -1, -dx, -(W - 1), -(W - 1)));
                traces.push_back(GFill(0, 0, -1, -dx, W - 1, -(W - 1)));
                traces.push_back(GFill(0, 0, 1, dx, -(W - 1), W - 1));
                traces.push_back(GFill(0, 0, -1, dx, -(W - 1), -(W - 1)));
                traces.push_back(GFill(0, 0, -1, dx, W - 1, -(W - 1)));
                
                if (j + 1 == c) continue;
                
                int dy = min(W, R - 2 - y - W);
                y += dy;
                vector <command> moves = get_moves(position(0, dy, 0));
                for (int k = 0; k < moves.size(); k++) {
                    for (int l = 0; l < 16; l++) traces.push_back(moves[k]);
                }
            }
            
            if (x + dx == R - W - 2) break;
            
            dx = min(W, R - W - 2 - x - dx);
            x += dx;
            vector <command> moves = get_moves(position(dx, -y, 0));
            for (int j = 0; j < moves.size(); j++) {
                for (int k = 0; k < 16; k++) traces.push_back(moves[j]);
            }
        }
    }
    
    {
        vector <command> moves1 = get_moves(position(0, 1, 0));
        vector <command> moves2 = get_moves(position(0, W - 1, 0));
        if (moves1.size() < moves2.size()) moves1.push_back(Wait());
        for (int i = 0; i < moves1.size(); i++) {
            traces.push_back(moves2[i]);
            traces.push_back(moves2[i]);
            traces.push_back(moves2[i]);
            traces.push_back(moves1[i]);
            traces.push_back(moves1[i]);
            traces.push_back(moves2[i]);
            traces.push_back(moves1[i]);
            traces.push_back(moves1[i]);
            traces.push_back(moves2[i]);
            traces.push_back(moves2[i]);
            traces.push_back(moves1[i]);
            traces.push_back(moves1[i]);
            traces.push_back(moves2[i]);
            traces.push_back(moves1[i]);
            traces.push_back(moves1[i]);
            traces.push_back(moves2[i]);
        }
        
        traces.push_back(FusionS(0, 1, 0));
        traces.push_back(FusionS(0, 1, 0));
        traces.push_back(FusionS(0, 1, 0));
        traces.push_back(FusionP(0, -1, 0));
        traces.push_back(FusionP(0, -1, 0));
        traces.push_back(FusionS(0, 1, 0));
        traces.push_back(FusionP(0, -1, 0));
        traces.push_back(FusionP(0, -1, 0));
        traces.push_back(FusionS(0, 1, 0));
        traces.push_back(FusionS(0, 1, 0));
        traces.push_back(FusionP(0, -1, 0));
        traces.push_back(FusionP(0, -1, 0));
        traces.push_back(FusionS(0, 1, 0));
        traces.push_back(FusionP(0, -1, 0));
        traces.push_back(FusionP(0, -1, 0));
        traces.push_back(FusionS(0, 1, 0));
    }
    
    {
        vector <command> moves = get_moves(position(0, 0, -W));
        for (int i = 0; i < moves.size(); i++) {
            traces.push_back(Wait());
            traces.push_back(moves[i]);
            traces.push_back(Wait());
            traces.push_back(moves[i]);
            traces.push_back(Wait());
            traces.push_back(moves[i]);
            traces.push_back(Wait());
            traces.push_back(moves[i]);
        }
        
        traces.push_back(FusionP(0, 0, 1));
        traces.push_back(FusionS(0, 0, -1));
        traces.push_back(FusionP(0, 0, 1));
        traces.push_back(FusionS(0, 0, -1));
        traces.push_back(FusionP(0, 0, 1));
        traces.push_back(FusionS(0, 0, -1));
        traces.push_back(FusionP(0, 0, 1));
        traces.push_back(FusionS(0, 0, -1));
    }
    
    {
        vector <command> moves = get_moves(position(-R + W + 3 + x, 0, 0));
        for (int i = 0; i < moves.size(); i++) {
            traces.push_back(moves[i]);
            traces.push_back(Wait());
            traces.push_back(moves[i]);
            traces.push_back(Wait());
        }
        
        traces.push_back(FusionS(-1, 0, 0));
        traces.push_back(FusionP(1, 0, 0));
        traces.push_back(FusionS(-1, 0, 0));
        traces.push_back(FusionP(1, 0, 0));
    }
    
    {
        vector <command> moves = get_moves(position(0, 0, -R + W + 3));
        for (int i = 0; i < moves.size(); i++) {
            traces.push_back(moves[i]);
            traces.push_back(Wait());
        }
        
        traces.push_back(FusionS(0, 0, -1));
        traces.push_back(FusionP(0, 0, 1));
    }
    
    {
        traces.push_back(Fission(0, 0, 1, 0));
        traces.push_back(FusionP(0, 0, -1));
        traces.push_back(FusionS(0, 0, 1));
        
        vector <command> moves = get_moves(position(x, R - 2, 1), position(W + 1, R - 2, W + 1));
        for (int i = 0; i < moves.size(); i++) traces.push_back(moves[i]);
    }
    
    {
        vector <command> moves = get_moves(position(0, -R + 3, 0));
        for (int i = 0; i < moves.size(); i++) traces.push_back(moves[i]);
    }
    
    {
        traces.push_back(Fission(1, 0, 0, 1));
        
        vector <command> moves_x = get_moves(position(min(W - 2, R - W * 2 - 4), 0, 0));
        for (int i = 0; i < moves_x.size(); i++) {
            traces.push_back(Wait());
            traces.push_back(moves_x[i]);
        }
        
        traces.push_back(Fission(0, 0, 1, 0));
        traces.push_back(Fission(0, 0, 1, 0));
        
        vector <command> moves_z = get_moves(position(0, 0, min(W - 2, R - W * 2 - 4)));
        for (int i = 0; i < moves_z.size(); i++) {
            traces.push_back(Wait());
            traces.push_back(Wait());
            traces.push_back(moves_z[i]);
            traces.push_back(moves_z[i]);
        }
    }
    
    x = W + 1;
    while (true) {
        int z = W + 1;
        int dx = min(W - 1, R - W - 2 - x);
        while (true) {
            int dz = min(W - 1, R - W - 2 - z);
            for (int i = 0; i < R - 2; i++) {
                traces.push_back(GFill(0, -1, 0, dx, 0, dz));
                traces.push_back(GFill(0, -1, 0, -dx, 0, dz));
                traces.push_back(GFill(0, -1, 0, -dx, 0, -dz));
                traces.push_back(GFill(0, -1, 0, dx, 0, -dz));
                
                if (i + 3 == R) continue;
                
                traces.push_back(SMove(0, 1, 0));
                traces.push_back(SMove(0, 1, 0));
                traces.push_back(SMove(0, 1, 0));
                traces.push_back(SMove(0, 1, 0));
            }
            
            if (z + dz == R - W - 2) break;
            
            dz = min(W, R - W - 2 - z - dz);
            z += W;
            
            vector <command> moves1 = get_moves(position(0, 0, W));
            vector <command> moves2 = get_moves(position(0, 0, dz));
            if (moves2.size() < moves1.size()) moves2.push_back(Wait());
            for (int i = 0; i < moves1.size(); i++) {
                traces.push_back(moves1[i]);
                traces.push_back(moves1[i]);
                traces.push_back(moves2[i]);
                traces.push_back(moves2[i]);
            }
            
            vector <command> moves = get_moves(position(0, -R + 3, 0));
            for (int i = 0; i < moves.size(); i++) {
                for (int j = 0; j < 4; j++) traces.push_back(moves[i]);
            }
        }
        
        if (x + dx == R - W - 2) break;
        
        dx = min(W, R - W - 2 - x - dx);
        x += W;
        
        vector <vector<command>> all_moves;
        all_moves.push_back(get_moves(position(W, 0, W + 1 - z)));
        all_moves.push_back(get_moves(position(dx, 0, W + 1 - z)));
        all_moves.push_back(get_moves(position(dx, 0, -R + W * 3 + 2)));
        all_moves.push_back(get_moves(position(W, 0, -R + W * 3 + 2)));
        while (true) {
            bool remaining = false;
            for (int i = 0; i < 4; i++) {
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
        
        vector <command> moves = get_moves(position(0, -R + 3, 0));
        for (int i = 0; i < moves.size(); i++) {
            for (int j = 0; j < 4; j++) traces.push_back(moves[i]);
        }
    }
    
    {
        vector <command> moves_z = get_moves(position(0, 0, -R + W + 3 + x));
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
        
        vector <command> moves_x = get_moves(position(-R + W + 3 + x, 0, 0));
        for (int i = 0; i < moves_x.size(); i++) {
            traces.push_back(Wait());
            traces.push_back(moves_x[i]);
        }
        
        traces.push_back(FusionP(1, 0, 0));
        traces.push_back(FusionS(-1, 0, 0));
    }
    
    {
        vector <command> moves_xz = get_moves(position(-x, 0, -x));
        for (int i = 0; i < moves_xz.size(); i++) traces.push_back(moves_xz[i]);
        
        vector <command> moves_y = get_moves(position(0, -R + 2, 0));
        for (int i = 0; i < moves_y.size(); i++) traces.push_back(moves_y[i]);
    }
    
    traces.push_back(Halt());
    
    return traces;
}

int main() {
    int R, W;
    scanf("%d", &R);
    
    if (R > 60) {
        W = 30;
    } else {
        W = 20;
    }
    
    output(calc(R, W));
    
    return 0;
}
