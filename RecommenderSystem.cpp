//
// Created by michael on 22/06/2020.
//

#include "RecommenderSystem.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>



const std::string OPEN_FAIL = "Unable to open file ";
const std::string NA = "NA";
const std::string INVALID_USER = "USER NOT FOUND";


/**
 * helper func, prints to cerr the error message
 * @param msg
 */
void printMessage(const std::string &msg, const std::string &path)
{
    std::cerr << msg << path << std::endl;
}

void printdVec(const std::vector<double> &vec)
{
    for (double elem : vec)
    {
        std::cout << elem << " ";
    }
    std::cout << std::endl;
}

void printStrVec(const std::vector<std::string> &vec)
{
    for (const std::string& elem : vec)
    {
        std::cout << elem << " ";
    }
    std::cout << std::endl;
}


int RecommenderSystem::loadData(const std::string &moviesAttributesFilePath,
                                const std::string &userRanksFilePath)
{
    std::ifstream movies(moviesAttributesFilePath);
    if (movies)
    {
        std::string line;
        while (std::getline(movies, line))
        {
            std::string movieName;
            std::istringstream lineStream(line);
            if (!(lineStream >> movieName))
            {
                return 0;
            }
            double val;
            while (lineStream >> val) _movies[movieName].push_back(val);
            //std::cout << movieName << std::endl; //r
            //printdVec(_movies[movieName]); //r
        }

    }
    else
    {
        printMessage(OPEN_FAIL, moviesAttributesFilePath);
        return 0;
    }
    std::ifstream clients(userRanksFilePath);
    if (clients)
    {
        std::string line;
        int i = 0;
        while (std::getline(clients, line))
        {
            std::istringstream lineStream(line);
            if (i == 0)
            {
                std::string val;
                while (lineStream >> val) _movieNames.push_back(val);
//                printStrVec(_movieNames); //r
            }
            else
            {
                std::string clientName;
                if (!(lineStream >> clientName))
                {
                    return 0;
                }
                std::string val;
                while (lineStream >> val)
                {
                    if (val == NA)
                    {
                        _clients[clientName].push_back(0.0);
                    }
                    else
                    {
                        _clients[clientName].push_back(std::stod(val));
                        _clientsRanksNum[clientName] = i;
                        i++;
                    }
                }
//                std::cout << clientName << " " << _clientsRanksNum[clientName] << std::endl; //r
                //printdVec(_clients[clientName]); //r
            }
            i = 1;
        }
    }
    else
    {
        printMessage(OPEN_FAIL, userRanksFilePath);
        return 0;
    }
    return 1;
}


std::string RecommenderSystem::recommendByContent(const std::string &userName)
{
    if (_clients.count(userName))
    {// normalization:
        std::vector<double> curNorm = _getNorm(userName);
        // create pref vector:
//        printdVec(curNorm);
//        std::cout << _movieNames[0] << std::endl;
        std::vector<double> prefVec = _createPrefVec(userName, curNorm);
//        printdVec(prefVec);
        return _findMovieByPref(userName, prefVec).name;
    }
    else
    {
        return INVALID_USER;
    }
}

std::vector<double> RecommenderSystem::_getNorm(const std::string &user)
{
    auto n = (double) _clientsRanksNum[user];
    double avg = 0.0;
    if (n != 0)
    {
        avg = std::accumulate(_clients[user].begin(), _clients[user].end(), 0.0) / n;
    }
    std::vector<double> curNorm(_clients[user]);
    for (double &elem : curNorm)
    {
        elem = (elem == 0.0) ? elem : elem - avg;
    }
    return curNorm;
}

std::vector<double>
RecommenderSystem::_createPrefVec(const std::string &user, const std::vector<double> &curNorm)
{
    std::vector<double> prefVec(_movies[_movieNames[0]].size());
    for (std::vector<double>::size_type i = 0; i < _clients[user].size(); i++)
    {
        if (curNorm[i] != 0)
        {
            double scalar = curNorm[i];
            std::vector<double> curMovie(_movies[_movieNames[i]]);
//                std::cout << _movieNames[i] << std::endl;
//                printdVec(_movies[_movieNames[i]]);
            for (double &elem : curMovie)
            {
                elem = scalar * elem;
            }
            for (std::vector<double>::size_type j = 0; j < prefVec.size(); j++)
            {
                prefVec[j] = prefVec[j] + curMovie[j];
            }
        }
    }// finding the closest movie vector in the helper method
    return prefVec;
}

resMovie RecommenderSystem::_findMovieByPref(const std::string &user,
                                             const std::vector<double> &prefVec)
{
    std::string closest;
    double closestScore = -2.0;
    double prefNorm = _norm(prefVec);
    for (std::vector<double>::size_type i = 0; i < _movieNames.size(); i++)
    {
        if (_clients[user][i] == 0.0)
        {
            double normsProd = _norm(_movies[_movieNames[i]]) * prefNorm;
            double curScore = _dotProd(_movies[_movieNames[i]], prefVec) * (1 / normsProd);
            if (curScore > closestScore)
            {
                closestScore = curScore;
                closest = _movieNames[i];
            }
        }
    }
    resMovie out = {.score = closestScore, .name = closest};
    return out;
}

double RecommenderSystem::_dotProd(const std::vector<double> &a, const std::vector<double> &b)
{
    double out = 0.0;
    for (std::vector<double>::size_type j = 0; j < a.size(); j++)
    {
        out += a[j] * b[j];
    }
    return out;
}

double RecommenderSystem::_norm(const std::vector<double> &vec)
{
    double sum = 0.0;
    for (double elem : vec)
    {
        sum += elem * elem;
    }
    return sqrt(sum);
}
//
//resMovie RecommenderSystem::_findMovieByHistory(const std::vector<double> &movieAttributes,
//                                             const std::map<std::string, double> &userHistory)
//{
//    resMovie closest = {.score = -2.0, .name = NA};
//    double movieNorm = _norm(movieAttributes);
//    for (const auto& pair : userHistory)
//    {
//        double curNorm = _norm(_movies[pair.first]) * movieNorm;
//        double curScore = _dotProd(movieAttributes, _movies[pair.first]) * (1 / curNorm);
//        if (closest.score < curScore)
//        {
//            closest.score = curScore;
//            closest.name = pair.first;
//        }
//    }
//    return closest;
//}

std::vector<resMovie> RecommenderSystem::_findMovieByHistory(const std::vector<double> &movieAttributes,
                                                             const std::map<std::string, double> &userHistory)
{
    std::vector<resMovie> res;
    double movieNorm = _norm(movieAttributes);
    for (const auto& pair : userHistory)
    {
        double curNorm = _norm(_movies[pair.first]) * movieNorm;
        double curScore = _dotProd(movieAttributes, _movies[pair.first]) * (1 / curNorm);
        resMovie curMovie = {.score = curScore, .name = pair.first};
        res.push_back(curMovie);
    }
    std::sort (res.begin(), res.end());
    std::reverse(res.begin(), res.end());
    return res;
}

double
RecommenderSystem::predictMovieScoreForUser(const std::string &movieName, const std::string&userName,
                                            int k)
{
    if (_clients.count(userName) && _movies.count(movieName))
    {
        double numerator = 0.0;
        double denominator = 0.0;
        std::map<std::string, double> clientHistory;
        for (size_t i = 0; i < _movieNames.size(); i++) // create map to users ratings
        {
            if (_clients[userName][i] != 0)
            {
                clientHistory[_movieNames[i]] = _clients[userName][i];
            }
        }
        std::vector<resMovie> sorted = _findMovieByHistory(_movies[movieName], clientHistory);
        for (int i = 0; i < k; i++) // get the k most resemble movies
        {
            resMovie closest = sorted[i];
            numerator += closest.score * clientHistory[closest.name];
            denominator += closest.score;
        }
        return numerator * (1 / denominator);
    }
    else
    {
        return NOT_EXSISTS;
    }
}

std::string RecommenderSystem::recommendByCF(const std::string &userName, int k)
{
    if (_clients.count(userName))
    {
        std::string bestPrediction;
        double bestScore = -2.0;
        for (size_t i = 0; i < _movieNames.size(); i++) // create map to users ratings
        {
            if (_clients[userName][i] == 0)
            {
                double curScore = predictMovieScoreForUser(_movieNames[i], userName, k);
                if (bestScore < curScore)
                {
                    bestScore = curScore;
                    bestPrediction = _movieNames[i];
                }
            }
        }
        return bestPrediction;
    }
    else
    {
        return INVALID_USER;
    }
}




