#include "profiler.hpp"

#include <fmt/format.h>
#include "aes.hpp"

using namespace aes;

aes::ProfileScope* aes::ProfileScope::last = nullptr;

void Instrumentor::startSession(std::string const& name)
{
	currentSession = ProfileSession{ name };
	sessionStartPoint = std::chrono::high_resolution_clock::now();
}

ProfileSession Instrumentor::stopSession()
{
	std::chrono::duration<ProfileTime_t, std::milli> const elapsed = std::chrono::high_resolution_clock::now() - sessionStartPoint;
	currentSession.elapsedSessionTime = elapsed.count();
	return std::move(currentSession);
}

void Instrumentor::updateProfileData(ProfileData const& data)
{
	auto it = currentSession.profileDatas.find(data.name);
	if (it != currentSession.profileDatas.end())
	{
		it->second.elapsedTime += data.elapsedTime;
		it->second.count++;
		return;
	}
	currentSession.profileDatas[data.name] = data;
}

ProfileScope::ProfileScope(const char* name_):
	name(name_),
	parent(last),
	start(std::chrono::high_resolution_clock::now())
{
	last = this;
}

ProfileScope::~ProfileScope()
{
	std::chrono::duration<ProfileTime_t, std::milli> const elapsed = std::chrono::high_resolution_clock::now() - start;

	Instrumentor::instance().updateProfileData(ProfileData{name, parent != nullptr ? parent->name : "none", elapsed.count()});
	if (last == this)
		last = last->parent;
}

