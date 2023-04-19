#include "utils.h"

namespace utils
{
    std::string generateToken()
    {
        std::random_device engine;
        std::uniform_int_distribution<int> distribution(0, 25);

        std::string token;

        for (int i = 0; i < 10; i++)
        {
            token += 'a' + distribution(engine);
        }

        return token;
    }
}