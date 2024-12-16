#ifndef _OTHELLO_CACHE_H_
#define _OTHELLO_CACHE_H_

#include <unordered_map>
#include <list>
#include <utility>
#include <cstdint> // uint64_t
#include <mutex>
#include <iostream>
#include <shared_mutex>

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
        // まず共有ロックで読み取り
        {
            std::shared_lock<std::shared_mutex> sharedLock(cache_mutex);
            auto it = cache_map.find(key);
            if (it == cache_map.end()) {
                return false;
            }
            value = it->second->second;
        }
        // 次に排他ロックでリストを更新
        {
            std::unique_lock<std::shared_mutex> uniqueLock(cache_mutex);
            auto it = cache_map.find(key);
            if (it != cache_map.end()) {
                // アクセスされた項目をリストの先頭に移動
                cache_list.splice(cache_list.begin(), cache_list, it->second);
                value = it->second->second;
                return true;
            }
        }

        return false;
    }

    // キャッシュに値を挿入または更新
    void put(Bitboard key, ValueType value) {
        std::unique_lock<std::shared_mutex> uniqueLock(cache_mutex); // 排他ロック

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

    // 現在のキャッシュサイズを取得
    size_t size() const {
        std::shared_lock<std::shared_mutex> sharedLock(cache_mutex);
        return cache_map.size();
    }

    // キャッシュを辞書としてエクスポート
    std::unordered_map<Bitboard, ValueType> export_cache() const {
        std::shared_lock<std::shared_mutex> sharedLock(cache_mutex);
        std::unordered_map<Bitboard, ValueType> copy;
        for(const auto &pair : cache_list) {
            copy[pair.first] = pair.second;
        }
        return copy;
    }

    // キャッシュを辞書からインポート
    void import_cache(const std::unordered_map<Bitboard, ValueType>& data) {
        std::unique_lock<std::shared_mutex> uniqueLock(cache_mutex); // 排他ロック
        for (const auto& pair : data) {
            put(pair.first, pair.second);
        }
    }

private:
    size_t capacity; // キャッシュ容量
    std::list<std::pair<Bitboard, ValueType>> cache_list; // LRU順に保持
    std::unordered_map<Bitboard, typename std::list<std::pair<Bitboard, ValueType>>::iterator> cache_map; // キーとリストイテレータのマップ
    mutable std::shared_mutex cache_mutex; // 共有ミューテックス
};

#endif // _OTHELLO_CACHE_H_