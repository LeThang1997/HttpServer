
#ifndef json_h
#define json_h

#include <nlohmann/json.hpp>

using json = nlohmann::json;

#define REQUIRED(j, n) do { j.at(#n).get_to(n); } while (0)
#define OPTIONAL(j, n) do { try { n = j[#n]; } catch (std::exception& e) { } } while (0)

#endif /* json_h */
