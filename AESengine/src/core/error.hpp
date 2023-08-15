#ifndef AES_ERROR_HPP
#define AES_ERROR_HPP

#include <type_traits>
#include <utility>

#include "aes.hpp"

/*
 * Error class Inspired by rust and Boost leaf
 * However returning error sucks, we may want to go back to exceptions in the future
 * we will use this only as common error object when exception will be back
 */
namespace aes {

	using UnderlyingError_t = int32_t;

	enum class AESError : UnderlyingError_t
	{
		Undefined,
		MemoryAllocationFailed,
		RHIDeviceCreationFailed,
		GPUBufferCreationFailed,
		GPUTextureCreationFailed,
		GPUBufferMappingFailed,
		SamplerCreationFailed,
		ShaderCompilationFailed,
		ShaderCreationFailed,
		FontInitFailed,
		BlendStateCreationFailed,
		RenderTargetCreationFailed,
		SamplerApplicationFailed
	};

	template<typename T>
	class [[nodiscard]] Result
	{
	public:
		using ValueType = T;
		using ErrorCodeType = AESError;

		Result(Result&& rhs) noexcept
		{
			isError = rhs.isError;
			if (isError)
			{
				payload.err = rhs.error();
			}
			else
			{
				payload.val = std::move(rhs.payload.val);
			}
		}

		Result(T&& val) noexcept 
		{
			isError = false;
			payload.val = std::forward<T>(val);
		}

		Result(AESError err) noexcept 
		{
			isError = true;
			payload.err = err;
		}

		Result(Result const&) = delete;
		
		[[nodiscard]] operator bool() const noexcept
		{
			return isError;
		}

		ValueType const& value() const& noexcept
		{
			AES_ASSERT(this->operator bool());
			return payload.val;
		}

		ValueType& value() & noexcept
		{
			AES_ASSERT(this->operator bool());
			return payload.val;
		}

		ValueType const&& value() const&& noexcept
		{
			AES_ASSERT(this->operator bool());
			return std::move(payload.val);
		}

		ValueType&& value() && noexcept
		{
			AES_ASSERT(this->operator bool());
			return std::move(payload.val);
		}

		ValueType const* operator->() const noexcept
		{
			return &value();
		}

		ValueType* operator->() noexcept
		{
			return &value();
		}

		[[nodiscard]] ErrorCodeType error() const noexcept
		{
			AES_ASSERT(!this->operator bool());
			return payload.err;
		}

		~Result() noexcept(std::is_nothrow_destructible<T>::value)
		{
			if AES_CPPIFCONSTEXPR(!std::is_trivially_destructible<ValueType>::value)
			{
				if (isError)
				{
					payload.val.~ValueType();
				}
			}
		}

	private:
		// same shit as optional
		union U
		{
			U() {}
			~U() {}
			ErrorCodeType err;
			ValueType val;
		} payload;
		bool isError;
	};

	template<>
	class [[nodiscard]] Result<void>
	{
	public:
		using ValueType = void;
		using ErrorCodeType = UnderlyingError_t;

		Result() noexcept
		{

		}

		Result(AESError err) noexcept : err(static_cast<ErrorCodeType>(err)), isError(true)
		{

		}

		[[nodiscard]] operator bool() const noexcept
		{
			return isError;
		}

		[[nodiscard]] ErrorCodeType error() const noexcept
		{
			AES_ASSERT(!this->operator bool());
			return err;
		}

	private:
		ErrorCodeType err;
		bool isError = false;
	};

	[[nodiscard]] inline const char* to_string(AESError err)
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

	inline std::ostream& operator<<(std::ostream& stream, AESError err)
	{
		stream << to_string(err);
		return stream;
	}
}

#ifdef AES_CPP20
template<>
struct fmt::formatter<aes::AESError> 
{
	template<typename ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return ctx.begin();
	}

	template<typename FormatContext>
	auto format(aes::AESError const& err, FormatContext& ctx)
	{
		return fmt::format_to(ctx.out(), "{}", to_string(err));
	};
};
#endif
#endif