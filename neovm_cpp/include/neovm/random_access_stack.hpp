#ifndef NEOVM_RANDOM_ACCESS_STACK_HPP
#define NEOVM_RANDOM_ACCESS_STACK_HPP

#include <vector>
#include <list>

namespace neo
{
	namespace vm
	{
		template <typename T>
		class RandomAccessStack
		{
		private:
			std::vector<T> list;

		public:
			size_t size() const {
				return list.size();
			}

			void clear()
			{
				list.clear();
			}

			void insert(size_t index, T value)
			{
				if(index > list.size())
					throw NeoVmException("too large position to insert");
				list.push_back(value);
				if (index == list.size() - 1)
					return;
				auto index_in_list = list.size() - index - 1;
				for (size_t i = list.size() - 1; i >= index_in_list + 1; i--)
				{
					list[i] = list[i - 1];
				}
				list[index_in_list] = value;
			}

			T peek(size_t index = 0) const
			{
				if (index >= list.size())
					throw NeoVmException("index out of range");
				auto index_in_list = list.size() - 1 - index;
				return list[index_in_list];
			}

			T pop()
			{
				return remove(0);
			}

			void push(T item)
			{
				list.push_back(item);
			}

			void push_back(T item)
			{
				push(item);
			}

			T remove(size_t index)
			{
				if (index >= list.size() || index < 0)
					throw NeoVmException("index out of range");
				auto index_in_list = list.size() - index - 1;
				auto item = list[index_in_list];
				// list.remove(item); // TODO: change to remove at index
				std::vector<T> result;
				for (size_t i = 0; i < index_in_list; i++)
				{
					result.push_back(list[i]);
				}
				if (index_in_list + 1 < list.size())
				{
					for (size_t i = index_in_list + 1; i < list.size(); i++)
					{
						result.push_back(list[i]);
					}
				}
				list = result;
				return item;
			}

			void set(size_t index, T item)
			{
				if (index >= list.size())
					throw NeoVmException("index out of range");
				list[list.size() - index - 1] = item;
			}
		};
	}
}

#endif
