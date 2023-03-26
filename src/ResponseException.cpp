#include "exception/ResponseException.hpp"

#include "ResponseStatus.hpp"

static const ResponseStatus STATUS;

ResponseException::ResponseException(const int idx)
    : ExceptionTemplate(STATUS.reason.at(idx)), index(idx) {}
