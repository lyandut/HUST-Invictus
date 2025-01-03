#include <iostream>
#include <cstdint>
#include <sstream>

using namespace std;

int main() {
      cout << "-----------------------------" << endl;
      cout << "| 阶数 |     最少移动次数     |" << endl;  
      for(int i = 1; i < 64; ++i) {
            printf("| %-2d  | ", i); 
            string str = to_string(((uint64_t)1 << i) - 1);
            int size = (20 - str.size()) / 2, mod = (20 - str.size()) % 2;
            string str1(size, ' '), str2(size + mod, ' ');
            cout << str1 << str << str2 << "|" << endl;
      }
      cout << "-----------------------------" << endl;
}