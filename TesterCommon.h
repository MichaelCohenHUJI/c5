#ifndef CPP_EX2_TESTER_H

#define OPERATION_PREDICT_MOVIE_SCORE ("predicc")
#define OPERATION_RECOMMEND_BY_CF ("best_predicc")
#define OPERATION_RECOMMEND_BY_CONTENT ("by_content")

#include <string>
#include <vector>

enum TestOperationType
{
	RecommendByContent, PredictMovieScore, RecommendByCF
};

struct TestOperation
{
	const TestOperationType type;
	const std::string userName;
	const std::string movieTitle;
	const int k;

	TestOperation() : type(), userName(), movieTitle(), k(0)
	{}

	TestOperation(const TestOperationType type, std::string userName) :
			type(type), userName(std::move(userName)), movieTitle(), k(0)
	{}

	TestOperation(const TestOperationType type, std::string userName, const int k) :
			type(type), userName(std::move(userName)), movieTitle(), k(k)
	{}

	TestOperation(const TestOperationType type, std::string userName, std::string movieTitle,
	              const int k) :
			type(type), userName(std::move(userName)), movieTitle(std::move(movieTitle)), k(k)
	{}
};

TestOperationType decodeTestOperation(const std::string &desc);

std::string encodeTestOperation(const TestOperationType &type);

void loadOperations(const std::string &path, std::vector<TestOperation> &vec);

std::ostream &operator<<(std::ostream &os, const TestOperation &op);

#define CPP_EX2_TESTER_H

#endif //CPP_EX2_TESTER_H
