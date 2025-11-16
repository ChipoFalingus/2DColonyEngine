
#include "Furniture.h"
#include "Item.h"

// Is there a better way to hold this stuff??

Furniture chestFurniture(
    std::make_unique<Item>("chest", L'■', sf::Color(100, 65, 23)),
    10, 10
);


Furniture bedFurniture(
    std::make_unique<Item>("bed", L'B', sf::Color(100, 65, 23)),
    10, 10
);

Furniture Anvil(
    std::make_unique<Item>("anvil", L'+', sf::Color(46, 46, 46)),
    10, 10
);

Furniture CarpentryBench(
    std::make_unique<Item>("bench", L'π', sf::Color(117, 66, 12)),
    10, 10
);

Furniture GunBench(
    std::make_unique<Item>("gunbench", L'π', sf::Color(135, 135, 135)),
    10, 10
);

Furniture Chair(
    std::make_unique<Item>("chair", L'h', sf::Color(148, 77, 0)),
    10, 10
);

Furniture Table(
    std::make_unique<Item>("table", L'T', sf::Color(148, 77, 0)),
    10, 10
);