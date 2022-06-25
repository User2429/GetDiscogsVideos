#pragma once

#include "GetDiscogsVideos.h"

const int cd{ 256 };

class ProgressMonitor
{
public:
	ProgressMonitor(int iterations, int status_percentage);
	void Update(int i);
	void UpdateFinal();

private:
	char str_time_elapsed[cd]{ "" };
	char str_time_end[cd]{ "" };
	char str_time_remaining[cd]{ "" };
	char str_time_start[cd]{ "" };

	char status[cd]{ "" };
	char str_i[cd]{ "" };
	char str_ipc[cd]{ "" };
	char str_i_pad[cd]{ "" };

	int err;

	int iterations;
	int iterations_size;
	int pct_completed;
	int progress_percentage;
	int status_percentage;

	int i_size{ 1 };
	int i_status;

	struct tm tm_start;	
	struct tm tm_elapsed;
	struct tm tm_end;
	struct tm tm_remaining;

	time_t time_current{};
	time_t time_elapsed{};
	time_t time_end{};
	time_t time_remaining{};
	time_t time_start{};

	void Initialize(int i);
	void SetTimes(int i);
	void WriteStatus(int i);
	void WriteValues(int i);

	void WriteTimeStrings();
};
