#include <iostream>
#include <vector>
#include <numeric>

int main() 
{
  std::vector<int> v { 1, 2, 3, 4, 5};

  int sum = 0;
  for(auto i = v.begin(); i != v.end(); ++i) {
    sum += *i;
  }

  std::cout << "sum = " << sum << std::endl;

  sum = 0;
  for (auto n : v) sum += n;
  std::cout << "sum = " << sum << std::endl;

  return 0;
}
