#ifndef AES_ERROR_HPP
#define AES_ERROR_HPP

#include <variant>
#include <utility>

#include "aes.hpp"
#include "errorCodes.hpp"

namespace aes {
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

		Result(T&& val) noexcept : value_(std::forward<T>(val))
		{

		}

		Result(T const& val) noexcept : value_(val)
		{

		}

		Result(AESError err) noexcept : value_(static_cast<ErrorCodeType>(err))
		{

		}

		operator bool() const noexcept
		{
			return std::holds_alternative<ValueType>(value_);
		}

		ValueType const& value() const& noexcept
		{
			AES_ASSERT(this->operator bool());
			return std::get<ValueType>(value_);
		}

		ValueType& value() & noexcept
		{
			AES_ASSERT(this->operator bool());
			return std::get<ValueType>(value_);
		}

		ValueType const&& value() const&& noexcept
		{
			AES_ASSERT(this->operator bool());
			return std::move(std::get<ValueType>(value_));
		}

		ValueType&& value() && noexcept
		{
			AES_ASSERT(this->operator bool());
			return std::move(std::get<ValueType>(value_));
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
			AES_ASSERT(!this->operator bool());
			return std::get<ErrorCodeType>(value_);
		}

	private:
		std::variant<ErrorCodeType, ValueType> value_;
	};

	template<>
	class Result<void>
	{
	public:
		using ValueType = void;
		using ErrorCodeType = UnderlyingError_t;

		Result() noexcept
		{

		}

		Result(AESError err) noexcept : value_(static_cast<ErrorCodeType>(err))
		{

		}

		operator bool() const noexcept
		{
			return std::holds_alternative<std::monostate>(value_);
		}

		ErrorCodeType error() const noexcept
		{
			AES_ASSERT(!this->operator bool());
			return std::get<ErrorCodeType>(value_);
		}

	private:
		std::variant<std::monostate, ErrorCodeType> value_;
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
		AES_UNREACHABLE();
	}

	//inline std::ostream& operator<<(std::ostream& stream, AESError err)
	//{
	//	stream << to_string(err);
	//	return stream;
	//}
}

#endif