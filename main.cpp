#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <optional>
#include <variant>

#define V_TYPE(v, t) std::holds_alternative<t>(v)

template <typename K, typename V, int N = 5> class BTree {
  static_assert(N > 2);

public:
  BTree() : root(LeafNode()) {}

  V *find(const K &key) { return find_in_node(root, key); }

  V *traverse(const K &key) {
    if (head_ == nullptr)
      return nullptr;
    auto *node = head_;
    while (node != nullptr) {
      for (auto i = 0; i < node->count_; ++i) {
        if (node->pairs_[i].first == key) {
          return &node->pairs_[i].second;
        }
      }
      node = node->next_;
    }
    return nullptr;
  }

  void insert(const K &key, const V &value);
  void erase(const K &key) {}

private:
  struct InternalNode;
  constexpr static size_t MAX_KEY = N - 1;
  constexpr static size_t MIN_KEY = std::ceil(N / 2) - 1;
  constexpr static size_t MAX_CHILD = N;
  constexpr static size_t MIN_CHILD = std::ceil(N / 2);

  struct LeafNode {
    std::array<std::pair<K, V>, N> pairs_;
    size_t count_ = 0;
    LeafNode *next_ = nullptr;

    struct SplitResult {
      K key;
      LeafNode node;
    };

    std::optional<SplitResult> insert(const K &key, const V &value) {
      if (need_split_if_insert()) {
        return insert_and_split(key, value);
      }
      insert_not_split(key, value);
      return std::nullopt;
    };

  private:
    bool need_split_if_insert() const {
      if (count_ + 1 >= MAX_CHILD) {
        return true;
      }
      return false;
    }

    void insert_not_split(const K &key, const V &value) {
      for (auto i = 0; i < count_; i++) {
        if (key < pairs_[i].first) {
        }
      }
      // TODO::
    }

    SplitResult insert_and_split(const K &key, const V &value) {
      // TODO::
      return {};
    }
  };
  using Node = std::variant<LeafNode, std::unique_ptr<InternalNode>>;

  struct InternalNode {
    std::array<K, N> keys_;
    std::array<Node, N + 1> children_;
    size_t count_ = 0;

    InternalNode(const K &key, Node left, Node right) {
      keys_[0] = key;
      children_[0] = std::move(left);
      children_[1] = std::move(right);
    }

    struct SplitResult {
      K key;
      std::unique_ptr<InternalNode> node;
    };

    std::optional<SplitResult> insert(const K &key, const V &value) {
      const auto ci = child(key);
      auto &childnode = children_[ci];
      if (V_TYPE(childnode, LeafNode)) {
        if (auto res = std::get<LeafNode>(childnode).insert(key, value); res) {
          if (!need_split_if_insert()) {
            insert_not_split(key, std::move(res->node));
            return std::nullopt;
          } else {
            return insert_and_split(key, std::move(res->node));
          }
        }
      } else {

        if (auto res = std::get<std::unique_ptr<InternalNode>>(childnode)
                           .get()
                           ->insert(key, value);
            res) {
          if (!need_split_if_insert()) {
            insert_not_split(key, std::move(res->node));
            return std::nullopt;
          } else {
            return insert_and_split(key, std::move(res->node));
          }
        }
      }
      return std::nullopt;
    };

  private:
    size_t child(const K &key) const {
      for (auto i = 0; i < count_; i++) {
        if (key < keys_[i]) {
          return i;
        }
      }
      return count_;
    }

    bool need_split_if_insert() const {
      if (count_ + 1 >= MAX_KEY)
        return true;
      return false;
    }

    void insert_not_split(const K &key, Node node) {
      // TODO::
    }

    SplitResult insert_and_split(const K &key, Node node) {
      // TODO::
      return {};
    }
  };

  V *find_in_leaf(LeafNode &node, const K &key) {
    for (auto i = 0; i < node.count_; ++i) {
      if (node.pairs_[i].first == key) {
        return &node.pairs_[i].second;
      }
    }
    return nullptr;
  }

  V *find_in_internal(InternalNode &node, const K &key) {
    for (auto i = 0; i < node.count_; ++i) {
      if (key < node.keys_[i]) {
        return find_in_node(node.children_[i], key);
      }
    }
    return find_in_node(node.children_[node.count_], key);
  }

  V *find_in_node(Node &node, const K &key) {
    if (V_TYPE(node, LeafNode)) {
      return find_in_leaf(leaf(node), key);
    } else [[likely]] {
      return find_in_internal(internal(node), key);
    }
  }

  InternalNode &internal(Node &node) {
    return *std::get<std::unique_ptr<InternalNode>>(node).get();
  }

  LeafNode &leaf(Node &node) { return std::get<LeafNode>(node); }

  Node root;
  LeafNode *head_ = nullptr;
};

template <typename K, typename V, int N>
void BTree<K, V, N>::insert(const K &key, const V &value) {
  if (V_TYPE(root, LeafNode)) {
    if (auto res = leaf(root).insert(key, value); res) {
      root = std::make_unique<InternalNode>(res->key, std::move(res->node),
                                            std::move(root));
    }
  } else {
    if (auto res = internal(root).insert(key, value); res) {
      root = std::make_unique<InternalNode>(res->key, std::move(res->node),
                                            std::move(root));
    }
  }
}
int main() {
  BTree<int, int, 5> t;
  auto V = t.find(5);
  t.insert(1, 1);
}
