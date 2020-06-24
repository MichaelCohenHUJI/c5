/**
 * @section DESCRIPTION
 * Performs operations on schools solution for RecommenderSystem specified by instruction file
 *
 * @section USAGE
 * Connect to school environment, compile and run:
 * 	g++ -Wall -Wvla -Wextra -Werror -g -std=c++17 TestRunnerSchool.cpp TesterCommon.h TesterCommon.cpp -o TestRunnerSchool
 * 	TestRunnerSchool <test instruction file path> <movie file path> <user file path>
 * 	e.g TestRunnerSchool test_instructions.txt movies_big.txt ranks_big.txt
 *
 * @section RUNNING TIME
 * There will be major differences in running time since we cannot link the school solution
 * and instead we run the entire school solution again for each operation
 * as described in the PDF.
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
#include <vector>
#include <cstdlib>
#include "TesterCommon.h"

// TODO: Move to optional program arguments
#define TEST_RUNNER_OUTPUT_FILE ("test_school_out.txt")

#define TEST_SCHOOL_SOLUTION_PATH ("~labcc2/www/ex5/school_solution/RecommenderSystem")
#define MAX_BUFFER 128

/**
 * A somewhat messy hack but I have no time.
 *
 * As a wise man once wrote:
 *
 * Once you are done trying to 'optimize' this routine,
 * and have realized what a terrible mistake that was,
 * please increment the following counter as a warning
 * to the next guy:
 *
 * TOTAL_HOURS_WASTED_HERE = 42
 */
std::string getStdoutFromCmd(std::string cmd)
{
	std::string data;
	FILE *file;
	char buffer[MAX_BUFFER];
	cmd.append(" 2>&1");
	file = popen(cmd.c_str(), "r");
	while (file && fgets(buffer, MAX_BUFFER, file) != nullptr)
	{
		data.append(buffer);
	}
	pclose(file);
	return data;
}

void runTestOperationsForSchool(const std::string &moviePath, const std::string &userPath,
                                const std::vector<TestOperation> &ops, std::ofstream &testOut)
{
	for (const TestOperation &op : ops)
	{
		std::stringstream cmd;
		cmd << TEST_SCHOOL_SOLUTION_PATH << " " << moviePath << " " << userPath;

		testOut << op;

		switch (op.type)
		{
			case RecommendByContent:
				cmd << " content recommend " << op.userName;
				testOut << getStdoutFromCmd(cmd.str());
				break;

			case PredictMovieScore:
				cmd << " cf predict " << op.movieTitle << " " << op.userName << " " << op.k;
				testOut << getStdoutFromCmd(cmd.str());

				break;

			case RecommendByCF:
				cmd << " cf recommend " << op.userName << " " << op.k;
				testOut << getStdoutFromCmd(cmd.str());
		}

		testOut << std::endl;

	}
}

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		std::cerr << "Tester (School) Usage: TestRunner <operations file path>"
		             " <movie file path> <user file path>"
		          << std::endl;
		exit(EXIT_FAILURE);
	}

	std::vector<TestOperation> operations;
	loadOperations(argv[1], operations);

	std::ofstream testOut(TEST_RUNNER_OUTPUT_FILE);

	runTestOperationsForSchool(argv[2], argv[3], operations, testOut);
}
