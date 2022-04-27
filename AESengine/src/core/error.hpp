#ifndef AES_ERROR_HPP
#define AES_ERROR_HPP

#include <variant>
#include <utility>

#include "aes.hpp"

namespace aes {

	using UnderlyingError_t = int32_t;

	enum class AESError : UnderlyingError_t
	{
		Undefined,
		MemoryAllocationFailed,
		GPUBufferCreationFailed,
		GPUTextureCreationFailed,
		GPUBufferMappingFailed,
		ShaderCompilationFailed,
		ShaderCreationFailed,
		FontInitFailed,
		BlendStateCreationFailed,
	};

	template<typename T>
	class Result
	{
	public:
		using ValueType = T;
		using ErrorCodeType = UnderlyingError_t;

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

}

#endif