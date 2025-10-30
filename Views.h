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
	using std::views::all_t;	

	enum class AppendPosition
	{
		InRange, InAppend, InEnd
	};

	template<view TView>
	class AppendView : public view_interface<AppendView<TView>>
	{
	private:
		using TValue = range_value_t<TView>;
		using TIterator = iterator_t<TView>;

		TView _view;
		TValue _value;		

		class AppendIterator
		{
		private:
			AppendView* _parent {};
			TIterator _it;		
			AppendPosition _position = AppendPosition::InRange;
		public:
			using iterator_concept = std::bidirectional_iterator_tag;
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = range_value_t<TView>;
			using difference_type = range_difference_t<TView>;
			using reference = range_reference_t<TView>;

			constexpr AppendIterator() requires std::default_initializable<TIterator> = default;

			constexpr AppendIterator(AppendView& parent, TIterator it, AppendPosition position):
				_parent(&parent),
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
						return _parent->_value;

					case AppendPosition::InEnd:
						throw std::out_of_range("Cannot get value at end");
				}
			}

			constexpr AppendIterator& operator++()
			{
				switch(_position)
				{
					case AppendPosition::InRange:
						if(++_it == std::ranges::end(_parent->_view))
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

			constexpr AppendIterator& operator--() requires std::bidirectional_iterator<TIterator>
			{
				switch(_position)
				{
					case AppendPosition::InRange:
						--_it;
						break;

					case AppendPosition::InAppend:
						--_it;
						_position = AppendPosition::InRange;
						break;

					case AppendPosition::InEnd:
						_position = AppendPosition::InAppend;
						break;
				}

				return *this;
			}

			constexpr AppendIterator operator--(int) requires std::bidirectional_iterator<TIterator>
			{
				AppendIterator tmp = *this;
				--(*this);
				return tmp;
			}

			constexpr bool operator==(const AppendIterator& other) const
			{
				return _it == other._it && _position == other._position;
			}
		};
	public:
		constexpr AppendView() requires
			std::default_initializable<TView> &&
			std::default_initializable<TValue> = default;

		constexpr AppendView(TView range, TValue value):
			_view(std::move(range)),
			_value(std::move(value))
		{}
		AppendView(const AppendView&) requires std::copyable<TView> = default;
		AppendView(AppendView&&) = default;

		constexpr auto begin()
		{
			if(_view.begin() != _view.end())
			{
				return AppendIterator(*this, _view.begin(), AppendPosition::InRange);
			}
			else
			{
				return AppendIterator(*this, _view.begin(), AppendPosition::InAppend);
			}
		}

		constexpr auto end()
		{
			return AppendIterator(*this, _view.end(), AppendPosition::InEnd);
		}

		AppendView& operator=(const AppendView&) requires std::copyable<TView> = default;
		AppendView& operator=(AppendView&&) = default;
	};

	template<typename TRange>
	AppendView(TRange&&, range_value_t<TRange>) -> AppendView<all_t<TRange>>;

	template<view TView>
	class ChunkView : public view_interface<ChunkView<TView>>
	{
	private:
		using TIterator = iterator_t<TView>;
		using TSize = std::iter_difference_t<TIterator>;

		TView _view;
		TSize _size = 1;

		class ChunkIterator
		{
		private:
			ChunkView* _parent {};
			TIterator _from;
			TIterator _to;
		public:
			using iterator_concept = std::bidirectional_iterator_tag;
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = std::ranges::subrange<TIterator>;
			using difference_type = std::iter_difference_t<TIterator>;

			constexpr ChunkIterator() requires std::default_initializable<TIterator> = default;

			constexpr ChunkIterator(ChunkView& parent, TIterator from):
				_parent(&parent),
				_to(from)
			{
				_from = _to;
				std::ranges::advance(_to, _parent->_size, std::ranges::end(_parent->_view));
			}

			constexpr value_type operator*() const
			{
				return std::ranges::subrange(_from, _to);
			}

			constexpr ChunkIterator& operator++()
			{
				_from = _to;
				std::ranges::advance(_to, _parent->_size, std::ranges::end(_parent->_view));
				return *this;
			}

			constexpr ChunkIterator operator++(int)
			{
				ChunkIterator tmp = *this;
				++(*this);
				return tmp;
			}

			constexpr ChunkIterator& operator--() requires std::bidirectional_iterator<TIterator>
			{
				_to = _from;
				std::ranges::advance(_from, -_parent->_size, std::ranges::begin(_parent->_view));
				return *this;
			}

			constexpr ChunkIterator operator--(int) requires std::bidirectional_iterator<TIterator>
			{
				ChunkIterator tmp = *this;
				--(*this);
				return tmp;
			}

			constexpr bool operator==(const ChunkIterator& other) const
			{
				return _from == other._from;
			}
		};
	public:
		constexpr ChunkView() requires std::default_initializable<TView> = default;

		constexpr ChunkView(TView view, TSize size):
			_view(std::move(view)),
			_size(size)
		{
			if(_size == 0)
			{
				throw std::invalid_argument("Size cannot be 0");
			}
		}

		ChunkView(const ChunkView&) requires std::copyable<TView> = default;
		ChunkView(ChunkView&&) = default;

		constexpr auto begin()
		{
			return ChunkIterator(*this, std::ranges::begin(_view));
		}

		constexpr auto end()
		{
			return ChunkIterator(*this, std::ranges::end(_view));
		}

		constexpr auto size() const requires std::ranges::sized_range<TView>
		{
			auto n = std::ranges::size(_view);
			return (n + _size - 1) / _size;
		}

		ChunkView& operator=(const ChunkView&) requires std::copyable<TView> = default;
		ChunkView& operator=(ChunkView&&) = default;
	};

	template<typename TRange>
	ChunkView(TRange&&, size_t) -> ChunkView<all_t<TRange>>;

	template<view TView, view TOtherView>
		requires std::same_as<range_value_t<TView>, range_value_t<TOtherView>>
	class ConcatView : public view_interface<ConcatView<TView, TOtherView>>
	{
	private:
		TView _view;
		TOtherView _otherView;

		class ConcatIterator
		{
		private:
			ConcatView* _parent {};
			bool _isInSecond = false;
			iterator_t<TView> _firstIt;
			iterator_t<TOtherView> _secondIt;
		public:
			using iterator_concept = std::bidirectional_iterator_tag;
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = range_value_t<TView>;
			using difference_type = range_difference_t<TView>;
			using reference = range_reference_t<TView>;

			constexpr ConcatIterator() requires
				std::default_initializable<iterator_t<TView>> &&
				std::default_initializable<iterator_t<TOtherView>> = default;

			constexpr ConcatIterator(ConcatView& parent, bool isEnd):
				_parent(&parent),
				_isInSecond(false)
			{
				auto& first = parent._view;
				auto& second = parent._otherView;

				if(isEnd)
				{
					_firstIt = std::ranges::end(first);
					_secondIt = std::ranges::end(second);
					_isInSecond = true;
				}
				else
				{
					_firstIt = std::ranges::begin(first);
					_secondIt = std::ranges::begin(second);

					if(_firstIt == std::ranges::end(first))
					{
						_isInSecond = true;
					}
				}
			}

			constexpr reference operator*() const
			{
				if(_isInSecond)
				{
					return *_secondIt;
				}
				else
				{
					return *_firstIt;
				}
			}

			constexpr ConcatIterator& operator++()
			{
				if(_isInSecond)
				{
					++_secondIt;
				}
				else
				{
					if(++_firstIt == std::ranges::end(_parent->_view))
					{
						_isInSecond = true;
						_secondIt = std::ranges::begin(_parent->_otherView);
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

			constexpr ConcatIterator& operator--() 
				requires std::bidirectional_iterator<iterator_t<TView>> &&
						 std::bidirectional_iterator<iterator_t<TOtherView>>
			{
				if(_isInSecond)
				{
					if(_secondIt == std::ranges::begin(_parent->_otherView))
					{
						_isInSecond = false;
						_firstIt = --std::ranges::end(_parent->_view);
					}
					else
					{
						--_secondIt;
					}
				}
				else
				{
					--_firstIt;
				}

				return *this;
			}

			constexpr ConcatIterator operator--(int)
				requires std::bidirectional_iterator<iterator_t<TView>>&&
						 std::bidirectional_iterator<iterator_t<TOtherView>>
			{
				ConcatIterator tmp = *this;
				--(*this);
				return tmp;
			}

			constexpr bool operator==(const ConcatIterator& other) const
			{
				return _isInSecond == other._isInSecond &&
					  (_isInSecond ? _secondIt == other._secondIt : _firstIt == other._firstIt);
			}
		};
	public:
		constexpr ConcatView() requires 
			std::default_initializable<TView> && 
			std::default_initializable<TOtherView> = default;

		constexpr ConcatView(TView range, TOtherView otherRange):
			_view(std::move(range)),
			_otherView(std::move(otherRange))
		{}

		ConcatView(const ConcatView&) requires std::copyable<TView> = default;
		ConcatView(ConcatView&&) = default;

		constexpr auto begin()
		{
			return ConcatIterator(*this, false);
		}

		constexpr auto end()
		{
			return ConcatIterator(*this, true);
		}

		constexpr auto size() requires sized_range<TView> && sized_range<TOtherView>
		{
			return std::ranges::size(_view) + std::ranges::size(_otherView);
		}

		ConcatView& operator=(const ConcatView&) requires std::copyable<TView> = default;
		ConcatView& operator=(ConcatView&&) = default;
	};

	template<typename TRange, typename TOtherRange>
	ConcatView(TRange&&, TOtherRange&&) -> ConcatView<all_t<TRange>, all_t<TOtherRange>>;

	template<view TView, typename TComparer, typename TProjection>
	class OrderedView : public view_interface<OrderedView<TView, TComparer, TProjection>>
	{
	private:
		using T = range_value_t<TView>;

		TView _view;
		TComparer _comparer;
		TProjection _projection;
		mutable bool _sorted = false;
		mutable std::vector<T> _sortedRange;
	public:
		constexpr OrderedView(TView view, TComparer comparer, TProjection projection):
			_view(std::move(view)),
			_comparer(std::move(comparer)),
			_projection(std::move(projection))
		{}
		OrderedView(const OrderedView&) requires std::copyable<TView> = default;
		OrderedView(OrderedView&&) = default;

		constexpr auto begin() const
		{
			EnsureSorted();
			return std::ranges::begin(_sortedRange);
		}

		constexpr auto end() const
		{
			EnsureSorted();
			return std::ranges::end(_sortedRange);
		}

		OrderedView& operator=(const OrderedView&) requires std::copyable<TView> = default;
		OrderedView& operator=(OrderedView&&) = default;
	private:
		constexpr void EnsureSorted() const
		{
			if(!_sorted)
			{
				_sorted = true;
				_sortedRange = std::vector(std::ranges::begin(_view), std::ranges::end(_view));
				std::ranges::sort(_sortedRange, _comparer, _projection);
			}
		}
	};

	template<typename TRange, typename TComparer, typename TProjection>
	OrderedView(TRange&&, TComparer, TProjection) -> OrderedView<all_t<TRange>, TComparer, TProjection>;
}