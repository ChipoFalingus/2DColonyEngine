#pragma once
#include <vector>
#include <functional>
#include <optional>
#include <entt/entt.hpp>

#include "Jobs/JobType.h"
#include "Utility/ItemLocation.h"


enum class JobState {
	Active,
	Queued,
	Waiting,
	Completed,
	Failed,
	Dangerous
};


struct Job {
	entt::entity villager = entt::null; // Who is assigned to the job
	entt::entity preferredTool = entt::null;
	SkillType type;
	JobState state = JobState::Queued;

	int x, y; // Where the job requires you to be
	int priority = 0; // Higher priority jobs get assigned first

	Job(entt::entity v, entt::entity preferredTool, SkillType skillType)
		:villager(v), preferredTool(preferredTool), type(skillType) {
	}

	virtual ~Job() = default;

	virtual void update() {}
	virtual void onInterrupt() {}
	//virtual void waitingUpdate() {}
	//virtual void onFail() {}
};

void evaluateJobDanger(Job* job);

// Shouldn't be static
class JobManager {
public:
	// Holds all available jobs and gives them to villagers whenever possible
	static std::vector<Job*> JobList;

	static void addJob(Job* job) {
		JobList.push_back(job);
	}
	static void removeJob(Job* job) {
		JobList.erase(
			std::remove(JobList.begin(), JobList.end(), job),
			JobList.end()
		);
		delete job;
	}

	static void assignJobs();
	static void update();
};