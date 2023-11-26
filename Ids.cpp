# include "Ids.hpp"

void	Ids::addId(const unsigned short id)
{
	auto it = std::find(this->_idVec.begin(), this->_idVec.end(), id);
    if (it != this->_idVec.end())
        return;
	this->_idVec.push_back(id);
}

void	Ids::delId(const unsigned short id)
{
	auto it = std::find(this->_idVec.begin(), this->_idVec.end(), id);
    if (it != this->_idVec.end())
        this->_idVec.erase(it);
}

const std::vector<unsigned short> &Ids::getIdVec()
{
	return (this->_idVec);
};

const unsigned short	&Ids::getId()
{
	return (this->_id);
};