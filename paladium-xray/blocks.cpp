#include "blocks.hpp"

bool blocks::detect_minerals = true;
std::vector<whitelisted_block>* whitelisted_blocks = new std::vector<whitelisted_block>();

std::vector<whitelisted_block>* blocks::get_whitelisted_blocks()

{
	return whitelisted_blocks;
}


void blocks::initialize()
{	/* Minerais:*/

	if (detect_minerals)
	{
		whitelisted_blocks->push_back(whitelisted_block("Amethyst Ore", 546, vec3f(0.63F, 0.45F, 1.F))); // Amethyst
		whitelisted_blocks->push_back(whitelisted_block("Titane Ore", 547, vec3f(0.31F, 0.31F, 0.31F))); // Titane
		whitelisted_blocks->push_back(whitelisted_block("Paladium Ore", 548, vec3f(0.95F, 0.3F, 0.025F))); // Paladium
		whitelisted_blocks->push_back(whitelisted_block("GreenPaladium Ore", 549, vec3f(0.31F, 0.75F, 0.13F))); // Paladium Vert
		whitelisted_blocks->push_back(whitelisted_block("Findium Ore", 550, vec3f(0.9F, 0.7F, 0.01F))); // Findium
		whitelisted_blocks->push_back(whitelisted_block("Trixium Ore", 551, vec3f(0.1F, 0.6F, 0.6F))); // Trixium
		whitelisted_blocks->push_back(whitelisted_block("Redstone Ore", 73, vec3f(1.F, 0.F, 0.F))); // Redstone
		whitelisted_blocks->push_back(whitelisted_block("Gold Ore", 14, vec3f(0.94F, 0.94F, 0.29F))); // Gold

	}
	else {
		whitelisted_blocks->push_back(whitelisted_block("Chest", 54, vec3f(1.F, 0.F, 0.F))); // Normal Chest (RED)
		whitelisted_blocks->push_back(whitelisted_block("Trapped Chest", 146, vec3f(0.F, 0.F, 1.F))); // Trapp (BLUE)


		whitelisted_blocks->push_back(whitelisted_block("Drawer", 2991, vec3f(0.F, 1.F, 0.F))); // Drawer (GREEN)
		whitelisted_blocks->push_back(whitelisted_block("Drawer", 2992, vec3f(0.F, 1.F, 0.F)));
		whitelisted_blocks->push_back(whitelisted_block("Drawer", 2993, vec3f(0.F, 1.F, 0.F)));
		whitelisted_blocks->push_back(whitelisted_block("Drawer", 2994, vec3f(0.F, 1.F, 0.F)));
		whitelisted_blocks->push_back(whitelisted_block("Drawer", 2995, vec3f(0.F, 1.F, 0.F)));
		whitelisted_blocks->push_back(whitelisted_block("Drawer", 2996, vec3f(0.F, 1.F, 0.F)));
		whitelisted_blocks->push_back(whitelisted_block("Drawer", 2997, vec3f(0.F, 1.F, 0.F)));

		whitelisted_blocks->push_back(whitelisted_block("Ore Chest", 2971, vec3f(0.F, 1.F, 0.F))); // Custom chest (GREEN)
		whitelisted_blocks->push_back(whitelisted_block("Paladium Chest", 450, vec3f(0.F, 1.F, 0.F)));
		whitelisted_blocks->push_back(whitelisted_block("Endium Chest", 451, vec3f(0.F, 1.F, 0.F)));
		whitelisted_blocks->push_back(whitelisted_block("Titanium Chest", 452, vec3f(0.F, 1.F, 0.F)));
		whitelisted_blocks->push_back(whitelisted_block("Amethyst Chest", 453, vec3f(0.F, 1.F, 0.F)));
		whitelisted_blocks->push_back(whitelisted_block("GreenPaladium Chest", 454, vec3f(0.F, 1.F, 0.F)));

		whitelisted_blocks->push_back(whitelisted_block("Spawner", 52, vec3f(1.F, 1.F, 1.F))); // Spawner (WHITE)
	}
}

bool blocks::is_block_whitelisted(int block_id)
{
	for (whitelisted_block& b : *whitelisted_blocks)
	{
		if (b.block_id == block_id)
		{
			return true;
		}
	}

	return false;
}

vec3f* blocks::get_block_color(int block_id)
{
	for (whitelisted_block& b : *whitelisted_blocks)
	{
		if (b.block_id == block_id)
		{
			return &b.color;
		}
	}

	return new vec3f(0.F, 0.F, 0.F);
}