#pragma once

#include "svUtils.h"

#include <memory>
#include <optional>
#include <vector>
#include <span>
#include <stack>
#include <string>
#include <string_view>
#include <iostream>


namespace consoleMenu{
    using svUtils::LineOptions;
    using svUtils::wrapToLength;
    using std::reference_wrapper;
    using std::ref;
    using std::optional;
    using std::make_optional;
    using std::stack;
    using std::vector;
    using std::span;
    using std::unique_ptr;
    using std::make_unique;
    using std::string;
    using std::to_string;
    using std::string_view;
    using std::ostream;
}

namespace consoleMenu {
    const auto& SPACECHARACTER = ' ';
    const auto& SPACESTRING = " ";

    struct MenuSettings {
        unsigned short spaceAfterBullet{1};
        unsigned short briefIndentSpaces{0};
        unsigned short detailsIndentSpaces{0};
        unsigned short maxLineLength{80};
        bool hidden{false};
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
            wrapToLength(
                os,
                bulletString,
                {
                    .indent{ indent },
                    .maxLineLength{ maxLineLength },
                    .delimiter{SPACESTRING},
                    .startingPositionInLine{indentSpaces}
                }
            );

            if (bulletLength > 0) 
                indent += string(bulletLength, SPACECHARACTER);
            
            wrapToLength(
                os,
                item,
                {
                    .indent{ indent },
                    .maxLineLength{ maxLineLength },
                    .delimiter{ SPACESTRING },
                    .startingPositionInLine{ bulletLength }
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
            ):
            contents{contents},
            settings{settings},
            children{std::move(children)}
            {};

            MenuNode(
                const MenuContents& contents,
                const MenuSettings& settings
            ) :
                contents{ contents },
                settings{ settings }
                
            {};

            inline void hide(){ settings.hidden = true;}
            inline void unhide(){ settings.hidden = false;}

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

            void hideAllDescendants(){ 
                for (auto& node : children) {
                    node->hideAllDescendants();
                }
            }

            void unhideAllDescendants(){ 
                for (auto& node : children) {
                    node->unhideAllDescendants();
                }
            }


            using optionalNodeRef = optional<reference_wrapper<MenuNode>>;
            using nodePtr = unique_ptr<MenuNode>;

            optionalNodeRef getMenuNodebyPath(span<unsigned short> path) {
                MenuNode& node = *this;
                for (const auto& index : path) {
                    if (index < node.children.size()) {
                        if(!node.children.at(index)) return {}; // Check for null pointer
                        auto remainingPathLength = path.size() - 1;
                        if(0 == remainingPathLength) return { *(node.children.at(index)) };
                        return node.getMenuNodebyPath(path.last(remainingPathLength));
                    }else{
                        return {};
                    }
                }
                return {};
            }

            void unhideToPath(span<unsigned short> path) {
                this->unhideDirectDescendants();
                auto remainingPathLength = path.size() - 1;
                if (0 == remainingPathLength) return;
                unhideToPath(path.last(remainingPathLength));
            }

            ostream& addBriefs(
                ostream& os
            ) const{
                int itemNum = 1;
                for (const auto& node : children) {
                    auto bullet = 
                        to_string(itemNum++) + 
                        string(settings.spaceAfterBullet,SPACECHARACTER);
                    
                    if(!node->settings.hidden){
                        MenuContents::addItem(
                            os,
                            node->contents.brief,
                            settings.briefIndentSpaces,
                            settings.maxLineLength,
                            bullet
                        );
                        node->addBriefs(os);
                    }
                }
                return os;
            }

    };

    class Menu{
        public :
            using nodePtrsVector = vector<unique_ptr<MenuNode>>;
            MenuNode root{
                std::move(MenuContents{{},{}}),
                std::move(
                    MenuSettings {
                        .spaceAfterBullet{1},
                        .briefIndentSpaces{0},
                        .detailsIndentSpaces{0},
                        .maxLineLength{80},
                        .hidden{true}
                    }
                )
            };

            ostream& getMenuTillPath(
                ostream& os, 
                span<unsigned short> path
            ) {
                root.hideAllDescendants();
                root.unhideToPath(path);
                return root.addBriefs(os);
                return os;
            }

            using optionalNodeRef = optional<reference_wrapper<MenuNode>>;
            optionalNodeRef addChildNodeAtPath(
                span<unsigned short> path,
                const MenuContents& contents,
                const MenuSettings& settings,
                nodePtrsVector& children
            ) {
                auto maybeNode = root.getMenuNodebyPath(path);
                if (!maybeNode) return {};
                auto& node = maybeNode.value().get();
                node.children.emplace_back(
                    make_unique<MenuNode>(
                        contents,
                        settings,
                        children
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