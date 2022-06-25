#pragma once

//	Advance declarations.
bool FileFound(std::ifstream& ifs);

std::string GetString(const std::string_view&, const size_t&, const size_t&);
std::string GetStringSingle(const std::string_view&, const size_t&, const size_t&);
std::string IndexToString(const int);

void GetGenreStyle(std::string_view&, size_t&, std::vector<std::string>&, const std::string_view, const std::string_view,
	const std::string_view, const size_t, const size_t, const size_t, const long i, bool&);
void ReplaceXML(std::string&);
void ReplaceXML_2(std::string&, const std::string_view, const std::string_view);

//	Functions.
bool GetBoolean(char*);
bool ReadBlock(std::ifstream&, std::vector<char>&, const long long, const long, bool&);
bool SetFiles(std::ifstream&, std::ofstream&, const std::string&, const std::string&);
bool ValidateBlock(size_t&, bool);

int GetIterations(std::ifstream&, const long long);
int GetMaxRecords(char*, int);

long long GetBlockSize(char*);

void CompleteDelimiterArrays(std::string_view&, std::vector<size_t>&, std::vector<size_t>&, std::vector<size_t>&, 
	std::vector<long>&, const long);
void FindTag(std::vector<char>&, std::string_view, size_t&);
void Finish(std::ofstream&, const double);
void GetArtists(std::string_view&, size_t&, std::vector<std::string>&, const long);
void GetGenre(std::string_view&, size_t&, std::vector<std::string>&, const long, bool&);
void GetLabels(std::string_view&, size_t&, std::vector<std::string>&, const long);
void GetReleased(std::string_view&, size_t&, std::vector<std::string>&, const long);
void GetStyle(std::string_view&, size_t&, std::vector<std::string>&, const long, bool&);
void GetTitle(std::string_view&, size_t&, std::vector<std::string>&, const long);
void GetVideos(std::string_view&, const size_t&, std::vector<std::string>&, const long);
void ReserveVectors(std::vector<size_t>&, std::vector<size_t>&, const long);
void ResizeVectors(std::vector<size_t>&, std::vector<size_t>&, std::vector<size_t>&, std::vector<long>&, 
	std::vector<std::string>&, std::vector<std::string>&, std::vector<std::string>&, std::vector<std::string>&, 
	std::vector<std::string>&, std::vector<std::string>&, const long);
void RewindReadPosition(std::ifstream&, const long long, const size_t);
void SetBlock(std::vector<char>&, std::string_view&, const size_t, const long);
void SetDelimiterArrays(std::string_view&, std::vector<size_t>&, std::vector<size_t>&, long&);
void SetIDs(std::string_view&, std::vector<size_t>&, std::vector<long>&, const long);
void SetFirstRecord(std::string_view&, std::vector<size_t>&, std::vector<size_t>&, std::vector<long>&);
void SetLastRecord(std::string_view&, std::vector<size_t>&, std::vector<size_t>&, std::vector<long>& v1_id, const long nrec);
void SetupOutput(std::string&, const std::string&, const std::string&, const int, const bool);
