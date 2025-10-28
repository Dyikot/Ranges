#pragma once

#include <ranges>
#include <algorithm>
#include <functional>
#include <optional>
#include <stdexcept>

namespace Views
{
	using std::ranges::view;
	using std::ranges::view_interface;
	using std::ranges::sized_range;
	using std::ranges::range_value_t;
	using std::ranges::range_difference_t;
	using std::ranges::range_reference_t;
	using std::ranges::iterator_t;

	enum class AppendPosition
	{
		InRange, InAppend, InEnd
	};

	template<view TRange>
	class AppendView: public view_interface<AppendView<TRange>>
	{
	private:
		using T = range_value_t<TRange>;
		using TIterator = iterator_t<TRange>;

		TRange _range;
		T _value;		

		class AppendIterator
		{
		private:
			AppendView& _parent;
			TIterator _it;		
			AppendPosition _position;
		public:
			using reference = range_reference_t<TRange>;

			constexpr AppendIterator(AppendView& parent, TIterator it, AppendPosition position):
				_parent(parent),
				_it(it),
				_position(position)
			{}

			constexpr reference operator*() const
			{
				switch(_position)
				{
					case AppendPosition::InRange:
						return *_it;

					case AppendPosition::InAppend:
						return _parent._value;

					case AppendPosition::InEnd:
						throw std::out_of_range("Cannot get value at end");
				}
			}

			constexpr AppendIterator& operator++()
			{
				switch(_position)
				{
					case AppendPosition::InRange:
						if(++_it == _parent._range.end())
						{
							_position = AppendPosition::InAppend;
						}
						break;

					case AppendPosition::InAppend:
						_position = AppendPosition::InEnd;
						break;

					case AppendPosition::InEnd:
						throw std::out_of_range("Cannot iterate out of end");
						break;
				}
				
				return *this;
			}

			constexpr AppendIterator operator++(int)
			{
				AppendIterator tmp = *this;
				++(*this);
				return tmp;
			}

			constexpr bool operator==(const AppendIterator& other) const
			{
				return _it == other._it && _position == other._position;
			}

			constexpr bool operator!=(const AppendIterator& other) const
			{
				return !(*this == other);
			}
		};
	public:
		constexpr AppendView(TRange range, T value):
			_range(std::move(range)),
			_value(std::move(value))
		{}
		AppendView(const AppendView&) requires std::copyable<TRange> = default;
		AppendView(AppendView&&) = default;

		constexpr auto begin()
		{
			if(_range.begin() != _range.end())
			{
				return AppendIterator(*this, _range.begin(), AppendPosition::InRange);
			}
			else
			{
				return AppendIterator(*this, _range.begin(), AppendPosition::InAppend);
			}
		}

		constexpr auto end()
		{
			return AppendIterator(*this, _range.end(), AppendPosition::InEnd);
		}

		AppendView& operator=(const AppendView&) requires std::copyable<TRange> = default;
		AppendView& operator=(AppendView&&) = default;
	};

	template<view TRange>
	class ChunkView: public view_interface<ChunkView<TRange>>
	{
	private:
		using T = range_value_t<TRange>;
		using TIterator = iterator_t<TRange>;

		TRange _range;
		size_t _size;

		class ChunkIterator
		{
		private:
			TRange& _range;
			size_t _size;
			TIterator _begin;
			TIterator _end;
		public:
			constexpr ChunkIterator(TRange& range, size_t size, TIterator begin):
				_range(range),
				_size(size),
				_begin(begin),
				_end(begin)
			{
				MoveNext();
			}

			constexpr auto operator*() const
			{
				return std::ranges::subrange(_begin, _end);
			}

			constexpr ChunkIterator& operator++()
			{
				MoveNext();
				return *this;
			}

			constexpr ChunkIterator operator++(int)
			{
				ChunkIterator tmp = *this;
				++(*this);
				return tmp;
			}

			constexpr bool operator==(const ChunkIterator& other) const
			{
				return _begin == other._begin && _end == other._end;
			}

			constexpr bool operator!=(const ChunkIterator& other) const
			{
				return !(*this == other);
			}

			void MoveNext()
			{
				_begin = _end;
				std::ranges::advance(_end, _size, _range.end());
			}
		};
	public:
		constexpr ChunkView(TRange range, size_t size):
			_range(std::move(range)),
			_size(size)
		{
			if(_size == 0)
			{
				throw std::invalid_argument("Size cannot be 0");
			}
		}

		ChunkView(const ChunkView&) requires std::copyable<TRange> = default;
		ChunkView(ChunkView&&) = default;

		constexpr auto begin()
		{
			return ChunkIterator(_range, _size, _range.begin());
		}

		constexpr auto end()
		{
			return ChunkIterator(_range, _size, _range.end());
		}

		ChunkView& operator=(const ChunkView&) requires std::copyable<TRange> = default;
		ChunkView& operator=(ChunkView&&) = default;
	};

	template<view TRange, view TOtherRange>
		requires std::same_as<range_value_t<TRange>, range_value_t<TOtherRange>>
	class ConcatView : public view_interface<ConcatView<TRange, TOtherRange>>
	{
	private:
		using T = range_value_t<TRange>;

		TRange _range;
		TOtherRange _otherRange;

		class ConcatIterator
		{
		private:
			ConcatView& _parent;
			bool _isInSecond;
			iterator_t<TRange> _firstIterator;
			iterator_t<TOtherRange> _secondIterator;
		public:
			using reference = range_reference_t<TRange>;

			constexpr ConcatIterator(ConcatView& parent, bool isEnd):
				_parent(parent),
				_isInSecond(false)
			{
				TRange& first = parent._range;
				TOtherRange& second = parent._otherRange;

				if(isEnd)
				{
					_firstIterator = first.end();
					_secondIterator = second.end();
					_isInSecond = true;
				}
				else
				{
					_firstIterator = first.begin();
					_secondIterator = second.begin();

					if(_firstIterator == first.end())
					{
						_isInSecond = true;
					}
				}
			}

			constexpr reference operator*() const
			{
				if(_isInSecond)
				{
					return *_secondIterator;
				}
				else
				{
					return *_firstIterator;
				}
			}

			constexpr ConcatIterator& operator++()
			{
				if(_isInSecond)
				{
					++_secondIterator;
				}
				else
				{
					if(++_firstIterator == _parent._range.end())
					{
						_isInSecond = true;
						_secondIterator = _parent._otherRange.begin();
					}
				}

				return *this;
			}

			constexpr ConcatIterator operator++(int)
			{
				ConcatIterator tmp = *this;
				++(*this);
				return tmp;
			}

			constexpr bool operator==(const ConcatIterator& other) const
			{
				return _isInSecond == other._isInSecond &&
					(_isInSecond
						? _secondIterator == other._secondIterator
						: _firstIterator == other._firstIterator);
			}

			constexpr bool operator!=(const ConcatIterator& other) const
			{
				return !(*this == other);
			}
		};

	public:
		constexpr ConcatView(TRange range, TOtherRange otherRange):
			_range(std::move(range)),
			_otherRange(std::move(otherRange))
		{}

		ConcatView(const ConcatView&) requires std::copyable<TRange> = default;
		ConcatView(ConcatView&&) = default;

		constexpr auto begin()
		{
			return ConcatIterator(*this, false);
		}

		constexpr auto end()
		{
			return ConcatIterator(*this, true);
		}

		constexpr auto size() requires sized_range<TRange> && sized_range<TOtherRange>
		{
			return std::ranges::size(_range) + std::ranges::size(_otherRange);
		}

		ConcatView& operator=(const ConcatView&) requires std::copyable<TRange> = default;
		ConcatView& operator=(ConcatView&&) = default;
	};

	template<view TRange, typename TComparer, typename TProjection>
	class OrderedView : public view_interface<OrderedView<TRange, TComparer, TProjection>>
	{
	private:
		using T = range_value_t<TRange>;

		TComparer _comparer;
		TProjection _projection;
		TRange _range;
		bool _sorted = false;
		std::vector<T> _sortedRange;
	public:
		constexpr OrderedView(TRange&& range, TComparer comparer, TProjection projection):
			_range(std::forward<TRange>(range)),
			_comparer(std::move(comparer)),
			_projection(std::move(projection))
		{}
		OrderedView(const OrderedView&) requires std::copyable<TRange> = default;
		OrderedView(OrderedView&&) = default;

		constexpr auto begin()
		{
			TrySort();
			return _sortedRange.begin();
		}

		constexpr auto end()
		{
			TrySort();
			return _sortedRange.end();
		}

		OrderedView& operator=(const OrderedView&) requires std::copyable<TRange> = default;
		OrderedView& operator=(OrderedView&&) = default;
	private:
		constexpr void TrySort()
		{
			if(!_sorted)
			{
				_sorted = true;
				_sortedRange = std::vector(_range.begin(), _range.end());
				std::ranges::sort(*_sortedRange, _comparer, _projection);
			}
		}
	};
}