#pragma once
#include <list>

class Subscriber
{
public:
	Subscriber(void);
	virtual ~Subscriber(void);

	/**
	  * Add a subscriber to alert on death
	  */
	void AddSubscriber(Subscriber* _subscriber);
	
	/**
	  * Remove a subscriber if it has died
	  */
	void RemoveSubscriber(Subscriber* _subscriber);

	/**
	  * Removes all current subscribers
	  */
	void StopSubscribing();

	/**
	  * Alert a subscription that it is about to be invalidated
	  */
	virtual void EndSubscription(Subscriber*);

private:
	/**
	  * A list of subscribers to alert on death
	  */
	std::list<Subscriber*> subscribers_;
};
