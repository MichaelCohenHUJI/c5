#include <iostream>
#include <deque>
#include <chrono> // timing

double timePushBack(int numValues) 
{
  std::deque<int> v;
  auto t1 = std::chrono::high_resolution_clock::now();

  for(int i = 0; i < numValues; ++i) {
    v.push_back(i);
  }

  auto t2 = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
}

double timePushFront(int numValues) 
{
  std::deque<int> v;
  auto t1 = std::chrono::high_resolution_clock::now();
  
  for(int i = 0; i < numValues; ++i) {
    v.insert(v.begin(),i);
  }

  auto t2 = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
}

int main() 
{
  const int numIterations = 10;
  const int startVal = 500;

  int numToInsert = startVal;
  for(int i = 0; i < numIterations; ++i) 
    {
      double timeToPushBack = timePushBack(numToInsert);
      double timeToPushFront = timePushFront(numToInsert);
      std::cout << numToInsert << ", " << timeToPushBack << ", " << timeToPushFront << std::endl;
    numToInsert *= 2;
  }

  return 0;
}
