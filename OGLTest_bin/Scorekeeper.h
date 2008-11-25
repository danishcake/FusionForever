#pragma once
#include <TinyXML.h>

struct ChallengeVariantRecord
{
	int defeats;
	int victories;

	ChallengeVariantRecord(int _defeats, int _victories)
	{
		defeats = _defeats;
		victories = _victories;
	}
};

class Scorekeeper
{
private:
	TiXmlDocument scores_doc_;
	Scorekeeper(void);
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
