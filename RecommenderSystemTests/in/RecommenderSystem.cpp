//
// Created by michael on 22/06/2020.
//

#include "RecommenderSystem.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>


#define LOAD_FAIL -1
#define LOAD_SUCCESS 0
const std::string OPEN_FAIL = "Unable to open file ";
const std::string NA = "NA";
const std::string INVALID_USER = "USER NOT FOUND";


/**
 * helper func, prints to cerr the error message regarding bad file path input
 * @param msg
 * @param path
 */
void printMessage(const std::string &msg, const std::string &path)
{
    std::cerr << msg << path << std::endl;
}

/**
 * a function which loads data from movie attributes file and clients rank history
 * @param moviesAttributesFilePath
 * @param userRanksFilePath
 * @return 0 upon success, -1 upon failure
 */
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
                return LOAD_FAIL;
            }
            double val;
            while (lineStream >> val)
            {
                _movies[movieName].push_back(val);
            }
        }
    }
    else
    {
        printMessage(OPEN_FAIL, moviesAttributesFilePath);
        return LOAD_FAIL;
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
            { // parse first line, which is the line with the movie names
                std::string val;
                while (lineStream >> val)
                {
                    _movieNames.push_back(val);
                }
            }
            else
            { // parse the rest of the file line by line (client data)
                std::string clientName;
                if (!(lineStream >> clientName))
                {
                    return LOAD_FAIL;
                }
                std::string val;
                while (lineStream >> val)
                {
                    _updateUserRank(i, clientName, val);
                }
            }
            i = 1;
        }
    }
    else
    {
        printMessage(OPEN_FAIL, userRanksFilePath);
        return LOAD_FAIL;
    }
    return LOAD_SUCCESS;
}

/**
 * helper method, updates user's rank vector according to the input file
 * @param i number of the watched movies so far + 1
 * @param clientName
 * @param val current movie rank parsed from the file
 */
void RecommenderSystem::_updateUserRank(int &i, const std::string &clientName,
                                        const std::string &val)
{
    if (val == NA)
    {
        _clients[clientName].push_back(0.0);
    }
    else
    {
        _clients[clientName].push_back(std::stod(val));
        _clientsRanksNum[clientName] = i; // saves the number of movies watched
        // by each client
        i++;
    }
}

/**
 * implementation of the content based algorithm, which uses existing ranks if movies and their
 * attributes to recommend a movie to the client
 * @param userName client name
 * @return movie recommended upon success, invalid client name message upon failure
 */
std::string RecommenderSystem::recommendByContent(const std::string &userName)
{
    if (_clients.count(userName))
    {// normalization:
        std::vector<double> curNorm = _getNormRankVec(userName);
        // create pref vector:
        std::vector<double> prefVec = _createPrefVec(userName, curNorm);
        return _findMovieByPref(userName, prefVec).name;
    }
    else
    {
        return INVALID_USER;
    }
}

/**
 * helper method to calculate the normalized ranks vector of a given client
 * @param user client name
 * @return the normalized preference vector of the client
 */
std::vector<double> RecommenderSystem::_getNormRankVec(const std::string &user)
{
    double n = _clientsRanksNum[user];
    double avg = 0.0;
    if (n != 0.0)
    {
        avg = std::accumulate(_clients[user].begin(), _clients[user].end(), 0.0);
        avg = avg / n;
    }
    std::vector<double> curNorm(_clients[user]);
    for (double &elem : curNorm)
    {
        elem = (elem == 0.0) ? elem : elem - avg;
    }
    return curNorm;
}

/**
 * creates the preference vector of a given client based on past ranks and movie attributes
 * @param user client name
 * @param curNorm normalized preference vector of the client
 * @return the clients preference vector
 */
std::vector<double>
RecommenderSystem::_createPrefVec(const std::string &user, const std::vector<double> &curNorm)
{
    std::vector<double> prefVec(_movies[_movieNames[0]].size());
    for (std::vector<double>::size_type i = 0; i < _clients[user].size(); i++)
    {
        if (curNorm[i] != 0.0)
        {
            double scalar = curNorm[i];
            std::vector<double> curMovie(_movies[_movieNames[i]]);
            for (double &elem : curMovie)
            {
                elem *= scalar;
            }
            for (std::vector<double>::size_type j = 0; j < prefVec.size(); j++)
            {
                prefVec[j] += curMovie[j];
            }
        }
    }
    return prefVec;
}

/**
 * finds the best movie to recommend based on the users' preference vector of movie attributes
 * @param user clients' name
 * @param prefVec clients' preference vector
 * @return a resMovie struct, contains the recommended movie's name and resemblance score
 */
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
            double curScore = _dotProd(_movies[_movieNames[i]], prefVec) / normsProd;
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

/**
 * helper method which calculate the dot product of two vectors
 * @param a first vector
 * @param b second vector
 * @return the dot product
 */
double RecommenderSystem::_dotProd(const std::vector<double> &a, const std::vector<double> &b)
{
    double out = 0.0;
    for (std::vector<double>::size_type j = 0; j < a.size(); j++)
    {
        out += a[j] * b[j];
    }
    return out;
}

/**
 * helper method which calculate the norm of a given vector
 * @param vec
 * @return the norm of vec
 */
double RecommenderSystem::_norm(const std::vector<double> &vec)
{
    double sum = 0.0;
    for (double elem : vec)
    {
        sum += pow(elem, 2.0);
    }
    return sqrt(sum);
}

/**
 * creates a vector of past ranked movies, sorted by resemblance to certain movie attributes, from
 * the closest to the most different one
 * @param movieAttributes the movie attributes according to which we sort
 * @param userHistory a map of the clients' past movies and their rankings
 * @return the sorted vector of past ranked movies in the above mentioned order
 */
std::vector<resMovie>
RecommenderSystem::_findMovieByHistory(const std::vector<double> &movieAttributes,
                                       const std::map<std::string, double> &userHistory)
{
    std::vector<resMovie> res;
    double movieNorm = _norm(movieAttributes);
    for (const auto& pair : userHistory)
    {
        double curNorm = _norm(_movies[pair.first]) * movieNorm;
        double curScore = _dotProd(movieAttributes, _movies[pair.first]) / curNorm;
        resMovie curMovie = {.score = curScore, .name = pair.first};
        res.push_back(curMovie);
    }
    std::sort (res.begin(), res.end(), _compResMovie);
    return res;
}

/**
 * predicts a clients rank to a movie they didn't watch based on past rankings of the k movies
 * with attributes closest to the movie we want to predict for. using the scoring method
 * described in the pdf.
 * @param movieName the movie for which we predict the clients' rank
 * @param userName client name
 * @param k
 * @return the prediction of the clients' rank to the movie. if userName/movieName are not in the
 * database, returns -1
 */
double RecommenderSystem::predictMovieScoreForUser(const std::string &movieName,
                                                   const std::string&userName, int k)
{
    if (_clients.count(userName) && _movies.count(movieName))
    {
        double numerator = 0.0;
        double denominator = 0.0;
        std::map<std::string, double> clientHistory;
        for (std::vector<std::string>::size_type i = 0; i < _movieNames.size(); i++)
        { // create map to users ratings
            if (_clients[userName][i] != 0.0)
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
        return numerator / denominator;
    }
    else
    {
        return NOT_EXSISTS;
    }
}

/**
 * gets the best movie to recommend to the client by predicting users rank to the movies they did
 * not watch already and saving the best scoring movie between those. prediction is based on the
 * k closest ranked movies by the user, as mentioned in the method predictMovieScoreForUser.
 * @param userName clients name
 * @param k
 * @return the name of the movie for which our prediction is the highest
 */
std::string RecommenderSystem::recommendByCF(const std::string &userName, int k)
{
    if (_clients.count(userName))
    {
        std::string bestPrediction;
        double bestScore = -2.0;
        for (size_t i = 0; i < _movieNames.size(); i++) // create map to users ratings
        {
            if (_clients[userName][i] == 0.0)
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

/**
 * comperator function for the resMovie struct, based on the movies' score.
 * @param lhs
 * @param rhs
 * @return the bool value of the statement lhs.score > rhs.score
 */
bool RecommenderSystem::_compResMovie(const resMovie &lhs, const resMovie &rhs)
{
    return lhs.score > rhs.score;
}


