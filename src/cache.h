#ifndef _OTHELLO_CACHE_H_
#define _OTHELLO_CACHE_H_

#include <unordered_map>
#include <list>
#include <utility>
#include <cstdint> // uint64_t
#include <mutex>
#include <iostream>

// 型エイリアスの定義
using Bitboard = uint64_t;
using ValueType = double;

// LRUキャッシュクラスの定義
class Cache {
public:
    // コンストラクタ: キャッシュ容量を設定
    Cache(size_t capacity) : capacity(capacity) {}

    // キャッシュから値を取得
    // キーが存在する場合はtrueを返し、値を設定
    bool get(Bitboard key, ValueType& value) {
        std::lock_guard<std::mutex> lock(cache_mutex); // スレッドセーフ
        auto it = cache_map.find(key);
        if (it == cache_map.end()) {
            return false;
        }
        // アクセスされた項目をリストの先頭に移動
        cache_list.splice(cache_list.begin(), cache_list, it->second);
        value = it->second->second;
        return true;
    }

    // キャッシュに値を挿入または更新
    void put(Bitboard key, ValueType value) {
        std::lock_guard<std::mutex> lock(cache_mutex); // スレッドセーフ
        auto it = cache_map.find(key);
        if (it != cache_map.end()) {
            // 値を更新し、先頭に移動
            it->second->second = value;
            cache_list.splice(cache_list.begin(), cache_list, it->second);
            return;
        }

        // 新しい項目をリストの先頭に挿入
        cache_list.emplace_front(key, value);
        cache_map[key] = cache_list.begin();

        // 容量を超えた場合、最後の項目を削除
        if (cache_map.size() > capacity) {
            auto last = cache_list.back();
            cache_map.erase(last.first);
            cache_list.pop_back();
        }
    }

    int size() {
        return cache_map.size();
    }
    
    // キャッシュを辞書としてエクスポート
    std::unordered_map<Bitboard, ValueType> export_cache() {
        std::lock_guard<std::mutex> lock(cache_mutex);
        std::unordered_map<Bitboard, ValueType> copy;
        for(auto &pair : cache_list) {
            copy[pair.first] = pair.second;
        }
        return copy;
    }

    // キャッシュを辞書からインポート
    void import_cache(const std::unordered_map<Bitboard, ValueType>& data) {
        std::lock_guard<std::mutex> lock(cache_mutex); // キャッシュ全体をロック
        std::cout << "import_cache called. Data size: " << data.size() << std::endl;
        std::flush(std::cout);
        for (const auto& pair : data) {
            std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
            std::flush(std::cout);
            // 直接キャッシュを更新する（put を呼び出さない）
            auto it = cache_map.find(pair.first);
            if (it != cache_map.end()) {
                it->second->second = pair.second;
                cache_list.splice(cache_list.begin(), cache_list, it->second);
            } else {
                cache_list.emplace_front(pair.first, pair.second);
                cache_map[pair.first] = cache_list.begin();
                if (cache_map.size() > capacity) {
                    auto last = cache_list.back();
                    cache_map.erase(last.first);
                    cache_list.pop_back();
                }
            }
        }
    }

private:
    size_t capacity; // キャッシュ容量
    std::list<std::pair<Bitboard, ValueType>> cache_list; // LRU順に保持
    std::unordered_map<Bitboard, typename std::list<std::pair<Bitboard, ValueType>>::iterator> cache_map; // キーとリストイテレータのマップ
    std::mutex cache_mutex; // スレッドセーフのためのミューテックス
};

#endif // _OTHELLO_CACHE_H_