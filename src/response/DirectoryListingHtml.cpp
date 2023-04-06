#include "DirectoryListingHtml.hpp"

#include "utility.hpp"

std::string DirectoryListingHtml::generate(const std::set<File>& entries) {
  return head() + body(entries) + "\n";
}

std::string DirectoryListingHtml::head(void) {
  return "<!DOCTYPE html>"
         "<html>"
         "  <head>"
         "    <title>Index"
         "</title>"
         "    <style>"
         "      body {"
         "        font-family: sans-serif;"
         "      }"
         "      table {"
         "        border-collapse: collapse;"
         "        width: 100%;"
         "      }"
         "      th,"
         "      td {"
         "        text-align: left;"
         "        padding: 8px;"
         "      }"
         "      tr:nth-child(even) {"
         "        background-color: #f2f2f2;"
         "      }"
         "      th {"
         "        background-color: #4CAF50;"
         "        color: white;"
         "      }"
         "    </style>"
         "  </head>";
}

std::string DirectoryListingHtml::body(const std::set<File>& entries) {
  std::string html =
      "  <body>"
      "    <h1>Index"
      "</h1>"
      "    <table>"
      "      <tr>"
      "        <th>Name</th>"
      "        <th>Last modified</th>"
      "        <th>Size</th>"
      "      </tr>";
  for (std::set<File>::const_iterator entry_iter = entries.begin();
       entry_iter != entries.end(); ++entry_iter) {
    html +=
        "      <tr>"
        "        <td>" +
        entry_iter->name +
        "        </td>"
        "        <td>" +
        formatTime("%Y-%m-%d %H:%M:%S", entry_iter->last_modified) +
        "        </td>"
        "        <td>" +
        toString(entry_iter->size) +
        "        </td>"
        "      </tr>";
  }
  html +=
      "    </table>"
      "  </body>"
      "</html>";
  return html;
}
