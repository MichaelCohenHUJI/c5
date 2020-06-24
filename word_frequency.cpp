#include <iostream>
#include <map>
#include <string>

int main() 
{
  // map of words and their frequencies 
  std::map<std::string, int> freq; 
  std::string word; // input buffer for words
  while (std::cin >> word) { 
    // save the word in the map and update counter
    freq[word]++; 
  } 

  // iterate over the map and print    
  for (auto iter = freq.begin(); iter != freq.end(); ++iter)  
    { 
      std::cout << iter->second << " " 
		<< iter->first << std::endl; 
    } 
  return 0; 
}
