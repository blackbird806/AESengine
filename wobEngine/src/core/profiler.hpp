#ifndef WOB_PROFILER_HPP
#define WOB_PROFILER_HPP

#include "macro_helpers.hpp"
#include <cstdint>

#ifdef WOB_ENABLE_PROFILINGs

#define WOB_PROFILE_FUNCTION() ::aes::ProfileScope WOB_CONCAT(WOB_internal_profile_func_, __COUNTER__)(__FUNCTION__);
#define WOB_PROFILE_SCOPE(name) ::aes::ProfileScope WOB_CONCAT(WOB_internal_profile_func_, __COUNTER__)(name); 

#define WOB_PROFILE_FRAME()
#define WOB_PROFILE_FRAMEN(name)

#else

#define WOB_PROFILE_FUNCTION()
#define WOB_PROFILE_SCOPE(name) 

#define WOB_PROFILE_FRAME()
#define WOB_PROFILE_FRAMEN(name)

#endif

#define WOB_START_PROFILE_SESSION(name) ::wob::startInstrumentorSession(name)
#define WOB_STOP_PROFILE_SESSION() ::wob::endInstrumentorSession()

namespace wob
{
	using ProfileTime_t = double;

	struct ProfileData
	{
		const char* name;
		const char* parentName;
		ProfileTime_t elapsedTime;
#ifdef _WIN32
		size_t count = 1;
#else
		std::size_t count = 1;
#endif
	};

	struct ProfileSession
	{
		ProfileSession() noexcept;

		ProfileSession(const char* name, ProfileTime_t elapsed) noexcept;

		const char* name;
		ProfileTime_t elapsedSessionTime;
		struct ProfilerImpl* impl;
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