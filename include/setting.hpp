#ifndef SETTING_HPP_
#define SETTING_HPP_

#include <string>

/* setting for default value */
const std::string DEFAULT_ERROR_DIRECTORY = "html/default_error/";
const std::string DEFAULT_ERROR_PAGE = "html/default_error/error.html";
const std::string DEFAULT_PATH = "conf/default.conf";
const std::string DEFAULT_PORT = "80";
const std::string DIRECTORY_LISTING_PAGE = "static/autoindex_template.html";

/* setting for data size */
const int CAPABLE_EVENT_SIZE = 8;
const std::size_t BUFFER_SIZE = 65536;

/* setting for max time */
const std::time_t KEEPALIVE_TIMEOUT = 30;
const std::time_t SESSION_TIMEOUT = 1800;
const std::time_t CGI_TIMEOUT = 30;
const std::string COOKIE_MAX_AGE = "3600";

#endif
