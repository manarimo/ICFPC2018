#include <iostream>

using namespace std;

int main() {
  unsigned char c;
  cin.read((char *)&c, 1);
  const int R = c;
  cerr << "R = " << R << endl;
  const int bytes = (R * R * R + 7) / 8;
  cerr << "Reading " << bytes << " bytes" << endl;

  bool field[R][R][R]; // x, y, z
  {
    int x = 0, y = 0, z = 0;
    for (int i = 0; i < bytes; i++) {
      cin.read((char *)&c, 1);
      for (int j = 0; j < 8 && x < R; j++) {
	field[x][y][z] = (c >> j) & 1;
	z++;
	if (z == R) z = 0, y++;
	if (y == R) y = 0, x++;
      }
    }
  }

  int count = 0;
  for (int i = 0; i < R; i++) {
    for (int j = 0; j < R; j++) {
      for (int k = 0; k < R; k++) {
	if (field[i][j][k])  {
	  count++;
	}
      }
    }
  }
  cerr << "COUNT: " << count << endl;
  
  return 0;
}
