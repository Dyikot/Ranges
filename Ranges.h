#pragma once

#include "RangeAdaptors.h"

namespace Ranges
{
	template<typename TFunc>
	constexpr auto Aggregate(TFunc&& func)
	{
		return Adaptors::AggregateAdaptor<TFunc>(std::forward<TFunc>(func));
	}

	template<typename TFunc, typename TAccumulate>
	constexpr auto Aggregate(TFunc&& func, const TAccumulate& seed)
	{
		return Adaptors::AgregateAdaptor2<TFunc, TAccumulate>(std::forward<TFunc>(func), seed);
	}	

	template<typename TPredicate>
	constexpr auto All(TPredicate&& predicate)
	{
		return Adaptors::AllAdaptor<TPredicate>(std::forward<TPredicate>(predicate));
	}	

	template<typename TPredicate>
	constexpr auto Any(TPredicate&& predicate)
	{
		return Adaptors::AnyAdaptor<TPredicate>(std::forward<TPredicate>(predicate));
	}	

	template<typename TValue>
	constexpr auto Append(const TValue& value)
	{
		return Adaptors::AppendAdaptor<TValue>(value);
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
	constexpr auto First(TPredicate&& predicate)
	{
		return Adaptors::FirstAdaptor2<TPredicate>(std::forward<TPredicate>(predicate));
	}

	constexpr auto FisrtOrDefault()
	{
		return Adaptors::FirstOrDefaultAdaptor();
	}

	template<typename TPredicate>
	constexpr auto FisrtOrDefault(TPredicate&& predicate)
	{
		return Adaptors::FirstOrDefaultAdaptor2<TPredicate>(std::forward<TPredicate>(predicate));
	}

	constexpr auto Join()
	{
		return std::views::join;
	}

	constexpr auto Keys()
	{
		return std::views::keys;
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
		return Adaptors::MaxAdaptor<TProjection>(std::move(projection));
	}	

	constexpr auto Min()
	{
		return Adaptors::MinAdaptor();
	}

	template<typename TProjection>
	constexpr auto MinBy(TProjection projection)
	{
		return Adaptors::MinAdaptor<TProjection>(std::move(projection));
	}	

	constexpr auto Order()
	{
		return Adaptors::OrderAdaptor<std::less>();
	}

	template<typename TProjection>
	constexpr auto OrderBy(TProjection projection)
	{
		return Adaptors::OrderAdaptor<std::less, TProjection>(std::move(projection));
	}

	constexpr auto OrderDescending()
	{
		return Adaptors::OrderAdaptor<std::greater>();
	}

	template<typename TProjection>
	constexpr auto OrderByDescending(TProjection projection)
	{
		return Adaptors::OrderAdaptor<std::greater, TProjection>(std::move(projection));
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

	constexpr auto Skip(size_t lenght)
	{
		return std::views::drop(lenght);
	}

	template<typename TPredicate>
	constexpr auto SkipWhile(TPredicate&& predicate)
	{
		return std::views::drop_while(std::forward<TPredicate>(predicate));
	}

	constexpr auto Slice(size_t start, size_t count)
	{
		return std::views::drop(start) | std::views::take(count);
	}

	template <typename TDelimeter>
	constexpr auto Split(TDelimeter&& deliemter)
	{
		return std::views::split(std::forward<TDelimeter>(deliemter));
	}

	constexpr auto Take(size_t lenght)
	{
		return std::views::take(lenght);
	}

	template<typename TPredicate>
	constexpr auto TakeWhile(TPredicate&& predicate)
	{
		return std::views::take_while(std::forward<TPredicate>(predicate));
	}

	template<template<typename...> typename TContainer>
	constexpr auto To()
	{
		return Adaptors::ToAdaptor<TContainer>();
	}

	template<typename TKeySelector, typename TElementSelector>
	constexpr auto ToUnorderedMap(TKeySelector&& keySelector, TElementSelector&& elementSelector)
	{
		return Adaptors::ToUnorderedMapAdaptor<TKeySelector, TElementSelector>(
			std::forward<TKeySelector>(keySelector),
			std::forward<TElementSelector>(elementSelector)
		);
	}

	template<typename TSelector>
	constexpr auto ToUnorderedMap(TSelector&& selector)
	{
		return Adaptors::ToUnorderedMapAdaptor2<TSelector>(std::forward<TSelector>(selector));
	}

	constexpr auto ToUnorderedMap()
	{
		return To<std::unordered_map>;
	}

	constexpr auto ToVector()
	{
		return To<std::vector>();
	}

	constexpr auto Values()
	{
		return std::views::values;
	}

	template<typename TPredicate>
	constexpr auto Where(TPredicate&& predicate)
	{
		return std::views::filter(std::forward<TPredicate>(predicate));
	}
}