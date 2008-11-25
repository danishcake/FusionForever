#include "StdAfx.h"
#include "Scorekeeper.h"

Scorekeeper::Scorekeeper(void)
{ 
	scores_doc_.LoadFile("ProgressRecord.xml");
	if(scores_doc_.FirstChild() == NULL)
	{
		TiXmlDeclaration* declaration = new TiXmlDeclaration("1.0", "", "");
		scores_doc_.LinkEndChild(declaration);
	}
	
}

Scorekeeper::~Scorekeeper(void)
{
	scores_doc_.SaveFile("ProgressRecord.xml");
}

ChallengeVariantRecord Scorekeeper::QueryProgress(std::string _challenge)
{
	TiXmlHandle progress_handle = TiXmlHandle(&scores_doc_);
	TiXmlElement* challenge_handle = progress_handle.FirstChild(_challenge).Element();
	if(challenge_handle)
	{
		TiXmlElement* wins = progress_handle.FirstChild(_challenge).FirstChild("Victory").Element();
		TiXmlElement* defeats = progress_handle.FirstChild(_challenge).FirstChild("Defeats").Element();
		if(wins && defeats)
		{
			int win_count = 0;
			int lose_count = 0;
			try
			{
				win_count = boost::lexical_cast<int, std::string>(wins->ValueStr());
			} catch(boost::bad_lexical_cast ex)
			{
				Logger::LogError(std::string("Unable to cast wins: ") + wins->ValueStr() + std::string(" to an int"));
			}
			try
			{
				lose_count = boost::lexical_cast<int, std::string>(defeats->ValueStr());
			} catch(boost::bad_lexical_cast ex)
			{
				Logger::LogError(std::string("Unable to cast defeats") + defeats->ValueStr() + std::string(" to an int"));
			}
			return ChallengeVariantRecord(lose_count, win_count);
		} else
			return ChallengeVariantRecord(0,0);
	} else
	{
		return ChallengeVariantRecord(0,0);
	}
}

void Scorekeeper::ReportVictory(std::string _challenge)
{
	TiXmlHandle progress_handle = TiXmlHandle(&scores_doc_);
	TiXmlElement* challenge_handle = progress_handle.FirstChild(_challenge).Element();
	if(challenge_handle)
	{
		TiXmlElement* wins = progress_handle.FirstChild(_challenge).FirstChild("Victory").Element();
		if(wins)
		{
			int win_count = 0;
			try
			{
				win_count = boost::lexical_cast<int, std::string>(wins->GetText());
			} catch(boost::bad_lexical_cast ex)
			{
				Logger::LogError(std::string("Unable to cast ") + wins->GetText() + std::string(" to an int"));
			}
			wins->Clear();
			TiXmlText* nText = new TiXmlText(boost::lexical_cast<std::string, int>(++win_count));
			wins->LinkEndChild(nText);
		} else
			Logger::LogError(std::string("Unable to record victory for challenge ") +_challenge + std::string(" as the Victory tag is malformed"));
	} else
	{
		TiXmlElement* challenge = new TiXmlElement(_challenge);
		TiXmlElement* wins = new TiXmlElement("Victory");
		TiXmlElement* defeats = new TiXmlElement("Defeat");
		TiXmlText* winText = new TiXmlText(boost::lexical_cast<std::string, int>(1));
		TiXmlText* defeatText = new TiXmlText(boost::lexical_cast<std::string, int>(0));

		scores_doc_.LinkEndChild(challenge);
		challenge->LinkEndChild(wins);
		challenge->LinkEndChild(defeats);

		wins->LinkEndChild(winText);
		defeats->LinkEndChild(defeatText);
	}
}

void Scorekeeper::ReportDefeat(std::string _challenge)
{
	TiXmlHandle progress_handle = TiXmlHandle(&scores_doc_);
	TiXmlElement* challenge_handle = progress_handle.FirstChild(_challenge).Element();
	if(challenge_handle)
	{
		TiXmlElement* defeats = progress_handle.FirstChild(_challenge).FirstChild("Defeat").Element();
		if(defeats)
		{
			int defeat_count = 0;
			try
			{
				defeat_count = boost::lexical_cast<int, std::string>(defeats->GetText());
			} catch(boost::bad_lexical_cast ex)
			{
				Logger::LogError(std::string("Unable to cast ") + defeats->GetText() + std::string(" to an int"));
			}
			defeats->Clear();
			TiXmlText* nText = new TiXmlText(boost::lexical_cast<std::string, int>(++defeat_count));
			defeats->LinkEndChild(nText);
		} else
			Logger::LogError(std::string("Unable to record defeat for challenge ") +_challenge + std::string(" as the Defeat tag is malformed"));
	} else
	{
		TiXmlElement* challenge = new TiXmlElement(_challenge);
		TiXmlElement* wins = new TiXmlElement("Victory");
		TiXmlElement* defeats = new TiXmlElement("Defeat");
		TiXmlText* winText = new TiXmlText(boost::lexical_cast<std::string, int>(0));
		TiXmlText* defeatText = new TiXmlText(boost::lexical_cast<std::string, int>(1));

		scores_doc_.LinkEndChild(challenge);
		challenge->LinkEndChild(wins);
		challenge->LinkEndChild(defeats);

		wins->LinkEndChild(winText);
		defeats->LinkEndChild(defeatText);
	}
}