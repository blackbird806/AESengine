#ifndef PROFILER_HPP
#define PROFILER_HPP

#include <unordered_map>
#include <string>
#include <chrono>
#include <optional>

#include "macro_helpers.hpp"

#ifdef AES_ENABLE_PROFILING

#define AES_PROFILE_FUNCTION() ::aes::ProfileScope AES_CONCAT(aes_internal_profile_func_, __COUNTER__)(__FUNCTION__);
#define AES_PROFILE_SCOPE(name) ::aes::ProfileScope AES_CONCAT(aes_internal_profile_func_, __COUNTER__)(name); 

#define AES_PROFILE_FRAME()
#define AES_PROFILE_FRAMEN(name)

#define AES_START_PROFILE_SESSION(name) ::aes::Instrumentor::instance().startSession(name);
#define AES_STOP_PROFILE_SESSION() ::aes::Instrumentor::instance().stopSession();

#else

#define AES_PROFILE_FUNCTION()
#define AES_PROFILE_SCOPE(name) 

#define AES_PROFILE_FRAME()
#define AES_PROFILE_FRAMEN(name)

#define AES_START_PROFILE_SESSION(name) 
#define AES_STOP_PROFILE_SESSION() 

#endif


namespace aes
{
	using ProfileTime_t = double;

	struct ProfileData
	{
		const char* name;
		const char* parentName;
		ProfileTime_t elapsedTime;
		size_t count = 1;
	};

	struct ProfileSession
	{
		std::string name;
		ProfileTime_t elapsedSessionTime;
		std::unordered_map<const char*, ProfileData> profileDatas;
	};
	
	class Instrumentor
	{
	public:

		static Instrumentor& instance()
		{
			static Instrumentor inst;
			return inst;
		}

		void startSession(std::string const& name);
		ProfileSession stopSession();
		
		void updateProfileData(ProfileData const& data);

	private:
		ProfileSession currentSession;
		std::chrono::time_point<std::chrono::high_resolution_clock> sessionStartPoint;

	};

	class ProfileScope
	{
	public:

		ProfileScope(const char* name_);

		~ProfileScope();

	private:

		/*thread_local ?*/ static ProfileScope* last;
		const char* name;
		ProfileScope* parent;
		std::chrono::time_point<std::chrono::high_resolution_clock> start;
	};

}


#endif