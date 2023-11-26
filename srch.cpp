#include "srch.hpp"

static std::string	convertToLower(std::string str)
{
	std::string lower_str;

	for (size_t i = 0; i < str.size(); i++)
		lower_str += tolower(str[i]);
	return (lower_str);
}

static int damerauLevenshtein(const std::string& s1, const std::string& s2) {
	int i = 0;
	int s1_len = s1.length();
	int s2_len = s2.length();
    std::vector<std::vector<int>> d(s1_len+1, std::vector<int>(s2_len+1));

    for(i = 0; i <= s1_len; i++)
        d[i][0] = i;
    for(int j = 0; j <= s2_len; j++)
        d[0][j] = j;

    for(i = 1; i <= s1_len; i++) {
        for(int j = 1; j <= s2_len; j++) {
            int cost = s1[i-1] == s2[j-1] ? 0 : 1;
            d[i][j] = std::min({d[i-1][j] + 1, d[i][j-1] + 1, d[i-1][j-1] + cost});
            if(i > 1 && j > 1 && s1[i-1] == s2[j-2] && s1[i-2] == s2[j-1])
                d[i][j] = std::min(d[i][j], d[i-2][j-2] + cost);
        }
    }

    return d[s1_len][s2_len];
}

static bool commonChar(const std::string& s1, const std::string& s2, int n) {
    std::unordered_set<char> charSet(s1.begin(), s1.end());
    int commonCount = 0;
    for (char c : s2) {
        if (charSet.find(c) != charSet.end()) {
            ++commonCount;
            if (commonCount >= n) {
                return true;
            }
        }
    }
    return false;
}

static int calculateAdjustedDistance(const std::string& s1, t_file& s2) {
    int minDistance = INT_MAX;
	int i = 0;
	int s1_len = s1.length();
	int s2_len = s2.lower_name.length();
	std::string sub;
	int	dlDistance;
	int multi_i;
	int adjustedDistance;
    while (i <= s2_len - s1_len) {
		sub = s2.lower_name.substr(i, s1_len);
		if (commonChar(s1, sub, (s1_len + 3) / 4))
		{
			dlDistance = damerauLevenshtein(s1, sub);
			s2.pertinence_brut = dlDistance;
			if (!dlDistance && i)
				adjustedDistance = 1 + i / 12;
			else
			{
				multi_i = dlDistance > 1 ? dlDistance : 1;
        		adjustedDistance = dlDistance + (i * multi_i + 1) / 2;
			}
        	if (adjustedDistance < minDistance) {
        	    minDistance = adjustedDistance;
        	}
		}
		++i;
    }
    return minDistance;
}

std::vector<std::string>	searchFiles(std::string search, std::vector<std::string> files_names, unsigned int limit)
{
	std::vector<t_file> 		checked_names;
	std::string					lower_search = convertToLower(search);
	//int							lower_search_size = lower_search.size() - 1;
	int							level_pertinence;
	unsigned int				end = 0;
	std::vector<std::string>	results = {};
	t_file						file_data;

	for (auto& file_name : files_names)
	{
		file_data.name = file_name;
		file_data.pertinence_brut = INT_MAX;
		file_data.lower_name = convertToLower(file_name);
		file_data.pertinence = calculateAdjustedDistance(lower_search, file_data);
		checked_names.push_back(file_data);
	}
	std::sort(checked_names.begin(), checked_names.end(), [](t_file a, t_file b) { if (a.pertinence == b.pertinence && a.pertinence_brut != b.pertinence_brut) return a.pertinence_brut < b.pertinence_brut; else if (a.pertinence == b.pertinence) return a.lower_name < b.lower_name; else return a.pertinence < b.pertinence; });
	level_pertinence = checked_names[0].pertinence;
	for (auto& file_data : checked_names)
	{
		end += file_data.pertinence - level_pertinence;
		level_pertinence = file_data.pertinence;
		if ((end >= limit && file_data.pertinence_brut > 0) || level_pertinence == INT_MAX)
			break ;
		results.push_back(file_data.name);
	}
	return (results);
}
