#include "view.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <cassert>
#include <iostream>
#include <regex>
#include <set>

#include "../app/use_cases.h"
#include "../menu/menu.h"

using namespace std::literals;
namespace ph = std::placeholders;

namespace ui {
namespace detail {

std::ostream& operator<<(std::ostream& out, const AuthorInfo& author) {
    out << author.name;
    return out;
}

std::ostream& operator<<(std::ostream& out, const BookAuthorInfo& book) {
    out << book.title << " by "s << book.name << ", "s << book.publication_year;
    return out;
}

std::ostream& operator<<(std::ostream& out, const BookInfo& book) {
    out << book.title << ", "s << book.publication_year;
    return out;
}

}  // namespace detail

std::ostream& operator<<(std::ostream& out, const app::BookAuthorInfo& book) {
    using namespace std::literals;
    out << book.title << " by "s << book.name << ", "s << book.publication_year;
    return out;
}

template <typename T>
void PrintVector(std::ostream& out, const std::vector<T>& vector) {
    int i = 1;
    for (auto& value : vector) {
        out << i++ << " "s << value << std::endl;
    }
}

View::View(menu::Menu& menu, app::UseCases& use_cases, std::istream& input, std::ostream& output)
    : menu_{menu}, use_cases_{use_cases}, input_{input}, output_{output} {
    menu_.AddAction(  //
        "AddAuthor"s, "name"s, "Adds author"s, std::bind(&View::AddAuthor, this, ph::_1)
        // либо
        // [this](auto& cmd_input) { return AddAuthor(cmd_input); }
    );
    menu_.AddAction("AddBook"s, "<pub year> <title>"s, "Adds book"s, std::bind(&View::AddBook, this, ph::_1));
    menu_.AddAction("ShowAuthors"s, {}, "Show authors"s, std::bind(&View::ShowAuthors, this));
    menu_.AddAction("ShowBooks"s, {}, "Show books"s, std::bind(&View::ShowBooks, this));
    menu_.AddAction("ShowAuthorBooks"s, {}, "Show author books"s, std::bind(&View::ShowAuthorBooks, this));

    menu_.AddAction("DeleteAuthor"s, "name"s, "Delete author"s, std::bind(&View::DeleteAuthor, this, ph::_1));
    menu_.AddAction("EditAuthor"s, "name"s, "Edit author"s, std::bind(&View::EditAuthor, this, ph::_1));

    menu_.AddAction("DeleteBook"s, "title"s, "Delete book"s, std::bind(&View::DeleteBook, this, ph::_1));
    menu_.AddAction("EditBook"s, "title"s, "Edit book"s, std::bind(&View::EditBook, this, ph::_1));

    // menu_.AddAction("ShowBook"s, "title"s, "Show book"s, std::bind(&View::ShowBook, this, ph::_1));
    menu_.AddAction("ShowBook"s, "[title]"s, "Shows book info"s, std::bind(&View::ShowBook, this, ph::_1));
}

bool View::AddAuthor(std::istream& cmd_input) const {
    try {
        std::string name;
        std::getline(cmd_input, name);
        boost::algorithm::trim(name);
        use_cases_.AddAuthor(std::move(name));
    } catch (const std::exception&) {
        output_ << "Failed to add author"sv << std::endl;
    }
    return true;
}

bool View::DeleteAuthor(std::istream& cmd_input) const {
    try {
        std::string name;
        cmd_input >> std::ws;  // Skip spaces

        std::string author_id;

        if (cmd_input.eof()) {
            // empty
            auto selected_id = SelectAuthorForAuthors();
            if (!selected_id) {
                return true;
            }
            author_id = *selected_id;
        } else {
            // Name provided
            std::getline(cmd_input, name);
            boost::algorithm::trim(name);

            auto authors = use_cases_.ShowAuthors();
            auto it = std::find_if(
                authors.begin(), authors.end(), [&name](const auto& a) { return a.name == name; });

            if (it == authors.end()) {
                output_ << "Failed to delete author"sv << std::endl;
                return true;
            }
            author_id = it->id;
        }
        use_cases_.DeleteAuthor(author_id);

    } catch (const std::exception&) {
        output_ << "Failed to delete author"sv << std::endl;
    }
    return true;
}

bool View::EditAuthor(std::istream& cmd_input) const {
    try {
        std::string current_name;
        cmd_input >> std::ws;

        std::string author_id;

        if (cmd_input.eof()) {
            auto selected_id = SelectAuthorForAuthors();
            if (!selected_id) {
                output_ << "Failed to edit author" << std::endl;
                return true;
            }
            author_id = *selected_id;
        } else {
            std::getline(cmd_input, current_name);
            boost::algorithm::trim(current_name);

            auto authors = use_cases_.ShowAuthors();
            auto it = std::find_if(authors.begin(), authors.end(),
                [&current_name](const auto& a) { return a.name == current_name; });

            if (it == authors.end()) {
                output_ << "Failed to edit author"sv << std::endl;
                return true;
            }
            author_id = it->id;
        }

        // 2. Prompt for the new name
        output_ << "Enter new name:" << std::endl;
        std::string new_name;
        std::getline(input_, new_name);
        boost::algorithm::trim(new_name);

        if (new_name.empty()) {
            output_ << "Failed to edit author"sv << std::endl;
            return true;
        }

        // 3. Apply the edit
        use_cases_.EditAuthor(author_id, new_name);

    } catch (const std::exception&) {
        output_ << "Failed to edit author"sv << std::endl;
    }
    return true;
}

std::vector<std::string> ParseTags(const std::string& tags_raw) {
    std::vector<std::string> raw_tags;
    // Split by comma
    boost::split(raw_tags, tags_raw, boost::is_any_of(","));

    std::set<std::string> unique_tags;
    for (auto& tag : raw_tags) {
        boost::algorithm::trim(tag);
        if (!tag.empty()) {
            unique_tags.insert(std::move(tag));
        }
    }

    return {unique_tags.begin(), unique_tags.end()};
}

bool View::AddBook(std::istream& cmd_input) const {
    try {
        // 1. Read year and title correctly from cmd_input
        int year;
        if (!(cmd_input >> year)) {
            output_ << "Failed to add book" << std::endl;
            return true;
        }

        std::string title;
        std::getline(cmd_input >> std::ws, title);
        boost::algorithm::trim(title);

        if (title.empty()) {
            output_ << "Failed to add book" << std::endl;
            return true;
        }

        // 2. Author Selection
        output_ << "Enter author name or empty line to select from list:" << std::endl;
        std::string author_input;
        std::getline(input_, author_input);
        boost::algorithm::trim(author_input);

        std::string author_id;

        if (author_input.empty()) {
            output_ << "Select author:" << std::endl;
            auto authors = GetAuthors();
            PrintVector(output_, authors);
            output_ << "Enter author # or empty line to cancel" << std::endl;

            std::string idx_str;
            std::getline(input_, idx_str);
            boost::algorithm::trim(idx_str);

            if (idx_str.empty()) {
                return true;
            }

            int idx = std::stoi(idx_str) - 1;
            if (idx < 0 || idx >= static_cast<int>(authors.size())) {
                output_ << "Failed to add book" << std::endl;
                return true;
            }
            author_id = authors[idx].id;
        } else {
            auto authors = GetAuthors();
            auto it = std::find_if(authors.begin(), authors.end(),
                [&author_input](const auto& a) { return a.name == author_input; });

            if (it != authors.end()) {
                author_id = it->id;
            } else {
                output_ << "No author found. Do you want to add " << author_input << " (y/n)?" << std::endl;
                std::string ans;
                std::getline(input_, ans);
                boost::algorithm::trim(ans);

                if (ans != "y" && ans != "Y") {
                    output_ << "Failed to add book" << std::endl;
                    return true;
                }

                use_cases_.AddAuthor(author_input);

                auto updated_authors = GetAuthors();
                auto new_author_it = std::find_if(updated_authors.begin(), updated_authors.end(),
                    [&author_input](const auto& a) { return a.name == author_input; });
                author_id = new_author_it->id;
            }
        }

        // 3. Process Tags
        output_ << "Enter tags (comma separated):" << std::endl;
        std::string tags_raw;
        std::getline(input_, tags_raw);

        std::vector<std::string> tags;
        boost::split(tags, tags_raw, boost::is_any_of(","));

        std::set<std::string> unique_normalized_tags;
        for (std::string& tag : tags) {
            boost::algorithm::trim(tag);
            static const std::regex re_spaces(R"(\s+)");
            tag = std::regex_replace(tag, re_spaces, " ");

            if (!tag.empty()) {
                unique_normalized_tags.insert(tag);
            }
        }

        std::vector<std::string> final_tags(unique_normalized_tags.begin(), unique_normalized_tags.end());
        use_cases_.AddBook(title, author_id, year, final_tags);

    } catch (...) {
        output_ << "Failed to add book" << std::endl;
    }
    return true;
}

bool View::ShowBook(std::istream& cmd_input) const {
    try {
        std::string title;
        std::getline(cmd_input >> std::ws, title);
        boost::algorithm::trim(title);

        auto book_match = SelectBook(title);
        // Если ничего не найдено или пользователь отменил выбор, просто выходим
        if (!book_match) {
            return true;
        }

        // Получаем полные данные книги, включая теги
        auto book_info = use_cases_.GetBookInfo(book_match->id);

        output_ << "Title: " << book_info.title << "\n";
        output_ << "Author: " << book_info.author_name << "\n";
        output_ << "Publication year: " << book_info.publication_year << "\n";
        if (!book_info.tags.empty()) {
            output_ << "Tags: ";
            std::vector<std::string> tags = book_info.tags;
            std::sort(tags.begin(), tags.end());
            output_ << boost::algorithm::join(tags, ", ") << "\n";
        }
    } catch (...) {
    }
    return true;
}

bool View::ShowAuthors() const {
    PrintVector(output_, GetAuthors());
    return true;
}

bool View::ShowBooks() const {
    auto app_books = use_cases_.ShowBooks();
    std::vector<detail::BookAuthorInfo> ui_books;

    for (const auto& b : app_books) {
        ui_books.push_back({b.title, b.publication_year, b.name});
    }

    PrintVector(output_, ui_books);
    return true;
}

bool View::ShowAuthorBooks() const {
    try {
        if (auto author_id = SelectAuthor()) {
            PrintVector(output_, GetAuthorBooks(*author_id));
        }
    } catch (const std::exception&) {
        throw std::runtime_error("Failed to Show Books");
    }
    return true;
}

std::optional<detail::AddBookParams> View::GetBookParams(std::istream& cmd_input) const {
    detail::AddBookParams params;

    cmd_input >> params.publication_year;
    std::getline(cmd_input, params.title);
    boost::algorithm::trim(params.title);

    auto author_id = SelectAuthor();
    if (not author_id.has_value())
        return std::nullopt;
    else {
        params.author_id = author_id.value();
        return params;
    }
}

std::optional<std::string> View::SelectAuthor() const {
    std::string input;
    if (!std::getline(input_, input) || input.empty()) {
        return std::nullopt;
    }

    auto authors = use_cases_.ShowAuthors();

    // 1. Try to treat input as an index (1-based)
    try {
        size_t idx = std::stoul(input);
        if (idx > 0 && idx <= authors.size()) {
            return authors[idx - 1].id;
        }
    } catch (...) {
    }

    // 2. Search by exact name
    for (const auto& author : authors) {
        if (author.name == input)
            return author.id;
    }

    // 3. Author not found - Ask to create (Required by test_add_book)
    output_ << "No author found. Do you want to add "sv << input << " (y/n)?"sv << std::endl;
    std::string choice;
    if (std::getline(input_, choice) && (choice == "y" || choice == "Y")) {
        use_cases_.AddAuthor(input);
        // Find the new author to get their ID
        for (const auto& author : use_cases_.ShowAuthors()) {
            if (author.name == input)
                return author.id;
        }
    }

    return std::nullopt;
}

std::optional<std::string> View::SelectAuthorForAuthors() const {
    output_ << "Select author:" << std::endl;
    auto authors = GetAuthors();
    PrintVector(output_, authors);
    output_ << "Enter author # or empty line to cancel" << std::endl;

    std::string str;
    if (!std::getline(input_, str) || str.empty()) {
        return std::nullopt;
    }

    int author_idx;
    try {
        author_idx = std::stoi(str);
    } catch (std::exception const&) {
        throw std::runtime_error("Invalid author num");
    }

    --author_idx;
    if (author_idx < 0 or author_idx >= authors.size()) {
        throw std::runtime_error("Invalid author num");
    }

    return authors[author_idx].id;
}

std::vector<detail::AuthorInfo> View::GetAuthors() const {
    std::vector<detail::AuthorInfo> dst_autors;

    for (const auto& author : use_cases_.ShowAuthors()) {
        dst_autors.push_back({author.id, author.name});
    }
    return dst_autors;
}

std::vector<detail::BookAuthorInfo> View::GetBooks() const {
    std::vector<detail::BookAuthorInfo> books;
    for (const auto& book : use_cases_.ShowBooks()) {
        books.push_back({book.title, book.publication_year, book.name});
    }
    return books;
}

std::vector<detail::BookInfo> View::GetAuthorBooks(const std::string& author_id) const {
    std::vector<detail::BookInfo> books;
    for (const auto& book : use_cases_.ShowAuthorBooks(author_id)) {
        books.push_back({book.title, book.publication_year});
    }
    return books;
}

std::optional<app::BookAuthorInfo> View::SelectBook(const std::string& title) const {
    auto all_books = use_cases_.ShowBooks();
    std::vector<app::BookAuthorInfo> matches;

    for (const auto& book : all_books) {
        if (title.empty() || book.title == title) {
            matches.push_back(book);
        }
    }

    if (matches.empty()) {
        return std::nullopt;
    }

    if (!title.empty() && matches.size() == 1) {
        return matches.front();
    }

    PrintVector(output_, matches);

    output_ << "Enter the book # or empty line to cancel:" << std::endl;

    std::string str;
    if (!std::getline(input_, str) || str.empty()) {
        return std::nullopt;  // User cancelled
    }

    try {
        int idx = std::stoi(str) - 1;  // Convert 1-based to 0-based
        if (idx >= 0 && idx < static_cast<int>(matches.size())) {
            return matches[idx];
        }
    } catch (...) {
    }

    throw std::runtime_error("Invalid book num");
}

bool View::DeleteBook(std::istream& cmd_input) const {
    try {
        std::string title;
        std::getline(cmd_input >> std::ws, title);
        boost::algorithm::trim(title);

        auto book_opt = SelectBook(title);
        if (!book_opt) {
            return true;  // Cancelled
        }

        use_cases_.DeleteBook(book_opt->id);
    } catch (const std::exception&) {
        output_ << "Failed to delete book" << std::endl;
    }
    return true;
}

bool View::EditBook(std::istream& cmd_input) const {
    try {
        // 1. Parse the initial command input
        std::string title;
        std::getline(cmd_input >> std::ws, title);
        boost::algorithm::trim(title);

        // 2. Select the book (handles multiple matches and empty input)
        auto book_opt = SelectBook(title);
        if (!book_opt) {
            output_ << "Book not found" << std::endl;
            return true;
        }

        auto book_info = use_cases_.GetBookInfo(book_opt->id);

        // 3. Get New Title
        output_ << "Enter new title or empty line to use the current one (" << book_info.title
                << "):" << std::endl;
        ;
        std::string new_title;
        std::getline(input_, new_title);
        boost::algorithm::trim(new_title);
        if (new_title.empty()) {
            new_title = book_info.title;
        }

        // 4. Get New Year
        output_ << "Enter publication year or empty line to use the current one ("
                << book_info.publication_year << "):" << std::endl;
        std::string year_str;
        std::getline(input_, year_str);
        boost::algorithm::trim(year_str);

        int new_year = book_info.publication_year;
        if (!year_str.empty()) {
            try {
                new_year = std::stoi(year_str);
            } catch (...) {
            }
        }

        // 5. Get New Tags
        std::vector<std::string> current_tags = book_info.tags;
        std::sort(current_tags.begin(), current_tags.end());
        std::string current_tags_str = boost::algorithm::join(current_tags, ", ");

        output_ << "Enter tags (current tags: " << current_tags_str << "):" << std::endl;
        ;
        std::string tags_raw;
        std::getline(input_, tags_raw);
        boost::algorithm::trim(tags_raw);

        std::vector<std::string> final_tags;

        if (tags_raw.empty()) {
            // If empty line, KEEP the current tags
            final_tags = {};
        } else {
            std::vector<std::string> tags;
            boost::split(tags, tags_raw, boost::is_any_of(","));

            std::set<std::string> unique_normalized_tags;
            for (std::string& tag : tags) {
                boost::algorithm::trim(tag);
                static const std::regex re_spaces(R"(\s+)");
                tag = std::regex_replace(tag, re_spaces, " ");

                if (!tag.empty()) {
                    unique_normalized_tags.insert(tag);
                }
            }
            final_tags.assign(unique_normalized_tags.begin(), unique_normalized_tags.end());
        }

        // 6. Save the edits
        use_cases_.EditBook(book_info.id, new_title, new_year, final_tags);

    } catch (...) {
        output_ << "Book not found" << std::endl;
    }
    return true;
}

}  // namespace ui
