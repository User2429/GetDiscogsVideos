#include "GetDiscogsVideos.h"
#include "Constants.h"
#include "Functions.h"

long long GetBlockSize(char* argv)
//	Bitshift operator is << (e.g. 1 << 21 = 2^21 = 2,097,152 bytes = 2 MiB).
{
	return static_cast<long long>(1) << std::stoi(argv);
}

bool GetBoolean(char* argv)
//	Return boolean flag. 
{
	std::stringstream ss{ argv };
	bool b;
	ss >> std::boolalpha >> b;
	return b;
}
int GetMaxRecords(char* argv, int cpr)
//	Upper bound for number of records to be read in a block.
{
	return 1 << (std::stoi(argv) - cpr);
}

void SetupOutput(std::string& XML_out, const std::string& XML_PathOut, const std::string& XML_PathOut_file, 
	const int i, const bool multiple_files)
//	Set the output file name with optional inclusion of index (e.g. "_01") if multiple output files are used.
{
	std::string XML_PathOut_i{ "" };
	if (multiple_files) XML_PathOut_i = IndexToString(i);
	XML_out = XML_PathOut + XML_PathOut_file + XML_PathOut_i + XML_PathOut_ext;
}

std::string IndexToString(const int i)
//	Convert integer n to string _0n, and integer nn to string _nn.
{
	std::string s = std::to_string(i);
	if (i < 10) s = "0" + s;
	s = "_" + s;
	return s;
}

bool SetFiles(std::ifstream& file_in, std::ofstream& file_out, const std::string& string_in, const std::string& string_out)
//	Set the input and output files. Open the input file at the end (ate) so that tellg() returns the file size.
{
	file_in.open(string_in, std::ios::in | std::ios::binary | std::ios::ate);
	if (!FileFound(file_in)) { return false; }

	file_out.open(string_out);
	return true;
}

bool FileFound(std::ifstream& ifs)
//	Return true if the file is found, false if it is not found.
{
	if (!ifs.is_open())
	{
		std::cout << ErrorFileNotFound << ErrorTerminate << '\n';
		return false;
	}

	std::cout << "XML file open." << '\n';
	return true;
}

int GetIterations(std::ifstream& XML_input, const long long sizeB)
//	Set estimated number of iterations as file size divided by block size. 
//	After using tellg(), reset the file read position to zero.
{
	long long file_size{ XML_input.tellg() };
	int iterations = (int)std::floor(file_size / sizeB + 0.5);
	XML_input.seekg(0);
	
	return iterations;
}

bool ReadBlock(std::ifstream& ifs, std::vector<char>& block, const long long size, const long block_count, bool& eof)
//	Read a block of the input file to a vector. Record the eof status.
//	block.reserve(size + 1) fails.
//	block.resize(size + 1)  fails.
//	Consequently block must be sized using a temporary, vtemp.
//	vtemp(size) fails, so write vtemp(size + 1).
//	A read error other than eof is terminal.
{
	std::vector<char> vtemp(size + 1);
	block = vtemp;

	ifs.read(block.data(), size);
	if (ifs.fail() && !ifs.eof())
	{
		std::cout << ErrorReadFail << "  Block: " << block_count << ErrorTerminate << '\n';
		return false;
	}

	eof = ifs.eof();
	return true;
}

void FindTag(std::vector<char>& block, std::string_view tag, size_t& pos)
//	pos is the position of the end of the search tag: e.g. for "se>" pos is the position of ">".
{
	std::string_view block_sv(block.data());
	pos = block_sv.rfind(tag);
	if (pos != std::string_view::npos) pos += tag.size() - 1;
}


bool ValidateBlock(size_t& rec_2, bool eof)
//	If eof the block is valid. 
//	If NOT eof THEN: If the record end tag found then the block is valid. If not found then it is not valid.
{
	if (eof) return true;
	if (rec_2 == std::string_view::npos)
	{
		std::cout << ErrorRecordNotFound << ErrorTerminate << '\n';
		return false;
	}
	return true;
}

void SetBlock(std::vector<char>& block, std::string_view& block_sv, const size_t rec_2, const long block_count)
//	For a given block and position rec_2 (end of the last "</record>" tag)
//  set block_sv to contain an integral number of records.
//	We assume that the file begins with <releases> plus a new line character.
//	So for the first block we start at releases.size() + 1.
{
	size_t start{ 0 };
	if (block_count == 0) start = releases.size() + 1;

	std::string_view temp(block.data());
	block_sv = temp.substr(start, rec_2 - start + 1);
}

void RewindReadPosition(std::ifstream& XML_input, const long long sizeB, const size_t rec_2)
//	The new read position is to be set at just past the end of the last complete record.
//	It is derived from the current read position at the end of the block, then going back
//	to the start (-sizeB) then forward (+rec_2) to the end of the search tag (end of "se>"),
//	then forward +2 for "'\n'<", so starting at "<" of the next "<release id =".
{
	long long loc_restart = XML_input.tellg();
	long long lr = rec_2;
	long long ls = sizeB;
	loc_restart += lr - ls + 2;
	XML_input.seekg(loc_restart);
}

void ReserveVectors(std::vector<size_t>& v1, std::vector<size_t>& v2, const long n)
//	Reserve vector sizes.
{
	v1.reserve(n);
	v2.reserve(n);
}

void SetDelimiterArrays(std::string_view& block_sv, std::vector<size_t>& vpos_rec_2, 
	std::vector<size_t>& vpos_vid_1, long& nrec)
//	Set arrays. Delimiters for start of video and end of record. Counter nrec is number of records with videos.
{
	size_t irec{ 0 };
	size_t ivid{ 0 };

	while (true)
	{
		ivid = block_sv.find(videos_1, irec);
		if (ivid == std::string_view::npos) return;
		irec = block_sv.find(release_2, ivid);		// If video is found end of record must exist.
		irec += len_release_2 - 1;

		vpos_rec_2.push_back(irec);
		vpos_vid_1.push_back(ivid);

		nrec++;
	}
}

void ResizeVectors(std::vector<size_t>& v1, std::vector<size_t>& v2, std::vector<size_t>& v3, std::vector<long>& v4, 
	std::vector<std::string>& v5, std::vector<std::string>& v6, std::vector<std::string>& v7, std::vector<std::string>& v8, 
	std::vector<std::string>& v9, std::vector<std::string>& v10, const long n)
//	Resize vectors.
{
	v1.resize(n);
	v2.resize(n);
	v3.resize(n);
	v4.resize(n);
	v5.resize(n);
	v6.resize(n);
	v7.resize(n);
	v8.resize(n);
	v9.resize(n);
	v10.resize(n);
}

void SetFirstRecord(std::string_view& block_sv, std::vector<size_t>& vpos_rec_1, 
	std::vector<size_t>& vpos_vid_1, std::vector<long>& v1_id)
//	Set the record start and id for the first record, based on searching backwards from the first video position.
{
	vpos_rec_1[0] = block_sv.rfind(release_1, vpos_vid_1[0]);

	size_t id_1 = vpos_rec_1[0] + len_release_full_1 + 1;
	size_t id_2 = block_sv.find('"', id_1);

	std::string_view id = block_sv.substr(id_1, id_2 - id_1);
	auto result = std::from_chars(id.data(), id.data() + id.size(), v1_id[0]);
}

void SetLastRecord(std::string_view& block_sv, std::vector<size_t>& vpos_rec_1, std::vector<size_t>& vpos_vid_1, 
	std::vector<long>& v1_id, const long nrec)
//	Set the record start and id for the last record, based on searching backwards from the last video position.
{
	size_t last = nrec - 1;
	vpos_rec_1[last] = block_sv.rfind(release_1, vpos_vid_1[last]);

	size_t id_1 = block_sv.find('"', vpos_rec_1[last] + len_release_full_1) + 1;
	size_t id_2 = block_sv.find('"', id_1);

	std::string_view id = block_sv.substr(id_1, id_2 - id_1);
	auto result = std::from_chars(id.data(), id.data() + id.size(), v1_id[last]);
}

void SetIDs(std::string_view& block_sv, std::vector<size_t>& vpos_rec_2, std::vector<long>& v1_id, const long nrec)
//	Set vector of ids for records after the first.
//	Search forward from the end of the previous record with videos.
//	Then find the next two ", which delimit the next id.
{
	for (long i = 1; i < nrec - 1; i++)
	{
		size_t id_1 = block_sv.find('"', vpos_rec_2[i - 1] + len_release_full_1) + 1;
		size_t id_2 = block_sv.find('"', id_1);

		std::string_view id = block_sv.substr(id_1, id_2 - id_1);
		auto result = std::from_chars(id.data(), id.data() + id.size(), v1_id[i]);
	}
}

void CompleteDelimiterArrays(std::string_view& block_sv, std::vector<size_t>& vpos_rec_1, std::vector<size_t>& vpos_rec_2,
	std::vector<size_t>& vpos_vid_1, std::vector<long>& v1_id, const long nrec)
//	Fill in record start values. Reset ids where required.
//	First and last records omitted as their values have already been set.
{
	for (long i = 1; i < nrec - 1; i++)
	{
		vpos_rec_1[i] = block_sv.find("<", vpos_rec_2[i - 1]);  // Default. Next "<" after previous record end.

		if (v1_id[i + 1] > v1_id[i] + 1)
		{
			vpos_rec_1[i] = block_sv.rfind(release_1, vpos_vid_1[i]);		// Record start position is counted back from video tag.

			size_t id_1 = block_sv.find('"', vpos_rec_1[i] + len_release_full_1) + 1;  // Update id for new record start.
			size_t id_2 = block_sv.find('"', id_1);

			std::string_view id = block_sv.substr(id_1, id_2 - id_1);
			auto result = std::from_chars(id.data(), id.data() + id.size(), v1_id[i]);
		}
	}
}

void GetArtists(std::string_view& record, size_t& recordpos, std::vector<std::string>& v1_artists, const long i)
//	artists_0 represents both "<artists>" and "</artists>" with locations loc_0 and loc_3.
//	"<name>" and "</name>" are searched for in string_view artists_sv between the artists tags.
{
	std::string artists{ "\"[" };

	size_t loca_0 = record.find(artists_0, recordpos);
	if (loca_0 == std::string_view::npos)
	{
		v1_artists[i] = NoneGiven;
		return;
	}
	loca_0 += len_artists_0;
	size_t loca_2{ 0 };
	size_t loca_3 = record.find(artists_0, loca_0);

	std::string_view artists_sv = record.substr(loca_0, loca_3 - loca_0);

	while (true)
	{
		size_t loca_1 = artists_sv.find(name_1, loca_2);
		if (loca_1 == std::string_view::npos) break;

		loca_1 += len_name_full_1;
		loca_2 = artists_sv.find(name_2, loca_1);

		std::string ar = GetString(artists_sv, loca_1, loca_2);
		if (artists.length() > 2) artists += ",";
		artists += ar;
		loca_2 += len_name_full_1;
	}

	recordpos = loca_3;
	ReplaceXML(artists);
	v1_artists[i] = artists + "]\"";
}

void GetTitle(std::string_view& record, size_t& recordpos, std::vector<std::string>& v3_title, const long i)
//	Get the release title.
{
	std::string title{ "" };
	size_t loct_1 = record.find(title_1, recordpos);
	if (loct_1 == std::string_view::npos)
	{
		v3_title[i] = NoneGiven;
		return;
	}
	loct_1 += len_title_full_1;

	size_t loct_2 = record.find(title_2, loct_1);

	recordpos = loct_2;
	title = GetStringSingle(record, loct_1, loct_2);
	ReplaceXML(title);
	v3_title[i] = title;
}

void GetLabels(std::string_view& record, size_t& recordpos, std::vector<std::string>& v4_labels, const long i)
// First find locl_0, i.e. "<labels>", then locl_1 and locl_2, i.e. "name=" and "\"" (double quote after "name=").
// Omit subsequent labels which are the same as the first. 
// Labels {l1, l1, l1} will be recorded as l1, labels {l1, l2} as l1, l2. 
// But Labels {l1, l2, l1, l2} as l1, l2, l2.
{
	size_t locl_0 = record.find(label_0, recordpos);
	if (locl_0 == std::string_view::npos)
	{
		v4_labels[i] = NoneGiven;
		return;
	}

	locl_0 += len_label_0;
	size_t locl_2{ 0 };
	size_t locl_3 = record.find(label_0, locl_0);

	std::string labels{ "\"[" };
	std::vector<std::string> labels_v;

	std::string_view labels_sv = record.substr(locl_0, locl_3 - locl_0);

	while (true)
	{
		bool found_la{ false };
		size_t locl_1 = labels_sv.find(label_1, locl_2);
		if (locl_1 == std::string_view::npos) break;

		locl_1 += len_label_1 + 1;
		locl_2 = labels_sv.find(label_2, locl_1);
		std::string la = GetString(labels_sv, locl_1, locl_2);

		for (auto iter = labels_v.begin(); iter != labels_v.end(); iter++)
		{
			found_la = (*iter == la);
			if (found_la) break;
		}

		if (!found_la) labels_v.push_back(la);
	}

	labels += labels_v[0];
	for (auto iter = labels_v.begin() + 1; iter != labels_v.end(); iter++)
	{
		labels += "," + *iter;
	}

	recordpos = locl_2;
	ReplaceXML(labels);
	v4_labels[i] = labels + "]\"";
}


void GetStyle(std::string_view& record, size_t& recordpos, std::vector<std::string>& v5_genre_style, const long i, 
	bool& style_found)
//	Get the release styles.
{
	GetGenreStyle(record, recordpos, v5_genre_style, style_0, style_1, style_2, len_style_0, len_style_full_1, len_style_2,
		i, style_found);
}

void GetGenre(std::string_view& record, size_t& recordpos, std::vector<std::string>& v5_genre_style, const long i, 
	bool& style_found)
//	Get the release genres. Only run if no styles have been found.
{
	if (style_found) return;
	GetGenreStyle(record, recordpos, v5_genre_style, genre_0, genre_1, genre_2, len_genre_0, len_genre_full_1, len_genre_2,
		i, style_found);
}

void GetGenreStyle(std::string_view& record, size_t& recordpos, std::vector<std::string>& v5_genre_style, 
	const std::string_view tag_0, const std::string_view tag_1, const std::string_view tag_2, const size_t len_0, 
	const size_t len_1, const size_t len_2, const long i, bool& tag_found)
//	Write the release styles (or genres).
{
	std::string genre_style{ "\"[" };

	size_t loc_0 = record.find(tag_0, recordpos);

	if (loc_0 == std::string_view::npos)
	{
		v5_genre_style[i] = NoneGiven;
		tag_found = false;
		return;
	}
	loc_0 += len_0;

	size_t loc_2{ 0 };
	size_t loc_3 = record.find(tag_0, loc_0);

	std::string_view genre_style_sv = record.substr(loc_0, loc_3 - loc_0);

	while (true)
	{
		size_t loc_1 = genre_style_sv.find(tag_1, loc_2);
		if (loc_1 == std::string_view::npos) break;
		loc_1 += len_1;
		loc_2 = genre_style_sv.find(tag_2, loc_1);

		std::string gs = GetString(genre_style_sv, loc_1, loc_2);
		if (genre_style.length() > 2) genre_style += ",";
		genre_style += gs;
		loc_2 += len_2;
	}

	genre_style += "]\"";

	tag_found = true;
	recordpos = loc_3;
	ReplaceXML(genre_style);
	v5_genre_style[i] = genre_style;
}

void GetReleased(std::string_view& record, size_t& recordpos, std::vector<std::string>& v6_released, const long i)
//	Get the release date.
{
	size_t locr_1 = record.find(released_1, recordpos);
	if (locr_1 == std::string_view::npos)
	{
		v6_released[i] = NoneGiven;
		return;
	}
	locr_1 += len_released_1;

	size_t locr_2 = record.find(released_2, locr_1);

	recordpos = locr_2;
	v6_released[i] = (std::string)record.substr(locr_1, locr_2 - locr_1);
}

void GetVideos(std::string_view& record, const size_t& recordpos, std::vector<std::string>& v7_videos, const long i)
//	Write videos. Maximum number per release is 500.
{
	size_t loct_2{ recordpos };
	std::string videos{ "\"[" };

	int video_counter{ 0 };

	while (video_counter < video_counter_max)
	{
		size_t loct_1 = record.find(title_1, loct_2);
		if (loct_1 == std::string_view::npos) break;

		loct_1 += len_title_full_1;
		loct_2 = record.find(title_2, loct_1);

		std::string video_title = GetString(record, loct_1, loct_2);

		size_t locs_1 = record.rfind("v=", loct_1 - len_title_full_1);
		std::string video_src = (std::string)record.substr(locs_1 + 2, src_len);

		if (videos.size() > 2) videos += ",";		// Add comma to the video std::string before the new data.
		videos += "{" + video_title + ":" + "\"\"" + video_src + "\"\"}";
		video_counter++;
	}

	videos += "]\"";
	ReplaceXML(videos);
	v7_videos[i] = videos;
}

std::string GetStringSingle(const std::string_view& record, const size_t& loc_1, const size_t& loc_2)
//	Return the string for a field with only one value (currently the only such field is Title).
//	Other text fields are arrays with multiple values.
//  Search for comma. Search for double quote ". If neither is found return the string unchanged.
//  Otherwise, replace any double quote by two double quotes (" ==> "") AND quote the entire string,
//	i.e. append one double quote to the beginning and end of the string.
//	The return string is not necessarily quoted (but strings for array fields are always quoted).
{
	std::string gs = (std::string)record.substr(loc_1, loc_2 - loc_1);
	size_t loc_c = gs.find(",");
	size_t loc_q = gs.find("\"");

	if (loc_c == std::string::npos && loc_q == std::string::npos) return gs;

	while (loc_q != std::string::npos)
	{
		gs.replace(loc_q, 1, "\"\"");
		loc_q = gs.find("\"", loc_q + 2);
	}
	gs = "\"" + gs + "\"";
	return gs;
}

std::string GetString(const std::string_view& record, const size_t& loc_1, const size_t& loc_2)
//	Replace any " by "". Quote the string with double double quotes ("") at the start and end.
{
	std::string gs = (std::string)record.substr(loc_1, loc_2 - loc_1);
	size_t loc_q = gs.find("\"");

	while (loc_q != std::string::npos)
	{
		gs.replace(loc_q, 1, "\"\"");
		loc_q = gs.find("\"", loc_q + 2);
	}

	gs = "\"\"" + gs + "\"\"";
	return gs;
}

void ReplaceXML(std::string& string_in)
//	Replace XML escape strings: &amp; &apos; &gt; &lt; &quot; by & ' > < " .
//	This includes the alternate representations, e.g. amp_38_XML is the XML code for amp_XML, etc.
//	Replace "\n", "\r" and "&#13;" (the XML code for \r) by the empty string "".
{
	ReplaceXML_2(string_in, amp_XML, amp);
	ReplaceXML_2(string_in, apos_XML, apos);
	ReplaceXML_2(string_in, gt_XML, gt);
	ReplaceXML_2(string_in, lt_XML, lt);
	ReplaceXML_2(string_in, quot_XML, quot);

	ReplaceXML_2(string_in, amp_38_XML, amp);
	ReplaceXML_2(string_in, amp_39_XML, apos);
	ReplaceXML_2(string_in, amp_62_XML, gt);
	ReplaceXML_2(string_in, amp_60_XML, lt);
	ReplaceXML_2(string_in, amp_34_XML, quot);

	ReplaceXML_2(string_in, "\n", "");
	ReplaceXML_2(string_in, "\r", "");
	ReplaceXML_2(string_in, amp_13_XML, "");
}

void ReplaceXML_2(std::string& string_in, const std::string_view e, const std::string_view r)
//	Replace all occurrences of the existing substring e by r in string_in.
{
	const size_t len = e.length();
	size_t loc = string_in.find(e);

	while (loc != std::string::npos)
	{
		string_in.replace(loc, len, r);
		loc = string_in.find(e, loc);
	}
}

void Finish(std::ofstream& file_out, const double start_time)
//	Finish.
{
	file_out.close();
}
