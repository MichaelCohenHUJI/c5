//
// Created by michael on 22/06/2020.
//

#ifndef EX5_RECOMMENDERSYSTEM_H
#define EX5_RECOMMENDERSYSTEM_H

#define NOT_EXSISTS -1

#include <vector>
#include <numeric>
#include <iostream>
#include <string>
#include <map>

//need to have:
typedef struct {double score; std::string name;} resMovie;
bool operator<(const resMovie &lhs, const resMovie &rhs) { return lhs.score < rhs.score; }


class RecommenderSystem
{
private:
    std::vector<std::string> _movieNames;
    std::map<std::string, std::vector<double> > _clients;
    std::map<std::string, int > _clientsRanksNum;
    std::map<std::string, std::vector<double> > _movies;
    resMovie _findMovieByPref(const std::string &user, const std::vector<double> &prefVec);
    static double _norm(const std::vector<double> &vec);
    static double _dotProd(const std::vector<double> &a, const std::vector<double> &b);
    std::vector<double> _createPrefVec(const std::string &user, const std::vector<double> &curNorm);
    std::vector<double> _getNorm(const std::string &user);
    std::vector<resMovie> _findMovieByHistory(const std::vector<double> &movieAttributes,
                                 const std::map<std::string, double> &userHistory);
public:
    int loadData(const std::string &moviesAttributesFilePath, const std::string &userRanksFilePath);
    std::string recommendByContent(const std::string &userName);
    double predictMovieScoreForUser(const std::string &movieName, const std::string &userName, int k);
    std::string recommendByCF(const std::string &userName, int k);

};


#endif //EX5_RECOMMENDERSYSTEM_H
