#include "GetDiscogsVideos.h"
#include "Constants.h"
#include "Functions.h"
#include "Progress.h"

//	Initialized
bool eof{ false };
bool style_found{ false };
double start_time = clock();
int output_files_count{ 1 };
int output_rows_count{ 0 };
long block_count{ 1 };
size_t record_block_2{ 0 };

//	Uninitialized
std::string XML_PathOut_full;
std::string_view block_sv;

std::vector<char> block;
std::vector <long> v1_id;
std::vector <std::string> v2_artists;
std::vector <std::string> v3_title;
std::vector <std::string> v4_labels;
std::vector <std::string> v5_genre_styles;
std::vector <std::string> v6_released;
std::vector <std::string> v7_videos;

//  File streams
std::ifstream XML_input;
std::ofstream outfile;

int main(int, char* argv[])
{
	const std::string XML_PathIn{ argv[1] };
	const std::string XML_PathOut{ argv[2] };
	const std::string XML_PathOut_file{ argv[3] };
	const long long sizeB = GetBlockSize(argv[4]);
	const int nrec_max = GetMaxRecords(argv[4], chars_per_record);
	const bool multiple_files = GetBoolean(argv[5]);
	const int output_rows_max{ std::stoi(argv[6]) };
	const int status_percentage{ std::stoi(argv[7]) };

	SetupOutput(XML_PathOut_full, XML_PathOut, XML_PathOut_file, output_files_count, multiple_files);
	if (!SetFiles(XML_input, outfile, XML_PathIn, XML_PathOut_full)) return 10;
	const int iterations = GetIterations(XML_input, sizeB);

	ProgressMonitor PM(iterations, status_percentage);

	while (true)
	{
		if (!ReadBlock(XML_input, block, sizeB, block_count, eof)) return 20;
		FindTag(block, release_2, record_block_2);

		if (!ValidateBlock(record_block_2, eof)) return 30;			// Exit with error: No eof and no end tag. 
		if (record_block_2 == std::string_view::npos) break;		// Exit: Eof and no end tag.

		SetBlock(block, block_sv, record_block_2, block_count);

		//	Process block_sv.
		long nrec{ 0 };
		std::vector<size_t> vpos_rec_1;
		std::vector<size_t> vpos_rec_2;
		std::vector<size_t> vpos_vid_1;

		ReserveVectors(vpos_rec_2, vpos_vid_1, nrec_max);
		SetDelimiterArrays(block_sv, vpos_rec_2, vpos_vid_1, nrec);
	
		//	If the block has no records with video then reset the read position and proceed with the next iteration
		if (nrec == 0) {
			RewindReadPosition(XML_input, sizeB, record_block_2);
			continue;
		}

		ResizeVectors(vpos_rec_1, vpos_rec_2, vpos_vid_1, v1_id, v2_artists, v3_title,
			v4_labels, v5_genre_styles, v6_released, v7_videos, nrec);
		SetFirstRecord(block_sv, vpos_rec_1, vpos_vid_1, v1_id);
		SetLastRecord(block_sv, vpos_rec_1, vpos_vid_1, v1_id, nrec);
		SetIDs(block_sv, vpos_rec_2, v1_id, nrec);
		CompleteDelimiterArrays(block_sv, vpos_rec_1, vpos_rec_2, vpos_vid_1, v1_id, nrec);

		//	Process record
		for (long i = 0; i < nrec; i++)
		{
			size_t recordpos{ 0 };
			std::string_view record = block_sv.substr(vpos_rec_1[i], vpos_rec_2[i] - vpos_rec_1[i] + 1);

			GetArtists(record, recordpos, v2_artists, i);
			GetTitle(record, recordpos, v3_title, i);
			GetLabels(record, recordpos, v4_labels, i);
			GetStyle(record, recordpos, v5_genre_styles, i, style_found);
			GetGenre(record, recordpos, v5_genre_styles, i, style_found);
			GetReleased(record, recordpos, v6_released, i);
			GetVideos(record, vpos_vid_1[i] - vpos_rec_1[i], v7_videos, i);
		}

		for (long i = 0; i < nrec; i++)
		{
			outfile << v1_id[i] << "," << v2_artists[i] << "," << v3_title[i] << "," << v4_labels[i] << ","
					<< v5_genre_styles[i] << "," << v6_released[i] << "," << v7_videos[i] << std::endl;

			if (multiple_files)
			{
				output_rows_count++;
				if (output_rows_count == output_rows_max)
				{
					output_rows_count = 0;
					output_files_count++;
					outfile.close();
					
					std::string XML_PathOut_i = IndexToString(output_files_count);
					XML_PathOut_full = XML_PathOut + XML_PathOut_file + XML_PathOut_i + XML_PathOut_ext;
					outfile.open(XML_PathOut_full);
				}
			}
		}
		//	End process for block_sv

		if (eof) break;						// Exit at eof
		RewindReadPosition(XML_input, sizeB, record_block_2);
		PM.Update(block_count);
		block_count++;
	}

	Finish(outfile, start_time);
	PM.UpdateFinal();

	return 0;
}