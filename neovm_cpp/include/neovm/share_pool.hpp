#ifndef NEOVM_SHARE_POOL_HPP
#define NEOVM_SHARE_POOL_HPP

#include <memory>
#include <stack>
#include <vector>
#include <functional>

namespace neo
{
	namespace utils
	{
		template <typename T>
		class ObjectPool
		{
		public:
			void add(T* obj)
			{
				_pool.push_back(obj);
			}
			std::vector<T*> &pool()
			{
				return _pool;
			}
		private:
			std::vector<T*> _pool;
		};
	}
}

#endif
