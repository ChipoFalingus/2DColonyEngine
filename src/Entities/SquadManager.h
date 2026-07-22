#pragma once
#include "Squad.h"

class SquadManager {
private:
	std::vector<Squad> squads;

public:
	std::vector<Squad>& getSquads() {
		return squads;
	}

	void update() {
		for (Squad& squad : squads) {
			squad.update();

			if (squad.memberCount() <= 0) {
				removeSquad(squad);
			}
		}
	}

	void createSquad(Creature* leader) {
		Squad newSquad;
		newSquad.addMember(leader);
		squads.push_back(newSquad);
	}

	void removeSquad(Squad& squad) {
		squads.erase(std::remove(squads.begin(), squads.end(), squad), squads.end());
	}

	void mergeSquads(Squad& squadA, Squad& squadB) {
		for (Creature* member : squadB.getMembers()) {
			squadA.addMember(member);
		}
		removeSquad(squadB);
	}
};