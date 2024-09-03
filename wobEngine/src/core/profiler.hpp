#ifndef AES_PROFILER_HPP
#define AES_PROFILER_HPP

#include "macro_helpers.hpp"
#include <cstdint>

#ifdef AES_ENABLE_PROFILINGs

#define AES_PROFILE_FUNCTION() ::aes::ProfileScope AES_CONCAT(aes_internal_profile_func_, __COUNTER__)(__FUNCTION__);
#define AES_PROFILE_SCOPE(name) ::aes::ProfileScope AES_CONCAT(aes_internal_profile_func_, __COUNTER__)(name); 

#define AES_PROFILE_FRAME()
#define AES_PROFILE_FRAMEN(name)

#else

#define AES_PROFILE_FUNCTION()
#define AES_PROFILE_SCOPE(name) 

#define AES_PROFILE_FRAME()
#define AES_PROFILE_FRAMEN(name)

#endif

#define AES_START_PROFILE_SESSION(name) ::aes::startInstrumentorSession(name)
#define AES_STOP_PROFILE_SESSION() ::aes::endInstrumentorSession()

namespace aes
{
	using ProfileTime_t = double;

	struct ProfileData
	{
		const char* name;
		const char* parentName;
		ProfileTime_t elapsedTime;
		std::size_t count = 1;
	};

	struct ProfileSession
	{
		ProfileSession() noexcept : name(nullptr), elapsedSessionTime(0.0)//, profileDatas(64)
		{

		}

		ProfileSession(const char* name, ProfileTime_t elapsed) noexcept : name(name), elapsedSessionTime(elapsed)//, profileDatas(64)
		{

		}


		const char* name;
		ProfileTime_t elapsedSessionTime;
		//HashMap<const char*, ProfileData> profileDatas;
	};

	void startInstrumentorSession(const char* name) noexcept;

	ProfileSession endInstrumentorSession() noexcept;
	
	class ProfileScope
	{
	public:

		ProfileScope(const char* name_);

		~ProfileScope();

	private:

		/*thread_local ?*/ static ProfileScope* last;
		const char* name;
		ProfileScope* parent;
		ProfileTime_t start;
	};

}

#endif