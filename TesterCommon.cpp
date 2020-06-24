#include <iostream>
#include <fstream>
#include <sstream>

#include "TesterCommon.h"

TestOperationType decodeTestOperation(const std::string &desc)
{
	if (desc == OPERATION_PREDICT_MOVIE_SCORE)
	{
		return PredictMovieScore;
	}
	else if (desc == OPERATION_RECOMMEND_BY_CF)
	{
		return RecommendByCF;
	}
	else if (desc == OPERATION_RECOMMEND_BY_CONTENT)
	{
		return RecommendByContent;
	}

	std::cerr << "Unknown operation: " << desc << std::endl;
	exit(EXIT_FAILURE);
}

std::string encodeTestOperation(const TestOperationType &type)
{
	switch (type)
	{
		case PredictMovieScore:
			return OPERATION_PREDICT_MOVIE_SCORE;
		case RecommendByCF:
			return OPERATION_RECOMMEND_BY_CF;
		case RecommendByContent:
			return OPERATION_RECOMMEND_BY_CONTENT;
		default:
			std::cerr << "Unknown operation type: " << type << std::endl;
			exit(EXIT_FAILURE);
	}
}

void loadOperations(const std::string &path, std::vector<TestOperation> &vec)
{
	std::ifstream infile(path);
	std::string line;

	TestOperationType type;
	std::string typeDesc;
	std::string userName;
	std::string movieTitle;
	int k;

	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		iss >> typeDesc;
		type = decodeTestOperation(typeDesc);

		switch (type)
		{
			case RecommendByCF:
				iss >> userName;
				iss >> k;
				vec.emplace_back(type, userName, k);
				break;

			case PredictMovieScore:
				iss >> movieTitle;
				iss >> userName;
				iss >> k;
				vec.emplace_back(type, userName, movieTitle, k);
				break;

			case RecommendByContent:
				iss >> userName;
				vec.emplace_back(type, userName);
				break;
		}
	}
}

std::ostream &operator<<(std::ostream &os, const TestOperation &op)
{
	switch (op.type)
	{
		case RecommendByCF:
			os << "[RecommendByCF] "
			   << "Name = " << op.userName << "; k = " << op.k << ":" << std::endl;
			break;

		case PredictMovieScore:
			os << "[PredictMovieScore] "
			   << "Movie = " << op.movieTitle << "; Name = " << op.userName
			   << "; k = " << op.k << ":" << std::endl;
			break;

		case RecommendByContent:
			os << "[RecommendByContent] "
			   << "Name = " << op.userName << ":" << std::endl;
	}

	return os;
}
