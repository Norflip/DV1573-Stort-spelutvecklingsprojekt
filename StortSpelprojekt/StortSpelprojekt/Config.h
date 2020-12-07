#pragma once
#include <fstream>

#define CONFIG_PRINT_ERRORS TRUE

namespace Config
{
	typedef const std::string& CKey;

	static const char m_delimiter = '=';
	static const char* m_filename = "config.cfg";

	inline std::unordered_map<std::string, float> m_configValues;
	inline bool m_dirty;

	inline void DebugPrintValues()
	{
		std::cout << "config values: ------" << std::endl;
		for (auto i : m_configValues)
			std::cout << "configLine: " << i.first << ' ' << m_delimiter << ' ' << std::to_string(i.second) << '\n';
		std::cout << "--------------" << std::endl;
	}

	inline bool ContainsKey(CKey key)
	{
		bool contains = m_configValues.find(key) != m_configValues.end();
#if CONFIG_PRINT_ERRORS
		if (!contains)
		{
			std::cout << "Config is missing key: \"" << key << "\"" << std::endl;
		}
#endif
		return contains;
	}

	inline void SetFloat(CKey key, float value) { m_configValues[key] = value; m_dirty = true; }
	inline void SetInt(CKey key, int value) { m_configValues[key] = FCAST(value); m_dirty = true; }
	inline void SetBool(CKey key, bool value) { m_configValues[key] = FCAST(ICAST(value)); m_dirty = true; }

	inline float GetFloat(CKey key, float def = 0.0f) { return ContainsKey(key) ? m_configValues[key] : def; }
	inline int GetInt(CKey key, int def = -1) { return ICAST(GetFloat(key, FCAST(def))); }
	inline bool GetBool(CKey key, bool def = false) { return GetInt(key, def) > 0; }

	inline bool TryGetFloat(CKey key, float& value) { bool tried = ContainsKey(key); if (tried) value = m_configValues[key]; }
	inline bool TryGetInt(CKey key, int& value) { bool tried = ContainsKey(key); if (tried) value = ICAST(m_configValues[key]); }
	inline bool TryGetBool(CKey key, bool& value) { bool tried = ContainsKey(key); if (tried) value = m_configValues[key] > 0; }

	inline void Load()
	{
		std::ifstream  file(m_filename);

		if (file.is_open())
		{
			std::string line, key;
			float value;
			m_configValues.clear();
			m_dirty = false;

			while (std::getline(file, line))
			{
				size_t index = line.find(m_delimiter);
				key = line.substr(0, index - 1);
				value = std::stof(line.substr(index + 2, line.length() - index - 2));
				//std::cout << "loading: \"" << key << "\" = \"" << std::to_string(value) << "\"" << std::endl;
				m_configValues.insert({ key, value });
			}


			file.close();
		}

		DebugPrintValues();
	}

	inline void Save()
	{
		if (m_dirty)
		{
			m_dirty = false;
			std::ofstream file(m_filename);
			if (file.is_open())
			{
				for (auto i : m_configValues)
					file << i.first << ' ' << m_delimiter << ' ' << std::to_string(i.second) << '\n';
				file.close();
			}
		}
	}

	inline void LoadOrDefault()
	{
		std::ifstream file(m_filename);

		if (!file.good())
		{
			m_dirty = true;
			m_configValues["FOV"] = 90;
			m_configValues["volumeSound"] = 0.5f;
			m_configValues["volumeMusic"] = 0.5f;
			m_configValues["sensitivity"] = 0.5f;

			std::cout << "creating default config" << std::endl;
			Save();
		}

		Load();
	}

}