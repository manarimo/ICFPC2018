#include <iostream>
#include <queue>
#include <map>
#include <tuple>
#include <cmath>

using namespace std;
typedef tuple<int, int, int> Pos;

int dx[6] = {1, -1, 0, 0, 0, 0};
int dy[6] = {0, 0, 1, -1, 0, 0};
int dz[6] = {0, 0, 0, 0, 1, -1};

int R;
int fill_count;
int max_depth;
bool field[250][250][250];
bool vis[250][250][250];

void read_input() {
  fill_count = 0;
  unsigned char c;
  cin.read((char *)&c, 1);
  R = c;
  cerr << "R = " << R << endl;
  const int bytes = (R * R * R + 7) / 8;
  cerr << "Reading " << bytes << " bytes" << endl;

  {
    int x = 0, y = 0, z = 0;
    for (int i = 0; i < bytes; i++) {
      cin.read((char *)&c, 1);
      for (int j = 0; j < 8 && x < R; j++) {
	field[x][y][z] = (c >> j) & 1;
	if (field[x][y][z]) {
	  fill_count++;
	}
	z++;
	if (z == R) z = 0, y++;
	if (y == R) y = 0, x++;
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

bool is_grounded() {
  clear_vis();
  int grounded_count = 0;
  max_depth = 1;
  priority_queue<pair<int, Pos>, vector<pair<int, Pos>>, greater<pair<int, Pos>>> pq;
  for (int i = 0; i < R; i++) {
    for (int k = 0; k < R; k++) {
      if (field[i][0][k]) {
	pq.push({1, make_tuple(i, 0, k)});
      }
    }
  }

  while (!pq.empty()) {
    auto e = pq.top();
    pq.pop();
    int depth = e.first;
    Pos c = e.second;
    int x = get<0>(c);
    int y = get<1>(c);
    int z = get<2>(c);

    if (vis[x][y][z]) continue;
    vis[x][y][z] = true;
    grounded_count++;
    max_depth = max(max_depth, depth);
    for (int i = 0; i < 6; i++) {
      int nx = x + dx[i];
      int ny = y + dy[i];
      int nz = z + dz[i];
      if (in_range(nx, ny, nz) && field[nx][ny][nz]) {
	pq.push({depth + 1, make_tuple(nx, ny, nz)});
      }
    }
  }

  cerr << "Max Depth: " << max_depth << endl;

  return grounded_count == fill_count;
}

int bfs(unsigned char x, unsigned char y, unsigned char z, bool search_void) {
  int ans = 0;
  queue<Pos> q;
  q.push(make_tuple(x, y, z));

  while (!q.empty()) {
    auto e = q.front();
    q.pop();
    int x = get<0>(e);
    int y = get<1>(e);
    int z = get<2>(e);

    if (vis[x][y][z]) continue;
    vis[x][y][z] = true;
    ans++;
    for (int i = 0; i < 6; i++) {
      int nx = x + dx[i];
      int ny = y + dy[i];
      int nz = z + dz[i];
      if (in_range(nx, ny, nz) && field[nx][ny][nz] != search_void) {
	q.push(make_tuple(nx, ny, nz));
      }
    }
  }
  return ans;
}

int main() {
  read_input();

  cerr << "COUNT: " << fill_count << endl;

  if (!is_grounded()) {
    cerr << "This model is INVALID" << endl;
  }

  int num_void_spaces = 0;
  int num_components = 0;
  int largest_component = 0;
  clear_vis();
  for (int i = 0; i < R; i++) {
    for (int j = 0; j < R; j++) {
      for (int k = 0; k < R; k++) {
	if (field[i][j][k]) {
	  int size = bfs(i, j, k, false);
	  if (size > 0) {
	    num_components++;
	    largest_component = max(largest_component, size);
	  }
	} else {
	  int size = bfs(i, j, k, true);
	  if (size > 0) {
	    num_void_spaces++;
	  }
	}
      }
    }
  }

  cerr << "log2 R = " << log2(R) << endl;
  cerr << "#void spaces: " << num_void_spaces << endl;
  cerr << "#components: " << num_components << endl;
  cerr << "largest component: " << largest_component << endl;
  cout << R << ", " << fill_count << ", " << num_components << ", " << largest_component << ", " << max_depth << ", " << num_void_spaces;
  return 0;
}

