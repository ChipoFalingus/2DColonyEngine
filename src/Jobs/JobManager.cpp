#include "Jobs/Job.h"
#include "Utility/CreatureUtils.h"
#include "Game.h"
#include "Utility/ItemLocation.h"

#include "Entities/CreatureComponents.h"

#include <limits>
#include "World/World.h"
#include "Utility/ItemUtils.h"

std::vector<Job*> JobManager::JobList;

struct Bid {
	Job* job;
	entt::entity villager;
	int score;
};

void JobManager::assignJobs() {

	std::vector<Bid> bids;

	auto& registry = mainWorld.registry;
	auto view = registry.view<Villager, JobComponent, Position, Skills>();

	for (auto [entity, job_component, position, skills] : view.each()) {

		for (auto& job : JobList) {

			if (!job) continue;
			if (job->state != JobState::Queued) continue;
			if (job_component.panicClock < job_component.panicDuration) continue;

			int score = 0;

			int dx = std::abs(position.x - job->x);
			int dy = std::abs(position.y - job->y);

			score += (dx + dy) / 50;
			score -= skills.getSkillLevel(job->type) * 10;
			//score += v->tiredness * 5;

			if (job_component.currentJob) score += 10;

			bids.push_back({ job, entity, score });
		}
	}

	std::sort(bids.begin(), bids.end(), [](const Bid& a, const Bid& b) {
		return a.score < b.score;
		});

	std::unordered_set<Job*> assignedJobs;
	std::unordered_set<entt::entity> assignedVillagers;

	for (auto& bid : bids) {
		// Skip job if its already assigned
		if (assignedJobs.count(bid.job))
			continue;

		// Guarantees reservation for most qualified villager
		assignedJobs.insert(bid.job);

		auto villager = registry.try_get<JobComponent>(bid.villager);

		if (villager->willAccept(bid.job)) {
			bid.job->villager = bid.villager;
			villager->proposeJob(bid.job);
			assignedVillagers.insert(bid.villager);
		}
	}
}

void JobManager::update() {
	for (Job* job : JobList) {
		if (!job) continue;
		if (job->state == JobState::Dangerous) {
			evaluateJobDanger(job);
		}
	}

	assignJobs();

	for (size_t i = 0; i < JobList.size(); ) {
		Job* job = JobList[i];

		if (!job) {
			i++;
			continue;
		}

		if (job->state == JobState::Completed || job->state == JobState::Failed) {

			auto jobComponent = mainWorld.registry.try_get<JobComponent>(job->villager);
			if (!jobComponent) {
				job->villager = entt::null;
			}
			else if (jobComponent->currentJob == job) {
				jobComponent->currentJob = nullptr;
			}

			JobList.erase(JobList.begin() + i);
			delete job;
		}
		else {
			i++;
		}
	}
}

