#include <iostream>
#include <deque>
#include <vector>
#include <chrono> // timing

double timeDequeAccess(int numValues) 
{
  std::deque<int> v(numValues);
  auto t1 = std::chrono::high_resolution_clock::now();

  for(int i = 0; i < numValues; ++i) {
    v[i] = i;
  }

  auto t2 = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
}

double timeVecAccess(int numValues) {
  std::vector<int> v(numValues);
  auto t1 = std::chrono::high_resolution_clock::now();

  for(int i = 0; i < numValues; ++i) {
    v[i] = i;
  }

  auto t2 = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
}

int main() 
{
  const int numIterations = 20;
  const int startVal = 500;

  int numToInsert = startVal;
  for(int i = 0; i < numIterations; ++i) 
    {
      double timeVec = timeVecAccess(numToInsert);
      double timeDeq = timeDequeAccess(numToInsert);
      std::cout << numToInsert << ", " << timeVec << ", " << timeDeq << std::endl;
      numToInsert *= 2;
    }

  return 0;
}
