#ifndef ERROR_HPP
#define ERROR_HPP

#include <variant>
#include <utility>

#include "aes.hpp"

namespace aes {

	enum class AESError
	{
		Undefined,
		GPUBufferCreationFailed,
		GPUTextureCreationFailed,
		GPUBufferMappingFailed,
		ShaderCompilationFailed,
		ShaderCreationFailed,
		FontInitFailed,
		BlendStateCreationFailed,
	};

	template<typename T, typename ErrorCode = AESError>
	class Result
	{
	public:
		using ValueType = T;
		using ErrorCodeType = ErrorCode;

		Result(T&& val) noexcept : value_(std::forward<T>(val))
		{

		}

		Result(T const& val) noexcept : value_(val)
		{

		}

		Result(ErrorCodeType err) noexcept : value_(err)
		{

		}

		operator bool() const noexcept
		{
			return std::holds_alternative<ValueType>(value_);
		}

		ValueType const& value() const& noexcept
		{
			return std::get<ValueType>(value_);
		}

		ValueType& value() & noexcept
		{
			return std::get<ValueType>(value_);
		}

		ValueType const&& value() const&& noexcept
		{
			return std::move(std::get<ValueType>(value_));
		}

		ValueType&& value() && noexcept
		{
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
			return std::get<ErrorCodeType>(value_);
		}

	private:
		std::variant<ValueType, ErrorCodeType> value_;
	};


	template<typename ErrorCode>
	class Result<void, ErrorCode>
	{
	public:
		using ValueType = void;
		using ErrorCodeType = ErrorCode;

		Result() noexcept : value_(true)
		{

		}

		Result(ErrorCodeType err) noexcept : value_(err)
		{

		}

		operator bool() const noexcept
		{
			return std::holds_alternative<bool>(value_);
		}

		ErrorCodeType error() const noexcept
		{
			return std::get<ErrorCodeType>(value_);
		}

	private:
		std::variant<bool, ErrorCodeType> value_;
	};

}

#endif