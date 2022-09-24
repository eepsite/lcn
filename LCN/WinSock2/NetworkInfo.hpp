#pragma once
class NetworkInfo
{
private:
	const char* nAddress = ("127.0.0.1");
	const int nCommunicationsPort = 54000;
	const int nRegistrationPort = 54005;
	const int nStreamPort = 54010;
public:
	NetworkInfo() {}
	~NetworkInfo() {}
public:
	const char* GetAddress() const;
	int GetCommunicationsPort() const;
	int GetRegistrationPort() const;
	int GetStreamPort() const;
};

