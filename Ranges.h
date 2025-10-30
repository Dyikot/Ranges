#pragma once

#include "RangeAdaptors.h"

namespace Ranges
{
	template<typename TFunc>
	constexpr auto Aggregate(TFunc func)
	{
		return Adaptors::AggregateAdaptor<TFunc>(std::move(func));
	}

	template<typename TFunc, typename TAccumulate>
	constexpr auto Aggregate(TFunc func, const TAccumulate& seed)
	{
		return Adaptors::AgregateAdaptor2<TFunc, TAccumulate>(std::move(func), seed);
	}	

	template<typename TPredicate>
	constexpr auto All(TPredicate predicate)
	{
		return Adaptors::AllAdaptor<TPredicate>(std::move(predicate));
	}	

	template<typename TPredicate>
	constexpr auto Any(TPredicate predicate)
	{
		return Adaptors::AnyAdaptor<TPredicate>(std::move(predicate));
	}	

	template<typename TValue>
	constexpr auto Append(TValue value)
	{
		return Adaptors::AppendAdaptor<TValue>(std::move(value));
	}

	constexpr auto AsView()
	{
		return std::views::all;
	}

	constexpr auto Average()
	{
		return Adaptors::AverageAdaptor();
	}

	template<typename TResult>
	constexpr auto Cast()
	{
		return std::views::transform([](const auto& item) 
		{
			return static_cast<TResult>(item);
		});
	}	

	constexpr auto Chunk(size_t size)
	{
		return Adaptors::ChunkAdaptor(size);
	}

	template<std::ranges::range TOtherRange>
	constexpr auto Concat(TOtherRange&& otherRange)
	{
		return Adaptors::ConcatAdaptor<TOtherRange>(std::forward<TOtherRange>(otherRange));
	}	

	template<typename T>
	constexpr auto Contains(const T& value)
	{
		return Adaptors::ContainsAdaptor<T>(value);
	}	

	constexpr auto Count()
	{
		return Adaptors::CountAdaptor();
	}	

	constexpr auto ElementAt(size_t position)
	{
		return Adaptors::ElementAtAdaptor(position);
	}

	constexpr auto ElementAtOrDefault(size_t position)
	{
		return Adaptors::ElementAtOrDefaultAdaptor(position);
	}

	template<typename T>
	constexpr auto Empty()
	{
		return std::views::empty<T>;
	}	

	constexpr auto First()
	{
		return Adaptors::FirstAdaptor();
	}	

	template<typename TPredicate>
	constexpr auto First(TPredicate predicate)
	{
		return Adaptors::FirstAdaptor2<TPredicate>(std::move(predicate));
	}

	constexpr auto FisrtOrDefault()
	{
		return Adaptors::FirstOrDefaultAdaptor();
	}

	template<typename TPredicate>
	constexpr auto FisrtOrDefault(TPredicate predicate)
	{
		return Adaptors::FirstOrDefaultAdaptor2<TPredicate>(std::move(predicate));
	}

	constexpr auto Join()
	{
		return std::views::join;
	}

	constexpr auto Last()
	{
		return Adaptors::LastAdaptor();
	}

	template<typename TPredicate>
	constexpr auto Last(TPredicate predicate)
	{
		return Adaptors::LastAdaptor2<TPredicate>(std::move(predicate));
	}

	constexpr auto LastOrDefault()
	{
		return Adaptors::LastOrDefaultAdaptor();
	}

	template<typename TPredicate>
	constexpr auto LastOrDefault(TPredicate predicate)
	{
		return Adaptors::LastOrDefaultAdaptor2<TPredicate>(std::move(predicate));
	}

	constexpr auto Max()
	{
		return Adaptors::MaxAdaptor();
	}

	template<typename TProjection>
	constexpr auto MaxBy(TProjection projection)
	{
		return Adaptors::MaxAdaptor<TProjection>(projection);
	}	

	constexpr auto Min()
	{
		return Adaptors::MinAdaptor();
	}

	template<typename TProjection>
	constexpr auto MinBy(TProjection projection)
	{
		return Adaptors::MinAdaptor<TProjection>(projection);
	}	

	constexpr auto Order()
	{
		return Adaptors::OrderAdaptor<std::less>();
	}

	template<typename TProjection>
	constexpr auto OrderBy(TProjection projection)
	{
		return Adaptors::OrderAdaptor<std::less, TProjection>(projection);
	}

	constexpr auto OrderByDescending()
	{
		return Adaptors::OrderAdaptor<std::greater>();
	}

	template<typename TProjection>
	constexpr auto OrderByDescending(TProjection projection)
	{
		return Adaptors::OrderAdaptor<std::greater, TProjection>(projection);
	}

	constexpr auto Range(int start, int count)
	{
		return std::views::iota(start, count);
	}

	constexpr auto Reverse()
	{
		return std::views::reverse;
	}

	template<typename TSelector>
	constexpr auto Select(TSelector&& selector)
	{
		return std::views::transform(std::forward<TSelector>(selector));
	}

	constexpr auto Slice(size_t start, size_t count)
	{
		return std::views::drop(start) | std::views::take(count);
	}

	template<size_t Size>
	constexpr auto ToArray()
	{
		return Adaptors::ToArrayAdaptor<Size>();
	}	

	template<template<typename> typename TContainer>
	constexpr auto To()
	{
		return Adaptors::ToAdaptor<TContainer>();
	}

	template<typename TPredicate>
	constexpr auto Where(TPredicate&& predicate)
	{
		return std::views::filter(std::forward<TPredicate>(predicate));
	}
}