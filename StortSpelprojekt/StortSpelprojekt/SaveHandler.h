#pragma once
#include <iostream>
#include <fstream>
#include <string>

#include "SaveState.h"

#define PRINT_IO TRUE
constexpr const char* FULL_FILE_PATH = "../TMP_SAVES/savefile.dat";

namespace SaveHandler
{
	static SaveState m_loadedState;
	static bool m_loaded;

	inline bool Contains()
	{
		std::ifstream f(FULL_FILE_PATH);
		return f.good();
	}

	inline void Save(const SaveState& state)
	{
		std::ofstream output_file(FULL_FILE_PATH, std::ios::binary);
		output_file.write((char*)&state, sizeof(SaveState));
		output_file.close();

#if PRINT_IO
		std::cout << "Saving" << FULL_FILE_PATH << "\tsize: " << sizeof(SaveState) << std::endl;
#endif

		m_loadedState = state;
		m_loaded = true;
	}

	inline const SaveState& CreateNew()
	{
#if PRINT_IO
		std::cout << "Creating new file @" << FULL_FILE_PATH << "\tsize: " << sizeof(SaveState) << std::endl;
#endif

		Save(SaveState::Default());
		return m_loadedState;
	}



	inline bool TryLoad(SaveState& state)
	{
#if PRINT_IO
		std::cout << "Trying to load @" << FULL_FILE_PATH << "\tsize: " << sizeof(SaveState) << std::endl;
#endif
		bool loaded = Contains();
		if (loaded)
		{
			std::ifstream input_file(FULL_FILE_PATH, std::ios::binary);
			input_file.read((char*)&m_loadedState, sizeof(SaveState));
			m_loaded = true;
			state = m_loadedState;
		}

		return loaded;
	}

	inline SaveState& LoadOrCreate ()
	{
#if PRINT_IO
		std::cout << "Loading @" << FULL_FILE_PATH << "\tsize: " << sizeof(SaveState) << std::endl;
#endif
		bool loaded = false;
		if (Contains())
		{
			std::ifstream input_file(FULL_FILE_PATH, std::ios::binary);
			input_file.read((char*)&m_loadedState, sizeof(SaveState));
		}
		else
		{
			CreateNew();
		}

		m_loaded = true;
		return m_loadedState;
	}

	inline bool RemoveSave()
	{
		if (Contains())
			remove(FULL_FILE_PATH);
	}
}