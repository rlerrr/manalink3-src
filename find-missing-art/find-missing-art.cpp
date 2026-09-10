// executable translation of ./find-missing-art.pl

#include <stdarg.h>
#include <stdlib.h>

#include <map>
#include <string>

#include <boost/regex.hpp>

#include "utils.h"
#include "File.h"

void
fatal(const char* fmt, ...)
{
  char buf[8000];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, 8000, fmt, args);
  va_end(args);

  fprintf(stderr, "%s\n", buf);

  exit(1);
}

void
find_replace_all(std::string& str, const char* needle, const char* replacement)
{
  std::string::size_type pos = 0, needle_len = strlen(needle), skip_replacement = strlen(replacement) + (!*needle ? 1 : 0);
  while ((pos = str.find(needle, pos)) != std::string::npos)
    {
      str.replace(pos, needle_len, replacement);
      pos += skip_replacement;
    }
}

struct Csvdata
{
  std::string full_name;
  std::string filename;
  bool in_manalink;
  bool in_shandalar;
  bool is_token;
};

int
main(int, char**)
{
  typedef std::map<int, Csvdata> CsvMap;
  CsvMap csv;
  boost::cmatch match;

  bool okchar[256] = {0};
  for (int i = 'a'; i <= 'z'; ++i)
    okchar[i] = true;
  for (int i = 'A'; i <= 'Z'; ++i)
    okchar[i] = true;
  for (int i = '0'; i <= '9'; ++i)
    okchar[i] = true;
  okchar[','] = true;
  okchar[' '] = true;
  okchar['\''] = true;
  okchar['-'] = true;

  File ml("Manalink.csv");
  boost::regex rgx_id_fullname_coded_exp_cardtype("^([0-9]+);"
						  "([^;]+);"
						  "([01]);"
						  "[^;]*;"
						  "[^;]*;"
						  "([^;]+);"
						  "[^;]+;"
						  "([^;]+);");
  boost::regex rgx_quoted("^\"(.+)\"$");

  char buf[1024];
  ml.readline(buf, 1024);	// ignore first line
  int linenum = 1;
  while (ml.readline(buf, 1024))
    {
      ++linenum;
      if (!buf[0] || !strncmp(buf, "#;", 2))
	continue;

      if (!boost::regex_search(buf, match, rgx_id_fullname_coded_exp_cardtype) || match.size() != 6)
	fatal("Manalink.csv: Could not parse line %d:\n%s", linenum, buf);

      int csvid = atoi(match[1].str().c_str());
      std::string full_name = match[2].str();
      if (boost::regex_search(full_name, rgx_quoted))
	{
	  full_name.erase(full_name.begin());	// erase leading "
	  full_name.erase(full_name.end() - 1);	// erase trailing "
	  find_replace_all(full_name, "\"\"", "\"");
	}

      if (csv.find(csvid) != csv.end())
	fatal("Manalink.csv: CSV collision at #%d: previously saw <<%s>>, now have <<%s>>",
	      csvid, csv[csvid].full_name.c_str(), full_name.c_str());

      char filename[full_name.length() + 2];
      strcpy(filename, full_name.c_str());
      for (char* p = filename; *p; ++p)
	if (!okchar[(int)(unsigned char)(*p)])
	  *p = '_';

      Csvdata& data = csv[csvid];
      data.full_name = full_name;
      data.filename = std::string("../CardArtManalink/") + filename + ".jpg";
      data.in_manalink = *match[3].first == '1';
      data.is_token = *match[5].first == '8' || match[4].str() == "1000h";
      if (data.is_token)
	data.in_manalink = false;
      data.in_shandalar = false;
    }

  ml.close();

  File cl("../cardlist.txt");

  boost::regex rgx_total("^[0-9]+ cards\\.$");
  boost::regex rgx_id_name("\\.([0-9]+)\t(.+)");

  linenum = 0;
  while (cl.readline(buf, 1024))
    {
      ++linenum;
      if (!buf[0] || boost::regex_search(buf, match, rgx_total))
	continue;

      if (!boost::regex_search(buf, match, rgx_id_name) || match.size() != 3)
	fatal("../cardlist.txt: Could not parse line %d:\n%s", linenum, buf);

      int csvid = atoi(match[1].str().c_str());
      CsvMap::iterator it = csv.find(csvid);
      if (it == csv.end())
	fatal("Could not find card #%d in manalink.csv for ../cardlist.txt line %d:\n%s",
	      csvid, linenum, buf);

      it->second.in_shandalar = true;
    }
  cl.close();

  File missing("missing.txt", "w");
  int num_missing = 0;
  for (CsvMap::const_iterator it = csv.begin(), end = csv.end(); it != end; ++it)
    {
      const Csvdata& data = it->second;

      if (!data.in_manalink && !data.in_shandalar && !data.is_token)
	continue;

      File jpg(data.filename.c_str(), "rb", true);
      if (!jpg.ok())
	{
	  ++num_missing;
	  missing.printf("%s %s %s   \"%s\"\n",
			 data.in_manalink ? "Mlink" : "     ",
			 data.in_shandalar ? "Shand" : "     ",
			 data.is_token ? "Token" : "     ",
			 data.filename.c_str());
	}
    }
  missing.printf("%d total missing.\n", num_missing);
}
