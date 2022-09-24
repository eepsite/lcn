#pragma once
class VersionInfo
{
private:
	const int nReqVersion = 2;
public:
	VersionInfo() {}
	~VersionInfo() {}
public:
	int GetVersion() const;
};

