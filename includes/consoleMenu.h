#pragma once

#include "osUtils.h"
#include "svUtils.h"
#include "userInput.h"

#include <limits>
#include <algorithm>
#include <functional>
#include <memory>
#include <variant>
#include <optional>
#include <iterator>
#include <tuple>
#include <vector>
#include <span>
#include <stack>
#include <string>
#include <string_view>
#include <iostream>


namespace consoleMenu{
    using svUtils::LineOptions;
    using svUtils::wrapToLength;
    using std::numeric_limits;
    using std::mismatch;
    using std::function;
    using std::bind;
    namespace placeholders = std::placeholders;
    using std::reference_wrapper;
    using std::variant;
    using std::holds_alternative;
    using std::optional;
    using std::make_optional;
    using std::stack;
    using std::prev;
    using std::tuple;
    using std::vector;
    using std::span;
    using std::unique_ptr;
    using std::make_unique;
    using std::string;
    using std::to_string;
    using std::stoull;
    using std::string_view;
    using std::istream;
    using std::ostream;
    using std::cout;
    using namespace std::literals::string_view_literals;
}

namespace consoleMenu {
    static constexpr char SPACECHARACTER = ' ';
    static constexpr string_view SPACESTRING = " ";
    static constexpr short DEFAULT_MAX_LINE_LENGTH = 80;

    struct MenuSettings {
        unsigned short spaceAfterBullet{1};
        unsigned short briefIndentSpaces{0};
        unsigned short detailsIndentSpaces{0};
        unsigned short maxLineLength{ DEFAULT_MAX_LINE_LENGTH };
        bool hidden{ false };
    };

    struct MenuContents {
        string brief{};
        string details{};

        static ostream& addItem(
            ostream& os,
            string_view item,
            size_t indentSpaces,
            size_t maxLineLength,
            string_view bulletString
        ) {
            auto bulletLength = bulletString.length();
            auto indent = string(indentSpaces, SPACECHARACTER);
            os << '\n' << indent;
            
            if (bulletLength > 0){
                os << bulletString;
                indent += string(bulletLength, SPACECHARACTER);
                indentSpaces += bulletLength;
            
            }
            wrapToLength(
                os,
                item,
                {
                    .indent{ indent },
                    .maxLineLength{ maxLineLength },
                    .delimiter{ SPACESTRING },
                    .startingPositionInLine{ indentSpaces }
                }
            );
            return os;
        }
    };

    constexpr string pathString(const span<const unsigned short> path) {
        string pathString{};
        for (const auto& index : path) pathString += to_string(index) + "-";
        if (path.size() > 0) pathString.erase(pathString.length() - 1, 1);
        return pathString;
    }

    class MenuNode {
    public:
        using nodePtrsVector = vector<unique_ptr<MenuNode>>;
        MenuContents contents;
        MenuSettings settings;
        nodePtrsVector children{};

        MenuNode(
            const MenuContents& contents,
            const MenuSettings& settings,
            nodePtrsVector& children
        ) :
            contents{ contents },
            settings{ settings },
            children{ std::move(children) }
        {};

        MenuNode(
            const MenuContents& contents,
            const MenuSettings& settings
        ) :
            contents{ contents },
            settings{ settings }

        {};

        inline void hide() { settings.hidden = true; }
        inline void unhide() { settings.hidden = false; }

        void hideDirectDescendants() {
            for (auto& node : children) {
                node->hide();
            }
        }

        void unhideDirectDescendants() {
            for (auto& node : children) {
                node->unhide();
            }
        }

        void hideAllDescendants() {
            for (auto& node : children) {
                node->hide();
                node->hideAllDescendants();
            }
        }

        void unhideAllDescendants() {
            for (auto& node : children) {
                node->hide();
                node->unhideAllDescendants();
            }
        }


        using optionalNodeRef = optional<reference_wrapper<MenuNode>>;
        using nodePtr = unique_ptr<MenuNode>;

        optionalNodeRef nodeAtRelativePath(span<const unsigned short> relativePath) {
            MenuNode& node = *this;
            if (relativePath.empty()) return { node }; // Return this node if relativePath is empty
            for (const auto& index : relativePath) {
                if (index < node.children.size()) {
                    if (!node.children.at(index)) return {}; // Check for null pointer
                    auto remainingPathLength = relativePath.size() - 1;
                    if (0 == remainingPathLength) return { *(node.children.at(index)) };
                    return node.nodeAtRelativePath(relativePath.last(remainingPathLength));
                }else {
                    return {};
                }
            }
            return {};
        }

       bool isValidPath(span<const unsigned short> path){
           reference_wrapper<MenuNode> currentNode = *this;
            for (const auto& nodeIndex : path) {
                if(path[0] < currentNode.get().children.size()){
                    currentNode = *(currentNode.get().children.at(path[0]));
                    path = path.last(path.size() - 1);
                } else {
                    return false;
                }
            }
            return true;
        }

        void unhideToPath(span<const unsigned short> path) {
            this->unhideDirectDescendants();
            if  (0 == path.size()) return;
            if (path[0] < children.size()) {
                children.at(path[0])->unhideToPath(path.last(path.size() - 1));
            }else {
                throw "Unable to access menu node at path " + pathString(path);
            }
        }

        ostream& addBriefs(
            ostream& os
        ) const {
            int itemNum = 1;
            for (const auto& node : children) {
                auto bulletString =
                    to_string(itemNum++)
                    + "."
                    + string(settings.spaceAfterBullet, SPACECHARACTER);

                if (!node->settings.hidden) {
                    MenuContents::addItem(
                        os,
                        node->contents.brief,
                        node->settings.briefIndentSpaces,
                        node->settings.maxLineLength,
                        bulletString
                    );
                    node->addBriefs(os);
                }
            }
            return os;
        }

    };

    class Menu {

        public:

            vector<unsigned short> currentMenuPath = {}; // Current Node Path from Root
            MenuNode root{
                std::move(MenuContents{{},{}}),
                std::move(
                    MenuSettings {
                            .spaceAfterBullet{1},
                            .briefIndentSpaces{0},
                            .detailsIndentSpaces{0},
                            .maxLineLength{DEFAULT_MAX_LINE_LENGTH},
                            .hidden{true}
                        }
                    )
                };


            ostream& getMenuFromRootPath(
                ostream& os, 
                span<const unsigned short> path
            ) {
                root.hideAllDescendants();
                root.unhideToPath(path);
                root.addBriefs(os);
                //os << '\n' << userPrompt();
                return os;
            }
            
            struct RelativePath {
                span<const unsigned short> commonPath;
                span<const unsigned short> remainingPath;
            };

            RelativePath calculateRelativePath (
                span<const unsigned short> currentPath,
                span<const unsigned short> finalPath
            ){
                auto [currIt, _] = 
                    mismatch(
                        currentPath.begin(),
                        currentPath.end(),
                        finalPath.begin(),
                        finalPath.end()
                    );
                
                span<const unsigned short> commonPath{currentPath.begin(), currIt };

                auto [finalIt, commonIt] =
                    mismatch(
                        finalPath.begin(),
                        finalPath.end(),
                        commonPath.begin(),
                        commonPath.end()
                    );


                // Common path is at root
                if (finalIt == finalPath.begin()) {
                    return {
                        {}, // Common path is root
                        finalPath // remainingPath is final path
                    };
                }
                
                // Common path is final path
                if (finalIt == finalPath.end()) {
                    return {
                        finalPath, // commonPath is final path
                        {} // remainingPath is Empty
                    };
                }

                // Get the remaining path
                return {
                    {commonPath.begin(), commonIt}, // CommonPath
                    { finalIt, finalPath.end()}
                };
            }
 
            ostream& changeMenu(
                ostream & os,
                span<const unsigned short> currentPath,
                span<const unsigned short> finalPath
            ) {
                // Find and Verify Common Node and Final Node
                auto [commonNodePath, remainingPath] = calculateRelativePath(currentPath, finalPath);
                auto maybeCommonNode = root.nodeAtRelativePath(commonNodePath);
                if (!maybeCommonNode) return os;
                auto& commonNode = maybeCommonNode.value().get();
                
                auto maybeFinalNode = commonNode.nodeAtRelativePath(remainingPath);
                if (!maybeFinalNode) return os;
                auto& finalNode = maybeFinalNode.value().get();

                commonNode.hideAllDescendants();
                commonNode.unhideToPath(remainingPath);
                root.addBriefs(os);
                //os << '\n' << userPrompt();
                return os;
            }

            using optionalNodeRef = optional<reference_wrapper<MenuNode>>;
            optionalNodeRef addChildNodeAtPath(
                span<const unsigned short> path,
                const MenuContents& contents,
                const MenuSettings& settings = MenuSettings{}
            ) {
                auto maybeNode = root.nodeAtRelativePath(path);
                if (!maybeNode) return {};
                auto& node = maybeNode.value().get();
                
                // Check if the size has reached maxximum
                if (node.children.size() >= numeric_limits<const unsigned short>::max()) {
                    throw "Cannot add child node because maximum number of children was reached for parent node";
                }

                node.children.emplace_back(
                    make_unique<MenuNode>(
                        contents,
                        settings
                    )
                );
                if(!node.children.back()) return{};
                return {*(node.children.back())};
            }

            string_view userPrompt(){
                return "\nSelect a Valid Option (or enter 'b' to go back a level; 'q' to quit):"sv;
            };

            string_view userOptionInvalid(){
                return "\nThe option provided was invalid."sv;
            };

            string_view userOptionError(){
                return "\nNo valid option was provided.\n"sv;
            };
            
            unsigned short optionToNodeIndex(const unsigned short option) {
                return option-1;
            };

            optional<variant<char, unsigned short>>
            getValidUserOption(
                istream& is,
                ostream& os
            ) {
               
                auto printSV = [this](ostream& os, const string_view sv) {
                    os << sv;
                };

                auto printPromptFunction = bind(function(printSV), placeholders::_1, userPrompt());
                auto printInvalidInputMessage = bind(function(printSV), placeholders::_1, userOptionInvalid());
                auto printErrorMessage = bind(function(printSV), placeholders::_1, userPrompt());

                auto isValidOptionInput =
                    [](string_view userInput) -> bool {
                    if (
                        userInput.length() == 1 &&
                        userInput.at(0) == 'b' || userInput.at(0) == 'q'
                        ) return true;

                    using ioUtils::IntegerString;
                    if (
                        IntegerString::isInteger(userInput) &&
                        !IntegerString::isNegative(userInput) &&
                        stoull(string(userInput)) < static_cast<unsigned long long>(numeric_limits<const unsigned short>::max())
                        ) return true;

                    return false;
                };

                auto stringToOption =
                    [](string_view userInput) -> variant<char, unsigned short> {
                    if (userInput.length() == 1 && userInput.at(0) == 'b') return { 'b' };
                    if (userInput.length() == 1 && userInput.at(0) == 'q') return { 'q' };
                    return static_cast<unsigned short>(stoull(string(userInput)));
                };

                auto isValidOptionOutput =
                    [this](const variant<char, unsigned short>& userOption) -> bool {
                    if (holds_alternative<char>(userOption)) {
                        auto charOpt = get<char>(userOption);
                        if (charOpt == 'b' || charOpt == 'q') return true;
                    }
                    else if (holds_alternative<unsigned short>(userOption)) {
                        auto numOpt = get<unsigned short>(userOption);
                        const auto& currentNode = this->root.nodeAtRelativePath(currentMenuPath).value().get();
                        if ( numOpt > 0 && numOpt <= currentNode.children.size()) return true;
                    }
                    return false;
                };

                return
                    ioUtils::getValidInput(
                        printPromptFunction,
                        printInvalidInputMessage,
                        printErrorMessage,
                        function(isValidOptionInput),
                        {},
                        function(stringToOption),
                        {},
                        function(isValidOptionOutput),
                        {},
                        is,
                        os
                    );
            }


            void displayMenu(istream &is, ostream& os) {
                
                bool exit = false;
                auto clearScreenIfStdOut = [&os]() {
                    if (os.rdbuf() == cout.rdbuf()) {
                        osUtils::clearScreen();
                    }
                };

                clearScreenIfStdOut();
                getMenuFromRootPath(os,{});

                while(!exit){
                    cout << "\ncurrentPath=" << pathString(currentMenuPath);
                    auto userInput = getValidUserOption(is, os);
                    
                    if (!userInput.has_value()) {
                        os << userOptionError();
                        return; 
                    }

                    auto &userOption = userInput.value();
                    span currentPathSpan = currentMenuPath ;
                    auto currentPathLength = currentPathSpan.size();
                    
                    if (holds_alternative<char>(userOption)) {
                        char charOption = get<char>(userOption);
                        if (charOption == 'q') {
                            exit = true;
                            break;
                        }else if (charOption == 'b') {
                            // Calculate New Path
                            if (0 == currentPathLength) {
                                os << "\n This is the top level menu. Cannot go back\n";
                                continue;
                            }
                            auto newPathSpan = currentPathSpan.first(currentPathLength - 1);

                            // Print Menu
                            clearScreenIfStdOut();
                            changeMenu(os, currentPathSpan, newPathSpan);

                            // Update Path
                            auto lastPathIterator = prev(currentMenuPath.end());
                            currentMenuPath.erase(lastPathIterator);
                        }
                    }else if (holds_alternative<unsigned short>(userOption)) {
                        // Update Path
                        auto selectedNodeIndex = optionToNodeIndex(get<unsigned short >(userOption));
                        currentMenuPath.emplace_back(selectedNodeIndex);

                        // Print Menu
                        clearScreenIfStdOut();
                        changeMenu(os, currentPathSpan, currentMenuPath);

                    }else {
                        os << userOptionError();
                        return;
                    }
                }
            }
    };

    Menu& getMenu() {
        static Menu mainMenu;
        return mainMenu;
    }
                 

   
}