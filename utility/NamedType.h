#ifndef _NAMED_TYPE_H
#define _NAMED_TYPE_H

namespace dip
{
	template <typename T, typename Parameter>
	class NamedType
	{
	public:
		NamedType(T const& value) : value_(value) {}
		NamedType(T&& value) : value_(std::move(value)) {}
		T& get() { return value_; }
		T const& get() const { return value_; }
	private:
		T value_;
	};
}

#endif