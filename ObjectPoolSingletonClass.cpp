#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

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


static class ObjectPoolSingleton
{
private: //variables
	static const uint8_t m_maxNoOfObjs = 3;
	vector<unique_ptr<Object>> m_objectPool;

	static ObjectPoolSingleton* m_poolClassObj;
	static std::mutex m_mtx;

	

private: //methods
	ObjectPoolSingleton()
	{
		for (int i = 0; i < m_maxNoOfObjs; ++i)
		{
			m_objectPool.emplace_back(make_unique<Object>());
		}
	}
	~ObjectPoolSingleton()
	{

	}
public:
	static ObjectPoolSingleton* getPoolClassObject()
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
				m_poolClassObj = new ObjectPoolSingleton();
			}
		}

		return m_poolClassObj;
	}

	Object* acquireObject()
	{
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

ObjectPoolSingleton* ObjectPoolSingleton::m_poolClassObj = nullptr;
mutex ObjectPoolSingleton::m_mtx;

#if 0
int main()
{
	ObjectPoolSingleton* pool = ObjectPoolSingleton::getPoolClassObject();

	auto obj1 = pool->acquireObject();
	auto obj2 = pool->acquireObject();
	auto obj3 = pool->acquireObject();

	//Total object already acuried. So, pool will create new object.
	auto obj4 = pool->acquireObject();

	obj1->printHello(1);
	obj2->printHello(2);
	obj3->printHello(3);
	obj4->printHello(4);

	pool->releaseObject(obj1);
	pool->releaseObject(obj2);
	pool->releaseObject(obj3);

	//Pool is full. So, it can not accomodate this object.
	pool->releaseObject(obj4);

	return 0;
}
#endif