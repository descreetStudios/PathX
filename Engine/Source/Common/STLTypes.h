#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <deque>
#include <list>
#include <forward_list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <optional>
#include <variant>
#include <tuple>
#include <utility>
#include <mutex>
#include <memory>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <functional>
#include <chrono>
#include <span>
#include <bitset>
#include <any>
#include <initializer_list>

/* Strings */
using String = std::string;
using WString = std::wstring;
using StringView = std::string_view;
using SStream = std::stringstream;
using OSStream = std::ostringstream;
using ISStream = std::istringstream;

/* Filesystem */
namespace fs = std::filesystem;
using Path = fs::path;
using IFStream = std::ifstream;
using OFStream = std::ofstream;

/* Sequential Containers */
template<typename T>
using Vector = std::vector<T>;

template<typename T, size_t N>
using Array = std::array<T, N>;

template<typename T>
using Deque = std::deque<T>;

template<typename T>
using List = std::list<T>;

template<typename T>
using ForwardList = std::forward_list<T>;

/* Associative Containers */
template<typename T>
using Set = std::set<T>;

template<typename T>
using UnorderedSet = std::unordered_set<T>;

template<typename K, typename V>
using Map = std::map<K, V>;

template<typename K, typename V>
using UnorderedMap = std::unordered_map<K, V>;

/* Container Adapters */
template<typename T>
using Stack = std::stack<T>;

template<typename T>
using Queue = std::queue<T>;

template<typename T>
using PriorityQueue = std::priority_queue<T>;

/* Utilities */
template<typename... T>
using Tuple = std::tuple<T...>;

template<typename T1, typename T2>
using Pair = std::pair<T1, T2>;

template<typename T>
using Optional = std::optional<T>;

template<typename... T>
using Variant = std::variant<T...>;

using Mutex = std::mutex;

using Any = std::any;

/* Memory */
template<typename T>
using UniquePtr = std::unique_ptr<T>;

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

/* Functional */
template<typename Sig>
using Function = std::function<Sig>;

/* Views */
template<typename T>
using Span = std::span<T>;

/* Chrono */
namespace ch = std::chrono;
using Clock = ch::system_clock;
using TimePoint = ch::time_point<Clock>;

/* Misc */
template<std::size_t N>
using Bitset = std::bitset<N>;

template<typename T>
using InitializerList = std::initializer_list<T>;
