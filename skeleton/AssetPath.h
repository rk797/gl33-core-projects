#pragma once

#include <filesystem>
#include <memory>
#include <string>

//https://stackoverflow.com/questions/4815423/how-do-i-set-the-working-directory-to-the-solution-directory
class AssetPath
{
public:

    static std::shared_ptr<AssetPath> Instance();

    explicit AssetPath();

    ~AssetPath();

    // Returns correct address based on platform
    [[nodiscard]]
    std::string Get(std::string const& address) const;

    [[nodiscard]]
    std::string Get(char const * address) const;

private:

    inline static std::weak_ptr<AssetPath> _instance {};
    std::string mAssetPath {};

};