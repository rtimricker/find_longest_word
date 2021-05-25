
#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include <memory>

using namespace std;

using StringVector = vector<string>;
using IntVector = vector<int>;

// ==============================
// m-ary tree
//

struct Node;
using NodePtrVector = vector<Node*>;
using NodePtr = Node*;

struct Node {
    inline Node() : key {0}, EOW {0}, parent {nullptr} {}
    char key;
    bool EOW;
    NodePtrVector child;
    NodePtr parent;
};

inline auto newNode(const char key) -> Node*
{
    auto temp = make_shared<Node*> (new Node);
    (*temp)->key = key;
    return *temp;
}

//struct {
//    inline bool operator() (const string & i, const string & j) { return (i.length() < j.length());}
//} lessthan;
struct {
    inline bool operator() (const string & i, const string & j) { return (i.length() > j.length());}
} greaterthan;

inline static Node * root{nullptr};

//auto mySort = [](const string & lhs, const string & rhs) {
//             return lhs.length() == rhs.length() ?
//                    lhs > rhs : lhs.length() > rhs.length(); };

inline constexpr auto setRoot(Node * _root) -> void
{
    root = _root;
}

inline auto getRoot() -> Node*
{
    return root;
}

//struct {
//    bool operator() (Node * node, char key) { return node->key == key; }
//} myfind;
// Pass key in val, root or parent node of key node, on success a return true and retNode has its node.
inline auto find_char_node (char key, NodePtr node, NodePtr & retNode) -> bool
{
    for  (auto iter : node->child) {
        if (key == iter->key) {
            retNode = iter;
            return true;
        }
    }
    retNode = nullptr;
    return false;
}

inline auto isWord (
                string word, 
                Node * node, 
                Node * & retNode
                ) -> bool
{
    if (node == nullptr) {
        return false;
    }

    size_t len = word.length();
    if (len == 0) {
        return false;
    }

    string wordstr;
    retNode = node;

    wordstr.clear();
    bool retval {false};
    for (auto letter : word) {
        retval = find_char_node (letter, node, retNode);
        if (retval) {
//            wordstr += letter;
            node = retNode;
        } else {
            return false;       // letter in word not there
        }
    }

    // have to test for EOW after the loop; all letter in word accounted for
    if (retval && retNode->EOW) {
        return true;
    }

    return false;
}

inline auto isCompoundWord(
                string word,
                NodePtr node,
                NodePtr & retNode,
                StringVector & retVector
                ) -> bool
{
    if (node == nullptr) {
        return false;
    }

    //cout << "searching for all words in tree: [" << word << "] " << endl;
    size_t len = word.length();
    if (len == 0) {
        return false;
    }

    string wordstr;
    retNode = node;

    retVector.clear();
    //bool retval {false};
    for (auto letter : word) {
        bool retval = find_char_node (letter, node, retNode);
        if (retval) {
            wordstr += letter;
            if (retNode->EOW || wordstr.length() == len - 1) {
                if (word.length() != wordstr.length()) { 
                    retVector.emplace_back(wordstr);
                }
            }
            node = retNode;
        } else {
            return false;
        }
    }

    if (retVector.size()) {
        return true;
    }

    return false;
}

inline auto add_word_to_tree(string word, Node * node) -> void
{

    if (node == nullptr) {
        return;
    }

    Node * retNode{nullptr};
    Node * lastNode{nullptr};
    size_t len = word.length();
    string temp_str;
    for (auto iter : word) {
        if (node != nullptr) {
            retNode = nullptr;
            bool retval = find_char_node (iter, node, retNode);
            if (retval == false) {
                lastNode = newNode(iter);
                lastNode->parent = node;                // new node's parent
                (node->child).emplace_back(lastNode);   // add word
                retNode = nullptr;
                retval = find_char_node (iter, node, retNode);
                if (retval == true) {
                    node = retNode;
                }
            } else {
                temp_str += retNode->key;
                if (temp_str.length() == len) {
                    if (!retNode->EOW) {
                        retNode->EOW = true;    // since adding word within another word
                    }
                    return;
                }
                node = retNode;
            }
        }
    }
    lastNode->EOW = true;
}

class DFS {
public:
    constexpr DFS () {}

    inline auto pre(const Node * node, IntVector * ans) -> const void {
        ans->emplace_back(node->key);
        for (Node * child : node->child) {
            pre(child, ans);
        }
    }

    inline constexpr auto findDepthOfTree(const struct Node *node) -> int
    {
        if (node == NULL) {
            return 0;
        }
        int maxDepth{0};
        for (vector<Node*>::const_iterator c_it = cbegin(node->child); c_it != cend(node->child); ++c_it) {
            maxDepth = max(maxDepth, findDepthOfTree(*c_it));
        }
        return maxDepth + 1;
    }
};

NodePtrVector node_delete;
StringVector v_the_list;

int display_count = 0;

int main()
{
    string line;
    NodePtr retNode = nullptr;

    auto total_lines_read = 0;

    string filename ("wordsforproblem.txt");
    //string filename ("test_case.txt");        // make things work with test case first
    ifstream myfile ( filename );

    // fill vector with the lines from file

    if (myfile.is_open())
    {
        while ( getline (myfile, line) )
        {
            line.erase(line.find_last_not_of(" \n\r\t")+1); // trim white space
	    if (line != "")
	    {
	        v_the_list.emplace_back(line);
                total_lines_read++;
    	    }
	}
        myfile.close();
    } else {
        cout << "myfile NOT open" << endl;
    }
//    queue <char> q;

    root = newNode('/');
    setRoot(root);

    for (auto word : v_the_list) {
        add_word_to_tree(word, root);
    }
    cout << endl;
    retNode = nullptr;

    //string word = "ratcatdogcat";
    //word = "cat";
    //word = "dog";
    //word = "dogc";
    //word = "rat";
    //word = "cats";
    //word = "dogcatsdog";
    //word = "catsdog";
    //word = "catsdogcats";
    //word = "sdogcats";
    //word = "sdog";
    //word = "ratcatdogcat";
    //word = "catsdogcats";
    //word = "catxdogcatsrat";
    //word = "catsx";
    //word = "hippopotamuses";
    //word = "catdogcat";
    //word = "dogcat";
    //word = "dogcats";
    //word = "catx";

// ==========
    StringVector matchVector;

    // this seems to allow for the output to be alphabetic and length sorted
    sort(begin(v_the_list), end(v_the_list), greaterthan);

//StringVector test_list;
//string word = "ethylenediaminetetraacetates";
//test_list.emplace_back(word);
//word = "ethylenediaminetetraacetate";
//test_list.emplace_back(word);
//    for (auto word : test_list)
    for (auto word : v_the_list) 
    {
        StringVector retVector;
        StringVector tempvector;
        StringVector listOfWords;

        listOfWords.emplace_back(word);
        map <string, string> wordMap;
        string lastPrefix;
        string lastRemainder;

        while (listOfWords.size()) {
            for (auto list_word : listOfWords) {
                // get all words in compound word
                //bool retval = isCompoundWord(list_word, getRoot(), retNode, retVector); 
                isCompoundWord(list_word, getRoot(), retNode, retVector); 
                // make sure its a unique string entry vector
                if (retVector.size()) {
                    for (auto prefix : retVector) {
                        //string remainder = list_word.substr(prefix.length(), -1);
                        string remainder(move(list_word.substr(prefix.length(), -1)));
                        if (prefix.length() < 2 || remainder.length() < 2) continue;
                        if (remainder.length() >= 2) {
                            //wordMap.emplace(prefix, remainder);
                            auto it = wordMap.end();
                            it = wordMap.emplace_hint(it, prefix,remainder);
                            bool retval = isWord (remainder, getRoot(), retNode);
                            tempvector.emplace_back(remainder);
                            if (retval == true) {
                                // found compound word
                                lastPrefix = move(prefix);
                                lastRemainder = move(remainder);
                                break;
                            }
                        }
                    }
                }
            }
            listOfWords = move(tempvector);
            tempvector.clear();
        }

        string wordFromList(move(word));

restart: 
        for (auto m_word : wordMap) {
            if ((m_word.first.length() < 2) || (m_word.second.length() < 2)) continue;
            if (lastRemainder.compare(m_word.second) == 0) {
                lastRemainder = m_word.first + m_word.second;   // next Remainder
                if (wordFromList.compare(lastRemainder) == 0) {
//                cout << "MATCH" << endl;
                    matchVector.emplace_back(wordFromList);

                    auto it = wordMap.find(m_word.first);
                    if (it != end(wordMap)) {
                        wordMap.erase(it);
                    }
                    break;
                }
                goto restart;
           }
        }
    }

    cout << "words read in: " << v_the_list.size() << endl;
    cout << "total lines read in: " <<  total_lines_read << endl;
    cout << "Compound words found: " << matchVector.size() << endl;
    size_t matchCnt {0};

    for (auto w : matchVector) {
        cout << "\t[" << w << "]" << endl;
        matchCnt++;
        if (matchCnt >= 10) break;
    }

    DFS dfs;

    int depth = dfs.findDepthOfTree(root);
    cout << endl << "Tree Depth: " << depth << endl;

    if (root) {
        cout << "we have a root node " << endl;
        cout << "size of child vector at root: " << root->child.size() << " " << endl;
    }

    return 0;
}
