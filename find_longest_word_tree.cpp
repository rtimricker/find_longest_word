//
// g++  -Wall -O3 -std=c++20 -O3 -o find_longest_word_tree find_longest_word_tree.cpp
//
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

// ==============================
// m-ary tree
//

struct Node;

struct Node {
    explicit inline Node(char ch) : key {ch}, EOW {0}, parent {nullptr} {}
    inline Node() : key {0}, EOW {0}, parent {nullptr} {}
    char key;
    bool EOW;
    Node * parent;
    vector<Node*> child;
    ~Node () {}
};

#if 0
inline auto newNode = [] (const char key) {
    shared_ptr<Node*> temp = make_shared<Node*>(new Node(key));
    return *temp;
};
#else
inline auto newNode = [] (const char key) {
//    auto p = new Node(key);
//    return p;
    return new Node(key);
};
#endif

inline static Node * root {nullptr};

inline auto greaterthan = [](const string & i, const string & j) { return (i.length() > j.length());};

inline constexpr auto setRoot(Node * _root) -> void
{
    root = _root;
}

inline auto getRoot() -> Node *
{
    return root;
}

// Pass key in val, root or parent node of key node, on success a return true and retNode has its node.
inline auto find_char_node (const char key, const Node * node, Node * & retNode) -> bool
{
    retNode = nullptr;
    for  (auto & iter : node->child) {
        auto val = (*iter).key;
        if (key == val) {
            retNode = iter;
            return true;
        }
    }
    retNode = nullptr;
    return false;
}

inline auto isWord (
                const string & word, 
                Node * node
                ) -> bool
{
    if (node == nullptr || word.length() == 0) {
        return false;
    }

    Node * retNode {nullptr};

    bool retval {false};
    for (auto & letter : word) {
        retval = find_char_node (letter, node, retNode);
        if (retval) {
            node = retNode;
        } else {
            break;
        }
    }

    // have to test for EOW after the loop; all letters in word accounted for
    if (retval && retNode->EOW) {
        return true;
    }

    return false;
}

inline auto isCompoundWord(
                const string & word,
                Node * node,
                vector<string> & retVector
                ) -> bool
{
    if (node == nullptr || word.length() == 0) {
        return false;
    }

    string wordstr;
    retVector.clear();
    Node * retNode;

    for (auto & letter : word) {
        if (find_char_node (letter, node, retNode)) {
            wordstr += letter;
            if (retNode->EOW || wordstr.length() == word.length() - 1) {
                retVector.emplace_back(wordstr);
            }
            node = retNode;
        } else {
            break;
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

    Node * retNode {nullptr};
    Node * lastNode {nullptr};
    string temp_str;
    for (auto & letter : word) {
        if (node != nullptr) {
            bool retval = find_char_node (letter, node, retNode);
            if (retval == false) {
                lastNode = newNode(letter);
                lastNode->parent = node;                // new node's parent
                (node->child).emplace_back(lastNode);   // add word
                retval = find_char_node (letter, node, retNode);        // confirm new letter is there
                if (retval == true) {
                    node = retNode;
                }
            } else {
                temp_str += retNode->key;
                if (temp_str.length() == word.length()) {
                    if (!retNode->EOW) {
                        retNode->EOW = true;    // since adding word within another word, mark
                    }
                    return;
                }
                node = retNode;
            }
        }
    }
    lastNode->EOW = true;               // mark
}

class DFS {
public:
    constexpr DFS () {}

//    inline auto pre(const Node * node, vector<int> * ans) -> const void {
//        ans->emplace_back(node->key);
//        for (Node * child : node->child) {
//            pre(child, ans);
//        }
//    }

    //inline constexpr auto findDepthOfTree(const struct Node *node) -> int
    inline auto findDepthOfTree(const Node * node) -> int
    {
        if (node == NULL) {
            return 0;
        }
        int maxDepth {0};
        //for (vector<Node*>::const_iterator c_it = cbegin(node->child); c_it != cend(node->child); ++c_it) {
        for (auto & it : node->child) {
            //maxDepth = max(maxDepth, findDepthOfTree(*c_it));
            maxDepth = max(maxDepth, findDepthOfTree(it));
        }
        return maxDepth + 1;
    }
};

vector<Node*> node_delete;

// Prints the m-ary tree level wise
auto _LevelOrderTraversal(Node * root) -> void
{
    if (root == nullptr) {
        return;
    }

//    cout << "_LevelOrderTraversal()\n";

    // Standard level order traversal code
    // using queue
    queue<Node *> q; // Create a queue
    q.push(root); // Enqueue root
    node_delete.emplace_back(root);
    while (!q.empty()) {
        int n = q.size();

        // If this node has children
        while (n > 0) {
            // Dequeue an item from queue and print it
            Node * p = q.front();
            q.pop();
//            cout << "[" << (int)p->key << ":" << p->key << "]";

            // Enqueue all children of the dequeued item
            for (size_t i=0; i < p->child.size(); ++i) {
                q.push(p->child[i]);
                node_delete.emplace_back(p->child[i]);
            }

            n--;
        }
    }
    // correct a memory leak
    for(auto n : node_delete) {
        if (n) {
            delete n;
            n = nullptr;
        }
    }
}

vector<string> v_the_list;

int display_count = 0;

int main()
{
    string line;
    auto total_lines_read = 0;

    string filename ("wordsforproblem.txt");
    //string filename ("test_case.txt");        // make things work with test case first
    ifstream myfile ( filename );

    // fill vector with the lines from file

    if (myfile.is_open()) {
        while (getline (myfile, line)) {
            line.erase(line.find_last_not_of(" \n\r\t")+1); // trim white space
	    if (line != "") {
	        v_the_list.emplace_back(line);
                total_lines_read++;
    	    }
	}
        myfile.close();
    } else {
        cout << "myfile NOT open\n";
        return 0;
    }

    root = newNode('/');
    setRoot(root);

    for (auto & word : v_the_list) {
        add_word_to_tree(word, root);
    }
    cout << "\n";

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
    vector<string> matchVector;

    // this seems to allow for the output to be alphabetic and length sorted
    sort(begin(v_the_list), end(v_the_list), greaterthan);

//vector<string> test_list;
//string word = "ethylenediaminetetraacetates";
//test_list.emplace_back(word);
//word = "ethylenediaminetetraacetate";
//test_list.emplace_back(word);
//    for (auto word : test_list)
    for (auto & word : v_the_list) 
    {
        vector<string> retVector;
        vector<string> listOfWords;

        listOfWords.emplace_back(word);
        map <string, string> wordMap;
        string lastPrefix;
        string lastRemainder;

        while (listOfWords.size()) {
            vector<string> tempvector;
            for (auto & list_word : listOfWords) {
                // get all words in compound word
                bool retval = isCompoundWord(list_word, getRoot(), retVector); 
                if (retval && retVector.size()) {
                    for (auto prefix : retVector) {
                        string remainder = list_word.substr(prefix.length(), -1);
                        if (prefix.length() < 2 || remainder.length() < 2) continue;
                        auto it = end(wordMap);
                        it = wordMap.emplace_hint(it, prefix, remainder);
                        retval = isWord (remainder, getRoot());
                        tempvector.emplace_back(remainder);
                        if (retval == true) {
                            // found compound word
                            lastPrefix = prefix;
                            lastRemainder = remainder;
                            break;
                        }
                    }
                }
            }
            listOfWords = tempvector;
        }
        string wordFromList(word);

restart: 
        for (auto & m_word : wordMap) {
            if ((m_word.first.length() < 2) || (m_word.second.length() < 2)) continue;
            if (lastRemainder.compare(m_word.second) != 0) continue;
            lastRemainder = m_word.first + m_word.second;   // next Remainder
            if (wordFromList.compare(lastRemainder) == 0) {
                //cout << "MATCH" << endl;
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

    cout << "words read in: " << v_the_list.size() << "\n";
    cout << "total lines read in: " <<  total_lines_read << "\n";
    cout << "Compound words found: " << matchVector.size() << "\n";
    size_t matchCnt {0};

    for (auto & w : matchVector) {
        cout << "\t[" << w << "]\n";
        matchCnt++;
        if (matchCnt >= 10) break;
    }

    DFS dfs;

    int depth = dfs.findDepthOfTree(root);
    cout << "\nTree Depth: " << depth << "\n";

    if (root) {
        cout << "we have a root node\n";
        cout << "size of child vector at root: " << root->child.size() << " \n";
    }

    cout << "delete all Nodes\n";
    _LevelOrderTraversal(root);

    return 0;
}

