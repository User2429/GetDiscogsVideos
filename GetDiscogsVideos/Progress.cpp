#include "Progress.h"

ProgressMonitor::ProgressMonitor(int it, int sp) : iterations{ it }, status_percentage{ sp }
//	Constructor. Set and post the start time.
{
	i_status = (int)std::floor(((double)(iterations * status_percentage) / 100) + 0.5);
	if (i_status < 1) i_status = 1;

	// Get the length of iterations for formatting.
	iterations_size = (int)trunc(log10(iterations)) + 1;

	time(&time_start);
	memcpy(&tm_start, localtime(&time_start), sizeof(tm_start));
	strftime(str_time_start, cd, "%b %d %H:%M:%S", &tm_start);

	char str_start[cd]{ "GetDiscogsVideos Start Time: " };
	strcat(str_start, str_time_start);
	strcat(str_start, ".\n ");

	std::cout << str_start << std::endl;
}

void ProgressMonitor::Update(int i)
//	For small number of iterations post status for every iteration.
//	For large number post every nth iteration (exit otherwise).
//	Always post the 1st iteration.
{
	if (i > 1 && i % i_status != 0) { return; }

	Initialize(i);
	SetTimes(i);
	WriteValues(i);
	WriteTimeStrings();
	WriteStatus(i);

	std::cout << status << std::endl;
}

void ProgressMonitor::Initialize(int i)
//	Re-initialize char arrays (C strings).
{
	status[0] = '\0';
	str_i[0] = '\0';
	str_ipc[0] = '\0';
	str_i_pad[0] = '\0';

	str_time_remaining[0] = '\0';

	if (i > 0) i_size = (int)trunc(log10(i)) + 1;
}

void ProgressMonitor::SetTimes(int i)
//	Set times and convert to tm.
{
	time(&time_current);
	time_elapsed = time_current - time_start;
	time_end = time_start + time_elapsed * iterations / ((long long)i + 1);
	time_remaining = time_end - time_current;

	memcpy(&tm_elapsed, gmtime(&time_elapsed), sizeof(tm_elapsed));
	memcpy(&tm_remaining, gmtime(&time_remaining), sizeof(tm_remaining));
	memcpy(&tm_end, localtime(&time_end), sizeof(tm_end));
}

void ProgressMonitor::WriteValues(int i)
//	Write i and iPctCompleted.
{
	pct_completed = (int)(100 * i / iterations);
	sprintf(str_i, "%d", i);
	sprintf(str_ipc, "%d", pct_completed);
}

void ProgressMonitor::WriteTimeStrings()
//	Convert times to char. Write str_time_remaining.
{
	strftime(str_time_end, cd, "%H:%M:%S", &tm_end);	
	strftime(str_time_elapsed, cd, "%H:%M:%S", &tm_elapsed);

	char str_TR_d[cd]{ "" };
	char str_TR_h[cd]{ "" };
	char str_TR_m[cd]{ "" };

	if ((tm_remaining).tm_yday != 0) sprintf(str_TR_d, "%dd ",  (tm_remaining).tm_yday);
	strcat(str_time_remaining, str_TR_d);
	if ((tm_remaining).tm_hour != 0) sprintf(str_TR_h, "%dh ", (tm_remaining).tm_hour);
	strcat(str_time_remaining, str_TR_h);
	if ((tm_remaining).tm_min < 10) strcat(str_time_remaining, " ");
	sprintf(str_TR_m, "%dm ", (tm_remaining).tm_min); 
	strcat(str_time_remaining, str_TR_m);
}

void ProgressMonitor::WriteStatus(int i)
//	Concatenate strings to form status output.
{
	strcat(status, "Step ");
	sprintf(str_i_pad, "%*s", iterations_size - i_size, "");
	strcat(status, str_i_pad);
	strcat(status, str_i);

	//	Add spaces to make percent lengths uniform (i.e. "  5", " 10", "100").
	if (pct_completed < 10)  strcat(status, " ");
	if (pct_completed < 100) strcat(status, " ");

	strcat(status, " (");
	strcat(status, str_ipc);
	strcat(status, "%). Time elapsed ");
	strcat(status, str_time_elapsed);

	//	Omit the estimated completion time for the last iteration.
	if (i < iterations) {
		strcat(status, ".  Expected completion in ");
		strcat(status, str_time_remaining);
		strcat(status, "at ");
		strcat(status, str_time_end);
	}

	strcat(status, ".");
}

void ProgressMonitor::UpdateFinal()
//	Post status on completion.
{
	status[0] = '\0';	// reinitialize

	time(&time_end);
	memcpy(&tm_end, localtime(&time_end), sizeof(tm_end));
	strftime(str_time_end, cd, "%b %d %H:%M:%S", &tm_end);

	strcat(status, "\nGetDiscogsVideos End Time: ");
	strcat(status, str_time_end);
	strcat(status, ".\n ");

	std::cout << status << std::endl;
}
