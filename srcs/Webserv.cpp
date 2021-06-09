#include "websrv.h"

Webserv::Webserv() : _conSize(0)
{}

Webserv::~Webserv()
{
	for (size_t index; index < this->_conSize; index++)
		(*this)[index].close();
}


void		Webserv::addServer(int port)
{
	this->_servers.push_back(Server(port, this->_servers.size(), this));
	this->updateIndexs(-2);
}

Connection &Webserv::operator[](int index)
{
	if (index > this->_conSize || index < 0)
		throw std::out_of_range("Out of Webserv range");
	std::vector<int>::iterator it = std::upper_bound(this->_indexTable.begin(), this->_indexTable.end(), index);
	--it;
	index -= *it;
	return _servers[it - this->_indexTable.begin()][index];
}

void	Webserv::updateIndexs(int index)
{
	if (index >= (int)this->_servers.size())
		return ;
	index++;
	this->_conSize++;
	if (index < 0)
	{
		if (this->_indexTable.size() == 0)
			this->_indexTable.push_back(0);
		else
		{
			index = this->_servers.size() - 1;
			this->_indexTable.push_back(this->_servers[index].size());
			index = this->_indexTable.size() - 1;
			this->_indexTable[index] += this->_indexTable[index - 1];
		}
	}
	else
	{
		while(index < this->_indexTable.size())
		{
			this->_indexTable[index]++;
			index++;
		}
	}
}

Server	&Webserv::serverAt(int index)
{
	return this->_servers[index];
}
