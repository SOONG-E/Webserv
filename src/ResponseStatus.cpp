#include "ResponseStatus.hpp"

const std::string ResponseStatus::CODES[] = {"200", "400", "404",
                                             "411", "413", "500", "501", "505"};
const std::string ResponseStatus::REASONS[] = {"OK",
                                               "Bad Request",
                                               "Not Found",
                                               "Length Required",
                                               "Payload Too Large",
                                               "Internal Server Error",
                                               "Not Implement",
                                               "HTTP Version Not Supported"};
