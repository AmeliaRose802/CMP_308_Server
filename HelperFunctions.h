#pragma once
//Add opperator support the sockaddr_in so it can be used in map. 
//Code from file on my learning space
bool operator==(const sockaddr_in& left, const sockaddr_in& right)
{
	return (left.sin_port == right.sin_port)
		&& (memcmp(&left.sin_addr, &right.sin_addr, sizeof(left.sin_addr)) == 0);
}

bool operator<(const sockaddr_in& left, const sockaddr_in& right)
{
	int c = memcmp(&left.sin_addr, &right.sin_addr, sizeof(left.sin_addr));
	if (c < 0)
		return true;
	else if (c == 0)
		return left.sin_port < right.sin_port;
	else
		return false;
}

bool sendToAllClients(Message msg, std::set<sockaddr_in> connections, SOCKET sock) {

	for (auto const& conn : connections)
	{
		// Send the same data back to the address it came from.
		if (sendto(sock, (const char *)&msg, sizeof(Message), 0,
			(const sockaddr *)&conn, sizeof(conn)) != sizeof(Message))
		{
			return true;
		}
	}
	return false;
}


void printMap(std::map<int, GameObject> objects) {
	for (auto elem : objects)
	{
		elem.second.print();
	}
}