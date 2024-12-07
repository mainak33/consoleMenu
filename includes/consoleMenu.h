#pragma once

#include "svUtils.h"

#include <algorithm>
#include <memory>
#include <optional>
#include <iterator>
#include <vector>
#include <span>
#include <stack>
#include <string>
#include <string_view>
#include <iostream>


namespace consoleMenu{
    using svUtils::LineOptions;
    using svUtils::wrapToLength;
    using std::mismatch;
    using std::reference_wrapper;
    using std::optional;
    using std::make_optional;
    using std::stack;
    using std::prev;
    using std::vector;
    using std::span;
    using std::unique_ptr;
    using std::make_unique;
    using std::string;
    using std::to_string;
    using std::string_view;
    using std::ostream;
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
            if (relativePath.empty()) return { node }; // Return this node if relativePath is epty
            for (const auto& index : relativePath) {
                if (index < node.children.size()) {
                    if (!node.children.at(index)) return {}; // Check for null pointer
                    auto remainingPathLength = relativePath.size() - 1;
                    if (0 == remainingPathLength) return { *(node.children.at(index)) };
                    return node.nodeAtRelativePath(relativePath.last(remainingPathLength));
                }
                else {
                    return {};
                }
            }
            return {};
        }

        void unhideToPath(span<const unsigned short> path) {
            if  (0 == path.size()) return;
            if (path[0] < children.size()) {
                this->unhideDirectDescendants();
                children[path[0]]->unhideToPath(path.last(path.size() - 1));
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

            using nodePtrsVector = vector<unique_ptr<MenuNode>>;
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

            string_view userPrompt(){
                return "Select Option (enter b to go back a level):"sv;
            };

            ostream& getMenuFromRootPath(
                ostream& os, 
                span<const unsigned short> path
            ) {
                root.hideAllDescendants();
                root.unhideToPath(path);
                root.addBriefs(os);
                os << '\n' << userPrompt();
                return os;
            }
            
            
            struct RelativePath {
                span<const unsigned short> commonPath;
                span<const unsigned short> remainingPath;
            };

            RelativePath relativePath (
                span<const unsigned short> currentPath,
                span<const unsigned short> finalPath
            ){
                auto [currIt, _] = 
                    mismatch(
                        currentPath.begin(),
                        currentPath.end(),
                        finalPath.begin()
                    );
                
                span<const unsigned short> commonPath{currentPath.begin(), currIt };

                auto [finalIt, commonIt] =
                    mismatch(
                        finalPath.begin(),
                        finalPath.end(),
                        commonPath.begin()
                    );

                // Common path is at root
                if (finalIt == finalPath.begin()) {
                    return {
                        {}, // Common path is root
                        finalPath // remainingPath is final path
                    };
                }
                
                // Get the common path
                return {
                    {commonPath.begin(), commonIt}, // CommonPath
                    { prev(finalIt), finalPath.end()}
                };
            }
 
            ostream& changeMenu(
                ostream & os,
                span<const unsigned short> currentPath,
                span<const unsigned short> finalPath
            ) {
                // Find and Verify Common Node and Final Node
                auto [commonNodePath, remainingPath] = relativePath(currentPath, finalPath);
                auto maybeCommonNode = root.nodeAtRelativePath(commonNodePath);
                if (!maybeCommonNode) return os;
                auto& commonNode = maybeCommonNode.value().get();
                
                auto maybeFinalNode = commonNode.nodeAtRelativePath(remainingPath);
                if (!maybeFinalNode) return os;
                auto& finalNode = maybeFinalNode.value().get();

                commonNode.hideAllDescendants();
                commonNode.unhideToPath(remainingPath);
                return root.addBriefs(os);
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
                node.children.emplace_back(
                    make_unique<MenuNode>(
                        contents,
                        settings
                    )
                );
                if(!node.children.back()) return{};
                return {*(node.children.back())};
            }
    };

    Menu& getMenu() {
        static Menu mainMenu;
        return mainMenu;
    }
                 

   
}