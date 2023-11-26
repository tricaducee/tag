#ifndef SRCH_HPP
# define SRCH_HPP
# include <algorithm>
# include <filesystem>
# include <cstdlib>
# include <fstream>
# include <unordered_set>

typedef struct s_file
{
	std::string	name;
	std::string lower_name;
	int			pertinence;
	int			pertinence_brut;
}				t_file;

// std::string	convertToLower(std::string str);
// int damerauLevenshtein(const std::string& s1, const std::string& s2);
// bool commonChar(const std::string& s1, const std::string& s2, int n);
// int calculateAdjustedDistance(const std::string& s1, t_file& s2);
std::vector<std::string>	searchFiles(std::string search, std::vector<std::string> files_names, unsigned int limit);
#endif