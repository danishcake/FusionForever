#pragma once

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
	  * Alert a subscription that it is about to be invalidated
	  */
	virtual void EndSubscription(Subscriber* _source);

private:
	/**
	  * A list of subscribers to alert on death
	  */
	std::list<Subscriber*> subscribers_;
};
