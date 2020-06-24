//
// Created by michael on 22/06/2020.
//

#include "hadar.h"


bool gFirstLine = true;

// --------------------------------- PARSING FILES ----------------------------------------------//

void RecommenderSystem::_parseStringLine(const string &line, vector<string> &vec)
{
    std::istringstream iss(line);
    do
    {// reading the word
        string word;
        iss >> word;
        //adding the word to a provided vector
        if (!word.empty())
        {
            vec.push_back(word);
        }
    } while (iss); // repeat for all words in line
}


void RecommenderSystem::_parseEachLine(const string &line, vector<double> &vec)
{
    std::istringstream iss(line);
    do
    {// reading the word
        string word;
        iss >> word;
        //adding the word to a provided vector
        if (!word.empty())
        {
            word == "NA" ? vec.push_back(NA) : vec.push_back(std::stoi(word)); // NA will 0
        }
    } while (iss); // repeat for all words in line

}

string RecommenderSystem::_popFirstWord(string &line)
{
    string word = line.substr(START, line.find_first_of(" \t"));
    line = line.substr(line.find_first_of(" \t"));
    return word;
}

bool RecommenderSystem::_parseFile(const string &filePath, bool isUserRankFile)
{
    std::ifstream file;
    file.open(filePath);
    if (!file.good())
    {
        std::cout << BAD_PATH << filePath << std::endl;
        file.close();
        return INVALID;
    }
    string line;
    while (std::getline(file, line))
    {
        if (gFirstLine && isUserRankFile)
        { // first line represents movies names (only in user ranks file)
            _parseStringLine(line, _movieNames);
            gFirstLine = false;
        }
        else
        { // each of the following lines will be added as key:value
            string key = _popFirstWord(line); //movieName of userName
            vector<double> tempVec;
            _parseEachLine(line, tempVec);
            isUserRankFile ? _userRankingsMap[key] = tempVec : _movieTraitsMap[key] = tempVec;

        }
    }
    gFirstLine = true;
    return SUCCESS;
}

bool
RecommenderSystem::loadData(const string &moviesAttributesFilePath, const string &userRanksFilePath)
{
    bool cond1 = _parseFile(moviesAttributesFilePath, false);
    bool cond2 = _parseFile(userRanksFilePath, true);
    if (cond1 == SUCCESS && cond2 == SUCCESS)
    {
        return SUCCESS;
    }
    return FAILURE;
}

//-------------------------------recommend by content--------------------------------------------//

string RecommenderSystem::recommendByContent(const string &userName)
{
    if (_userRankingsMap.find(userName) == _userRankingsMap.end())
    {
        return BAD_USER;
    }
    vector<double> userRanks = _userRankingsMap[userName];
    // PART I - normalize
    double avg = _getAverage(userRanks);
    for (auto &element : userRanks)
    {
        if (element != 0)
        {
            element -= avg;
        }
    }
    //PART II - get preference vector
    vector<double> prefVec = generatePrefVec(userRanks);
    // PART III - find the best fitted movie
    return findResemblance(prefVec, userRanks);
}

double RecommenderSystem::_getAverage(const vector<double> &vec)
{
    double sum = std::accumulate(vec.begin(), vec.end(), DOUBLE_INDICATOR);
    double size = 0;
    for (auto &element : vec)
    { // get size by ignoring NA values
        if (element != 0)
        {
            size++;
        }
    }
    return sum / size;
}

vector<double> RecommenderSystem::generatePrefVec(const vector<double> &normVec)
{
    int size = _movieTraitsMap[_movieNames[START]].size();
    vector<double> resVec(size);
    for (vector<double>::size_type i = 0; i != normVec.size(); i++)
    {
        double weight = normVec[i];
        if (weight != 0)
        {
            string movieName = _movieNames[i];
            vector<double> tempVec = _movieTraitsMap[movieName];
            _multByConst(weight, tempVec);
            _addUpVects(resVec, tempVec);
        }
    }
    return resVec;
}

string RecommenderSystem::findResemblance(vector<double> &prefVec, vector<double> &userRanks)
{
    double maxTheta = -DBL_MAX;
    string bestFitMovie = " ";
    for (vector<double>::size_type i = 0; i != userRanks.size(); i++)
    {// TODO whitehead is replaced with americansniper though it shouldnt (iteration ~120)
        if (userRanks[i] == NA)
        {
            string movieName = _movieNames[i];
            vector<double> movieTraits = _movieTraitsMap[movieName];
            double theta = compAngle(prefVec, movieTraits);
            if (theta > maxTheta)
            {
                maxTheta = theta;
                bestFitMovie = movieName;
            }
        }
    }
    return bestFitMovie;
}

//-------------------------------predict movie score for user -----------------------------------//

double RecommenderSystem::predictMovieScoreForUser(const string &movieName, const string &userName,
                                                   int k)
{
    if (std::find(_movieNames.begin(), _movieNames.end(), movieName) == _movieNames.end() ||
        _userRankingsMap.find(userName) == _userRankingsMap.end())
    { // no such movie or no such user name
        return INVALID;
    }
    vector<std::pair<std::pair<string, double>, double> > movieResem; // ((movie:index):resemb
    vector<std::pair<string, double>> watched = didWatch(const_cast<string &>(userName));
    vector<double> watchedMovieTraits = _movieTraitsMap[movieName];
    // iterate over all watched movies and find resemblance to movieName
    for (auto &movie : watched)
    {
        vector<double> currMovieTraits = _movieTraitsMap[movie.first];
        double resemblance = compAngle(watchedMovieTraits, currMovieTraits);
        movieResem.emplace_back(movie, resemblance);
    }
    return forecastRating(movieResem, const_cast<string &>(userName), k);


}

bool RecommenderSystem::sortBySecond(const std::pair<std::pair<string, double>, double> &a,
                                     const std::pair<std::pair<string, double>, double> &b)
{
    return a.second > b.second;
}

double
RecommenderSystem::forecastRating(vector<std::pair<std::pair<string, double>, double> > &dataVec,
                                  string &userName, int k)
{
    sort(dataVec.begin(), dataVec.end(), sortBySecond); //sorting data vec according to second
    // value of pair

    double numerator = 0; // sum_(j=0 to k) [ s_(mj) * r_(xj) ]
    double denominator = 0;// sum_(j=0 to k) [ s_(mj) ]
    for (int j = 0; j < k; j++) // we can assume k < size of vector
    {
        double resemblance = dataVec[j].second;
        string movieName = dataVec[j].first.first;
        int indexOfMovie = dataVec[j].first.second;
        double specificRating = _userRankingsMap[userName].at(indexOfMovie);
        denominator += resemblance;
        numerator += dataVec[j].second * specificRating;
    }
    return numerator / denominator;
}

//-------------------------------------recommend by cf-------------------------------------------//

string RecommenderSystem::recommendByCF(const string &userName, int k)
{
    if (_userRankingsMap.find(userName) == _userRankingsMap.end())
    {
        return BAD_USER;
    }
    vector<std::pair<string, double>> results;
    for (auto &movie: didNotWatch(const_cast<string &>(userName)))
    {
        double resemb = predictMovieScoreForUser(movie, userName, k);
        results.emplace_back(movie, resemb);
    }
    return getMovieWithMaxResemblance(results);
}

//----------------------------------utility methods----------------------------------------------//

void RecommenderSystem::_multByConst(double val, vector<double> &vec)
{
    for (auto &element : vec)
    {
        element *= val;
    }
}

void RecommenderSystem::_addUpVects(vector<double> &vec, const vector<double> &other)
{
    for (size_t i = 0; i < vec.size(); i++)
    {
        vec[i] += other[i];
    }
}

double RecommenderSystem::dotProduct(const vector<double> &vec1, const vector<double> &vec2)
{
    double product = 0;
    for (size_t i = 0; i < vec1.size(); i++)
    {
        product = product + (vec1[i] * vec2[i]);
    }
    return product;
}

double RecommenderSystem::norm(vector<double> &vec)
{
    double normSquared = 0;
    for (auto &element : vec)
    {
        normSquared += element * element;
    }
    return sqrt(normSquared);
}

double RecommenderSystem::compAngle(vector<double> &vec1, vector<double> &vec2)
{
    return (dotProduct(vec1, vec2) / (norm(vec1) * norm(vec2)));
}

vector<std::pair<string, double>> RecommenderSystem::didWatch(string &username)
{
    vector<std::pair<string, double>> didWatch;
    vector<double> userRanks = _userRankingsMap[username];
    for (size_t i = 0; i < userRanks.size(); i++)
    {
        if (userRanks[i] != 0)
        {
            string movieName = _movieNames[i];
            didWatch.emplace_back(movieName, i); // saving the movie name and the index in movie
            // names for future reach
        }
    }
    return didWatch;

}

vector<string> RecommenderSystem::didNotWatch(string &username)
{
    vector<string> didntWatch;
    vector<double> userRanks = _userRankingsMap[username];
    for (size_t i = 0; i < userRanks.size(); i++)
    {
        if (userRanks[i] == 0)
        {
            string movieName = _movieNames[i];
            didntWatch.push_back(movieName);
        }
    }
    return didntWatch;
}

string RecommenderSystem::getMovieWithMaxResemblance(vector<std::pair<string, double>> &data)
{
    double maxResemb = 0;
    string outputMovie = " ";
    for (auto &pair: data)
    {
        if (pair.second > maxResemb)
        {
            maxResemb = pair.second;
            outputMovie = pair.first;
        }
    }
    return outputMovie;
}


int main()
{
    string moviePath = "movies_small.txt";
    string ranksPath = "ranks_small.txt";
    RecommenderSystem obj;
    obj.loadData(moviePath, ranksPath);
    std::cout << obj.recommendByContent("Molly") << std::endl;
}

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
