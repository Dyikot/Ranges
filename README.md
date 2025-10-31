## About
The Ranges library is a collection of functions that allow you to perform various manipulations with ranges. 
Currently, the library includes more than 40 functions, including projection, filtering, searching, sorting, and more. 
The use of functions is done through the pipe operator "|".
Execution can be eager or deferred. 
For example, if the maximum value needs to be found, execution will occur immediately, but if a filtering is needed, it will be performed deferred—that is, during the iteration.
## Examples
### Projection and filtration
In the following example, we generate a range from 0 to 9, select only the even numbers, and square them.
```
auto result = Ranges::Range(0, 10)
	| Ranges::Where([](auto v) { return v % 2 == 0; })
	| Ranges::Select([](auto v) { return v * v; });
// Output: 0 4 16 36 64
```
### Sorting
In the following example, we will sort numbers in descending and ascending order.
```
std::vector<int> range = { 2, 1, 4, 3, 5 };	

auto ascending = range | Ranges::Order();
// Output: 1 2 3 4 5
auto descending = range | Ranges::OrderByDescending();
// Output: 5 4 3 2 1
```
In the following example, we will sort the Person structure by fields.
```
std::vector<Person> people = { { "Jake", 20 }, { "Alexander", 35 }, { "Emily", 27 } };

auto byAge = people | Ranges::OrderBy(&Person::Age);
// Output: { "Jake", 20 } { "Emily", 27 } { "Alexander", 35 }
auto byName = people | Ranges::OrderBy(&Person::Name);
// Output: { "Alexander", 35 } { "Emily", 27 } { "Jake", 20 }
```
### Searching
The following example searches for a person by age.
```
std::vector<Person> people = { { "Jake", 20 }, { "Alexander", 35 }, { "Emily", 27 } };

auto ageMoreThan30 = people | Ranges::First([](const auto& p) { return p.Age > 30; });
// Output: ('Alexander', 35)
auto ageLessThan20 = people | Ranges::FisrtOrDefault([](const auto& p) { return p.Age < 20; });
// Returns std::optional<Person> with std::nullopt
```
### Converting to a container
The following example demonstrates conversion to different containers.
```
Person p[] = { {"Alex", 20}, {"Anna", 32} };

std::vector<std::string> vector = p 
	| Ranges::Select(&Person::Name) 
	| Ranges::ToVector();
std::unordered_map<std::string, Person> map = p 
	| Ranges::ToUnorderedMap(&Person::Name, std::identity());
std::list<Person> list = p 
	| Ranges::To<std::list>();
```
### Finding the minimum and maximum
The following example is for numbers.
```
std::vector<int> n = { 0, 1, 2, 3, 4 };

auto max = n | Ranges::Max();
// Output: 4
auto max = n | Ranges::Min();
// Output: 0
```
The following example searches by method or field.
```
Person p[] = { {"Alex", 20}, {"Anna", 32} };

auto oldest = p | Ranges::MaxBy(&Person::Age);
// Output: {"Anna", 32}
auto youngest = p | Ranges::MinBy(&Person::GetAge);
// Output: {"Alex", 20}
```
### Concatenation
In the next example, we combine two ranges, reverse, and take the first 5.
```
std::vector<int> r1 = { 0, 1, 2, 3, 4 };
std::array<int, 3> r2 = { 5, 6, 7 };

auto view = r1 | Ranges::Concat(r2) | Ranges::Reverse() | Ranges::Take(5);
// Output: 7 6 5 4 3
```