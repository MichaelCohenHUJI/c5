/**
 * @section DESCRIPTION
 * Performs operations on RecommenderSystem specified by instruction file.
 * You can also run it with valgrind and compare the results with school solution.
 *
 * @section USAGE IN CMD
 * 	g++ -Wall -Wvla -Wextra -Werror -g -std=c++17 TestRunner.cpp TesterCommon.h TesterCommon.cpp RecommenderSystem.h RecommenderSystem.cpp -o TestRunner
 * 	TestRunner <test instruction file path> <movie file path> <user file path>
 * 	e.g TestRunner test_instructions.txt movies_big.txt ranks_big.txt
 * Tip: to measure time in Linux you can run `time TestRunner ...`
 *
 * @section USAGE IN CLION
 * - Add the next line CMakeLists.txt and save:
 * 		add_executable(test_runner TesterCommon.h TesterCommon.cpp TestRunner.cpp RecommenderSystem.cpp RecommenderSystem.h)
 * - Reload cmake (a prompt will be shown automatically)
 * - Choose test_runner in configuration list
 * - Edit configuration - add program arguments:
 * 		<test instruction file path> <movie file path> <user file path>
 * - Run (with / without valgrind if you have configured valgrind on your computer using WSL)
 *
 * Note that CLion compiles the files in cmake-build-default directory (or similar)
 *   so the file paths are relative to there (you can also just put them there directly).
 *
 * Also note that there may be differences in float/double values from school computers
 *
 * @section TEST INSTRUCTION FILE FORMAT
 * Each line contains an operation:
 * 		predicc <movie title> <user name> <k>
 * 		best_predicc <user name> <k>
 * 		by_content <user name>
 * 	See constants in TesterCommon.h file
 */

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>
#include "RecommenderSystem.h"
#include "TesterCommon.h"

// TODO: Move to program arguments
#define TEST_RUNNER_OUTPUT_FILE ("test_out.txt")

void runTestOperationsForStudent(const std::string &moviePath, const std::string &userPath,
                                 const std::vector<TestOperation> &ops, std::ofstream &testOut)
{
	RecommenderSystem rs;
	int result = rs.loadData(moviePath, userPath);
	if (result == 1)
	{
		exit(EXIT_FAILURE);
	}

	for (const TestOperation &op : ops)
	{
		testOut << op;

		switch (op.type)
		{
			case RecommendByCF:
				testOut << rs.recommendByCF(op.userName, op.k);
				break;

			case PredictMovieScore:
				testOut << rs.predictMovieScoreForUser(op.movieTitle, op.userName, op.k);
				break;

			case RecommendByContent:
				testOut << rs.recommendByContent(op.userName);
				break;
		}

		testOut << std::endl << std::endl;

	}
}

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		std::cerr << "Tester Usage: TestRunner <operations file path>"
		             " <movie file path> <user file path>"
		          << std::endl;
		exit(EXIT_FAILURE);
	}

	std::vector<TestOperation> operations;
	loadOperations(argv[1], operations);

	std::ofstream testOut(TEST_RUNNER_OUTPUT_FILE);

	runTestOperationsForStudent(argv[2], argv[3], operations, testOut);
}
