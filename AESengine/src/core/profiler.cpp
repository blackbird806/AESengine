#include "profiler.hpp"

#include <chrono>
#include "aes.hpp"
#include "hashmap.hpp"

using namespace aes;

aes::ProfileScope* aes::ProfileScope::last = nullptr;

namespace aes
{
	class Instrumentor
	{
	public:

		static Instrumentor& instance()
		{
			static Instrumentor inst;
			return inst;
		}

		void startSession(const char* name);
		ProfileSession stopSession();

		void updateProfileData(ProfileData const& data);

	private:
		ProfileSession currentSession;
		std::chrono::time_point<std::chrono::high_resolution_clock> sessionStartPoint;
	};

	void startInstrumentorSession(const char* name) noexcept
	{
		Instrumentor::instance().startSession(name);
	}

	ProfileSession endInstrumentorSession() noexcept
	{
		return Instrumentor::instance().stopSession();
	}
}

void Instrumentor::startSession(const char* name)
{
	currentSession = ProfileSession(name, 0.0);
	sessionStartPoint = std::chrono::high_resolution_clock::now();
}

ProfileSession Instrumentor::stopSession()
{
	std::chrono::duration<ProfileTime_t, std::milli> const elapsed = std::chrono::high_resolution_clock::now() - sessionStartPoint;
	currentSession.elapsedSessionTime = elapsed.count();
	return currentSession;
}

void Instrumentor::updateProfileData(ProfileData const& data)
{
	ProfileData* d = nullptr;
	//if (currentSession.profileDatas.tryFind(data.name, d))
	//{
	//	d->elapsedTime += data.elapsedTime;
	//	d->count++;
	//	return;
	//}
	//currentSession.profileDatas[data.name] = data;
}

ProfileScope::ProfileScope(const char* name_):
	name(name_),
	parent(last)
	//start(std::chrono::high_resolution_clock::now())
{
	last = this;
}

ProfileScope::~ProfileScope()
{
	//std::chrono::duration<ProfileTime_t, std::milli> const elapsed = std::chrono::high_resolution_clock::now() - start;

	//Instrumentor::instance().updateProfileData(ProfileData{name, parent != nullptr ? parent->name : "none", elapsed.count()});
	if (last == this)
		last = last->parent;
}
