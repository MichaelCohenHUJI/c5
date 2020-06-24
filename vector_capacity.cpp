#include <iostream>
#include <vector>

int main() 
{
  std::vector<int> test;
  // test.reserve(5);
  for(unsigned int i = 0; i < 5; i++) 
    {
      test.push_back(i);
    }
  //  test.shrink_to_fit();
  std::cout << "size = " << test.size()
	    << " capacity= "
	    << test.capacity() << std::endl;
  return 0;
}
