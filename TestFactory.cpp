/**
 * @section DESCRIPTION
 * Generates a random test instruction file for RecommenderSystem
 * that can be used with the attached TestRunner or TestRunnerSchool.
 *
 * @section USAGE
 * Compile and run :)
 * g++ -Wall -Wvla -Wextra -Werror -g -std=c++17 TestFactory.cpp TesterCommon.h TesterCommon.cpp -o TestFactory
 * TestFactory <movie file path> <user file path>
 * e.g. TestFactory movies_big.txt ranks_big.txt
 *
 *
 * @section TEST INSTRUCTION FILE FORMAT
 * Each line contains an operation:
 * 		predicc <movie title> <user name> <k>
 * 		best_predicc <user name> <k>
 * 		by_content <user name>
 * 	See constants in TesterCommon.h file
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <random>
#include "TesterCommon.h"

// TODO: Move to optional program arguments
#define TEST_FACTORY_OUTPUT_FILE ("test_instructions.txt")
#define TEST_COUNT (500)
#define MAX_K (10)

typedef std::pair<std::string, int> UserWithRatingCount;

void loadUsers(const std::string &path, std::vector<UserWithRatingCount> &vec)
{
	std::ifstream file(path);
	std::string line;
	std::getline(file, line);
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string name;
		std::string currentRank;
		iss >> name;
		int countRanks = 0;
		while (iss >> currentRank)
		{
			if (currentRank != "NA")
			{
				countRanks++;
			}
		}

		vec.emplace_back(name, countRanks);
	}
}

void loadMovies(const std::string &path, std::vector<std::string> &vec)
{
	std::ifstream file(path);
	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string name;
		iss >> name;
		vec.push_back(name);
	}
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Test Factory Usage: TestFactory <movie file path> <user file path>"
		          << std::endl;
		exit(EXIT_FAILURE);
	}

	std::vector<std::string> moviesTitles;
	std::vector<UserWithRatingCount> usersWithRatingCount;

	loadMovies(argv[1], moviesTitles);
	loadUsers(argv[2], usersWithRatingCount);

	std::default_random_engine generator{
			static_cast<long unsigned int>(std::time(nullptr))
	};

	std::uniform_int_distribution<int> opDist(0, 2); // OMG MAGIC NUMBER
	std::uniform_int_distribution<int> userDist(0, usersWithRatingCount.size() - 1);
	std::uniform_int_distribution<int> movieDist(0, moviesTitles.size() - 1);

	std::ofstream testOut(TEST_FACTORY_OUTPUT_FILE);

	if (!testOut.is_open())
	{
		std::cerr << "Failed to open output file." << std::endl;
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < TEST_COUNT; i++)
	{
		const auto opTypeCode = static_cast<TestOperationType>(opDist(generator));
		const auto opTypeDesc = encodeTestOperation(opTypeCode);
		const auto userId = userDist(generator);
		const auto &userName = usersWithRatingCount[userId].first;

		if (opTypeCode == RecommendByContent)
		{
			testOut << opTypeDesc << " " << userName << std::endl;
			continue;
		}

		const int unratedMovieCount =
				(int) moviesTitles.size() - usersWithRatingCount[userId].second - 1;
		const int maxK = std::min(MAX_K, unratedMovieCount);

		std::uniform_int_distribution<int> kDist(1, maxK);
		int k = kDist(generator);

		if (opTypeCode == RecommendByCF)
		{
			testOut << opTypeDesc << " " << userName << " " << k << std::endl;
			continue;
		}

		const auto movieId = movieDist(generator);
		const auto &movieTitle = moviesTitles[movieId];

		testOut << opTypeDesc << " " << movieTitle << " " << userName << " " << k << std::endl;
	}
}
