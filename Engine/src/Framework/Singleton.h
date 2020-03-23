#pragma once

template <typename T>
class Singleton
{
public:
	static T* Instance()
	{
		if (m_instance == nullptr)
		{
			m_instance = new T;
		}
		return m_instance;
	}

protected:
	Singleton() {}
private:
	static T* m_instance;
};

template <typename T> T* Singleton<T>::m_instance = nullptr;
