#pragma once
#include <memory>
#include "Job.h"

struct JobCompare {
	bool operator()(Job* a, Job* b) const {
		return a->priority < b->priority;
	}
};