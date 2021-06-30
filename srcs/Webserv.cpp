#include "Webserv.hpp"

Webserv::Webserv() : _conSize(0)
{}

Webserv::~Webserv()
{
	for (size_t index = 0; index < this->_conSize; index++)
		(*this)[index].close();
}


void		Webserv::addServer(ServerData const &data)
{
	this->_servers.push_back(Server(data, this->_servers.size(), this));
	this->updateIndexs(-2, 1);
}

Connection &Webserv::operator[](size_t index)
{
	if (index > this->_conSize || index < 0)
		throw std::out_of_range("Out of Webserv range");
	std::vector<int>::iterator it = std::upper_bound(this->_indexTable.begin(), this->_indexTable.end(), index);
	--it;
	index -= *it;
	return _servers[it - this->_indexTable.begin()][index];
}

void	Webserv::updateIndexs(int index, int type)
{
	if (index >= (int)this->_servers.size())
		return ;
	index++;
	this->_conSize += type;
	if (index < 0)
	{
		if (this->_indexTable.size() == 0)
			this->_indexTable.push_back(0);
		else
		{
			index = this->_servers.size() - 2;
			this->_indexTable.push_back(this->_servers[index].size());
			index = this->_indexTable.size() - 1;
			this->_indexTable[index] += this->_indexTable[index - 1];
		}
	}
	else
	{
		while(index < (int)this->_indexTable.size())
		{
			this->_indexTable[index] += type;
			index++;
		}
	}
}

Server	&Webserv::serverAt(int index)
{
	return this->_servers[index];
}

void	hookPollIn(Webserv &web, size_t i)
{
	if (web[i].is_Server())
	{
		try{
			web[i].getServer()->connect();}
		catch (std::runtime_error & e) {
			outputLogs(e.what());
		}
	}
	else
	{
		web[i].read();
		web._pollArray[i].events = POLLOUT | POLLIN;
	}
}

void	hookPollOut(Webserv &web, size_t i)
{
	if (web[i].getRequest().getData().length())
		web[i].getRequest().parseRequest();
	if (web[i].getRequest().isDone) {
		web[i].send();
		if (web[i].getRequest().getConnectionType() == "close" || web[i].getRequest().getRequestError())
			web[i].getServer()->erase(i);
		web[i].getRequest().clear();
		web[i].getResponse().clear();
	}
	web._pollArray[i].events = POLLIN;
}

void	Webserv::hook()
{
	int		p;
	size_t	size;

	while (1)
	{
		p = poll(&(this->_pollArray[0]), this->_conSize, -1);
		if (p < 0)
			break ;
		size = this->_conSize;
		for (size_t i = 0; i < size; i++)
		{
			if (this->_pollArray[i].revents == 0)
				continue ;
			if ((this->_pollArray[i].revents & POLLNVAL) || (this->_pollArray[i].revents & POLLERR))
				exit(1);
			if (this->_pollArray[i].revents & POLLIN)
				hookPollIn(*this, i);
			if (this->_pollArray[i].revents & POLLOUT)
				hookPollOut(*this, i);
			if (this->_pollArray[i].revents & POLLHUP)
				(*this)[i].getServer()->erase(i);
		}
	}
}

void	Webserv::init(std::vector<ServerData> const &svsdata)
{
	std::vector<int> ports;
	std::vector<std::string> hosts;

	for (size_t index = 0; index < svsdata.size(); index++)
	{
		for (size_t j = 0; j < ports.size(); j++)
		{
			if (svsdata[index].getPort() == ports[j] && svsdata[index].getHost() == hosts[j])
			{
				this->_servers[j].addData(svsdata[index]);
				goto MAINLOOP;
			}
		}
		try {
			this->addServer(svsdata[index]);}
		catch (std::runtime_error &e) {
			outputLogs(e.what());
			continue ;
		}
		ports.push_back(svsdata[index].getPort());
		hosts.push_back(svsdata[index].getHost());
		MAINLOOP: continue;
	}
	if (this->_servers.size() == 0)
	{
		outputLogs("Error: Could not create any server! Webserv stoping...");
		exit(1);
	}
}
