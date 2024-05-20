#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

using namespace std;

class Object
{
public:
	Object() = default;
	~Object() = default;
};


/*
	Applcation of Object Pool:

	1.) Database connection object pool
	2.) Websocket connection object pool for Stock Trading broker with NSE/BSE.
	3.) Bulky object creation.
*/
class ObjectPool
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
			unique_ptr<Object> obj = std::move(m_objectPool.back());
			m_objectPool.pop_back();
			return obj.get();
		}

		/*
		*	Pool is empty. So needs to create new object.
		* 
			If our application doesn't allow to create new object
			then wait here till pools get available objects.
		*/
		cout << "Pool is full. So creating new object\n";
		return make_unique<Object>().get();
	}

	bool releaseObject(Object* obj)
	{
		if (obj == nullptr)
		{
			cout << "Object can not be empty.\n";
			return false;
		}

		if (m_objectPool.size() >= m_maxPoolCapacity) {
			cout << "Pool is full. So we can not put this object into pool.\n";
			return false;
		}

		m_objectPool.emplace_back(make_unique<Object>(*obj));
		return true;
	}
};

#if 1
int main()
{
	ObjectPool pool(3);

	auto obj1 = pool.acquireObject();
	auto obj2 = pool.acquireObject();
	auto obj3 = pool.acquireObject();

	//Total object already acuried. So, pool will create new object.
	auto obj4 = pool.acquireObject();

	pool.releaseObject(obj1);
	pool.releaseObject(obj2);
	pool.releaseObject(obj3);

	//Pool is full. So, it can not accomodate this object.
	pool.releaseObject(obj4);

	return 0;
}
#endif

