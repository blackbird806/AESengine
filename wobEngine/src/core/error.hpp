#ifndef WOB_ERROR_HPP
#define WOB_ERROR_HPP

#include "wob.hpp"
#include "errorCodes.hpp"

namespace wob {
	/**
	 * Error class Inspired by rust and Boost leaf
	 * However returning error sucks, we may want to go back to exceptions in the future
	 * we will use this only as common error object when exception will be back
	 * <update> exception sucks too I may try global errorx to simulate them
	 */
	template<typename T>
	class [[nodiscard]] Result
	{
	public:
		using ValueType = T;
		using ErrorCodeType = AESError;

		enum class TagType
		{
			Error, 
			Value
		};

		Result(T&& val) noexcept : value_(wob::forward<T>(val)), tag(TagType::Value)
		{

		}

		Result(T const& val) noexcept : value_(val), tag(TagType::Value)
		{

		}

		Result(AESError err) noexcept : value_(static_cast<ErrorCodeType>(err)), tag(TagType::Error)
		{

		}

		~Result()
		{
			if (hasValue())
			{
				value_.~ValueType();
			}
		}

		bool hasValue() const noexcept
		{
			return tag == TagType::Value;

		}

		operator bool() const noexcept
		{
			return hasValue;
		}

		ValueType const& value() const& noexcept
		{
			WOB_ASSERT(this->operator bool());
			return value_;
		}

		ValueType& value() & noexcept
		{
			WOB_ASSERT(this->operator bool());
			return value_;
		}

		ValueType const&& value() const&& noexcept
		{
			WOB_ASSERT(this->operator bool());
			return value_;
		}

		ValueType&& value() && noexcept
		{
			WOB_ASSERT(this->operator bool());
			return wob::move(value_);
		}

		ValueType const* operator->() const noexcept
		{
			return &value();
		}

		ValueType* operator->() noexcept
		{
			return &value();
		}

		ErrorCodeType error() const noexcept
		{
			WOB_ASSERT(!this->operator bool());
			return error_;
		}


	private:
		TagType tag;
		union {
			ErrorCodeType error_,
			ValueType value_
		};
	};

	template<>
	class Result<void>
	{
	public:
		using ValueType = void;
		using ErrorCodeType = UnderlyingError_t;

		enum class TagType
		{
			Error,
			Valid
		};


		Result() noexcept : tag(TagType::Valid)
		{

		}

		Result(AESError err) noexcept : error_(static_cast<ErrorCodeType>(err)), tag(TagType::Error)
		{

		}

		operator bool() const noexcept
		{
			return tag == TagType::Valid;
		}

		ErrorCodeType error() const noexcept
		{
			WOB_ASSERT(!this->operator bool());
			return error_;
		}

	private:
		TagType tag;
		ErrorCodeType error_;
	};

	inline const char* to_string(AESError err)
	{
#define CASE(X) case AESError::X: return #X;
		switch (err)
		{
			CASE(Undefined)
			CASE(MemoryAllocationFailed)
			CASE(GPUBufferCreationFailed)
			CASE(GPUTextureCreationFailed)
			CASE(GPUBufferMappingFailed)
			CASE(SamplerCreationFailed)
			CASE(ShaderCompilationFailed)
			CASE(ShaderCreationFailed)
			CASE(FontInitFailed)
			CASE(BlendStateCreationFailed)
		}
#undef CASE
		WOB_UNREACHABLE();
	}
}

#endif