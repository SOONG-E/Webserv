#include "ResponseStatus.hpp"

const std::string ResponseStatus::CODES[] = {
    "200", "201", "204", "400", "404", "405",
    "411", "413", "500", "501", "504", "505",
};

const std::string ResponseStatus::REASONS[] = {
    "OK",                          // 200
    "Created",                     // 201
    "No Content",                  // 204
    "Bad Request",                 // 400
    "Not Found",                   // 404
    "Method Not Allowed",          // 405
    "Length Required",             // 411
    "Payload Too Large",           // 413
    "Internal Server Error",       // 500
    "Not Implement",               // 501
    "Gateway Timeout",             // 504
    "HTTP Version Not Supported",  // 505
};
