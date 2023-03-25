#include "ResponseException.hpp"

#include "ResponseStatus.hpp"

static ResponseStatus status;

ResponseException::ResponseException(const int idx)
    : ExceptionTemplate(status.reason.at(idx)), index(idx) {}
