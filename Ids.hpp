#ifndef IDS_CLASS_HPP
# define IDS_CLASS_HPP
# include <string>
# include <iostream>
# include <unordered_map>
# include <vector>

# define MAX_MAP_SIZE 4096

class Ids
{
	public:
		Ids(unsigned short id) : _id(id) {};
		~Ids() {};
		
		const	std::vector<unsigned short> &getIdVec();
		const	unsigned short				&getId();
		void	addId(const unsigned short id);
		void	delId(const unsigned short id);
	private:
		const unsigned short			_id;
		std::vector<unsigned short>	_idVec;
};

#endif
