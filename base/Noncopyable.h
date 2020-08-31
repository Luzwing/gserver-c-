#ifndef CPAN_NONCOPYABLE_H
#define CPAN_NONCOPYABLE_H

class Noncopyable
{
	public:
		Noncopyable(const Noncopyable&) = delete;
		void operator=(const Noncopyable&) = delete;

	protected:
		Noncopyable() = default;
		~Noncopyable() = default;
};




#endif
