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

		whitelisted_blocks->push_back(whitelisted_block("Amethyst Ore", 563, vec3f(0.63F, 0.45F, 1.F))); // Amethyst

		whitelisted_blocks->push_back(whitelisted_block("Titane Ore", 564, vec3f(0.31F, 0.31F, 0.31F))); // Titane

		whitelisted_blocks->push_back(whitelisted_block("Paladium Ore", 565, vec3f(0.95F, 0.3F, 0.025F))); // Paladium

		whitelisted_blocks->push_back(whitelisted_block("GreenPaladium Ore", 566, vec3f(0.31F, 0.75F, 0.13F))); // Paladium Vert

		whitelisted_blocks->push_back(whitelisted_block("Findium Ore", 567, vec3f(0.9F, 0.7F, 0.01F))); // Findium

		whitelisted_blocks->push_back(whitelisted_block("Trixium Ore", 568, vec3f(0.1F, 0.6F, 0.6F))); // Trixium


		whitelisted_blocks->push_back(whitelisted_block("Redstone Ore", 73, vec3f(1.F, 0.F, 0.F))); // Redstone
		whitelisted_blocks->push_back(whitelisted_block("Gold Ore", 14, vec3f(0.94F, 0.94F, 0.29F))); // Gold

	}
	else {
		whitelisted_blocks->push_back(whitelisted_block("Spawner", 52, vec3f(1.F, 1.F, 1.F))); // Spawner (WHITE)

		whitelisted_blocks->push_back(whitelisted_block("Chest", 54, vec3f(1.F, 0.F, 0.F))); // Normal Chest (RED)
		whitelisted_blocks->push_back(whitelisted_block("Trapped Chest", 146, vec3f(0.F, 0.F, 1.F))); // Trapp (BLUE)

		whitelisted_blocks->push_back(whitelisted_block("Paladium Chest", 463, vec3f(0.F, 1.F, 0.F))); // Custom chest (GREEN)
		whitelisted_blocks->push_back(whitelisted_block("Endium Chest", 464, vec3f(0.F, 1.F, 0.F)));
		whitelisted_blocks->push_back(whitelisted_block("Titanium Chest", 465, vec3f(0.F, 1.F, 0.F)));
		whitelisted_blocks->push_back(whitelisted_block("Amethyst Chest", 466, vec3f(0.F, 1.F, 0.F)));
		whitelisted_blocks->push_back(whitelisted_block("GreenPaladium Chest", 467, vec3f(0.F, 1.F, 0.F)));

		whitelisted_blocks->push_back(whitelisted_block("Ore Chest", 3003, vec3f(0.F, 1.F, 0.F))); // Custom chest (GREEN)

		whitelisted_blocks->push_back(whitelisted_block("Drawer", 3043, vec3f(0.F, 1.F, 0.F))); // Drawer (GREEN)
		whitelisted_blocks->push_back(whitelisted_block("Drawer", 3044, vec3f(0.F, 1.F, 0.F)));
		whitelisted_blocks->push_back(whitelisted_block("Drawer", 3045, vec3f(0.F, 1.F, 0.F)));
		whitelisted_blocks->push_back(whitelisted_block("Drawer", 3046, vec3f(0.F, 1.F, 0.F)));
		whitelisted_blocks->push_back(whitelisted_block("Drawer", 3047, vec3f(0.F, 1.F, 0.F)));
		whitelisted_blocks->push_back(whitelisted_block("Drawer", 3048, vec3f(0.F, 1.F, 0.F)));
		whitelisted_blocks->push_back(whitelisted_block("Drawer", 3049, vec3f(0.F, 1.F, 0.F)));
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