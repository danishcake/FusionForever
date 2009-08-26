#include "StdAfx.h"
#include "Subscriber.h"

Subscriber::Subscriber(void)
{
}

Subscriber::~Subscriber(void)
{
	StopSubscribing();
}

void Subscriber::AddSubscriber(Subscriber* _subscriber)
{
	subscribers_.push_back(_subscriber);
}

void Subscriber::RemoveSubscriber(Subscriber* _subscriber)
{
	subscribers_.remove(_subscriber);
}

void Subscriber::StopSubscribing()
{
	BOOST_FOREACH(Subscriber* subscriber, subscribers_)
	{
		subscriber->EndSubscription(this);
	}
	subscribers_.clear();
}

void Subscriber::EndSubscription(Subscriber* _source)
{
	//Do nothing, but overloaded versions should check the incoming pointer, and invalidate them if they match
}

