#pragma once

const int chars_per_record{ 10 };	// 1 << 10 = 2^10 = 1024. Lower bound for expected number of characters per record (avg).
const int video_counter_max{ 500 };	// maximum number of videos written per record.
const size_t src_len{ 11 };			// Length of YouTube video src identifier.

//	Strings
const std::string NoneGiven{ "None Given" };
const std::string XML_PathOut_ext{ ".csv" };

//	XML special characters
const std::string_view amp{"&"};
const std::string_view apos{"'"};
const std::string_view gt{">"};
const std::string_view lt{"<"};
const std::string_view quot{"\"\""};

const std::string_view amp_XML{ "&amp;" };
const std::string_view apos_XML{ "&apos;" };
const std::string_view gt_XML{ "&gt;" };
const std::string_view lt_XML{ "&lt;" };
const std::string_view quot_XML{ "&quot;" };

const std::string_view amp_13_XML{ "&#13;" };
const std::string_view amp_34_XML{ "&#34;" };
const std::string_view amp_38_XML{ "&#38;" };
const std::string_view amp_39_XML{ "&#39;" };
const std::string_view amp_60_XML{ "&#60;" };
const std::string_view amp_62_XML{ "&#64;" };

//	Errors
const std::string ErrorFileNotFound{ "File not found or not open." };
const std::string ErrorReadFail{ "File read failed." };
const std::string ErrorRecordNotFound{ "Read block did not contain at least one complete record." };
const std::string ErrorTerminate{ " Process terminated." };

//	Tags. Use the minimal unambiguous value, to minimize searching.
//	E.g. finding "sts>" is equivalent to finding "<artists>".
const std::string_view artists_0{ "sts>" };				// "<artists>".
const std::string_view genre_0{ "res>" };				// "<genres">
const std::string_view genre_1{ "<g" };					// "<genre">
const std::string_view genre_2{ "</g" };				// "</genre">
const std::string_view label_0{ "ls>" };				// "<labels>"
const std::string_view label_1{ "me=" };				// "name="
const std::string_view label_2{ "\"" };					// Second " quote after name=
const std::string_view name_1{ "<na" };					// "<name>"
const std::string_view name_2{ "</na" };				// "</name>"
const std::string_view release_1{ "<release " };		// Includes a terminal blank. Not "<release" as this is also in "<released".
const std::string_view release_2{ "se>" };				// "</release>". Not "</re" as this is also in "</released>". 
const std::string_view released_1{ "ed>" };				// "<released>"
const std::string_view released_2{ "</re" };			// "</released>"
const std::string_view releases{ "<releases>" };		// "<releases>"
const std::string_view style_0{ "les>" };				// "<styles">
const std::string_view style_1{ "<s" };					// "<style">
const std::string_view style_2{ "</s" };				// "</style">
const std::string_view title_1{ "<ti" };				// "<title>"
const std::string_view title_2{ "</ti" };				// "</title>"
const std::string_view videos_1{ "<v" };				// "<videos>". Assume always before "<video ".

const std::string_view genre_full_1{ "<genre>" };
const std::string_view name_full_1{ "<name>" };
const std::string_view release_full_1{ "<release id=" };
const std::string_view style_full_1{ "<style>" };
const std::string_view title_full_1{ "<title>" };

//	Lengths
const size_t len_artists_0 = artists_0.length();
const size_t len_genre_0 = genre_0.length();
const size_t len_genre_2 = genre_2.length();
const size_t len_label_0 = label_0.length();
const size_t len_label_1 = label_1.length();
const size_t len_name_1 = name_1.length();
const size_t len_released_1 = released_1.length();
const size_t len_release_2 = release_2.length();
const size_t len_style_0 = style_0.length();
const size_t len_style_2 = style_2.length();

const size_t len_genre_full_1 = genre_full_1.length();
const size_t len_name_full_1 = name_full_1.length();
const size_t len_release_full_1 = release_full_1.length();
const size_t len_style_full_1 = style_full_1.length();
const size_t len_title_full_1 = title_full_1.length();