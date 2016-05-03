#ifdef NDEBUG
#undef NDEBUG//at now assert will work even in Release build
#endif
#include <cassert>
#include <string>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include "json11.hpp"
#include <list>
#include <set>
#include <unordered_map>
#include <algorithm>

using namespace json11;
using std::string;

// Check that Json has the properties we want.
#include <type_traits>
#define CHECK_TRAIT(x) static_assert(std::x::value, #x)
CHECK_TRAIT(is_nothrow_constructible<Json>);
CHECK_TRAIT(is_nothrow_default_constructible<Json>);
CHECK_TRAIT(is_copy_constructible<Json>);
CHECK_TRAIT(is_nothrow_move_constructible<Json>);
CHECK_TRAIT(is_copy_assignable<Json>);
CHECK_TRAIT(is_nothrow_move_assignable<Json>);
CHECK_TRAIT(is_nothrow_destructible<Json>);

void parse_from_stdin() {
    string buf;
    string line;
    while (std::getline(std::cin, line)) {
        buf += line + "\n";
    }

    string err;
    auto json = Json::parse(buf, err);
    if (!err.empty()) {
        printf("Failed: %s\n", err.c_str());
    } else {
        printf("Result: %s\n", json.dump().c_str());
    }
}

int main(int argc, char **argv) {
    if (argc == 2 && argv[1] == string("--stdin")) {
        parse_from_stdin();
        return 0;
    }

    const string simple_test =
        R"({"k1":"v1", "k2":42, "k3":["a",123,true,false,null]})";

    string err;
    auto json = Json::parse(simple_test, err);

    std::cout << "k1: " << json["k1"].string_value() << "\n";
    std::cout << "k3: " << json["k3"].dump() << "\n";

    for (auto &k : json["k3"].array_items()) {
        std::cout << "    - " << k.dump() << "\n";
    }

    const string comment_test = R"({
      // comment /* with nested comment */
      "a": 1,
      // comment
      // continued
      "b": "text",
      /* multi
         line
         comment */
      // and single-line comment
      "c": [1, 2, 3]
    })";

    string err_comment;
    auto json_comment = Json::parse(
      comment_test, err_comment, JsonParse::COMMENTS);
    if (!err_comment.empty()) {
        printf("Failed: %s\n", err_comment.c_str());
    } else {
        printf("Result: %s\n", json_comment.dump().c_str());
    }

    string failing_comment_test = R"({
      /* bad comment
      "a": 1,
    })";

    string err_failing_comment;
    auto json_failing_comment = Json::parse(
      failing_comment_test, err_failing_comment, JsonParse::COMMENTS);
    if (!err_failing_comment.empty()) {
        printf("Failed: %s\n", err_failing_comment.c_str());
    } else {
        printf("Result: %s\n", json_failing_comment.dump().c_str());
    }

    failing_comment_test = R"({
      / / bad comment })";

    json_failing_comment = Json::parse(
      failing_comment_test, err_failing_comment, JsonParse::COMMENTS);
    if (!err_failing_comment.empty()) {
        printf("Failed: %s\n", err_failing_comment.c_str());
    } else {
        printf("Result: %s\n", json_failing_comment.dump().c_str());
    }

    failing_comment_test = R"({// bad comment })";

    json_failing_comment = Json::parse(
      failing_comment_test, err_failing_comment, JsonParse::COMMENTS);
    if (!err_failing_comment.empty()) {
        printf("Failed: %s\n", err_failing_comment.c_str());
    } else {
        printf("Result: %s\n", json_failing_comment.dump().c_str());
    }

    failing_comment_test = R"({
          "a": 1
        }/)";

    json_failing_comment = Json::parse(
      failing_comment_test, err_failing_comment, JsonParse::COMMENTS);
    if (!err_failing_comment.empty()) {
        printf("Failed: %s\n", err_failing_comment.c_str());
    } else {
        printf("Result: %s\n", json_failing_comment.dump().c_str());
    }

    failing_comment_test = R"({/* bad
                                  comment *})";

    json_failing_comment = Json::parse(
      failing_comment_test, err_failing_comment, JsonParse::COMMENTS);
    if (!err_failing_comment.empty()) {
        printf("Failed: %s\n", err_failing_comment.c_str());
    } else {
        printf("Result: %s\n", json_failing_comment.dump().c_str());
    }

    std::list<int> l1 { 1, 2, 3 };
    std::vector<int> l2 { 1, 2, 3 };
    std::set<int> l3 { 1, 2, 3 };
    assert(Json(l1) == Json(l2));
    assert(Json(l2) == Json(l3));

    std::map<string, string> m1 { { "k1", "v1" }, { "k2", "v2" } };
    std::unordered_map<string, string> m2 { { "k1", "v1" }, { "k2", "v2" } };
    assert(Json(m1) == Json(m2));

    // Json literals
    Json obj = Json::object({
        { "k1", "v1" },
        { "k2", 42.0 },
        { "k3", Json::array({ "a", 123.0, true, false, nullptr }) },
    });

    std::cout << "obj: " << obj.dump() << "\n";

    assert(Json("a").number_value() == 0);
    assert(Json("a").string_value() == "a");
    assert(Json().number_value() == 0);

    assert(obj == json);
    assert(Json(42) == Json(42.0));
    assert(Json(42) != Json(42.1));

    const string unicode_escape_test =
        R"([ "blah\ud83d\udca9blah\ud83dblah\udca9blah\u0000blah\u1234" ])";

    const char utf8[] = "blah" "\xf0\x9f\x92\xa9" "blah" "\xed\xa0\xbd" "blah"
                        "\xed\xb2\xa9" "blah" "\0" "blah" "\xe1\x88\xb4";

    Json uni = Json::parse(unicode_escape_test, err);
    assert(uni[0].string_value().size() == (sizeof utf8) - 1);
    assert(std::memcmp(uni[0].string_value().data(), utf8, sizeof utf8) == 0);

    // Demonstrates the behavior change in Xcode 7 / Clang 3.7 described
    // here: https://llvm.org/bugs/show_bug.cgi?id=23812
    Json nested_array = Json::array { Json::array { 1, 2, 3 } };
    assert(nested_array.is_array());
    assert(nested_array.array_items().size() == 1);
    assert(nested_array.array_items()[0].is_array());
    assert(nested_array.array_items()[0].array_items().size() == 3);

    {
        const std::string good_json = R"( {"k1" : "v1"})";
        const std::string bad_json1 = good_json + " {";
        const std::string bad_json2 = good_json + R"({"k2":"v2", "k3":[)";
        struct TestMultiParse {
            std::string input;
            std::string::size_type expect_parser_stop_pos;
            size_t expect_not_empty_elms_count;
            Json expect_parse_res;
        } tests[] = {
            {" {", 0, 0, {}},
            {good_json, good_json.size(), 1, Json(std::map<string, string>{ { "k1", "v1" } })},
            {bad_json1, good_json.size() + 1, 1, Json(std::map<string, string>{ { "k1", "v1" } })},
            {bad_json2, good_json.size(), 1, Json(std::map<string, string>{ { "k1", "v1" } })},
            {"{}", 2, 1, Json::object{}},
        };
        for (const auto &tst : tests) {
            std::string::size_type parser_stop_pos;
            std::string err;
            auto res = Json::parse_multi(tst.input, parser_stop_pos, err);
            assert(parser_stop_pos == tst.expect_parser_stop_pos);
            assert(
                std::count_if(res.begin(), res.end(),
                              [](const Json& j) { return !j.is_null(); })
                == tst.expect_not_empty_elms_count);
            if (!res.empty()) {
                assert(tst.expect_parse_res == res[0]);
            }
        }
    }
    Json my_json = Json::object {
        { "key1", "value1" },
        { "key2", false },
        { "key3", Json::array { 1, 2, 3 } },
    };
    std::string json_str = my_json.dump();
    printf("%s\n", json_str.c_str());

    class Point {
    public:
        int x;
        int y;
        Point (int x, int y) : x(x), y(y) {}
        Json to_json() const { return Json::array { x, y }; }
    };

    std::vector<Point> points = { { 1, 2 }, { 10, 20 }, { 100, 200 } };
    std::string points_json = Json(points).dump();
    printf("%s\n", points_json.c_str());
}
