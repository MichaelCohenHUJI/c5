/**
 * @section DESCRIPTION
 * Performs operations on schools solution for RecommenderSystem specified by instruction file
 *
 * @section USAGE
 * Connect to school environment.
 *
 * Compile:
 * g++ -Wall -g -std=c++17 TestRunnerSchool.cpp TesterCommon.h TesterCommon.cpp -o TestRunnerSchool
 *
 * Run:
 * TestRunnerSchool <test instruction file path> <movie file path> <user file path>
 *
 * Examples:
 * TestRunnerSchool test_instructions_small.txt movies_small.txt ranks_small.txt
 * TestRunnerSchool test_instructions_big.txt movies_big.txt ranks_big.txt
 *
 * @section RUNNING TIME
 * There will be major differences in running time since we cannot link the school solution
 * and instead we run the entire school solution again for each operation
 * as described in the PDF.
 *
 * @section TEST INSTRUCTION FILE FORMAT
 * In case you want to write a test file yourself.
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
#define TEST_RUNNER_SCHOOL_OUTPUT_FILE ("test_school_out.txt")

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
std::string getStdoutFromCmd(std::string cmd, int *returnCode)
{
	std::string data;
	FILE *file;
	char buffer[MAX_BUFFER];
	cmd.append(" 2>&1");
	file = popen(cmd.c_str(), "r");

	while (fgets(buffer, MAX_BUFFER, file) != nullptr && file)
	{
		data.append(buffer);
	}

	*returnCode = pclose(file);

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

		int statusCode;

		switch (op.type)
		{
			case RecommendByContent:
				cmd << " content recommend " << op.userName;
				testOut << getStdoutFromCmd(cmd.str(), &statusCode);
				break;

			case PredictMovieScore:
				cmd << " cf predict " << op.movieTitle << " " << op.userName << " " << op.k;
				testOut << getStdoutFromCmd(cmd.str(), &statusCode);

				break;

			case RecommendByCF:
				cmd << " cf recommend " << op.userName << " " << op.k;
				testOut << getStdoutFromCmd(cmd.str(), &statusCode);
		}

		testOut << std::endl;

		if (statusCode != EXIT_SUCCESS)
		{
			std::cerr << "Failed to run against school solution or there was an error."
			          << std::endl << "Make sure you run this on school environment." << std::endl
			          << "Check output in file: " << TEST_RUNNER_SCHOOL_OUTPUT_FILE << std::endl;

			exit(EXIT_FAILURE);
		}

	}
}

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		std::cerr << "Tester (School) Usage: TestRunnerSchool <test instruction file path>"
		             " <movie file path> <user file path>"
		          << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "Running... It might take a while." << std::endl;

	std::vector<TestOperation> operations;
	loadOperations(argv[1], operations);

	std::ofstream testOut(TEST_RUNNER_SCHOOL_OUTPUT_FILE);

	runTestOperationsForSchool(argv[2], argv[3], operations, testOut);

	std::cout << "Finished. Output in file: " << TEST_RUNNER_SCHOOL_OUTPUT_FILE << std::endl;
}
