#include "AssetPath.h"

#include "Log.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#define VALUE(string) #string
#define TO_LITERAL(string) VALUE(string)

static bool LogCalledOnce = false;

//-------------------------------------------------------------------------------------------------

std::shared_ptr<AssetPath> AssetPath::Instance() {
	std::shared_ptr<AssetPath> shared_ptr = _instance.lock();
	if (shared_ptr == nullptr)
	{
		shared_ptr = std::make_shared<AssetPath>();
		_instance = shared_ptr;
	}
	return shared_ptr;
}

//-------------------------------------------------------------------------------------------------

AssetPath::AssetPath()
{
#if defined(ASSET_DIR)
	mAssetPath = std::filesystem::absolute(std::string(TO_LITERAL(ASSET_DIR))).string();
#endif

	static constexpr char const * OVERRIDE_ASSET_PATH = "./asset_dir.txt";
	if (std::filesystem::exists(OVERRIDE_ASSET_PATH))
	{
		std::ifstream nameFileout{};
		mAssetPath.clear();

		nameFileout.open(OVERRIDE_ASSET_PATH);
		while (nameFileout >> mAssetPath)
		{
			std::cout << mAssetPath;
		}
		nameFileout.close();
	    if (LogCalledOnce == false)
	    {
            Log::info("Override asset path is %s", mAssetPath.c_str());
	    }
	}
	else
	{
	    if (LogCalledOnce == false)
	    {
            Log::info("No override found, using the default directory: %s", mAssetPath.c_str());
	    }
	}

    LogCalledOnce = true;
}

//-------------------------------------------------------------------------------------------------

AssetPath::~AssetPath() = default;

//-------------------------------------------------------------------------------------------------

std::string AssetPath::Get(std::string const& address) const
{
	return Get(address.c_str());
}

//-------------------------------------------------------------------------------------------------

std::string AssetPath::Get(char const *address) const
{
    return std::filesystem::path(mAssetPath).append(address).string();
}

//-------------------------------------------------------------------------------------------------