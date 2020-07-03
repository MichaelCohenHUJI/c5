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

/**
 * a struct contains a movie name and it's resemblance score
 */
typedef struct {double score; std::string name; } resMovie;

/**
 * the class of our recommendation system
 */
class RecommenderSystem
{
private:
    std::vector<std::string> _movieNames; // in the order of the rank file movie list
    std::map<std::string, std::vector<double> > _clients; // clients and their past rankings
    std::map<std::string, int > _clientsRanksNum; // number of movies watched by each client
    std::map<std::string, std::vector<double> > _movies; // movies and their attribute rankings
    /**
     * finds the best movie to recommend based on the users' preference vector of movie attributes
     * @param user clients' name
     * @param prefVec clients' preference vector
     * @return a resMovie struct, contains the recommended movie's name and resemblance score
     */
    resMovie _findMovieByPref(const std::string &user, const std::vector<double> &prefVec);
    /**
     * helper method which calculate the norm of a given vector
     * @param vec
     * @return the norm of vec
     */
    static double _norm(const std::vector<double> &vec);
    /**
     * helper method which calculate the dot product of two vectors
     * @param a first vector
     * @param b second vector
     * @return the dot product
     */
    static double _dotProd(const std::vector<double> &a, const std::vector<double> &b);
    /**
     * creates the preference vector of a given client based on past ranks and movie attributes
     * @param user client name
     * @param curNorm normalized preference vector of the client
     * @return the clients preference vector
     */
    std::vector<double> _createPrefVec(const std::string &user, const std::vector<double> &curNorm);
    /**
     * helper method to calculate the normalized ranks vector of a given client
     * @param user client name
     * @return the normalized preference vector of the client
     */
    std::vector<double> _getNormRankVec(const std::string &user);
    /**
     * creates a vector of past ranked movies, sorted by resemblance to certain movie attributes,
     * from the closest to the most different one
     * @param movieAttributes the movie attributes according to which we sort
     * @param userHistory a map of the clients' past movies and their rankings
     * @return the sorted vector of past ranked movies in the above mentioned order
     */
    std::vector<resMovie> _findMovieByHistory(const std::vector<double> &movieAttributes,
                                              const std::map<std::string, double> &userHistory);
    /**
     * comperator function for the resMovie struct, based on the movies' score.
     * @param lhs
     * @param rhs
     * @return the bool value of the statement lhs.score > rhs.score
     */
    static bool _compResMovie(const resMovie &lhs, const resMovie &rhs);
    /**
     * helper method, updates user's rank vector according to the input file
     * @param i number of the watched movies so far + 1
     * @param clientName
     * @param val current movie rank parsed from the file
     */
    void _updateUserRank(int &i, const std::string &clientName, const std::string &val);
public:
    /**
     * a function which loads data from movie attributes file and clients rank history
     * @param moviesAttributesFilePath
     * @param userRanksFilePath
     * @return 0 upon success, -1 upon failure
     */
    int loadData(const std::string &moviesAttributesFilePath, const std::string &userRanksFilePath);
    /**
     * implementation of the content based algorithm, which uses existing ranks if movies and their
     * attributes to recommend a movie to the client
     * @param userName client name
     * @return movie recommended upon success, invalid client name message upon failure
     */
    std::string recommendByContent(const std::string &userName);
    /**
     * predicts a clients rank to a movie they didn't watch based on past rankings of the k movies
     * with attributes closest to the movie we want to predict for. using the scoring method
     * described in the pdf.
     * @param movieName the movie for which we predict the clients' rank
     * @param userName client name
     * @param k
     * @return the prediction of the clients' rank to the movie. if userName/movieName are not in
     * the database, returns -1
     */
    double predictMovieScoreForUser(const std::string &movieName, const std::string &userName, int k);
    /**
     * gets the best movie to recommend to the client by predicting users rank to the movies they did
     * not watch already and saving the best scoring movie between those. prediction is based on the
     * k closest ranked movies by the user, as mentioned in the method predictMovieScoreForUser.
     * @param userName clients name
     * @param k
     * @return the name of the movie for which our prediction is the highest
     */
    std::string recommendByCF(const std::string &userName, int k);
};


#endif //EX5_RECOMMENDERSYSTEM_H
