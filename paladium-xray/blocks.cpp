#include "blocks.hpp"

std::vector<whitelisted_block>* whitelisted_blocks = new std::vector<whitelisted_block>();

std::vector<whitelisted_block>* blocks::get_whitelisted_blocks()
{
	return whitelisted_blocks;
}


void blocks::initialize()
{
	/* Minerais:

	*/

	whitelisted_blocks->push_back(whitelisted_block(546, vec3f(0.85F, 0.F, 1.F)));
	whitelisted_blocks->push_back(whitelisted_block(547, vec3f(0.31F, 0.31F, 0.31F))); // Titane
	whitelisted_blocks->push_back(whitelisted_block(548, vec3f(0.95F, 0.3F, 0.025F))); // Paladium
	whitelisted_blocks->push_back(whitelisted_block(549, vec3f(0.F, 1.F, 0.F)));
	whitelisted_blocks->push_back(whitelisted_block(550, vec3f(0.95F, 0.8F, 0.05F)));
	whitelisted_blocks->push_back(whitelisted_block(551, vec3f(0.F, 1.F, 0.6F)));
	whitelisted_blocks->push_back(whitelisted_block(73, vec3f(1.F, 0.F, 0.F))); // Redstone

	/* Coffres (pillage):
		whitelisted_blocks->push_back(whitelisted_block(54, vec3f(1.F, 0.F, 0.F))); // Normal Chest (RED)
	whitelisted_blocks->push_back(whitelisted_block(146, vec3f(0.F, 0.F, 1.F))); // Trapp (BLUE)


	whitelisted_blocks->push_back(whitelisted_block(2991, vec3f(0.F, 1.F, 0.F))); // Drawer (GREEN)
	whitelisted_blocks->push_back(whitelisted_block(2992, vec3f(0.F, 1.F, 0.F)));
	whitelisted_blocks->push_back(whitelisted_block(2993, vec3f(0.F, 1.F, 0.F)));
	whitelisted_blocks->push_back(whitelisted_block(2994, vec3f(0.F, 1.F, 0.F)));
	whitelisted_blocks->push_back(whitelisted_block(2995, vec3f(0.F, 1.F, 0.F)));
	whitelisted_blocks->push_back(whitelisted_block(2996, vec3f(0.F, 1.F, 0.F)));
	whitelisted_blocks->push_back(whitelisted_block(2997, vec3f(0.F, 1.F, 0.F)));

	whitelisted_blocks->push_back(whitelisted_block(2971, vec3f(0.F, 1.F, 0.F))); // Custom chest (GREEN)
	whitelisted_blocks->push_back(whitelisted_block(450, vec3f(0.F, 1.F, 0.F)));
	whitelisted_blocks->push_back(whitelisted_block(451, vec3f(0.F, 1.F, 0.F)));
	whitelisted_blocks->push_back(whitelisted_block(452, vec3f(0.F, 1.F, 0.F)));
	whitelisted_blocks->push_back(whitelisted_block(453, vec3f(0.F, 1.F, 0.F)));
	whitelisted_blocks->push_back(whitelisted_block(454, vec3f(0.F, 1.F, 0.F)));

	whitelisted_blocks->push_back(whitelisted_block(52, vec3f(1.F, 1.F, 1.F))); // Spawner (WHITE)
	*/


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