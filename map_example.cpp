#include <iostream>
#include <map>
#include <string>

int main() 
{
  std::map<const std::string, int> months;
  
  months["january"] = 31;
  months["february"] = 28;
  months["march"] = 31;

  std::string month;
  std::cin >> month;
  if(months.count(month)) {
      std::cout << "month -> " 
		<< months[month] << std::endl;
    } else {
      std::cout << "not found" << std::endl;
    }

  return 0;
}
