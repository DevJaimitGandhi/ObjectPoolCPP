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


/*
	Applcation of Object Pool:

	1.) Database connection object pool
	2.) Websocket connection object pool for Stock Trading broker with NSE/BSE.
	3.) Bulky object creation.
*/
static class ObjectPool
{

private:
	uint8_t m_maxPoolCapacity;
	vector<unique_ptr<Object>> m_objectPool;

public:
	ObjectPool()
	{
		m_maxPoolCapacity = 0;
	}

	ObjectPool(uint8_t poolCapacity)
	{
		m_maxPoolCapacity = poolCapacity;
		for (int i = 0; i < poolCapacity; ++i)
		{
			m_objectPool.emplace_back(make_unique<Object>());
		}
	}

	~ObjectPool()
	{
		/*
			As Object pool is a vector of shared pointer, 
			we do not require to delete the objects.
		*/
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

		if (m_objectPool.size() >= m_maxPoolCapacity) {
			cout << "releaseObject -> Pool is full. So we can not put this object into pool.\n";
			return false;
		}

		m_objectPool.emplace_back(make_unique<Object>(*obj));
		return true;
	}
};

#if 0
int main()
{
	ObjectPool pool(3);

	auto obj1 = pool.acquireObject();
	auto obj2 = pool.acquireObject();
	auto obj3 = pool.acquireObject();

	//Total object already acuried. So, pool will create new object.
	auto obj4 = pool.acquireObject();

	obj1->printHello(1);
	obj2->printHello(2);
	obj3->printHello(3);
	obj4->printHello(4);

	pool.releaseObject(obj1);
	pool.releaseObject(obj2);
	pool.releaseObject(obj3);

	//Pool is full. So, it can not accomodate this object.
	pool.releaseObject(obj4);

	return 0;
}
#endif

