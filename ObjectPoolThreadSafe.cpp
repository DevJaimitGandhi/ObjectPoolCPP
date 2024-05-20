#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <queue>

using namespace std;

static class Object
{
public:
	Object() = default;
	~Object() = default;

	void printHello(const uint8_t number)
	{
		cout << "Hello From -> " << std::to_string(number) << "\n";
	}
};

/*
*	To make Object pool thread safe, we have to apply mutual exclusion (mutex)
*	for public methods of acquiring and releasing the objects.
* 
*	Here, we will use "ObjectPoolSingletonClass.cpp and modify that class".
*/

static class ObjectPoolThreadSafe
{
private: //variables
	static const uint8_t m_maxNoOfObjs = 3;
	vector<unique_ptr<Object>> m_objectPool;

	static ObjectPoolThreadSafe* m_poolClassObj;
	static std::mutex m_mtx;

	std::mutex m_threadSafeMtx;


private: //methods
	ObjectPoolThreadSafe()
	{
		for (int i = 0; i < m_maxNoOfObjs; ++i)
		{
			m_objectPool.emplace_back(make_unique<Object>());
		}
	}
	~ObjectPoolThreadSafe()
	{

	}
public:
	static ObjectPoolThreadSafe* getPoolClassObject()
	{
		/*
		* We are using "DOUBLE CHECK LOCKING MECHANISM" for Singleton class.
		* This is also known as "Lazy initialization of Singleton class"
		*
		* If we want to use Early initialization of Singleton class then
		*  no need to do double check locking mechanism.
		* We have to intialize class object when our application starts.
		*/
		if (m_poolClassObj == nullptr)
		{
			std::lock_guard<mutex> lck(m_mtx);
			if (m_poolClassObj == nullptr)
			{
				m_poolClassObj = new ObjectPoolThreadSafe();
			}
		}

		return m_poolClassObj;
	}

	Object* acquireObject()
	{
		std::lock_guard<std::mutex> lck(m_threadSafeMtx);
		if (m_objectPool.size())
		{
			Object* obj = std::move(m_objectPool.back()).get();
			m_objectPool.pop_back();
			return obj;
		}

		/*
		*	Pool is empty. So needs to create new object.
		*
			If our application doesn't allow to create new object
			then wait here till pools get available objects.
		*/
		cout << "acquireObject -> Pool is full. So creating new object\n";
		return new Object();
	}

	bool releaseObject(Object* obj)
	{
		std::lock_guard<std::mutex> lck(m_threadSafeMtx);
		if (obj == nullptr)
		{
			cout << "releaseObject -> Object can not be empty.\n";
			return false;
		}

		if (m_objectPool.size() >= m_maxNoOfObjs) {
			cout << "releaseObject -> Pool is full. So we can not put this object into pool.\n";
			return false;
		}

		m_objectPool.emplace_back(make_unique<Object>(*obj));
		return true;
	}
};

ObjectPoolThreadSafe* ObjectPoolThreadSafe::m_poolClassObj = nullptr;
mutex ObjectPoolThreadSafe::m_mtx;

static void workerMethod()
{
	ObjectPoolThreadSafe* pool = ObjectPoolThreadSafe::getPoolClassObject();
	for (int i = 0; i < 10; ++i)
	{
		auto obj = pool->acquireObject();

		/*
			To create some task, we are making thread sleep.
			Think of it like worker thread is doing some task.
		*/
		std:: this_thread::sleep_for(std::chrono::microseconds(1));

		pool->releaseObject(obj);
	}
}

#if 1
int main()
{
	queue<thread> collectionOfThreadQueue;
	for (int i = 0; i < 10; ++i)
	{
		//thread t(workerMethod);
		
		collectionOfThreadQueue.push(thread(workerMethod));
	}

	while (collectionOfThreadQueue.size())
	{
		collectionOfThreadQueue.front().join();
		collectionOfThreadQueue.pop();
	}

	cout << "Task completed\n";

	return 0;
}
#endif