#include "blocks.hpp"
#include "config.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <vector>
#include <string>

#include <shlobj.h>

bool blocks::detect_minerals = true;
bool blocks::dev = false;

int blocks::chest_offset, blocks::ore_offset = 26; //Latest: Paladium v10

std::vector<whitelisted_block>* whitelisted_blocks = new std::vector<whitelisted_block>();

std::vector<whitelisted_block>* blocks::get_whitelisted_blocks()

{
	return whitelisted_blocks;
}


void blocks::initialize()
{

	if (dev)
	{
		/* Minerais:*/
		whitelisted_blocks->push_back(whitelisted_block("Amethyst Ore", (563 + ore_offset), vec3f(0.63F, 0.45F, 1.F))); // Amethyst

		whitelisted_blocks->push_back(whitelisted_block("Titane Ore", (564 + ore_offset), vec3f(0.31F, 0.31F, 0.31F))); // Titane

		whitelisted_blocks->push_back(whitelisted_block("Paladium Ore", (565 + ore_offset), vec3f(0.95F, 0.3F, 0.025F))); // Paladium

		whitelisted_blocks->push_back(whitelisted_block("GreenPaladium Ore", (566 + ore_offset), vec3f(0.31F, 0.75F, 0.13F))); // Paladium Vert

		whitelisted_blocks->push_back(whitelisted_block("Findium Ore", (567 + ore_offset), vec3f(0.9F, 0.7F, 0.01F))); // Findium

		whitelisted_blocks->push_back(whitelisted_block("Trixium Ore", (568 + ore_offset), vec3f(0.1F, 0.6F, 0.6F))); // Trixium


		whitelisted_blocks->push_back(whitelisted_block("Redstone Ore", 73, vec3f(1.F, 0.F, 0.F))); // Redstone
		whitelisted_blocks->push_back(whitelisted_block("Gold Ore", 14, vec3f(0.94F, 0.94F, 0.29F))); // Gold

	} else 
	{
		config::import_config("default_ores.json");
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