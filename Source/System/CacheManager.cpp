#include "Precompiled.hpp"
#include "CacheManager.hpp"

#include "MainContext.hpp"

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize the cache manager! "
    #define LogRecreateCacheError() "Failed to recreate the cache! "

    // Cache registry filename.
    const char* CacheRegistryFile = "d39bf4ad-b0b4-4807-99a5-75941ce8a997";

    // Cache registry format type (also the header's magic word).
    const char* FormatType = "CacheRegistry";

    // Cache registry version. Increasing it will cause upgrade of the cache, if not available it will be recreated.
    unsigned int RegistryVersion = 1;

    // Cache integrity version. Increasing it will force the whole cache to recreate.
    unsigned int IntegrityVersion = 1;
}

CacheManager::CacheManager() :
    m_initialized(false)
{
}

CacheManager::~CacheManager()
{
    Cleanup();
}

bool CacheManager::Initialize()
{
    Cleanup();

    // Emergency cleanup call on failure.
    auto EmergenyCleanup = MakeScopeGuard([&]()
    {
        // Cleanup if initialization failed.
        if(!m_initialized)
        {
            Cleanup();
        }
    });

    // Load the cache.
    if(!LoadCache())
    {
        Log() << "Recreating the cache...";

        // Recreate the cache if it doesn't exist or it can't be used for some reason.
        if(!RecreateCache())
        {
            Log() << LogInitializeError() << "Couldn't recreate the cache!";
            return false;
        }
    }

    // Success!
    m_initialized = true;

    return true;
}

void CacheManager::Cleanup()
{
    // Save cache registry.
    SaveCache();

    // Cleanup members.
    ClearContainer(m_registry);

    m_initialized = false;
}

std::string CacheManager::Lookup(std::string filename)
{
    // Find an existing entry.
    auto it = m_registry.find(filename);

    if(it != m_registry.end())
    {
        std::string identifier = boost::uuids::to_string(it->second);
        return identifier;
    }

    // Create a new identifier.
    boost::uuids::uuid uuid;
    std::string identifier;

    do
    {
        uuid = boost::uuids::random_generator()();
        identifier = boost::uuids::to_string(uuid);
    }
    while(boost::filesystem::exists(Main::CacheDir() + identifier));

    // Add a new cache entry.
    m_registry.insert(std::make_pair(filename, uuid));

    return identifier;
}

bool CacheManager::LoadCache()
{
    // Open the cache registry.
    std::ifstream registryFile(Main::CacheDir() + CacheRegistryFile);

    if(!registryFile.is_open())
    {
        Log() << "Can't open the cache registry!";
        return false;
    }

    // Read the file header.
    std::string formatType;
    unsigned int registryVersion;
    unsigned int integrityVersion;

    registryFile >> formatType;
    registryFile >> registryVersion;
    registryFile >> integrityVersion;

    if(!registryFile.good())
    {
        Log() << "Can't read the cache registry header!";
        return false;
    }

    // Validate the header.
    if(formatType != FormatType)
    {
        Log() << "Invalid cache registry header!";
        return false;
    }

    if(registryVersion != registryVersion)
    {
        Log() << "Invalid cache registry version.";
        return false;
    }

    if(integrityVersion != IntegrityVersion)
    {
        Log() << "Cache integrity version mismatch.";
        return false;
    }

    // Move to next line after reading the header.
    registryFile >> std::ws;

    // Setup scope guard to clear registry entries we could've added past this point.
    auto clearRegistryEntires = MakeScopeGuard([&]()
    {
        ClearContainer(m_registry);
    });

    // Read the cache registry.
    std::string line;

    while(!registryFile.eof())
    {
        // Read the line.
        std::getline(registryFile, line);

        if(line.empty())
            continue;

        // Create the tokenizer.
        boost::escaped_list_separator<char> separator('\\', ' ', '\"');
        boost::tokenizer<boost::escaped_list_separator<char>> tokens(line, separator);

        // Next token read function.
        auto token = tokens.begin();

        auto ReadNextToken = [&](std::string& output) -> bool
        {
            if(token != tokens.end())
            {
                output = *token++;
                return true;
            }

            return false;
        };

        // Read the registry entry tokens.
        std::string identifier;
        std::string filename;

        if(!ReadNextToken(identifier) || !ReadNextToken(filename))
        {
            Log() << "Can't read a cache registry entry!";
            return false;
        }

        // Check if cache file is valid.
        if(!ValidateCache(identifier))
        {
            Log() << "Discarded an invalid cache registry entry.";
            continue;
        }
        
        // Convert identifier to UUID structure.
        boost::uuids::uuid uuid = boost::uuids::string_generator()(identifier);

        // Add a cache registry entry.
        m_registry.insert(std::make_pair(filename, uuid));
    }

    // Disable scope guard.
    clearRegistryEntires.Disable();
    
    return true;
}

bool CacheManager::RecreateCache()
{
    boost::system::error_code error;

    // Remove the cache directory and it's content.
    boost::filesystem::remove_all(Main::CacheDir(), error);

    if(error)
    {
        Log() << LogRecreateCacheError() << "Can't remove the cache directory.";
        return false;
    }

    // Create an empty cache directory.
    boost::filesystem::create_directory(Main::CacheDir(), error);

    if(error)
    {
        Log() << LogRecreateCacheError() << "Can't create the cache directory.";
        return false;
    }

    // Create the cache registry.
    std::ofstream registryFile(Main::CacheDir() + CacheRegistryFile);

    if(!registryFile.is_open())
    {
        Log() << LogRecreateCacheError() << "Can't create the cache registry.";
        return false;
    }

    registryFile.flush();
    registryFile.close();

    return true;
}

bool CacheManager::SaveCache()
{
    if(!m_initialized)
        return false;

    // Open the cache registry.
    std::ofstream registryFile(Main::CacheDir() + CacheRegistryFile);

    if(!registryFile.is_open())
    {
        Log() << "Can't open the cache registry.";
        return false;
    }

    // Write the file header.
    registryFile << FormatType << ' ';
    registryFile << RegistryVersion << ' ';
    registryFile << IntegrityVersion << '\n';

    // Write the cache entries.
    for(auto it = m_registry.begin(); it != m_registry.end(); ++it)
    {
        std::string identifier = boost::uuids::to_string(it->second);

        // Check if cache file is valid.
        if(!ValidateCache(identifier))
        {
            Log() << "Tried to write an invalid cache entry.";
            continue;
        }

        // Write a cache entry.
        registryFile << it->second << ' ';
        registryFile << '"' << it->first << '"' << '\n';
    }

    return true;
}

bool CacheManager::ValidateCache(std::string identifier)
{
    std::string file = Main::CacheDir() + identifier;

    // Make sure the cache exists.
    if(!boost::filesystem::exists(file))
    {
        return false;
    }

    // Make sure it's not empty.
    if(boost::filesystem::file_size(file) == 0)
    {
        return false;
    }

    return true;
}
