#include "StdAfx.h"
#include "Scorekeeper.h"

Scorekeeper::Scorekeeper(void)
{ 
	TiXmlDocument scores_doc_;
	if(!scores_doc_.LoadFile("ProgressRecord.xml"))
	{
		scores_doc_.ClearError();
		TiXmlDeclaration* declaration = new TiXmlDeclaration("1.0", "", "");
		TiXmlNode* root = new TiXmlElement("Scores");
		scores_doc_.LinkEndChild(root);
		scores_doc_.LinkEndChild(declaration);
		scores_doc_.SaveFile();
	}

	TiXmlHandle progress_handle = TiXmlHandle(&scores_doc_);
	TiXmlElement* challenge = progress_handle.FirstChild("Scores").FirstChild("Level").Element();
	while(challenge)
	{
		std::string filename;
		if(challenge->QueryValueAttribute("Filename", &filename) == TIXML_SUCCESS)
		{
			TiXmlHandle challenge_handle = TiXmlHandle(challenge);
			TiXmlElement* wins = challenge_handle.FirstChild("Victory").Element();
			TiXmlElement* defeats = challenge_handle.FirstChild("Defeat").Element();
			if(wins && defeats)
			{
				int win_count = 0;
				int lose_count = 0;
				try
				{
					win_count = boost::lexical_cast<int, std::string>(wins->GetText());
				} catch(boost::bad_lexical_cast ex)
				{
					Logger::ErrorOut() << "Unable to cast wins: " << wins->GetText() << " to an int\n";
				}
				try
				{
					lose_count = boost::lexical_cast<int, std::string>(defeats->GetText());
				} catch(boost::bad_lexical_cast ex)
				{
					Logger::ErrorOut() << "Unable to cast defeats: " << defeats->GetText() <<" to an int\n";
				}
				scores_[filename] = ChallengeVariantRecord(lose_count, win_count);
			} else
				Logger::ErrorOut() << "Either the win or defeat is missing from " << filename << "\n";
		} else
			Logger::ErrorOut() << "Querying the Level attribute of " << filename << " failed\n";

		challenge = static_cast<TiXmlElement*>(challenge->NextSibling("Level"));
	}
}

Scorekeeper::~Scorekeeper(void)
{
	TiXmlDocument scores_doc_;

	TiXmlDeclaration* declaration = new TiXmlDeclaration("1.0", "", "");
	TiXmlNode* root = new TiXmlElement("Scores");
	scores_doc_.LinkEndChild(declaration);
	scores_doc_.LinkEndChild(root);
	

	std::pair<std::string, ChallengeVariantRecord> score;
	BOOST_FOREACH(score, scores_)
	{
		TiXmlElement* level = new TiXmlElement("Level");
		
		TiXmlElement* wins = new TiXmlElement("Victory");
		TiXmlNode* wins_text = new TiXmlText(boost::lexical_cast<std::string, int>(score.second.victories));
		wins->LinkEndChild(wins_text);

		TiXmlElement* defeats = new TiXmlElement("Defeat");
		TiXmlNode* defeats_text = new TiXmlText(boost::lexical_cast<std::string, int>(score.second.defeats));
		defeats->LinkEndChild(defeats_text);

		level->SetAttribute("Filename", score.first);
		level->LinkEndChild(wins);
		level->LinkEndChild(defeats);
		
		root->LinkEndChild(level);
	}

	scores_doc_.SaveFile("ProgressRecord.xml");
}

ChallengeVariantRecord Scorekeeper::QueryProgress(std::string _challenge)
{
	if(scores_.find(_challenge) != scores_.end())
	{
		return scores_[_challenge];
	} else
		return ChallengeVariantRecord(0, 0);
}

void Scorekeeper::ReportVictory(std::string _challenge)
{
	if(scores_.find(_challenge) != scores_.end())
	{
		ChallengeVariantRecord cvr = scores_[_challenge];
		cvr.victories++;
		scores_[_challenge] = cvr;
	} else
	{
		scores_[_challenge] = ChallengeVariantRecord(0, 1);
	}
}

void Scorekeeper::ReportDefeat(std::string _challenge)
{
	if(scores_.find(_challenge) != scores_.end())
	{
		ChallengeVariantRecord cvr = scores_[_challenge];
		cvr.defeats++;
		scores_[_challenge] = cvr;
	} else
	{
		scores_[_challenge] = ChallengeVariantRecord(1, 0);
	}
}