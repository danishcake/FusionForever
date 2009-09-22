#pragma once
#include <TinyXML.h>
#include <map>

struct ChallengeVariantRecord
{
	int defeats;
	int victories;
	
	ChallengeVariantRecord()
	{
		defeats = 0;
		victories = 0;
	}

	ChallengeVariantRecord(int _defeats, int _victories)
	{
		defeats = _defeats;
		victories = _victories;
	}
};

class Scorekeeper
{
private:
	Scorekeeper(void);
	std::map<std::string, ChallengeVariantRecord> scores_;
public:
	~Scorekeeper(void);

	static Scorekeeper& Instance()
	{
		static Scorekeeper instance;
		return instance;
	}

	ChallengeVariantRecord QueryProgress(std::string _challenge);

	void ReportVictory(std::string _challenge);
	void ReportDefeat(std::string _challenge);
};
