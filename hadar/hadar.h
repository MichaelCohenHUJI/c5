//
// Created by michael on 22/06/2020.
//

#ifndef EX5_HADAR_H
#define EX5_HADAR_H


#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <filesystem>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <float.h>

#define SUCCESS 0
#define FAILURE 1
#define INVALID (-1)
#define BAD_PATH "Unable to open file "
#define BAD_USER "USER NOT FOUND"
#define NA 0
#define START 0
#define DOUBLE_INDICATOR (0.0)


using std::string;
using std::cerr;
using std::endl;
using std::vector;

/**
 *based on movie attributes file and a user ranks file, the class is responsible
 * of generating relevant recommendations to the user, in multiple ways.
 */
class RecommenderSystem
{
public:
    /**
     * loads the data to our recommendation system
     * @param moviesAttributesFilePath - filepath to movies attributes matrix
     * @param userRanksFilePath - filepath to user ranks matrix
     * @return 0 upon success, 1 otherwise.
     */
    bool loadData(const string &moviesAttributesFilePath, const string &userRanksFilePath);

    /**
     *generates a recommendation based on the content
     * @param userName - customer's user name
     * @return string - representing the recommended movie returned by the algorithm
     */
    string recommendByContent(const string &userName);

    /**
     * generates a prediction for a user's rank to a movie he hasn't seen yet, based
     * on the movies the user has already seen
     * @param movieName - movie to predict a rating for
     * @param userName - name of the customer
     * @param k - number of movies that are most similar to movieName
     * @return -1 if no such user or movie, float predicted rating otherwize
     */
    double predictMovieScoreForUser(const string &movieName, const string &userName, int k);

    /**
     * recommends a movie to the user based on the predictMovieScoreForUser algorithm
     * @param userName - name of the customer
     * @param k - number of movies that are most similar to movieName
     * @return string representing the name of the recommended movie
     */
    string recommendByCF(const string &userName, int k);


private:

    vector<string> _movieNames;
    std::map<string, vector<double>> _movieTraitsMap; // movieName:traits vec(scary,
    // funny etc..)
    std::map<string, vector<double>> _userRankingsMap;
    // userName:vector of (movie:rank) pairs

    /**
     * parses one of the files
     * @param filePath - path to file to parse
     * @param isUserRankFile - represents that the file is the user ranks
     * @return
     */
    bool _parseFile(const string &filePath, bool isUserRankFile);

    /**
     * parses each line, skipping spaces and inserting the values to provided vector
     * @param line some line to parse
     * @param vec vector of ints to insert line values to
     */
    static void _parseEachLine(const string &line, vector<double> &vec);

    /**
     * parses each line, skipping spaces and inserting the values to provided vector
     * @param line some line to parse
     * @param vec vector of string to insert line values to
     */
    static void _parseStringLine(const string &line, vector<string> &vec);

    /**
     * fetches the first word in a line, which is either a userName or a movieName
     * @param line some line to parse
     * @return first word in line
     */
    static string _popFirstWord(string &line);

    /**
     * calculates the average value of all items in vector
     * @param vec some vector of numbers
     * @return
     */
    static double _getAverage(const vector<double> &vec);

    vector<double> generatePrefVec(const vector<double> &normVec);

    /**
     * multiplies each element of vec by val(some const)
     */
    static void _multByConst(double val, vector<double> &vec);

    /**
     * adds other vec to vec (same size vectors)
     */
    static void _addUpVects(vector<double> &vec, const vector<double> &other);

    /**
     * computes dot product of two vectors of same length
     * @param vec1
     * @param vec2
     * @return dot product v1*v2
     */
    static double dotProduct(const vector<double> &vec1, const vector<double> &vec2);

    /**
     * calculates a norma of vector: sqrt(v[i]^2+...+v[n]^2)
     * @param vec some vector
     * @return norma
     */
    static double norm(vector<double> &vec);

    /**
     * calculate the similarity between two vectors, (the argument of arccos)
     * @param vec1
     * @param vec2
     * @return some 0<=value<=1, where 0 means no similarity and 1 means best similarity
     */
    static double compAngle(vector<double> &vec1, vector<double> &vec2);

    /**
     * for each movie a user hasent watched, calculates similarity, and extracts the movie
     * with the greatest resemblance
     * @param prefVec - preferences vector as calculated in generatePrefVec function
     * @param userRanks - the ranks a user has given to all movies (NA for which he hasnt watched)
     * @return name of the movie with the biggest resemblance
     */
    string findResemblance(vector<double> &prefVec, vector<double> &userRanks);

    /**
     * finds our which movie a username has watched (by ignoring NA values)
     * @param username some customer
     * @return vector of string representing movies the username have seen
     */
    vector<std::pair<string, double>> didWatch(string &username);

    /**
     * finds our which movie a username has not watched (all NA values)
     * @param username some customer
     * @return vector of string representing movies the username did watch
     */
    vector<string> didNotWatch(string &username);

    /**
     * forcasts a user rating for some movie using the algorithm provided.
     * @param dataVec - vector of pairs <movieName:resemblance>
     * @param k # of movies that are most similar
     * @return focasted rating of movieName
     */
    double
    forecastRating(vector<std::pair<std::pair<string, double>, double> > &dataVec, string &userName,
                   int k);

    /**
     * a helper function for built in sort, to sort by second element of pair in descending order
     * @param a some pair
     * @param b yet another pair
     * @return boolean value
     */
    static bool sortBySecond(const std::pair<std::pair<string, double>, double> &a,
                             const std::pair<std::pair<string, double>, double> &b);

    /**
     * iterates over a vector of pairs, find the largest resemblance (Vec.second)
     * and returns the corresponding movie name (vec.first)
     * @param data some vector with pairs elements (moviename:resemblance)
     * @return movie with the max resemblance
     */
    static string getMovieWithMaxResemblance(vector<std::pair<string, double>> &data);

};



#endif //EX5_HADAR_H
