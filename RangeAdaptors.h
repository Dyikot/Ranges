#pragma once

#include "Views.h"

namespace Ranges::Adaptors
{
	using std::ranges::view;
	using std::ranges::view_interface;
	using std::ranges::range;
	using std::ranges::sized_range;
	using std::ranges::range_value_t;
	using std::ranges::range_difference_t;
	using std::ranges::range_reference_t;
	using std::ranges::iterator_t;
	using std::views::all;

	template<typename TAdaptor>
	struct RangeAdaptor
	{
		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			return static_cast<const TAdaptor&>(*this)(std::forward<TRange>(range));
		}

		template<range TRange>
		constexpr friend decltype(auto) operator|(TRange&& range, RangeAdaptor&& adaptor)
		{
			return adaptor(std::forward<TRange>(range));
		}
	};

	template<typename TFunc>
	struct AggregateAdaptor : public RangeAdaptor<AggregateAdaptor<TFunc>>
	{
		TFunc Function;

		constexpr AggregateAdaptor(TFunc func):
			Function(std::move(func))
		{}

		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			range_value_t<TRange> result = {};
			for(const auto& item : range)
			{
				result = Function(result, item);
			}

			return result;
		}
	};

	template<typename TFunc, typename TAccumulate>
	struct AgregateAdaptor2 : public RangeAdaptor<AgregateAdaptor2<TFunc, TAccumulate>>
	{
		TFunc Function;
		TAccumulate Seed;

		constexpr AgregateAdaptor2(TFunc func, const TAccumulate& seed):
			Function(std::move(func)),
			Seed(seed)
		{}

		template<range TRange>
		constexpr TAccumulate operator()(TRange&& range) const
		{
			TAccumulate result = Seed;
			for(const auto& item : range)
			{
				result = Function(result, item);
			}

			return result;
		}
	};

	template<typename TPredicate>
	struct AllAdaptor : public RangeAdaptor<AllAdaptor<TPredicate>>
	{
		TPredicate Predicate;

		constexpr AllAdaptor(TPredicate predicate):
			Predicate(std::move(predicate))
		{}

		template<range TRange>
		constexpr bool operator()(TRange&& range) const
		{
			for(const auto& item : range)
			{
				if(!Predicate(item))
				{
					return false;
				}
			}

			return true;
		}
	};

	template<typename TPredicate>
	struct AnyAdaptor : public RangeAdaptor<AnyAdaptor<TPredicate>>
	{
		TPredicate Predicate;

		constexpr AnyAdaptor(TPredicate predicate):
			Predicate(std::move(predicate))
		{}

		template<range TRange>
		constexpr bool operator()(TRange&& range) const
		{
			for(const auto& item : range)
			{
				if(Predicate(item))
				{
					return true;
				}
			}

			return false;
		}
	};

	template<typename TValue>
	struct AppendAdaptor: public RangeAdaptor<AppendAdaptor<TValue>>
	{
		TValue Value;

		constexpr explicit AppendAdaptor(TValue value):
			Value(std::move(value))
		{}

		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			return Views::AppendView(std::forward<TRange>(range), Value);
		}
	};

	struct AverageAdaptor : public RangeAdaptor<AverageAdaptor>
	{
		template<range TRange>
		constexpr double operator()(TRange&& range) const
		{
			double sum = 0;

			for(const auto& item : range)
			{
				sum += item;
			}

			return sum / std::ranges::distance(range);
		}
	};

	struct ChunkAdaptor : public RangeAdaptor<ChunkAdaptor>
	{
		size_t Size;

		constexpr explicit ChunkAdaptor(size_t size):
			Size(size)
		{}

		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			return Views::ChunkView(std::forward<TRange>(range), Size);
		}
	};

	template<std::ranges::range TOtherRange>
	struct ConcatAdaptor : public RangeAdaptor<ConcatAdaptor<TOtherRange>>
	{
		TOtherRange&& OtherRange;

		constexpr ConcatAdaptor(TOtherRange&& otherRange):
			OtherRange(std::forward<TOtherRange>(otherRange))
		{}

		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			return Views::ConcatView(
				std::forward<TRange>(range), 
				std::forward<TOtherRange>(OtherRange)
			);
		}
	};

	template<typename T>
	struct ContainsAdaptor : public RangeAdaptor<ContainsAdaptor<T>>
	{
		T Value;

		constexpr ContainsAdaptor(const T& value):
			Value(value)
		{}

		template<range TRange>
		constexpr bool operator()(TRange&& range) const
			requires std::convertible_to<range_value_t<TRange>, T>
		{
			for(const auto& item : range)
			{
				if(item == Value)
				{
					return true;
				}
			}

			return false;
		}
	};

	struct CountAdaptor : public RangeAdaptor<CountAdaptor>
	{
		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			return std::ranges::distance(range);
		}
	};

	struct ElementAtAdaptor : public RangeAdaptor<ElementAtAdaptor>
	{
		size_t Position;

		constexpr explicit ElementAtAdaptor(size_t position):
			Position(position)
		{}

		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			auto it = range.begin();
			std::ranges::advance(it, Position, range.end());

			if(it == range.end())
			{
				throw std::out_of_range("Position is out of range");
			}

			return *it;			
		}
	};

	struct ElementAtOrDefaultAdaptor: public RangeAdaptor<ElementAtOrDefaultAdaptor>
	{
		size_t Position;

		constexpr explicit ElementAtOrDefaultAdaptor(size_t position):
			Position(position)
		{}

		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			using T = range_value_t<TRange>;

			auto it = range.begin();
			std::ranges::advance(it, Position, range.end());

			if(it == range.end())
			{
				return std::optional<T>();
			}

			return std::optional<T>(*it);
		}
	};

	struct FirstAdaptor : public RangeAdaptor<FirstAdaptor>
	{
		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			if(range.begin() == range.end())
			{
				throw std::runtime_error("Range is empty");
			}
			
			return *range.begin();
		}
	};

	template<typename TPredicate>
	struct FirstAdaptor2 : public RangeAdaptor<FirstAdaptor2<TPredicate>>
	{
		TPredicate Predicate;

		constexpr explicit FirstAdaptor2(TPredicate predicate):
			Predicate(std::move(predicate))
		{}

		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			for(const auto& item : range)
			{
				if(Predicate(item))
				{
					return item;
				}
			}

			throw std::runtime_error("Item not found");
		}
	};

	struct FirstOrDefaultAdaptor: public RangeAdaptor<FirstOrDefaultAdaptor>
	{
		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			using T = range_value_t<TRange>;

			if(range.begin() == range.end())
			{
				return std::optional<T>();
			}

			return std::optional<T>(*range.begin());
		}
	};

	template<typename TPredicate>
	struct FirstOrDefaultAdaptor2: public RangeAdaptor<FirstOrDefaultAdaptor2<TPredicate>>
	{
		TPredicate Predicate;

		constexpr explicit FirstOrDefaultAdaptor2(TPredicate predicate):
			Predicate(std::move(predicate))
		{}

		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			using T = range_value_t<TRange>;

			for(const auto& item : range)
			{
				if(Predicate(item))
				{
					return std::optional<T>(item);
				}
			}

			return std::optional<T>();
		}
	};

	struct LastAdaptor : public RangeAdaptor<LastAdaptor>
	{
		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			if(range.begin() == range.end())
			{
				throw std::runtime_error("Range is empty");
			}

			return *std::ranges::prev(std::ranges::end(range));
		}
	};

	template<typename TPredicate>
	struct LastAdaptor2: public RangeAdaptor<LastAdaptor2<TPredicate>>
	{
		TPredicate Predicate;

		constexpr explicit LastAdaptor2(TPredicate predicate):
			Predicate(std::move(predicate))
		{}

		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			if constexpr(std::ranges::bidirectional_range<TRange>)
			{
				for(auto it = std::ranges::rbegin(range); it != std::ranges::rend(range); it++)
				{
					if(Predicate(*it))
					{
						return *it;						
					}
				}
			}
			else
			{
				std::optional<range_value_t<TRange>> found;
				for(const auto& item : range)
				{
					if(Predicate(item))
					{
						found = item;
					}
				}

				if(found)
				{
					return found.value();
				}
			}


			throw std::runtime_error("Item not found");
		}
	};

	struct LastOrDefaultAdaptor : public RangeAdaptor<LastOrDefaultAdaptor>
	{
		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			using T = range_value_t<TRange>;

			if(range.begin() == range.end())
			{
				return std::optional<T>();
			}

			return std::optional<T>(*std::ranges::prev(range.end()));
		}
	};

	template<typename TPredicate>
	struct LastOrDefaultAdaptor2 : public RangeAdaptor<LastOrDefaultAdaptor2<TPredicate>>
	{
		TPredicate Predicate;

		constexpr explicit LastOrDefaultAdaptor2(TPredicate predicate):
			Predicate(std::move(predicate))
		{}

		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			using T = range_value_t<TRange>;

			if constexpr(std::ranges::bidirectional_range<TRange>)
			{
				for(auto it = std::ranges::rbegin(range); it != std::ranges::rend(range); it++)
				{
					if(Predicate(*it))
					{
						return std::optional<T>(*it);
					}
				}

				return std::optional<T>();
			}
			
			std::optional<T> found;
			for(const auto& item : range)
			{
				if(Predicate(item))
				{
					found = item;
				}
			}

			return found;
		}
	};

	template<typename TProjection = std::identity>
	struct MaxAdaptor : public RangeAdaptor<MaxAdaptor<TProjection>>
	{
		TProjection Projection;

		constexpr MaxAdaptor(TProjection projection = {}):
			Projection(projection)
		{}

		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			return std::ranges::max(std::forward<TRange>(range), {}, Projection);
		}
	};

	template<typename TProjection = std::identity>
	struct MinAdaptor : public RangeAdaptor<MinAdaptor<TProjection>>
	{
		TProjection Projection;

		constexpr MinAdaptor(TProjection projection = {}):
			Projection(projection)
		{}

		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			return std::ranges::min(std::forward<TRange>(range), {}, Projection);
		}
	};

	template<template<typename> typename TComparer, typename TProjection = std::identity>
	struct OrderAdaptor : public RangeAdaptor<OrderAdaptor<TComparer, TProjection>>
	{
		TProjection Projection;

		constexpr OrderAdaptor(TProjection projection = {}):
			Projection(std::move(projection))
		{}

		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			using T = std::invoke_result_t<TProjection, range_value_t<TRange>>;
			return Views::OrderedView(std::forward<TRange>(range), TComparer<T>(), Projection);
		}
	};

	template<size_t Size>
	struct ToArrayAdaptor : public RangeAdaptor<ToArrayAdaptor<Size>>
	{
		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			using T = range_value_t<TRange>;

			const auto minSize = std::min(static_cast<size_t>(std::ranges::distance(range)), Size);
			auto result = std::array<T, Size>();
			auto it = range.begin();

			for(size_t i = 0; i < minSize; i++, it++)
			{
				result[i] = *it;
			}

			return result;
		}
	};

	template<template<typename> typename TContainer>
	struct ToAdaptor : public RangeAdaptor<ToAdaptor<TContainer>>
	{
		template<range TRange>
		constexpr auto operator()(TRange&& range) const
		{
			return TContainer(range.begin(), range.end());
		}
	};
}