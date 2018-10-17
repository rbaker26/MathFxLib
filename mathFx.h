#pragma once
#include <type_traits>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <vector>


namespace mathFx {}

namespace mathFx 
{
	const unsigned int TRIG_ACCURACY = 19;
}
namespace mathFx
{
	const long double PI = 3.14159265359l;
}

namespace mathFx
{
	class TrigUnit
	{
		long double value;
	public:
		virtual long double get_value() const = 0;
		TrigUnit() {};
		/*TrigUnit(const TrigUnit& other) = delete;
		TrigUnit(TrigUnit&& other) noexcept = delete;
		TrigUnit& operator=(const TrigUnit& other) noexcept = delete;*/
	};
}

namespace mathFx 
{
	class radian : private TrigUnit
	{
	private:
		long double rad;
	public:
		long double get_radians() const
		{
			return rad;
		}
		//long double operator*(mathFx::degree deg)
		//{
		//	return (this->rad * );
		//}
		explicit radian(long double rad) : rad(rad) {};
		explicit radian(double rad) : rad(static_cast<long double>(rad)) {};
		explicit radian(long rad) : rad(static_cast<long double>(rad)) {};
		explicit radian(int rad) : rad(static_cast<long double>(rad)) {};
		explicit radian(short rad) : rad(static_cast<long double>(rad)) {};
		//radian(mathFx::degree deg) : rad(deg.get_degrees() * mathFx::PI / 180.0l) {};

		inline long double get_value() const override
		{
			return get_radians();
		}
	};
}

namespace mathFx
{
	class degree : private TrigUnit
	{
	private:
		long double deg;
	public:
		long double get_degrees() const
		{
			return deg;
		}
		explicit degree(const degree& d)
		{
			this->deg = d.deg;
		}
		explicit degree(long double deg) : deg(deg) {};
		explicit degree(double deg) : deg(static_cast<long double>(deg)) {};
		explicit degree(long deg) : deg(static_cast<long double>(deg % 360)) {};
		explicit degree(int deg) : deg(static_cast<long double>(deg%360)) {};
		explicit degree(short deg) : deg(static_cast<long double>(deg % 360)) {};
		degree(radian rad) : deg(rad.get_radians()*180.0l / mathFx::PI) {};

		degree& operator=(const degree& other)
		{
			this->deg = other.deg;
			return *this;
		}
		inline long double get_value() const override
		{
			return get_degrees();
		}
	};
}

namespace mathFx
{
	template<typename T>
	T sin(const T& t)
	{
		static_assert((std::is_same<mathFx::radian, T>::value) ||
			(std::is_same<mathFx::degree, T>::value),
			"T must be mathFx::degree or mathFx::radian");
		if constexpr (std::is_same<mathFx::radian, T>::value)
		{
			return radian(sin_transform(t.get_value()));
		}
		else
		{
			return degree(sin_transform(t.get_value()*mathFx::PI / 180.0l));
		}
	}
	template<typename T>
	T cos(const T& t)
	{
		static_assert((std::is_same<mathFx::radian, T>::value) ||
			(std::is_same<mathFx::degree, T>::value),
			"T must be mathFx::degree or mathFx::radian");
		if constexpr (std::is_same<mathFx::radian, T>::value)
		{
			return radian(cos_transform(t.get_value()));
		}
		else
		{
			return degree(cos_transform(t.get_value()*mathFx::PI / 180.0l));
		}
	}

}
namespace mathFx
{
	std::mutex sin_transform_lock;
	std::mutex cos_transform_lock;
	std::atomic<long double> accum_atomic{ 0.0l };
	inline void accumulate(long double num)
	{
		auto current = accum_atomic.load();
		while (!accum_atomic.compare_exchange_weak(current, current + num)) { ; }
	}
	std::unordered_map<unsigned int, long long> factorials = {
		{0,1},{1,1},{2,2},{3,6},{4,24},{5,120},
		{6,720},{7,5040},{8,40320},{9,362880},
		{10,3628800},{11,39916800},{12,479001600},
		{13,6227020800},{14,87178291200},{15,1307674368000},
		{16,20922789888000 },{17,355687428096000},
		{18,6402373705728000},{19,121645100408832000},
		{20,2432902008176640000}
	};
	inline long double sin_transform(long double rad)
	{
		std::scoped_lock<std::mutex> lock(sin_transform_lock);
		accum_atomic.store(0.0l);
		std::vector<std::thread> thread_pool;
		thread_pool.reserve(25);
		for (unsigned int i = 3; i < mathFx::TRIG_ACCURACY; i+=4)
		{
			thread_pool.emplace_back(accumulate, 
				                     -(std::pow(rad, i)  /static_cast<long double>(factorials[i])) + 
				                      (std::pow(rad, i+2)/static_cast<long double>(factorials[i+2])));
		}
		unsigned int thread_count = thread_pool.size();
		for (unsigned int i = 0; i < thread_count; ++i)
		{
			thread_pool.at(i).join();
		}
		return rad + accum_atomic.load();
	}

	inline long double cos_transform(long double rad)
	{
		std::scoped_lock < std::mutex> lock(cos_transform_lock);
		accum_atomic.store(0.0l);
		std::vector<std::thread> thread_pool;
		thread_pool.reserve(25);
		for (unsigned int i = 2; i < TRIG_ACCURACY; i += 4)
		{
			thread_pool.emplace_back(accumulate,
									 - (std::pow(rad, i) / static_cast<long double>(factorials[i])) +
									   (std::pow(rad, i + 2) / static_cast<long double>(factorials[i + 2])));
		}
		unsigned int thread_count = thread_pool.size();
		for (unsigned int i = 0; i < thread_count; ++i)
		{
			thread_pool.at(i).join();
		}
		return 1 + accum_atomic.load();
	}
}


